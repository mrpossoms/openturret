#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "structs.h"
#include "feature_matcher.h"
#include "frame_utils.h"
#include "spi.h"
#include "utils.h"
#include "vidi.h"

#define W (640)
#define H (480)
#define DS_W (128)
#define DS_H (32)
#define FEAT_SIZE 9
#define FEAT_SIZE_H ((FEAT_SIZE - 1) >> 1)

int spi_fd;
bool running = true;
bool motors_enabled = true;

uint8_t control_byte(int yaw, int pitch, int* yaw_ticks, int* pitch_ticks)
{
	uint8_t ctrl_byte = 0;

	if (yaw < 0) { ctrl_byte |= 0x80; }
	*yaw_ticks = clamp(yaw, -7, 7);

	if (pitch < 0) { ctrl_byte |= 0x08; }
	*pitch_ticks = clamp(pitch, -7, 7);

	return ctrl_byte | ((abs(*yaw_ticks) << 4) | abs(*pitch_ticks));
}

void wait_frame(vidi_cfg_t* cam, pixel_t frame[H][W])
{
	// this function blocks until a frame pointer is returned
	uint8_t* raw_frame = vidi_wait_frame(cam);
	size_t row_size = vidi_row_bytes(cam);

	for (int r = 0; r < H; r++)
	{
		memcpy(frame[r], raw_frame + (r * row_size), row_size);
	}
}
 

cal_t calibrate(vidi_cfg_t* cam, int spi_fd);

void steppers_off(int sig)
{
	spi_transfer(spi_fd, 0);
	running = false;
}


int main (int argc, char* const argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "video device neede: e.g. /dev/video0\n");
		return -1;
	}

	// define a configuration object for a camera, here
	// you can request frame size, pixel format, frame rate
	// and the camera which you wish to use.
	vidi_cfg_t cam = {
		.width = W,
		.height = H,
		.frames_per_sec = 30,
		.path = argv[1],
		.pixel_format = V4L2_PIX_FMT_YUYV
	};

	// vidi_config is used to open and configure the camera
	// device, but can also be used to reconfigure an
	// existing vidi_cfg_t camera instance.
	assert(0 == vidi_config(&cam));

	int opt = 0;
	while ((opt = getopt(argc, argv, "m:")) > -1)
	{
		switch(opt)
		{
			case 'm':
			{
				motors_enabled = atoi(optarg);
			} break;

		}
	}

	int control = V4L2_EXPOSURE_MANUAL;
	ioctl(cam.sys.fd, VIDIOC_S_CTRL, &control);
	int ctrl[] = { V4L2_CID_EXPOSURE, 33333 };
	ioctl(cam.sys.fd, VIDIOC_S_CTRL, ctrl);

	spi_fd = open("/dev/spidev0.0", O_RDWR);

	if (spi_fd >= 0)
	{
		spi_config(spi_fd);
	}

	struct sigaction action = {
		.sa_handler = steppers_off,
	};
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGABRT, &action, NULL);

	pixel_t frame[H][W] = {};
	uint8_t diff[DS_H][DS_W] = {};
	uint8_t ds_frame[DS_H][DS_W] = {};
	uint8_t ds_last_frame[DS_H][DS_W] = {};
	match_t last_match = {};
	uint8_t feature[FEAT_SIZE][FEAT_SIZE] = {};
	bool feature_set = false;
	char display[DS_H][DS_W] = {};

	int rows_drawn = 0;
	float targ_x = DS_W >> 1;
	float targ_y = DS_H >> 1;
	float targ_dx = 0, targ_dy = 0; 
	int frame_wait = 4;
	int frame_count = 0;
	mapping_t frame_mapping = frame_mapping_1to1;

	cal_t cal = calibrate(&cam, spi_fd);
	spi_transfer(spi_fd, 0);
	fprintf(stderr, "steps_per_row: %f, steps_per_col: %f\n", cal.steps_per_row, cal.steps_per_col);

	fputs("\033[?25l", stderr);
	vidi_request_frame(&cam);

	time_t last_sec = time(NULL);
	int last_sec_frames = 0, fps = 0;
	while (running)
	{
		time_t now = time(NULL);
		wait_frame(&cam, frame);

		// request the camera to capture a frame
		vidi_request_frame(&cam);


		const char spectrum[] = " .,':;|[{+*x88ASDDBDFSDFSDBHGJFU";

		// process
		float com_x = 0, com_y = 0;
		int com_points = 0;

		// down sample frame
		uint8_t temp_ds_frame[DS_H][DS_W];
		frame_downsample_uint8(
			(point_t) { DS_H, DS_W }, temp_ds_frame,
			(point_t) { H, W }, frame);

		frame_mapping((point_t){DS_H, DS_W}, temp_ds_frame, ds_frame);

		// compute center of mass
		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int last_grey = ds_last_frame[r][c];
			int grey = ds_frame[r][c];
			diff[r][c] = max(abs(grey-last_grey) - 8, 0);

			int delta_idx = max(diff[r][c], 0) / 10;
			int idx = grey / 10;
			//display[r][c] = spectrum[max(0, idx - delta_idx)];;
			display[r][c] = spectrum[max(0, delta_idx)];
			display[r][c] = spectrum[grey / 23];

			if (delta_idx >= 1)
			{
				com_x += c;
				com_y += r;
				com_points++;
			}
		}

		if (com_points > 3)// && frame_wait <= 0)
		{
			float last_targ_x = targ_x;
			float last_targ_y = targ_y;
			com_x /= com_points;
			com_y /= com_points;

			{ // lpf on the target coordinate
				const float power = 5;

				if (targ_x == (DS_H >> 1) && targ_y == (DS_H >> 1))
				{
					targ_x = com_x;
					targ_y = com_y;
				}

				targ_x = (com_x + targ_x * (power - 1.f)) / power;
				targ_y = (com_y + targ_y * (power - 1.f)) / power;		
			}

			float dx = targ_x - last_targ_x, dy = targ_y - last_targ_y;
			targ_dx = dx; targ_dy = dy;
			float targ_com_dist = sqrt(pow(targ_x - com_x, 2.0) + pow(targ_y - com_y, 2.0));

			// extract a feature from what's been marked as the target
			if (frame_wait <= 0 && com_points < 200 && targ_com_dist < 3)
			{
				frame_copy_uint8(
					FEAT_SIZE, FEAT_SIZE,
					feature,
					DS_H, DS_W,
					ds_frame,
					(win_t){ com_y - FEAT_SIZE_H, com_x - FEAT_SIZE_H }
				);
				feature_set = true;
			}

		}
		else
		{
			targ_dx = targ_dy = 0;
		}

		match_t match = {
			.score = -1,	
		};

		if (feature_set)
		{
			match = match_feature(
				(point_t){ DS_H, DS_W }, ds_frame,
				(point_t){ FEAT_SIZE, FEAT_SIZE }, feature,
				(win_t) {{}, { DS_H, DS_W }}
			);

			int dr = match.r - last_match.r, dc = match.c - last_match.c;
			if (sqrtf(dr * dr + dc * dc) > 10)
			{
				match.score = -1;	
				feature_set = false;
			}

			last_match = match;
		}

		int yaw = -((match.c + FEAT_SIZE_H) +  - (DS_W >> 1)) * cal.steps_per_col;
		int pitch = -((match.r + FEAT_SIZE_H) - (DS_H >> 1)) * cal.steps_per_row;
		int disp_yaw = yaw, disp_pitch = pitch;
		//if (com_points > 0 && frame_wait <= 0)

		float error = sqrt(yaw * yaw + pitch * pitch);
		float delta = sqrt(targ_dx * targ_dx + targ_dy * targ_dy);

		if (frame_wait <= 0 && error > 0 && delta < 2 && feature_set)
		{
			//if (match.score > 1000)
			{

			}

			do
			{
				int yaw_ticks, pitch_ticks;
				uint8_t ctrl_byte = control_byte(yaw, pitch, &yaw_ticks, &pitch_ticks);
				if (motors_enabled) { spi_transfer(spi_fd, ctrl_byte); }

				frame_wait += sqrt(pitch_ticks * pitch_ticks + yaw_ticks * yaw_ticks) * cal.frames_per_step * 1;
				yaw -= yaw_ticks;
				pitch -= pitch_ticks;
			}
			while(yaw != 0 || pitch != 0);
		}
		else
		{
			// turn steppers off to save energy
			spi_transfer(spi_fd, 0);
		}

		{ // drawing markers for visualization
			for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				display[(int)targ_y+j][(int)targ_x+i] = '#';
				display[DS_H >> 1][DS_W >> 1] = '+';
				display[(int)com_y+j][(int)com_x+i] = '?';
			}


			if (match.score >= 0)
			for (int i = 0; i < FEAT_SIZE; i++)
			for (int j = 0; j < FEAT_SIZE_H; j++)
			{
				display[match.r+j][match.c+i] = '@';
			}
		}


		if (rows_drawn > 0)
		{ // Erase the previously drawn rows
			static char move_up[16] = {};
			sprintf(move_up, "\033[%dA", rows_drawn);
			fprintf(stderr, "%s", move_up);
			rows_drawn = 0;
		}

		if (now > last_sec)
		{
			fps = frame_count - last_sec_frames;
			last_sec_frames = frame_count;
			last_sec = now;
		}

		// display
		if (frame_wait > 0) { fprintf(stderr, "\033[31m"); }
		const int sf = 1; // scale factor
		for (int r = 0; r < DS_H / sf; r++)
		{
			for (int c = 0; c < DS_W / (sf); c++)
			{
				fputc(display[r * sf][c * sf], stderr);
			}
			fputc('|', stderr);
			fputc('\n', stderr);
			rows_drawn += 1;
		}
		fprintf(stderr, "\033[0m");

		printf("COM points: %d error %0.3f frame_wait: %d   \n", com_points, error, frame_wait);
		printf("COM (%d, %d) targ∆: %0.3f, match_score: %0.3f \n", (int)targ_x, (int)targ_y, delta, match.score);
		printf("yaw, pitch: (%d, %d) frames: %d fps: %d \n", disp_yaw, disp_pitch, frame_count, fps);
		rows_drawn+=3;

		if (frame_wait > 0) { frame_wait--; }
		frame_count++;
		memcpy(ds_last_frame, ds_frame, sizeof(ds_last_frame));
	}

	spi_transfer(spi_fd, 0);

	return 0;
}


cal_t calibrate(vidi_cfg_t* cam, int spi_fd)
{
	cal_t cal = {};
	pixel_t frame[H][W] = {}, last_frame[H][W] = {}, settle_frame[H][W];
	uint8_t down_sampled[DS_H][DS_W] = {};
	uint8_t feature[FEAT_SIZE][FEAT_SIZE] = {};
	const int motor_steps = 7;

	const int dr = H / DS_H;
	const int dc = W / DS_W;

	{ // load cal if it exists
		int fd = open("ot.cal", O_RDONLY);
		if (sizeof(cal) == read(fd, &cal, sizeof(cal)))
		{
			close(fd);
			return cal;
		}
		close(fd);
	}

	const int steps = 1;
	for(int si = steps; si--;)
	{
		// let the video stream settle
		float expected_diff = 0;
		for (int i = 35; i--;)
		{
			frame_copy_pixel(H, W, last_frame, H, W, frame, (win_t){{}, {H, W}});
			frame_copy_pixel(H, W, settle_frame, H, W, frame, (win_t){{}, {H, W}});
			vidi_request_frame(cam); wait_frame(cam, frame);
			if (i < 10)
			{
				float diff = frame_difference((point_t){H, W}, last_frame, frame);
				expected_diff += diff; 
			}
		}

		expected_diff /= 10;
		printf("Expected diff: %f\n", expected_diff);

		// downsample the frame
		frame_downsample_uint8(
			(point_t) { DS_H, DS_W }, down_sampled,
			(point_t) { H, W }, frame);

		// store the feature
		frame_copy_uint8(
			FEAT_SIZE, FEAT_SIZE, 
			feature,
			DS_H, DS_W,
			down_sampled,
			(win_t){
				{ (DS_H / 2) - FEAT_SIZE, (DS_W / 2) - FEAT_SIZE },
				{ FEAT_SIZE, FEAT_SIZE }
			}
		);

		for (int r = -FEAT_SIZE_H; r <= FEAT_SIZE_H; r++)
		for (int c = -FEAT_SIZE_H; c <= FEAT_SIZE_H; c++)
		{
			feature[r + FEAT_SIZE_H][c + FEAT_SIZE_H] = down_sampled[(DS_H >> 1) + r][(DS_W >> 1) + c];	
		}

		// move on the yaw and pitch axis
		int pt, yt;
		spi_transfer(spi_fd, control_byte(motor_steps, motor_steps, &yt, &pt));

		// capture another frame (wait for a bit)
		vidi_request_frame(cam); wait_frame(cam, frame);
		int frames_waited = 1;
		float frame_diff;
		do
		{
			vidi_request_frame(cam); wait_frame(cam, frame);
			frame_diff = frame_difference((point_t){H, W}, settle_frame, frame);
			frame_copy_pixel(H, W, settle_frame, H, W, frame, (win_t){{}, {H, W}});
			printf("difference: %f\n", frame_diff);
			frames_waited++;
		}
		while (frame_diff > expected_diff);

		cal.frames_per_step = frames_waited / (float)motor_steps;
		printf("frames_per_step: %f\n", cal.frames_per_step);

		// downsample the frame
		frame_downsample_uint8(
			(point_t) { DS_H, DS_W }, down_sampled,
			(point_t) { H, W }, frame);

		// find the best feature
		match_t match = match_feature(
			(point_t){ DS_H, DS_W },
			down_sampled,
			(point_t){ FEAT_SIZE, FEAT_SIZE },
			feature,
			(win_t){{ 0, 0 }, { DS_H, DS_W }}  
		);

		spi_transfer(spi_fd, control_byte(-motor_steps, -motor_steps, &yt, &pt));
		fprintf(stderr, "best_score: %f, coord: (%d, %d)\n", match.score, match.r, match.c);

		// let the video stream settle
		for (int i = 30; i--;)
		{ vidi_request_frame(cam); wait_frame(cam, frame); }


		cal.steps_per_col += fabsf(motor_steps / ((float)(DS_W >> 1) - match.c));
		cal.steps_per_row += fabsf(motor_steps / ((float)(DS_H >> 1) - match.r));

		assert(!isinf(cal.steps_per_col * cal.steps_per_row));
	}

	cal.steps_per_col /= steps;
	cal.steps_per_row /= steps;

	{ // save cal
		int fd = open("ot.cal", O_WRONLY | O_CREAT, 0644);
		write(fd, &cal, sizeof(cal));
		close(fd);
	}

	return cal;
}


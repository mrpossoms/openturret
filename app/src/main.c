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
#include "utils.h"
#include "vidi.h"
#include "motor_control.h"
#include "display.h"

int spi_fd;
bool running = true;
bool motors_enabled = false;
bool display_debug = false;

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

static void stop(int sig)
{
	if (sig == SIGINT)
	{
		running = false;
	}

} 

cal_t calibrate(vidi_cfg_t* cam, int spi_fd);

int main (int argc, char* const argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "video device needed: e.g. /dev/video0\n");
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

	int control = V4L2_EXPOSURE_MANUAL;
	ioctl(cam.sys.fd, VIDIOC_S_CTRL, &control);
	int ctrl[] = { V4L2_CID_EXPOSURE, 33333 };
	ioctl(cam.sys.fd, VIDIOC_S_CTRL, ctrl);

	spi_fd = motor_control_init("/dev/spidev0.0");

	int opt = 0;
	while ((opt = getopt(argc, argv, "m:dp:y:")) > -1)
	{
		switch(opt)
		{
			case 'm':
			{
				motors_enabled = atoi(optarg);
			} break;

			case 'd':
			{
				display_debug = true;
				display_init();
			} break;
			case 'p':
			{
				int steps = atoi(optarg);
				int yaw_ticks, pitch_ticks;
				while (steps != 0)
				{
					motor_control(0, steps, &yaw_ticks, &pitch_ticks);
					steps -= pitch_ticks;				
				}

				printf("Pitching: %d steps\n", pitch_ticks);
				running = false;
			} break;
			case 'y':
			{
				int steps = atoi(optarg);
				int yaw_ticks, pitch_ticks;
				while (steps != 0)
				{
					motor_control(0, steps, &yaw_ticks, &pitch_ticks);
					steps -= yaw_ticks;					
				}

				printf("Pitching: %d steps\n", pitch_ticks);
				running = false;
			} break;
		}
	}

	signal(SIGINT, stop);
	signal(SIGABRT, stop);

	// frames
	pixel_t frame[H][W] = {}; // raw frame from the camera
	uint8_t diff[DS_H][DS_W] = {}; // down sampled grey difference between frames
	uint8_t ds_frame[DS_H][DS_W] = {}; // down sampled grey frame
	uint8_t ds_last_frame[DS_H][DS_W] = {}; // the last down sampled grey frame
	
	// features
	match_t last_match = {};
	uint8_t feature[FEAT_SIZE][FEAT_SIZE] = {};
	bool feature_set = false;
	
	// debug display buffer
	char display[DS_H][DS_W] = {};

	// tracking
	tracking_t track = {
		.target = {
			.coord = { DS_W >> 1, DS_H >> 1 },
			.delta = { 0, 0 },
		},
		.com = {
			.coord = { DS_W >> 1, DS_H >> 1 },
			.points = 0,
		}
	};
	
	int frame_wait = 4;
	int frame_count = 0;
	mapping_t frame_mapping = frame_mapping_1to1;

	cal_t cal = calibrate(&cam, spi_fd);
	motor_control_off();
	fprintf(stderr, "steps_per_row: %f, steps_per_col: %f\n", cal.steps_per_row, cal.steps_per_col);

	vidi_request_frame(&cam);


	while (running)
	{
		wait_frame(&cam, frame);

		// request the camera to capture a frame
		vidi_request_frame(&cam);


		track.com.coord = (vec2_t){};
		track.com.points = 0;

		// down sample frame
		uint8_t temp_ds_frame[DS_H][DS_W];
		frame_downsample_uint8(
			(point_t) { DS_H, DS_W }, temp_ds_frame,
			(point_t) { H, W }, frame);

		frame_mapping((point_t){DS_H, DS_W}, temp_ds_frame, ds_frame);

		// TODO: instead of just computing the average of all
		// changed pixels, it would be interesting to compute a
		// k-means or something to find the single region which
		// is most significant.
		// compute center of motion
		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int last_grey = ds_last_frame[r][c];
			int grey = ds_frame[r][c];
			diff[r][c] = max(abs(grey-last_grey) - 8, 0);

			int delta = max(diff[r][c], 0);
			if (delta >= 10)
			{
				track.com.coord += (vec2_t){ c, r };
				track.com.points++;
			}
		}

		if (display_debug)
		{
			display_frame_to_chars(DS_H, DS_W, ds_frame, display);
		}

		if (track.com.points > 3)// && frame_wait <= 0)
		{
			tracking_t last_track = track;

			track.com.coord /= (float)track.com.points;

			// TOOD: Instead of low passing the target coord, it would be much
			// more interesting to use a dynamics predictive filter (KF) 
			{ // lpf on the target coordinate
				const float power = 5;

				if (track.target.coord[0] == (DS_H >> 1) && track.target.coord[1] == (DS_H >> 1))
				{
					track.target.coord = track.com.coord;
				}

				track.target.coord = (track.com.coord + track.target.coord * (power - 1.f)) / power;
			}

			track.target.delta = track.target.coord - last_track.target.coord;
			float targ_com_dist = vec2_len(track.target.coord - track.com.coord);

			// extract a feature from what's been marked as the target
			if (frame_wait <= 0 && track.com.points < 200 && targ_com_dist < 3)
			{
				frame_copy_uint8(
					FEAT_SIZE, FEAT_SIZE,
					feature,
					DS_H, DS_W,
					ds_frame,
					(win_t){ track.com.coord[1] - FEAT_SIZE_H, track.com.coord[0] - FEAT_SIZE_H }
				);
				feature_set = true;
			}

		}
		else
		{
			track.target.delta = (vec2_t){ 0, 0 };
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

		float error = sqrt(yaw * yaw + pitch * pitch);
		float delta = vec2_len(track.target.delta);

		if (frame_wait <= 0 && error > 0 && delta < 2 && feature_set)
		{
			do
			{
				int yaw_ticks, pitch_ticks;
				if (motors_enabled) 
				{
					motor_control(yaw, pitch, &yaw_ticks, &pitch_ticks);
				}

				frame_wait += sqrt(pitch_ticks * pitch_ticks + yaw_ticks * yaw_ticks) * cal.frames_per_step * 1;
				yaw -= yaw_ticks;
				pitch -= pitch_ticks;
			}
			while(yaw != 0 || pitch != 0);
		}
		else
		{
			// turn steppers off to save energy
			motor_control_off();
		}

		if (display_debug)
		{ // drawing markers for visualization
			display_debug_info_to_chars(
				DS_H, DS_W, 
				track, 
				match, 
				yaw, pitch, 
				frame_count, 
				frame_wait, 
				display
			);
		}

		if (frame_wait > 0) { frame_wait--; }
		frame_count++;
		memcpy(ds_last_frame, ds_frame, sizeof(ds_last_frame));
	}

	motor_control_off();
	if (display_debug) { display_deinit(); }
	printf("stopped\n");

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
		motor_control(motor_steps, motor_steps, &yt, &pt);

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

		motor_control(-motor_steps, -motor_steps, &yt, &pt);
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
		assert(sizeof(cal) == write(fd, &cal, sizeof(cal)));
		close(fd);
	}

	return cal;
}
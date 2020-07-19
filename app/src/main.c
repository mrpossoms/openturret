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
#include <math.h>

#include "structs.h"
#include "feature_matcher.h"
#include "frame_utils.h"
#include "spi.h"
#include "utils.h"
#include "vidi.h"

#define W (640)
#define H (480)
#define DS_W (256)
#define DS_H (64)
#define FEAT_SIZE 7

int spi_fd;
bool running = true;

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
 

cal_t calibrate(vidi_cfg_t* cam, int spi_fd)
{

	const int FEAT_SIZE_H = FEAT_SIZE / 2;

	cal_t cal = {};
	pixel_t frame[H][W] = {}, last_frame[H][W] = {}, settle_frame[H][W];
	uint8_t down_sampled[DS_H][DS_W] = {};
	uint8_t feature[FEAT_SIZE][FEAT_SIZE] = {};
	const int motor_steps = 7;

	const int dr = H / DS_H;
	const int dc = W / DS_W;

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
			if (i < 30)
			expected_diff += frame_difference(H, W, last_frame, frame);
			
		}

		expected_diff /= 30;
		printf("Expected diff: %f\n", expected_diff);

		// downsample the frame
		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int ri = r * dr, ci = c * dc;
			down_sampled[r][c] = frame[ri][ci].Y;
		}

		// store the feature
		for (int r = -FEAT_SIZE_H; r <= FEAT_SIZE_H; r++)
		for (int c = -FEAT_SIZE_H; c <= FEAT_SIZE_H; c++)
		{
			feature[r + FEAT_SIZE_H][c + FEAT_SIZE_H] = down_sampled[(DS_H >> 1) + r][(DS_W >> 1) + c];	
		}

		// move on the yaw and pitch axis
		int pt, yt;
		spi_transfer(spi_fd, control_byte(motor_steps, motor_steps, &yt, &pt));

		// capture another frame (wait for a bit)
		int frames_waited = 1;
		float frame_diff;
		do
		{
			vidi_request_frame(cam); wait_frame(cam, frame);
			frame_diff = frame_difference(H, W, settle_frame, frame);
			frame_copy_pixel(H, W, settle_frame, H, W, frame, (win_t){{}, {H, W}});
			printf("difference: %f\n", frame_diff);
			frames_waited++;
		}
		while (frame_diff > expected_diff);

		cal.frames_per_step = frames_waited / (float)motor_steps;
		printf("frames_per_step: %f\n", cal.frames_per_step);

		// downsample the frame
		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int ri = r * dr, ci = c * dc;
			down_sampled[r][c] = frame[ri][ci].Y;
		}

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

	return cal;
}

void steppers_off(int sig)
{
	spi_transfer(spi_fd, 0);
	running = false;
}


int main (int argc, const char* argv[])
{
	// define a configuration object for a camera, here
	// you can request frame size, pixel format, frame rate
	// and the camera which you wish to use.
	vidi_cfg_t cam = {
		.width = W,
		.height = H,
		.frames_per_sec = 15,
		.path = argv[1],
		.pixel_format = V4L2_PIX_FMT_YUYV
	};

	// vidi_config is used to open and configure the camera
	// device, but can also be used to reconfigure an
	// existing vidi_cfg_t camera instance.
	assert(0 == vidi_config(&cam));

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

	pixel_t last_frame[H][W] = {};
	pixel_t frame[H][W] = {};
	uint8_t diff[DS_H][DS_W] = {};
	uint8_t targ_feat[FEAT_SIZE][FEAT_SIZE] = {};
	char display[DS_H][DS_W] = {};

	int rows_drawn = 0;
	float targ_x = DS_W >> 1;
	float targ_y = DS_H >> 1;
	float targ_dx = 0, targ_dy = 0; 
	int frame_wait = 4;
	int frame_count = 0;
	int error_time = 0;

	cal_t cal = calibrate(&cam, spi_fd);
	spi_transfer(spi_fd, 0);
	fprintf(stderr, "steps_per_row: %f, steps_per_col: %f\n", cal.steps_per_row, cal.steps_per_col);

	fputs("\033[?25l", stderr);
	vidi_request_frame(&cam);

	while (running)
	{
		wait_frame(&cam, frame);

		// request the camera to capture a frame
		vidi_request_frame(&cam);


		const char spectrum[] = " .,':;|[{+*x88ASDDBDFSDFSDBHGJFU";

		// process
		float com_x = 0, com_y = 0;
		int com_points = 0;
		const int dr = H / DS_H;
		const int dc = W / DS_W;


		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int ri = r * dr, ci = c * dc;
			int last_grey = last_frame[ri][ci].Y;
			int grey = frame[ri][ci].Y;
			int delta = max(abs(grey-last_grey) - 16, 0);
			diff[r][c] = delta;

			int delta_idx = max(diff[r][c], 0) / 10;
			int idx = grey / 10;
			display[r][c] = spectrum[max(0, idx - delta_idx)];;
			//display[r][c] = spectrum[max(0, delta_idx)];

			if (delta_idx >= 1)
			{
				com_x += c;
				com_y += r;
				com_points++;
			}
		}

		if (com_points > 5 && frame_wait <= 0)
		{
			float last_targ_x = targ_x;
			float last_targ_y = targ_y;
			float x = com_x / com_points;
			float y = com_y / com_points;

			const float power = 2;
			targ_x = (x + targ_x * (power - 1.f)) / power;
			targ_y = (y + targ_y * (power - 1.f)) / power;		


			float dx = targ_x - last_targ_x, dy = targ_y - last_targ_y;
			targ_dx = dx; targ_dy = dy;
		}


		for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		display[(int)targ_y+j][(int)targ_x+i] = '#';
		display[DS_H >> 1][DS_W >> 1] = '+';

		if (rows_drawn > 0)
		{ // Erase the previously drawn rows
			static char move_up[16] = {};
			sprintf(move_up, "\033[%dA", rows_drawn);
			fprintf(stderr, "%s", move_up);
			rows_drawn = 0;
		}

		int yaw = -(targ_x - (DS_W >> 1)) * cal.steps_per_col;
		int pitch = -(targ_y - (DS_H >> 1)) * cal.steps_per_row;
		int disp_yaw = yaw, disp_pitch = pitch;
		//if (com_points > 0 && frame_wait <= 0)

		error_time += sqrt(yaw * yaw + pitch * pitch);
		float delta = sqrt(targ_dx * targ_dx + targ_dy * targ_dy);

		if (frame_wait <= 0 && error_time > 10 && delta < 1)//max(abs(yaw), abs(pitch)) > 3)
		{
			if (yaw || pitch)
			{
				targ_x = DS_W >> 1;
				targ_y = DS_H >> 1;
			}

			error_time = 0;

			do
			{
				int yaw_ticks, pitch_ticks;
				uint8_t ctrl_byte = control_byte(yaw, pitch, &yaw_ticks, &pitch_ticks);
				spi_transfer(spi_fd, ctrl_byte);

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

		// display
		if (frame_wait > 0) { fprintf(stderr, "\033[31m"); }
		for (int r = 0; r < DS_H / 2; r++)
		{
			for (int c = 0; c < DS_W / 2; c++)
			{
				fputc(display[r * 2][c * 2], stderr);
			}
			fputc('|', stderr);
			fputc('\n', stderr);
			rows_drawn += 1;
		}
		fprintf(stderr, "\033[0m");

		printf("COM points: %d error_time %d frame_wait: %d   \n", com_points, error_time, frame_wait);
		printf("COM (%d, %d)\n", (int)targ_x, (int)targ_y);
		printf("yaw, pitch: (%d, %d) frames: %d \n", disp_yaw, disp_pitch, frame_count);
		rows_drawn+=3;

		if (frame_wait > 0) { frame_wait--; }
		frame_count++;
		memcpy(last_frame, frame, sizeof(frame));
	}

	spi_transfer(spi_fd, 0);

	return 0;
}

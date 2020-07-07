#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <math.h>

#include "spi.h"
#include "utils.h"
#include "vidi.h"

#define CAM_FOV 30
#define DEG_ROW 1
#define DEG_COL 1
#define DEG_PITCH_STEP 1
#define DEG_YAW_STEP 1

typedef union {
	struct {
		uint8_t r, g, b;
	};
	uint8_t v[3];
} rgb_t;


typedef struct {
	uint8_t Y, UV;
} pixel_t;

typedef struct {
	size_t width, height;
	rgb_t* buf;
} frame_t;

typedef struct {
	float steps_per_row;
	float steps_per_col;
} cal_t;


uint8_t control_byte(int yaw, int pitch, int* yaw_ticks, int* pitch_ticks)
{
	uint8_t ctrl_byte = 0;

	if (yaw < 0) { ctrl_byte |= 0x80; }
	*yaw_ticks = clamp(yaw, -7, 7);

	if (pitch < 0) { ctrl_byte |= 0x08; }
	*pitch_ticks = clamp(pitch, -7, 7);

	return ctrl_byte | ((abs(*yaw_ticks) << 4) | abs(*pitch_ticks));
}

void wait_frame(vidi_cfg_t* cam, pixel_t frame[480][640])
{
	// this function blocks until a frame pointer is returned
	uint8_t* raw_frame = vidi_wait_frame(cam);
	size_t row_size = vidi_row_bytes(cam);

	for (int r = 0; r < 480; r++)
	{
		memcpy(frame[r], raw_frame + (r * row_size), row_size);
	}
}

cal_t calibrate(vidi_cfg_t* cam, int spi_fd)
{
	#define FSIZE 7
	const int FSIZE_H = 3;

	cal_t cal = {};
	pixel_t frame[480][640] = {};
	uint8_t down_sampled[32][128] = {};
	uint8_t feature[FSIZE][FSIZE] = {};
	const int motor_steps = 5;

	int best_row, best_col, best_score = 100000000;

	const int dr = 480 / 32;
	const int dc = 640 / 128;

	// let the video stream settle
	for (int i = 60; i--;)
	{ vidi_request_frame(cam); wait_frame(cam, frame); }

	// downsample the frame
	for (int r = 0; r < 32; r++)
	for (int c = 0; c < 128; c++)
	{
		int ri = r * dr, ci = c * dc;
		down_sampled[r][c] = frame[ri][ci].Y;
	}

	// store the feature
	for (int r = -FSIZE_H; r <= FSIZE_H; r++)
	for (int c = -FSIZE_H; c <= FSIZE_H; c++)
	{
		feature[r + FSIZE_H][c + FSIZE_H] = down_sampled[16 + r][64 + c];	
	}

	// move on the yaw and pitch axis
	int pt, yt;
	spi_transfer(spi_fd, control_byte(motor_steps, motor_steps, &yt, &pt));

	// capture another frame (wait for a bit)
	for (int i = 60; i--;)
	{
		// request the camera to capture a frame
		vidi_request_frame(cam);
		wait_frame(cam, frame);
	}

	// downsample the frame
	for (int r = 0; r < 32; r++)
	for (int c = 0; c < 128; c++)
	{
		int ri = r * dr, ci = c * dc;
		down_sampled[r][c] = frame[ri][ci].Y;
	}

	// find the best feature
	for (int r = 10; r < 22; r++)
	for (int c = 32; c < (128 - 32); c++)
	{
		int score = 0;
		for (int i = -FSIZE_H; i <= FSIZE_H; i++)
		for (int j = -FSIZE_H; j <= FSIZE_H; j++)
		{
			score += abs(feature[i + FSIZE_H][j + FSIZE_H] - down_sampled[r + i][c + j]);
		}

		if (score < best_score)
		{
			best_row = r;
			best_col = c;
			best_score = score;
		}
	}

	spi_transfer(spi_fd, control_byte(-motor_steps, -motor_steps, &yt, &pt));
	fprintf(stderr, "best_score: %d, coord: (%d, %d)\n", best_score, best_row, best_col);

	// let the video stream settle
	for (int i = 60; i--;)
	{ vidi_request_frame(cam); wait_frame(cam, frame); }


	cal.steps_per_col = fabsf(motor_steps / (64.f - best_col));
	cal.steps_per_row = fabsf(motor_steps / (16.f - best_row));

	return cal;
}

int main (int argc, const char* argv[])
{
	// define a configuration object for a camera, here
	// you can request frame size, pixel format, frame rate
	// and the camera which you wish to use.
	vidi_cfg_t cam = {
		.width = 640,
		.height = 480,
		.frames_per_sec = 60,
		.path = argv[1],
		.pixel_format = V4L2_PIX_FMT_YUYV
	};

	// vidi_config is used to open and configure the camera
	// device, but can also be used to reconfigure an
	// existing vidi_cfg_t camera instance.
	assert(0 == vidi_config(&cam));

	int spi_fd = open("/dev/spidev0.0", O_RDWR);

	if (spi_fd >= 0)
	{
		spi_config(spi_fd);
	}

	pixel_t last_frame[480][640] = {};
	pixel_t frame[480][640] = {};
	char display[32][128] = {};

	int rows_drawn = 0;
	float targ_x = 64;
	float targ_y = 16;
	int frame_wait = 4;
	int frame_count = 0;

	cal_t cal = calibrate(&cam, spi_fd);
	spi_transfer(spi_fd, 0);
	fprintf(stderr, "steps_per_row: %f, steps_per_col: %f\n", cal.steps_per_row, cal.steps_per_col);


	fputs("\033[?25l", stderr);
	vidi_request_frame(&cam);

	while (1)
	{
		wait_frame(&cam, frame);

		// request the camera to capture a frame
		vidi_request_frame(&cam);


		const char spectrum[] = "  .,':;|[{+*x88";

		// process
		//float com_x = 64, com_y = 16;
		//int com_points = 1;
		float com_x = 0, com_y = 0;
		int com_points = 0;
		const int dr = 480 / 32;
		const int dc = 640 / 128;

		if (frame_count > 0)
		for (int r = 0; r < 32; r++)
		{
			for (int c = 0; c < 128; c++)
			{
				int ri = r * dr, ci = c * dc;
				int last_grey = last_frame[ri][ci].Y;//(last_frame[ri][ci].r + last_frame[ri][ci].g + last_frame[ri][ci].b) / 3;
				int grey = frame[ri][ci].Y;
				int delta = abs(grey-last_grey);

				int delta_idx = max(delta - 8, 0) / 18;
				int idx = grey / 18;
				display[r][c] = spectrum[delta_idx];//spectrum[max(0, idx - delta_idx)];;

				if (frame_wait <= 0)
				if (delta_idx >= 2)
				{
					com_x += c;
					com_y += r;
					com_points++;
				}
			}
		}

		if (com_points > 0 && frame_wait <= 0)
		{
			float x = com_x / com_points;
			float y = com_y / com_points;

			const float power = 5;
			targ_x = (x + targ_x * (power - 1.f)) / power;
			targ_y = (y + targ_y * (power - 1.f)) / power;			
		}

		display[(int)targ_y][(int)targ_x] = '#';
		display[16][64] = '+';

		if (rows_drawn > 0)
		{ // Erase the previously drawn rows
			static char move_up[16] = {};
			sprintf(move_up, "\033[%dA", rows_drawn);
			fprintf(stderr, "%s", move_up);
			rows_drawn = 0;
		}

		int yaw = -(targ_x - 64) * cal.steps_per_col;
		int pitch = -(targ_y - 16) * cal.steps_per_row;
		int disp_yaw = yaw, disp_pitch = pitch;
		//if (com_points > 0 && frame_wait <= 0)
		if (frame_wait <= 0 && max(abs(yaw), abs(pitch)) > 3)
		{
			if (yaw || pitch)
			{
				targ_x = 64;
				targ_y = 16;
			}

			do
			{
				int yaw_ticks, pitch_ticks;
				uint8_t ctrl_byte = control_byte(yaw, pitch, &yaw_ticks, &pitch_ticks);
				spi_transfer(spi_fd, ctrl_byte);

				frame_wait += sqrt(pitch_ticks * pitch_ticks + yaw_ticks * yaw_ticks) * 10;
				yaw -= yaw_ticks;
				pitch -= pitch_ticks;
			}
			while(yaw != 0 || pitch != 0);
		}

		// display
		for (int r = 0; r < 32; r++)
		{
			for (int c = 0; c < 128; c++)
			{
				fputc(display[r][c], stderr);
			}
			fputc('|', stderr);
			fputc('\n', stderr);
			rows_drawn += 1;
		}

		printf("COM points: %d   \n", com_points);
		printf("COM (%d, %d)\n", (int)targ_x, (int)targ_y);
		printf("yaw, pitch: (%d, %d) frames: %d\n", disp_yaw, disp_pitch, frame_count);
		rows_drawn+=3;

		frame_wait--;
		frame_count++;
		memcpy(last_frame, frame, sizeof(frame));
	}

	return 0;
}

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
				printf("Pitching: %d steps\n", steps);
				while (steps != 0)
				{
					motor_control(0, steps, &yaw_ticks, &pitch_ticks);
					steps -= pitch_ticks;				
				}
				sleep(1);
				motor_control_off();
				running = false;
			} break;
			case 'y':
			{
				int steps = atoi(optarg);
				int yaw_ticks, pitch_ticks;
				printf("Pitching: %d steps\n", steps);
				while (steps != 0)
				{
					motor_control(0, steps, &yaw_ticks, &pitch_ticks);
					steps -= yaw_ticks;					
				}
				sleep(1);
				motor_control_off();
				running = false;
			} break;
		}
	}

	signal(SIGINT, stop);
	signal(SIGABRT, stop);

	// frames
	pixel_t frame[H][W] = {}; // raw frame from the camera
	// uint8_t diff[DS_H][DS_W] = {}; // down sampled grey difference between frames
	// uint8_t ds_frame[DS_H][DS_W] = {}; // down sampled grey frame
	// uint8_t ds_last_frame[DS_H][DS_W] = {}; // the last down sampled grey frame
	context_t ctx = {
		.cal = calibrate(&cam, spi_fd),
	};

	// features
	// match_t last_match = {};
	// uint8_t feature[FEAT_SIZE][FEAT_SIZE] = {};
	// bool feature_set = false;
	


	// debug display buffer
	char display[DS_H][DS_W] = {};

	// tracking
	// tracking_t track = {
	// 	.target = {
	// 		.coord = { DS_W >> 1, DS_H >> 1 },
	// 		.delta = { 0, 0 },
	// 	},
	// 	.com = {
	// 		.coord = { DS_W >> 1, DS_H >> 1 },
	// 		.points = 0,
	// 	}
	// };
	for (unsigned i = 0; i < T_LEN; i++)
	{
		ctx.states[i].tracker = (tracking_t){
			.target = {
				.coord = { DS_W >> 1, DS_H >> 1 },				
			},
		};
	}
	
	// int frame_wait = 4;
	// int frame_count = 0;

	// mapping_t frame_mapping = frame_mapping_1to1;

	// cal_t cal = calibrate(&cam, spi_fd);
	motor_control_off();
	fprintf(stderr, "steps_per_row: %f, steps_per_col: %f\n", ctx.cal.steps_per_row, ctx.cal.steps_per_col);

	vidi_request_frame(&cam);


	while (running)
	{
		unsigned f = ctx.frame.count;
		state_t* x_t_1 = ctx.states + ((f - 1) % T_LEN); 
		state_t* x_t = ctx.states + (f % T_LEN);
		wait_frame(&cam, frame);

		// request the camera to capture a frame
		vidi_request_frame(&cam);


		x_t->tracker.com.coord = (vec2_t){};
		x_t->tracker.com.points = 0;

		// down sample frame
		// uint8_t temp_ds_frame[DS_H][DS_W];
		frame_downsample_uint8(
			(point_t) { DS_H, DS_W }, x_t->frame,
			(point_t) { H, W }, frame);

		// frame_mapping((point_t){DS_H, DS_W}, temp_ds_frame, ds_frame);

		// TODO: instead of just computing the average of all
		// changed pixels, it would be interesting to compute a
		// k-means or something to find the single region which
		// is most significant.
		// compute center of motion
		for (int r = 0; r < DS_H; r++)
		for (int c = 0; c < DS_W; c++)
		{
			int last_grey = x_t_1->frame[r][c];
			int grey = x_t->frame[r][c];
			// diff[r][c] = max(abs(grey-last_grey) - 8, 0);

			// int delta = max(diff[r][c], 0);
			int delta = max(abs(grey-last_grey) - 8, 0);
			if (delta >= 10)
			{
				x_t->tracker.com.coord += (vec2_t){ c, r };
				x_t->tracker.com.points++;
			}
		}

		if (display_debug)
		{
			display_frame_to_chars(DS_H, DS_W, x_t->frame, display);
		}

		if (x_t->tracker.com.points > 3)// && frame_wait <= 0)
		{
			x_t->tracker.com.coord /= (float)x_t->tracker.com.points;

			// TOOD: Instead of low passing the target coord, it would be much
			// more interesting to use a dynamics predictive filter (KF) 
			{ // lpf on the target coordinate
				const float power = 5;

				if (x_t->tracker.target.coord[0] == (DS_H >> 1) && x_t->tracker.target.coord[1] == (DS_H >> 1))
				{
					x_t->tracker.target.coord = x_t->tracker.com.coord;
				}

				x_t->tracker.target.coord = (x_t->tracker.com.coord + x_t->tracker.target.coord * (power - 1.f)) / power;
			}

			x_t->tracker.target.delta = x_t->tracker.target.coord - x_t_1->tracker.target.coord;
			float targ_com_dist = vec2_len(x_t->tracker.target.coord - x_t->tracker.com.coord);

			// extract a feature from what's been marked as the target
			if (ctx.frame.wait <= 0 && x_t->tracker.com.points < 200 && targ_com_dist < 3)
			{
				frame_copy_uint8(
					FEAT_SIZE, FEAT_SIZE,
					x_t->feature,
					DS_H, DS_W,
					x_t->frame,
					(win_t){ x_t->tracker.com.coord[1] - FEAT_SIZE_H, x_t->tracker.com.coord[0] - FEAT_SIZE_H }
				);
				x_t->feature_set = true;
			}

		}
		else
		{
			x_t->tracker.target.delta = (vec2_t){ 0, 0 };
		}

		// match_t match = {
		// 	.score = -1,	
		// };

		if (x_t->feature_set)
		{
			x_t->match = match_feature(
				(point_t){ DS_H, DS_W }, x_t->frame,
				(point_t){ FEAT_SIZE, FEAT_SIZE }, x_t->feature,
				(win_t) {{}, { DS_H, DS_W }}
			);

			int dr = x_t->match.r - x_t_1->match.r, dc = x_t->match.c - x_t_1->match.c;
			if (sqrtf(dr * dr + dc * dc) > 10)
			{
				x_t->match.score = -1;	
				x_t->feature_set = false;
			}

			// last_match = match;
		}

		int yaw = -((x_t->match.c + FEAT_SIZE_H) +  - (DS_W >> 1)) * ctx.cal.steps_per_col;
		int pitch = -((x_t->match.r + FEAT_SIZE_H) - (DS_H >> 1)) * ctx.cal.steps_per_row;
		int disp_yaw = yaw, disp_pitch = pitch;

		float error = sqrt(yaw * yaw + pitch * pitch);
		float delta = vec2_len(x_t->tracker.target.delta);

		if (ctx.frame.wait <= 0 && error > 0 && delta < 2 && x_t->feature_set)
		{
			do
			{
				int yaw_ticks, pitch_ticks;
				if (motors_enabled) 
				{
					motor_control(yaw, pitch, &yaw_ticks, &pitch_ticks);
				}

				ctx.frame.wait += sqrt(pitch_ticks * pitch_ticks + yaw_ticks * yaw_ticks) * ctx.cal.frames_per_step * 1;
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
				x_t->tracker, 
				x_t->match, 
				yaw, pitch, 
				ctx.frame.count, 
				ctx.frame.wait, 
				display
			);
		}

		if (ctx.frame.wait > 0) { ctx.frame.wait--; }
		ctx.frame.count++;
		// memcpy(ds_last_frame, ds_frame, sizeof(ds_last_frame));
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
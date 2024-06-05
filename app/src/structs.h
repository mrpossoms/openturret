#ifndef _OT_STRUCTS_H_
#define _OT_STRUCTS_H_

#include <inttypes.h>
#include <linux/types.h>
#include <stdlib.h>
#include <stdbool.h>

#define W (640)
#define H (480)
#define DS_W (128)
#define DS_H (32)
#define FEAT_SIZE 9
#define FEAT_SIZE_H ((FEAT_SIZE - 1) >> 1)
#define T_LEN (3)

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
	int r, c;
	float score;
} match_t;

typedef struct {
	size_t r, c;
} point_t;

typedef struct {
	point_t corner, size;
} win_t;

typedef struct {
	float steps_per_row;
	float steps_per_col;
	float frames_per_step;
} cal_t;

// typedef union {
// 	float v[2];
// 	struct {
// 		float x;
// 		float y;
// 	}
// } vec2_t;

typedef float vec2_t __attribute__ ((vector_size (8)));

typedef struct {
	struct {
		vec2_t coord;
		vec2_t delta;
	} target;

	struct {
		vec2_t coord;
		unsigned points;
	} com;
} tracking_t;

typedef struct {
	uint8_t frame[DS_H][DS_W];
	uint8_t feature[FEAT_SIZE][FEAT_SIZE];
	match_t match;
	tracking_t tracker;
	bool feature_set;
} state_t;

typedef struct {
	state_t states[T_LEN];

	struct {
		int wait;
		int count;
	} frame;

	cal_t cal;
} context_t;

#endif

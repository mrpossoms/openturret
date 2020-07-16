#ifndef _OT_STRUCTS_H_
#define _OT_STRUCTS_H_

#include <inttypes.h>
#include <linux/types.h>
#include <stdlib.h>

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


#endif

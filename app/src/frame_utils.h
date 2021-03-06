#ifndef _OT_FRAME_UTILS_H_
#define _OT_FRAME_UTILS_H_
#include "structs.h"

typedef void (*mapping_t)(point_t dims, uint8_t from[dims.r][dims.c], uint8_t to[dims.r][dims.c]);

void frame_mapping_1to1(point_t dims, uint8_t from[dims.r][dims.c], uint8_t to[dims.r][dims.c])
{
	for (size_t r = 0; r < dims.r; r++)
	for (size_t c = 0; c < dims.c; c++)
	{
		to[r][c] = from [r][c];
	}
}

float frame_difference(point_t dims, pixel_t f0[dims.r][dims.c], pixel_t f1[dims.r][dims.c])
{
	float diff = 0;	

	for (size_t ri = 0; ri < dims.r; ri++)
	for (size_t ci = 0; ci < dims.c; ci++)
	{
		diff += fabsf(f0[ri][ci].Y - f1[ri][ci].Y);
	}

	return diff / (dims.r * dims.c);
}

void frame_copy_pixel(
	size_t dr, const size_t dc,
	pixel_t dest[dr][dc],
	size_t sr, const size_t sc,
	pixel_t src[sr][sc],
	win_t src_win)
{
	for (;dr--;)
	{
		memcpy(dest[dr], src[dr + src_win.corner.r] + src_win.corner.c, sizeof(pixel_t) * dc);
	}
}

void frame_copy_uint8(
	size_t dr, const size_t dc,
	uint8_t dest[dr][dc],
	size_t sr, const size_t sc,
	uint8_t src[sr][sc],
	win_t src_win)
{
	for (;dr--;)
	{
		memcpy(dest[dr], src[dr + src_win.corner.r] + src_win.corner.c, sizeof(uint8_t) * dc);
	}
}

void frame_downsample_uint8(
	point_t dst_size,
	uint8_t dst[dst_size.r][dst_size.c],
	point_t src_size,
	pixel_t src[src_size.r][src_size.c])
{
	assert(dst_size.r < src_size.r);
	assert(dst_size.c < src_size.c);

	size_t src_r_per_dst_r = src_size.r / dst_size.r;
	size_t src_c_per_dst_c = src_size.c / dst_size.c;

	for (int r = 0; r < dst_size.r; r++)
	for (int c = 0; c < dst_size.c; c++)
	{ // for each row and col of the down sampled frame
		size_t sr = r * src_r_per_dst_r;
		size_t sc = c * src_c_per_dst_c;
		int avg_y = 0;

		for (int ri = sr; ri < sr + src_r_per_dst_r; ri++)
		for (int ci = sc; ci < sc + src_c_per_dst_c; ci++)
		{ // for each patch of the src frame
			avg_y += src[ri][ci].Y;
		}

		dst[r][c] = avg_y / (src_c_per_dst_c * src_r_per_dst_r);
	}
}


#endif

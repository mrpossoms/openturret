#ifndef _OT_FRAME_UTILS_H_
#define _OT_FRAME_UTILS_H_


float frame_difference(const size_t r, const size_t c, pixel_t f0[r][c], pixel_t f1[r][c])
{
	float diff = 0;	

	for (size_t ri = 0; ri < r; ri++)
	for (size_t ci = 0; ci < c; ci++)
	{
		diff += fabsf(f0[ri][ci].Y - f1[ri][ci].Y);
	}

	return diff / (r * c);
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


#endif
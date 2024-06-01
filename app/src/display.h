#ifndef _OT_DISPLAY_H_
#define _OT_DISPLAY_H_
#include <stdint.h>
#include "structs.h"

void display_init();

void display_frame_to_chars(size_t rows, size_t cols, uint8_t frame[rows][cols], char disp[rows][cols]);

void display_debug_info_to_chars(
	size_t rows, size_t cols, 
	tracking_t track, 
	match_t match, 
	int yaw, int pitch, 
	int frame_count,
	int frame_wait,
	char disp[rows][cols]
);

void display_deinit();

#endif

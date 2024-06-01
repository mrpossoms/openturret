#ifndef _OT_UTILS_H_
#define _OT_UTILS_H_
#include <math.h>

#include "structs.h"

static inline int min(int a, int b)
{
	if (a < b) { return a; }
	return b; 
}


static inline int max(int a, int b)
{
	if (a > b) { return a; }
	return b; 
}


static inline int clamp(int x, int lo, int hi)
{
	return min(hi, max(lo, x));
}

static inline float vec2_len(vec2_t v)
{
	return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

#endif
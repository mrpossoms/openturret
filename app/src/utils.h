#ifndef _OT_UTILS_H_
#define _OT_UTILS_H_

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


#endif
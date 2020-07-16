#ifndef _OT_FEATURE_MATCHER_H_
#define _OT_FEATURE_MATCHER_H_

#include "structs.h"

match_t match_feature(const point_t frame_size,
                      uint8_t frame[frame_size.r][frame_size.c],
		      const point_t feat_size,
                      uint8_t feature[feat_size.r][feat_size.c],
                      win_t search_win);


#endif

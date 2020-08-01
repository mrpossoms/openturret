#include "feature_matcher.h"
#include <math.h>

match_t match_feature(const point_t frame_size,
                      uint8_t frame[frame_size.r][frame_size.c],
		      const point_t feat_size,
                      uint8_t feature[feat_size.r][feat_size.c],
                      win_t search_win)
{
	match_t match = { .score = 256 * feat_size.r * feat_size.c };

	int feat_hr = (feat_size.r - 1) / 2; 
	int feat_hc = (feat_size.c - 1) / 2; 
	for (int r = feat_hr + 1; r < frame_size.r - (feat_hr + 1); r++)
	for (int c = feat_hc + 1; c < frame_size.c - (feat_hc + 1); c++)
	{
		float score = 0;
		for (int i = -feat_hr; i <= feat_hr; i++)
		for (int j = -feat_hc; j <= feat_hc; j++)
		{
			float k_score = 100;
			for (int k = -1; k <= 1; k++)
			for (int l = -1; l <= 1; l++)
			{
				float s = fabsf(feature[i + feat_hr][j + feat_hc] - frame[r + i][c + j]) / 255.f;
				if (s < k_score) { k_score = s; }
			}
			score += k_score;
		}
		score /= (feat_size.r * feat_size.c);

		if (score < match.score)
		{
			match.r = r;
			match.c = c;
			match.score = score;
		}
	}

	return match;
}

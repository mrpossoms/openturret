
#include <stdio.h>
#include <termios.h>
#include <term.h>
#include <unistd.h>
#include "display.h"
#include "utils.h"

#define FEAT_SIZE 9
#define FEAT_SIZE_H ((FEAT_SIZE - 1) >> 1)

struct termios G_TERM_OLD_SETTINGS;

void display_init()
{
	struct termios newt;

	// disable echo from stdin
	tcgetattr(STDIN_FILENO, &G_TERM_OLD_SETTINGS);
	newt = G_TERM_OLD_SETTINGS;
	newt.c_lflag &= ~ECHO;
	newt.c_lflag &= ~ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	// hide cursor
	fputs("\033[?25l", stderr);	
}

void display_frame_to_chars(size_t rows, size_t cols, uint8_t frame[rows][cols], char disp[rows][cols])
{
	const char spectrum[] = " .,':;|[{+*x88ASDDBDFSDFSDBHGJFU";

	for (int r = 0; r < rows; r++)
	for (int c = 0; c < cols; c++)
	{
		disp[r][c] = spectrum[frame[r][c] / 23];
	}
}

void display_debug_info_to_chars(
	size_t rows, size_t cols, 
	tracking_t track,
	match_t match,
	int yaw, int pitch,
	int frame_count,
	int frame_wait,
	char disp[rows][cols])
{
	static time_t last_sec;
	static int rows_drawn, last_sec_frames;

	int fps = 0;

	time_t now = time(NULL);

	for (int i = 0; i < 2; i++)
	for (int j = 0; j < 2; j++)
	{
		disp[(int)track.target.coord[1]+j][(int)track.target.coord[0]+i] = '#';
		disp[rows >> 1][cols >> 1] = '+';
		disp[(int)track.com.coord[1]+j][(int)track.com.coord[0]+i] = '?';
	}


	if (match.score >= 0)
	for (int i = 0; i < FEAT_SIZE; i++)
	for (int j = 0; j < FEAT_SIZE_H; j++)
	{
		disp[match.r+j][match.c+i] = '@';
	}

	if (rows_drawn > 0)
	{ // Erase the previously drawn rows
		static char move_up[16] = {};
		sprintf(move_up, "\033[%dA", rows_drawn);
		fprintf(stderr, "%s", move_up);
		rows_drawn = 0;
	}

	if (now > last_sec)
	{
		fps = frame_count - last_sec_frames;
		last_sec_frames = frame_count;
		last_sec = now;
	}

	// disp
	if (frame_wait > 0) { fprintf(stderr, "\033[31m"); }
	const int sf = 1; // scale factor
	for (int r = 0; r < rows / sf; r++)
	{
		for (int c = 0; c < cols / (sf); c++)
		{
			fputc(disp[r * sf][c * sf], stderr);
		}
		fputc('|', stderr);
		fputc('\n', stderr);
		rows_drawn += 1;
	}
	fprintf(stderr, "\033[0m");

	float error = sqrt(yaw * yaw + pitch * pitch);

	printf("COM points: %d error %0.3f frame_wait: %d   \n", track.com.points, error, frame_wait);
	printf("COM (%d, %d) targ∆: %0.3f, match_score: %0.3f \n", (int)track.target.coord[0], (int)track.target.coord[1], vec2_len(track.target.delta), match.score);
	printf("yaw, pitch: (%d, %d) frames: %d fps: %d \n", yaw, pitch, frame_count, fps);
	rows_drawn+=3;
}

void display_deinit()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &G_TERM_OLD_SETTINGS);
	fputs("\033[?25h", stderr);
}


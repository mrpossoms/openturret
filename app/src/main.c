#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "vidi.h"

#define CAM_FOV 30
#define DEG_ROW 1
#define DEG_COL 1
#define DEG_PITCH_STEP 1
#define DEG_YAW_STEP 1

static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 1000;

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


static void pabort(const char *s)
{
	perror(s);
	abort();
}


void config_spi(int fd)
{
	/*
	 * spi mode
	 */
	int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");
}


static void transfer_spi(int fd, uint8_t ctrl)
{
	int ret;
	
	uint8_t tx[1] = { ctrl };
	uint8_t rx[1] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = 0,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}


int min(int a, int b)
{
	if (a < b) { return a; }
	return b; 
}


int max(int a, int b)
{
	if (a > b) { return a; }
	return b; 
}


int clamp(int x, int lo, int hi)
{
	return min(hi, max(lo, x));
}

int main (int argc, const char* argv[])
{
	// define a configuration object for a camera, here
	// you can request frame size, pixel format, frame rate
	// and the camera which you wish to use.
	vidi_cfg_t cam = {
		.width = 640,
		.height = 480,
		.frames_per_sec = 60,
		.path = argv[1],
		.pixel_format = V4L2_PIX_FMT_YUYV
	};

	// vidi_config is used to open and configure the camera
	// device, but can also be used to reconfigure an
	// existing vidi_cfg_t camera instance.
	assert(0 == vidi_config(&cam));

	int spi_fd = open("/dev/spidev0.0", O_RDWR);

	if (spi_fd >= 0)
	{
		config_spi(spi_fd);
	}

	pixel_t last_frame[480][640] = {};
	pixel_t frame[480][640] = {};
	char display[32][128] = {};

	int rows_drawn = 0;
	float targ_x = 64;
	float targ_y = 16;
	int frame_wait = 4;
	int frame_count = 0;

	fputs("\033[?25l", stderr);

	// request the camera to capture a frame
	vidi_request_frame(&cam);

	while (1)
	{
		// this function blocks until a frame pointer is returned
		uint8_t* raw_frame = vidi_wait_frame(&cam);
		size_t row_size = vidi_row_bytes(&cam);

		// request the camera to capture a frame
		vidi_request_frame(&cam);

		for (int r = 0; r < 480; r++)
		{
			memcpy(frame[r], raw_frame + (r * row_size), row_size);
		}

		const char spectrum[] = "  .,':;|[{+*x88";

		// process
		//float com_x = 64, com_y = 16;
		//int com_points = 1;
		float com_x = 0, com_y = 0;
		int com_points = 0;
		const int dr = 480 / 32;
		const int dc = 640 / 128;

		if (frame_count > 0)
		for (int r = 0; r < 32; r++)
		{
			for (int c = 0; c < 128; c++)
			{
				int ri = r * dr, ci = c * dc;
				int last_grey = last_frame[ri][ci].Y;//(last_frame[ri][ci].r + last_frame[ri][ci].g + last_frame[ri][ci].b) / 3;
				int grey = frame[ri][ci].Y;
				int delta = abs(grey-last_grey);

				int delta_idx = max(delta - 8, 0) / 18;
				int idx = grey / 18;
				display[r][c] = spectrum[delta_idx];//spectrum[max(0, idx - delta_idx)];;

				if (frame_wait <= 0)
				if (delta_idx >= 2)
				{
					com_x += c;
					com_y += r;
					com_points++;
				}
			}
		}

		if (com_points > 0 && frame_wait <= 0)
		{
			float x = com_x / com_points;
			float y = com_y / com_points;

			const float power = 5;
			targ_x = (x + targ_x * (power - 1.f)) / power;
			targ_y = (y + targ_y * (power - 1.f)) / power;			
		}

		display[(int)targ_y][(int)targ_x] = '#';
		display[16][64] = '+';

		{ // Erase the previously drawn rows
			static char move_up[16] = {};
			sprintf(move_up, "\033[%dA", rows_drawn);
			fprintf(stderr, "%s", move_up);
			rows_drawn = 0;
		}

		int yaw = -(targ_x - 64) / 10;
		int pitch = -(targ_y - 16) / 5;
		//if (com_points > 0 && frame_wait <= 0)
		if (frame_wait <= 0)
		{
			if (yaw && pitch)
			{
				targ_x = 64;
				targ_y = 16;
			}

			do
			{
				uint8_t ctrl_byte = 0;

				if (yaw < 0) { ctrl_byte |= 0x80; }
				int yaw_ticks = clamp(yaw, -7, 7);

				if (pitch < 0) { ctrl_byte |= 0x08; }
				int pitch_ticks = clamp(pitch, -7, 7);

				ctrl_byte |= ((abs(yaw_ticks) << 4) | abs(pitch_ticks));

				transfer_spi(spi_fd, ctrl_byte);

				frame_wait += max(pitch_ticks, yaw_ticks) * 5;
				yaw -= yaw_ticks;
				pitch -= pitch_ticks;

			}
			while(yaw != 0 || pitch != 0);
		}

		// display
		for (int r = 0; r < 32; r++)
		{
			for (int c = 0; c < 128; c++)
			{
				fputc(display[r][c], stderr);
			}
			fputc('|', stderr);
			fputc('\n', stderr);
			rows_drawn += 1;
		}

		printf("COM points: %d   \n", com_points);
		printf("COM (%d, %d)\n", (int)targ_x, (int)targ_y);
		printf("yaw, pitch: (%d, %d) frames: %d\n", yaw, pitch, frame_count);
		rows_drawn+=3;

		frame_wait--;
		frame_count++;
		memcpy(last_frame, frame, sizeof(frame));
	}

	return 0;
}

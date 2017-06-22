/*
 * ilda.c
 */

#include <ilda.h>

enum ilda_state_t {
	ILDA_STATE_MAGIC				= 0,
	ILDA_STATE_RESERVED1			= 1,
	ILDA_STATE_FORMAT				= 2,
	ILDA_STATE_NAME					= 3,
	ILDA_STATE_COMPANY				= 4,
	ILDA_STATE_COUNT				= 5,
	ILDA_STATE_FRAME_NUM			= 6,
	ILDA_STATE_FRAME_CNT			= 7,
	ILDA_STATE_SCANNER				= 8,
	ILDA_STATE_RESERVED2			= 9,
	ILDA_STATE_RECORD				= 10,
};

enum ilda_coord_format_t {
	ILDA_COORD_FORMAT_3D_INDEX		= 0,
	ILDA_COORD_FORMAT_2D_INDEX		= 1,
	ILDA_COORD_FORMAT_PALETTE_COLOR	= 2,
	ILDA_COORD_FORMAT_3D_TRUE_COLOR	= 4,
	ILDA_COORD_FORMAT_2D_TRUE_COLOR	= 5,
};

struct ilda_ctx_t {
	enum ilda_state_t state;
	char buf[32];
	int index;
	enum ilda_coord_format_t format;
	int count;
	int fnum;
	int fcnt;
};

struct ilda_point_t {
	float x, y, z;
	float r, g, b;
	int index;
	int last;
	int blank;
};

static inline unsigned short value_of_2bytes(char * buf)
{
	return (unsigned short)((buf[0] << 8) | buf[1]);
}

static inline void ilda_push_byte(struct ilda_ctx_t * ictx, struct galvo_ctx_t * gctx, char byte)
{
	struct ilda_point_t point;

	switch(ictx->state)
	{
	case ILDA_STATE_MAGIC:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 4)
		{
			if(ictx->buf[0] == 'I' && ictx->buf[1] == 'L' && ictx->buf[2] == 'D' && ictx->buf[3] == 'A')
			{
				ictx->state = ILDA_STATE_RESERVED1;
				ictx->index = 0;
			}
			else
			{
				ictx->state = ILDA_STATE_MAGIC;
				ictx->index = 0;
			}
		}
		break;

	case ILDA_STATE_RESERVED1:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 3)
		{
			ictx->state = ILDA_STATE_FORMAT;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_FORMAT:
		ictx->format = (enum ilda_coord_format_t)byte;
		ictx->state = ILDA_STATE_NAME;
		ictx->index = 0;
		break;

	case ILDA_STATE_NAME:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 8)
		{
			ictx->state = ILDA_STATE_COMPANY;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_COMPANY:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 8)
		{
			ictx->state = ILDA_STATE_COUNT;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_COUNT:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 2)
		{
			ictx->count = (int)(value_of_2bytes(&ictx->buf[0]));
			ictx->state = ILDA_STATE_FRAME_NUM;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_NUM:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 2)
		{
			ictx->fnum = (int)(value_of_2bytes(&ictx->buf[0]));
			ictx->state = ILDA_STATE_FRAME_CNT;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_CNT:
		ictx->buf[ictx->index++] = byte;
		if(ictx->index >= 2)
		{
			ictx->fcnt = (int)(value_of_2bytes(&ictx->buf[0]));
			ictx->state = ILDA_STATE_SCANNER;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_SCANNER:
		ictx->state = ILDA_STATE_RESERVED2;
		ictx->index = 0;
		break;

	case ILDA_STATE_RESERVED2:
		if(ictx->count > 0)
		{
			ictx->state = ILDA_STATE_RECORD;
			ictx->index = 0;
		}
		else
		{
			ictx->state = ILDA_STATE_MAGIC;
			ictx->index = 0;
		}
		break;

	case ILDA_STATE_RECORD:
		ictx->buf[ictx->index++] = byte;
		switch(ictx->format)
		{
		case ILDA_COORD_FORMAT_3D_INDEX:
			if(ictx->index >= 8)
			{
				point.x = (signed short)(value_of_2bytes(&ictx->buf[0])) / 32768.0f;
				point.y = (signed short)(value_of_2bytes(&ictx->buf[2])) / 32768.0f;
				point.z = (signed short)(value_of_2bytes(&ictx->buf[4])) / 32768.0f;
				if(ictx->buf[6] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ictx->buf[6] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.index = ictx->buf[7];

/*				if(point.blank)
					galvo_set_color(gctx, 0, 0xffff, 0);
				else
					galvo_set_color(gctx, 0xffff, 0, 0);
				point.x *= 0.9;
				point.y *= 0.9;
				galvo_set_xy(gctx, point.x * 320 + 320, point.y * 240 + 240);
				galvo_refresh(gctx);
				printf("[%d] %f %f %f, %d, %d, %d\r\n", ictx->format, point.x, point.y, point.z, point.index, point.last, point.blank);*/

				ictx->state = ILDA_STATE_RECORD;
				ictx->index = 0;
				ictx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_2D_INDEX:
			if(ictx->index >= 6)
			{
				point.x = (signed short)(value_of_2bytes(&ictx->buf[0])) / 32768.0f;
				point.y = (signed short)(value_of_2bytes(&ictx->buf[2])) / 32768.0f;
				point.z = 0;
				if(ictx->buf[4] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ictx->buf[4] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.index = ictx->buf[5];

				ictx->state = ILDA_STATE_RECORD;
				ictx->index = 0;
				ictx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_PALETTE_COLOR:
			break;

		case ILDA_COORD_FORMAT_3D_TRUE_COLOR:
			if(ictx->index >= 10)
			{
				point.x = (signed short)(value_of_2bytes(&ictx->buf[0])) / 32768.0f;
				point.y = (signed short)(value_of_2bytes(&ictx->buf[2])) / 32768.0f;
				point.z = (signed short)(value_of_2bytes(&ictx->buf[4])) / 32768.0f;
				if(ictx->buf[6] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ictx->buf[6] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.b = ictx->buf[7] / 255.f;
				point.g = ictx->buf[8] / 255.f;
				point.r = ictx->buf[9] / 255.f;

				ictx->state = ILDA_STATE_RECORD;
				ictx->index = 0;
				ictx->count--;
			}
			break;

		case ILDA_COORD_FORMAT_2D_TRUE_COLOR:
			if(ictx->index >= 8)
			{
				point.x = (signed short)(value_of_2bytes(&ictx->buf[0])) / 32768.0f;
				point.y = (signed short)(value_of_2bytes(&ictx->buf[2])) / 32768.0f;
				point.z = 0;
				if(ictx->buf[4] && (1 << 7))
					point.last = 1;
				else
					point.last = 0;
				if(ictx->buf[4] && (1 << 6))
					point.blank = 1;
				else
					point.blank = 0;
				point.b = ictx->buf[5] / 255.f;
				point.g = ictx->buf[6] / 255.f;
				point.r = ictx->buf[7] / 255.f;

				ictx->state = ILDA_STATE_RECORD;
				ictx->index = 0;
				ictx->count--;
			}
			break;

		default:
			break;
		}

		if(ictx->count <= 0)
		{
			ictx->state = ILDA_STATE_MAGIC;
			ictx->index = 0;
		}
		break;

	default:
		ictx->state = ILDA_STATE_MAGIC;
		ictx->index = 0;
		break;
	}
}

void ilda_load_file(struct galvo_ctx_t * gctx, const char * file)
{
	struct ilda_ctx_t ictx;
	char buf[512];
	int fd, n, i;

	if(!gctx || !file)
		return;

	memset(&ictx, 0, sizeof(struct ilda_ctx_t));
	ictx.state = ILDA_STATE_MAGIC;
	ictx.index = 0;

	fd = open(file, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return;

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		for(i = 0; i < n; i++)
		{
			ilda_push_byte(&ictx, gctx, buf[i]);
		}
	}

	close(fd);
}

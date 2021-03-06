#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sandbox.h>

int sandbox_file_open(const char * path)
{
	int fd = open(path, O_RDONLY, (S_IRUSR | S_IRGRP | S_IROTH));
	return fd < 0 ? 0 : fd;
}

int sandbox_file_close(int fd)
{
	int ret = close(fd);
	return ret < 0 ? 0 : 1;
}

int sandbox_file_exist(const char * path)
{
	if(access(path, F_OK) == 0)
		return 0;
	return -1;
}

ssize_t sandbox_file_read(int fd, void * buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

ssize_t sandbox_file_read_nonblock(int fd, void * buf, size_t count)
{
	ssize_t ret;
	int flags;

	flags = fcntl(fd, F_GETFL);
	if(flags == -1)
		return 0;

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return 0;

	ret = sandbox_file_read(fd, buf, count);

	if(fcntl(fd, F_SETFL, flags) == -1)
		return 0;

	return ret;
}

ssize_t sandbox_file_write(int fd, const void * buf, size_t count)
{
	ssize_t ret = write(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

uint64_t sandbox_file_seek(int fd, uint64_t offset)
{
	return (uint64_t)lseek(fd, offset, SEEK_SET);
}

uint64_t sandbox_file_length(int fd)
{
	off_t off, ret;
	off = lseek(fd, 0, SEEK_CUR);
	ret = lseek(fd, 0, SEEK_END);
	lseek(fd, off, SEEK_SET);
	return (ret > 0) ? ret: 0;
}

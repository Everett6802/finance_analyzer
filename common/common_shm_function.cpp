#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "common_function.h"
#include "msg_dumper_wrapper.h"


static const char* SHM_ROOT_FILEPATH = "/dev/shm";
static const int MAX_FILEPATH_SIZE = 256;

#define GM_WRITE_LOG_BEGIN(format, ...)\
do{\
	static const int ERRMSG_BUF_SIZE = 256;\
	char errmsg_buf[ERRMSG_BUF_SIZE];\
	snprintf(errmsg_buf, ERRMSG_BUF_SIZE, format, __VA_ARGS__);

#define GM_WRITE_LOG_END()\
}while(0);

#define GM_WRITE_ERROR(format, ...)\
GM_WRITE_LOG_BEGIN(format, __VA_ARGS__)\
STATIC_WRITE_ERROR(errmsg_buf);\
GM_WRITE_LOG_END()

#define GM_WRITE_WARN(format, ...)\
GM_WRITE_LOG_BEGIN(format, __VA_ARGS__)\
STATIC_WRITE_WARN(errmsg_buf);\
GM_WRITE_LOG_END()

static void format_full_shm_filepath(const char* shm_filepath, char* full_filepath, int full_filepath_buf_size)
{
	snprintf(full_filepath, full_filepath_buf_size, "%s%s", SHM_ROOT_FILEPATH, shm_filepath);
}

// unsigned short open_shm_file(const char* shm_filepath, int flags, mode_t mode=0666)
// {
// 	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
// 	char full_filepath[MAX_FILEPATH_SIZE];
// 	format_full_shm_filepath(shm_filepath,full_filepath, MAX_FILEPATH_SIZE);
// 	int fd = open(full_filepath, flags, mode);
// }

// unsigned short unlink_shm_file(const char* shm_filepath)
// {
// 	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
// 	char full_filepath[MAX_FILEPATH_SIZE];
// 	format_full_shm_filepath(shm_filepath,full_filepath, MAX_FILEPATH_SIZE);
// }

unsigned short gm_add_data(const char* shm_filepath, const void* data, int data_size)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(data != NULL && "data should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
	unsigned short ret = RET_SUCCESS;
	char* mmap_ptr = NULL;
// If O_CREAT and O_EXCL are set, open() shall fail if the file exists
// Get the find descriptor
	int fd = open(full_filepath, O_RDWR | O_CREAT | O_EXCL, 0666);
	if (fd == -1)
	{
		GM_WRITE_ERROR("open() fails in file: %s, due to: %s", full_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Set the file size
    if(ftruncate(fd, sizeof(char) * data_size) == -1) 
    {
		GM_WRITE_ERROR("ftruncate() fails in file: %s, due to: %s", full_filepath, strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
    }
// Map to memory
	mmap_ptr = (char*)mmap(NULL, data_size, PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (mmap_ptr == MAP_FAILED)
	{
		GM_WRITE_ERROR("mmap() fails, due to: %s", strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
// Write the data into the memory
	memcpy(mmap_ptr, data, sizeof(char) * data_size);
	munmap(mmap_ptr, data_size);
OUT:
	if (fd != -1)
		close(fd);
	return ret;
}

unsigned short gm_delete_data(const char* shm_filepath)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
// Remove the file in the /dev/shm
	if (unlink(full_filepath) == ENOENT)
	{
		GM_WRITE_WARN("The SHM file[%s] does NOT exist", full_filepath);
	}
	return RET_SUCCESS;
}

unsigned short gm_read_data(const char* shm_filepath, void** data, int& data_size)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(data != NULL && "data should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
// If the specified file does not exist, it may optionally 
// (if O_CREAT is specified in flags) be created by open()
// Get the file descriptor
	int fd = open(full_filepath, O_RDONLY, 0666);
	if (fd == -1)
	{
		GM_WRITE_ERROR("Fail to open the SHM file[%s], due to: %s", full_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Count the file size
	struct stat buf;
	if (fstat(fd, &buf) == -1)
	{
		GM_WRITE_ERROR("fstat() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;	
	}
	data_size = buf.st_size;
// Map to memory
	char* mmap_ptr = (char*)mmap(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
	if (mmap_ptr == MAP_FAILED)
	{
		GM_WRITE_ERROR("mmap() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	char *data_tmp = new char[data_size];
	if (data_tmp == NULL)
	{
		STATIC_WRITE_ERROR("Fails to allocate memory: data_tmp");
		return RET_FAILURE_INSUFFICIENT_MEMORY;
	}
	memcpy(data_tmp, mmap_ptr, sizeof(char) * data_size);
	munmap(mmap_ptr, data_size);

	*data = data_tmp;
	return RET_SUCCESS;
}

unsigned short gm_write_data(const char* shm_filepath, void* data, int data_size)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(data != NULL && "data should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
// If the specified file does not exist, it may optionally 
// (if O_CREAT is specified in flags) be created by open()
// Get the file descriptor
	int fd = open(full_filepath, O_RDWR, 0666);
	if (fd == -1)
	{
		GM_WRITE_ERROR("Fail to open the SHM file[%s], due to: %s", full_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Count the file size
	struct stat buf;
	if (fstat(fd, &buf) == -1)
	{
		GM_WRITE_ERROR("fstat() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;	
	}
	assert (data_size >= 0 && data_size <= buf.st_size && "data_size should NOT be Out-of-Range");
	// data_size = buf.st_size;
// Map to memory
	char* mmap_ptr = (char*)mmap(NULL, data_size, PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (mmap_ptr == MAP_FAILED)
	{
		GM_WRITE_ERROR("mmap() fails, due to: %s", strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
	memcpy(mmap_ptr, data, sizeof(char) * data_size);
	munmap(mmap_ptr, data_size);
	return RET_SUCCESS;
}

unsigned short gm_get_data_ptr(const char* shm_filepath, void** data, int& data_size)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(data != NULL && "data should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
	unsigned short ret = RET_SUCCESS;
	char* mmap_ptr = NULL;
// If the specified file does not exist, it may optionally 
// (if O_CREAT is specified in flags) be created by open()
// Get the file descriptor
	int fd = open(full_filepath, O_RDWR, 0666);
	if (fd == -1)
	{
		GM_WRITE_ERROR("Fail to open the SHM file[%s], due to: %s", full_filepath, strerror(errno));
		return RET_FAILURE_SYSTEM_API;
	}
// Count the file size
	struct stat buf;
	if (fstat(fd, &buf) == -1)
	{
		GM_WRITE_ERROR("fstat() fails, due to: %s", strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	data_size = buf.st_size;
// Map to memory
	mmap_ptr = (char*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mmap_ptr == MAP_FAILED)
	{
		GM_WRITE_ERROR("mmap() fails, due to: %s", strerror(errno));
		ret = RET_FAILURE_SYSTEM_API;
		goto OUT;
	}
	*data = mmap_ptr;
OUT:
	close(fd);
	return ret;
}

unsigned short gm_put_data_ptr(void* data, int data_size)
{
	assert(data != NULL && "data should NOT be NULL");
	munmap(data, data_size);
	return RET_SUCCESS;
}

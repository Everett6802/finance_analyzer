#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include "common_function.h"
#include "msg_dumper_wrapper.h"


using namespace std;

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
	assert(shm_filepath != NULL && "shm_filepath shuold NOT be NULL");
	if (strncmp(shm_filepath, SHM_ROOT_FILEPATH, SHM_ROOT_FILEPATH_LEN) != 0)
	{
// Should be the relative file path
		if (shm_filepath[0] == '/')
		{
			GM_WRITE_ERROR("Incorect shm file path: %s", shm_filepath);
			assert(0 && "Incorect shm file path");
		}
// The filepath does NOT start with '/dev/shm'
// 		int filepath_left_len = strlen(filepath_buf) + 1 - (SHM_ROOT_FILEPATH_LEN + 1);
// 		memmove(filepath_buf, filepath_buf + SHM_ROOT_FILEPATH_LEN + 1, filepath_left_len);
		snprintf(full_filepath, full_filepath_buf_size, "%s/%s", SHM_ROOT_FILEPATH, shm_filepath);		
	}
	else
		strcpy(full_filepath, shm_filepath);
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

unsigned short read_shm_file_lines_ex(std::list<std::string>& line_list, const char* shm_filepath, const char* file_read_attribute, PTIME_RANGE_PARAM time_range_param, char data_seperate_character)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(file_read_attribute != NULL && "file_read_attribute should NOT be NULL");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
	if (!check_file_exist(shm_filepath))
	{
		STATIC_WRITE_FORMAT_ERROR("The shm file[%s] does NOT exist", shm_filepath);
		return RET_FAILURE_NOT_FOUND;		
	}

	unsigned short ret = RET_SUCCESS;
	char *data = NULL;
	int data_size = 0;
	ret = gm_get_data_ptr(full_filepath, (void**)&data, data_size);
	if (CHECK_FAILURE(ret))
		return ret;
	char* data_duplica = strdup(data);
	char* line = NULL;
	while ((line = strsep(&data_duplica, "\n")) != NULL)
	{
		if (strlen(line) == 0 || line[0] == '#')
			continue;
// CAUTION: To check the time range, I assume the time field index is 0 
		if (time_range_param != NULL)
		{
			static const int TIME_BUF_SIZE = 16;
			static char time_buf[TIME_BUF_SIZE];
			char* pos = strchr(line, data_seperate_character);
			if (pos == NULL)
			{
				GM_WRITE_ERROR("Incorrect data format in file[%s]: %s", full_filepath, line);
				ret = RET_FAILURE_INCORRECT_FORMAT;
				break;
			}
			int pos_index = pos - line;
			if (pos_index >= TIME_BUF_SIZE)
			{
				GM_WRITE_ERROR("Incorrect time format in file[%s], time string length: %d", full_filepath, pos_index);
				ret = RET_FAILURE_INCORRECT_FORMAT;
				break;
			}
			memset(time_buf, 0x0, sizeof(char) * TIME_BUF_SIZE);
			memcpy(time_buf, line, sizeof(char) * pos_index);
			TimeInRangeType time_in_range_type = TimeRangeParam::time_in_range_type(time_range_param, time_buf);
			if (time_in_range_type == TIME_BEFORE_RANGE)
				continue;
			else if (time_in_range_type == TIME_AFTER_RANGE)
				break;
		}
		string line_str(line);
		line_list.push_back(line_str);
	}

	if (data_duplica != NULL)
	{
		free(data_duplica);
		data_duplica = NULL;
	}
	gm_put_data_ptr(data, data_size);
	return ret;
}

unsigned short write_shm_file_lines_ex(const std::list<std::string>& line_list, const char* shm_filepath, const char* file_write_attribute, PTIME_RANGE_PARAM time_range_param, char data_seperate_character)
{
	assert(shm_filepath != NULL && "shm_filepath should NOT be NULL");
	assert(file_write_attribute != NULL && "file_write_attribute should NOT be NULL");
	static string new_line("\n");
	char full_filepath[MAX_FILEPATH_SIZE];
// Find the file full path in the /dev/shm
	format_full_shm_filepath(shm_filepath, full_filepath, MAX_FILEPATH_SIZE);
	if (check_file_exist(shm_filepath))
	{
		GM_WRITE_ERROR("The shm file[%s] already exist", shm_filepath);
		return RET_FAILURE_NOT_FOUND;		
	}

	unsigned short ret = RET_SUCCESS;
	static const int BUF_SIZE = 512;
	char line_buf[BUF_SIZE];
	list<string>::const_iterator iter = line_list.begin();
	string total_string;
// Write into file line by line
	while (iter != line_list.end())
	{
		string line = (string)*iter;
// CAUTION: To check the time range, I assume the time field index is 0 
		if (time_range_param != NULL)
		{
			static const int TIME_BUF_SIZE = 16;
			static char time_buf[TIME_BUF_SIZE];
			strncpy(line_buf, line.c_str(), BUF_SIZE);
			char* pos = strchr(line_buf, data_seperate_character);
			if (pos == NULL)
			{
				GM_WRITE_ERROR("Incorrect data format in file[%s]: %s", shm_filepath, line_buf);
				return RET_FAILURE_INCORRECT_FORMAT;
			}
			int pos_index = pos - line_buf;
			if (pos_index >= TIME_BUF_SIZE)
			{
				GM_WRITE_ERROR("Incorrect time format in file[%s], time string length: %d", shm_filepath, pos_index);
				return RET_FAILURE_INCORRECT_FORMAT;
			}
			memset(time_buf, 0x0, sizeof(char) * TIME_BUF_SIZE);
			memcpy(time_buf, line_buf, sizeof(char) * pos_index);
			TimeInRangeType time_in_range_type = TimeRangeParam::time_in_range_type(time_range_param, time_buf);
			if (time_in_range_type == TIME_BEFORE_RANGE)
				continue;
			else if (time_in_range_type == TIME_AFTER_RANGE)
				break;
		}
		total_string += (line + new_line);
		iter++;
	}

	ret = gm_add_data(full_filepath, (void*)total_string.c_str(), total_string.size());
	if (CHECK_FAILURE(ret))
		return ret;
	return RET_SUCCESS;
}

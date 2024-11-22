#ifndef __FS_API_H__
#define __FS_API_H__

extern "C"
{
#include "tal_fs.h"
}

class FS_API 
{
public :
	FS_API() {}
	virtual int mkdir(const char *path) = 0;
	virtual int remove(const char *path) = 0;
	virtual int exist(const char *path) = 0;
	virtual int rename(const char *pathFrom,const char *pathTo) = 0;
	
	virtual TUYA_DIR openDir(const char *path) = 0;
	virtual int closeDir(TUYA_DIR dir) = 0;
	virtual TUYA_FILEINFO readDir(TUYA_DIR dir) = 0;
	virtual int getDirName(TUYA_FILEINFO info,const char** name) = 0;
	virtual int isDirectory(const char *path) = 0;

	virtual TUYA_FILE open(const char *path) = 0;
	virtual TUYA_FILE open(const char *path, const char * mode) = 0;
	virtual int  close(TUYA_FILE fd) = 0;
	virtual char read(TUYA_FILE fd) = 0;
	virtual int  read(const char *buf,int size,TUYA_FILE fd) = 0;
	virtual int  readtillN(char *buf, int size,TUYA_FILE fd) = 0;
	virtual int  write(const char *buf,int size,TUYA_FILE fd) = 0;
	virtual void flush(TUYA_FILE fd) = 0;
	virtual int  feof(TUYA_FILE fd) = 0;
	virtual int  lseek(TUYA_FILE fd,int offs, int whence) = 0;
	virtual int  position(TUYA_FILE fd) = 0;
	virtual int  filesize(const char *filepath) = 0;

};

#endif

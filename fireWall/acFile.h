#ifndef _ACFILE_H
#define _ACFILE_H

#include <linux/kernel.h>
#include <uapi/linux/in.h>
#include <linux/net.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/byteorder/generic.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/namei.h>
#include <linux/syscalls.h>
#include <linux/export.h>
#include <linux/uaccess.h>
#include <linux/limits.h>

//#include "acFile.c"
struct blockFile {

			  int count;
				unsigned long fileID;
			  struct list_head list;
};

int blockWriteF(void);
int unblockWriteF(void);
int blockReadF(void);
int unblockReadF(void);
int checkFileID(const struct path path);
int searchFileID(const char __user *fileName, unsigned long* pathId);


#endif

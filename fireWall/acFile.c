#include <linux/kernel.h>
#include <linux/cred.h>
#include <linux/rwsem.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/namei.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>
#include <linux/export.h>
#include <linux/uaccess.h>
#include <linux/limits.h>

#include "acFile.h"

//dnt need it cuz of header
// struct blockFile {
//
// 			  int count;
// 				unsigned long fileID;
// 			  struct list_head list;
// };

static DEFINE_MUTEX(mutRead);
static DEFINE_MUTEX(mutWrite);
static LIST_HEAD(AllfileBlock);
static int counterRead;
/*
int blockWriteF(void);
int unblockWriteF(void);
int blockReadF(void);
int unblockReadF(void);
int checkFileID(struct nameidata *filePath);*/

int blockWriteF(void){

	//block write
	mutex_lock(&mutWrite);

	//if no problem then return 0
	return 0;
}

int unblockWriteF(void){

	//unblock write
	mutex_unlock(&mutWrite);

	//if no problem then return 0
	return 0;
}

int blockReadF(void) {
	//blocking read mutex
	mutex_lock(&mutRead);

	//adding number of readers if
	counterRead++;

	//checking if it is the first reader then block the write
	//we cant do both
	if(counterRead == 1) {
		mutex_lock(&mutWrite);
	}

	//unlock the read mutex
	mutex_unlock(&mutRead);

	//return 0 if everything went well
	return 0;
}

int unblockReadF(void) {

	//blocking read mutex
	mutex_lock(&mutRead);

	//decreasing # of readers
	counterRead--;

	//checking for reader mutex
	//if it was the last reader then block the writer
	if(counterRead == 0) {
		mutex_unlock(&mutWrite);
	}

	//unlock the read mutex
	mutex_unlock(&mutRead);
	//return 0 if no errors
	return 0;
}

//function trying to get the leng g from the path not working
//
// long lengString(const char __user *str)
// {
// 	unsigned long max_addr, src_addr;
//
// 	max_addr = user_addr_max();
// 	src_addr = (unsigned long)str;
// 	if (likely(src_addr < max_addr)) {
// 		unsigned long max = max_addr - src_addr;
// 		return do_strnlen_user(str, ~0ul, max);
// 	}
// 	return 0;
// }


//if (current_cred()->uid != 0)
//     return -EPERM;


int checkFileID(const struct path filePath){

	int checkMutext;
	struct blockFile *flBlock;
	//strcut inode *temp;

	//getting the file ID
	//printk("passed check top");
	unsigned long checkfileID = filePath.dentry->d_inode->i_ino;
//printk("variable  %d", checkFileID);
	checkMutext = blockReadF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	if(list_empty(&AllfileBlock)){
		checkMutext = unblockReadF();
		if(checkMutext != 0){
			return -ENOLCK;
		}
		return 0;
	}

	//printk("passed check 1");

	list_for_each_entry(flBlock, &AllfileBlock, list){

		if(flBlock->fileID == checkfileID ){

			checkMutext = unblockReadF();
			if(checkMutext != 0){
				return -ENOLCK;
			}
			checkMutext = blockWriteF();
			if(checkMutext != 0){
				return -ENOLCK;
			}

			flBlock->count++;

			checkMutext = unblockWriteF();
			if(checkMutext != 0){
				return -ENOLCK;
			}

			return -EINVAL;

		}

	}
	//printk("passed check 2");
	checkMutext = unblockReadF();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	return 0;
}

//searchFile will get the fileid using the kernelpath return 0 if succeed
int searchFileID(const char __user *fileName, unsigned long* pathId){

	//variables for checking files
	int fileLen;
	int checkFile;
	char *tempFile;
	struct path filePath;
	//struct nameidata fileP;

	//TODO might check for file
	//https://www.fsl.cs.sunysb.edu/kernel-api/re250.html
	//PATH_MAX mentioned on piazza and its the max for a file path
	fileLen = strnlen_user(fileName, PATH_MAX);
	// if(fileLen == 0){
	// 	return -EINVAL;
	// }

	//we need to get the pointer for the Kernel
	tempFile = (char*)kmalloc(fileLen, GFP_KERNEL);

	//converting fileNaame from user to kernel to tempFile
	//change to return something .
	if(copy_from_user(tempFile, fileName, fileLen)){
		if(tempFile == NULL){
			kfree(tempFile);
			return -EFAULT;
		}
	}

	//trying to get the path for the file inside the kernel pointer char
	//checking if we have a problem with the kernel path
	//freeing mem that we have kmalloc with tempFile
	//
	//https://elixir.bootlin.com/linux/v4.7/ident/kern_path
	//https://stackoverflow.com/questions/31342628/how-does-path-lookup-in-linux-kernel-work
	checkFile = kern_path(tempFile, LOOKUP_FOLLOW, &filePath);
	if(checkFile){
		kfree(tempFile);
		return -EFAULT;
	}

	//i have tried path_lookupat
	//didnt work had to convert my path to nameidata which took extra time doing conversion
	//it had an end way. or maybe i did it wrong.
	//checkFile path_lookupat(fileP, LOOKUP_FOLLOW, &filePath)


	//this will be copying the i_inode that has in the struct inode
	//which is unique on each file or thtas what i understood
	//line 639
	//https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h
	*pathId = filePath.dentry->d_inode->i_ino;

	// kfree the kmalloc
	kfree(tempFile);

	//return 0 succeed
	return 0;
}


//syscall to do the
SYSCALL_DEFINE0(fc421_reset){

	//struck variables for deleating
  struct blockFile *flBlock;
  struct blockFile *flBlockTemp;
	int checkMutext;


	//checking if root
	if(current_uid().val != 0){
		return -EPERM;
	}

	//lock write mutex
	checkMutext = blockWriteF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//listing all the files blocked and removing them from the list
	list_for_each_entry_safe(flBlock, flBlockTemp, &AllfileBlock, list) {
		list_del(&flBlock->list);
		kfree(flBlock);
	}

	//un lock write mutex
	checkMutext = unblockWriteF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	return 0;
}

/* Syscall to add a file name to the blocked file linked list */
SYSCALL_DEFINE1(fc421_block_file,const char *,filename){

		struct blockFile *flBlock;
		struct blockFile *flBlockTemp;
    unsigned long tempfileID;
		int checkMutext;
		int checkFile;


		//checking if file is null or  if its not root
		if(filename == NULL){
			return -EINVAL;
		}
		//not sure if onlu root should block files
		if(current_uid().val != 0){
			return -EPERM;
		}

		printk("passed block 1");
		//searching for fileId
  	checkFile = searchFileID(filename, &tempfileID);
    if(checkFile != 0) {
      return -EFAULT;
    }
		printk("passed block 2");

		//lock read mutex
		checkMutext = blockReadF();
		if(checkMutext != 0){
			return -ENOLCK;
		}
	 	list_for_each_entry(flBlockTemp, &AllfileBlock, list) {

				//if we find a mathing fileID in our data structure
				//return error EEXIST
			 if(flBlockTemp->fileID == tempfileID){

				 	checkMutext = unblockReadF();
		 			if(checkMutext != 0){
		 				return -ENOLCK;
		 			}
					return -EEXIST;

			 }
		}

		printk("passed block 3");
		//unlock read mutex
		checkMutext = unblockReadF();
		if(checkMutext != 0){
			return -ENOLCK;
		}

		//creating the new file to add to our list
		flBlock = (struct blockFile*)kmalloc(sizeof(struct blockFile), GFP_KERNEL);
		flBlock->count = 0;
		flBlock->fileID = tempfileID;

		//
		checkMutext = blockWriteF();
		if(checkMutext != 0){
			return -ENOLCK;
		}

printk("passed block 4");
		//adding the new file id to our data structure
		list_add(&flBlock->list, &AllfileBlock);
		checkMutext = unblockWriteF();
		if(checkMutext != 0){
			return -ENOLCK;
		}

printk("passed block 5");
		//return 0 if succeed
  return 0;
}

/* Syscall to remove a file name from the blocked file linked list */
SYSCALL_DEFINE1(fc421_unblock_file,const char *, filename){

	struct blockFile *flBlock;
	struct blockFile *flBlockTemp;
	unsigned long tempfileID;
	int checkMutext;
	int checkFile;

	//checking if file is null or  if its not root
	if(filename == NULL){
		return -EINVAL;
	}
	if(current_uid().val != 0){
		return -EPERM;
	}

	//checking if our data structure is empty
	//if its return error and nothing to unblock
	if(list_empty(&AllfileBlock)){
		return -ENOENT;
	}


	//checking for fileID
	checkFile = searchFileID(filename, &tempfileID);
	if(checkFile != 0) {
		return -EINVAL;
	}

	//locking write mutex
	checkMutext = blockWriteF();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	list_for_each_entry_safe(flBlock,flBlockTemp, &AllfileBlock, list ){

		//if we find the fileID in our data structure delete it and return 0;
		//unlock mutex
		if(flBlock->fileID == tempfileID){

			//deleting similar ID file
			list_del(&flBlock->list);
			kfree(flBlock);
			checkMutext = unblockWriteF();
			if(checkMutext != 0){
				return -ENOLCK;
			}
			return 0;
		}
	}

	//if its not found return error and unlock mutex
	checkMutext = unblockWriteF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//return 0
  return -ENOENT;
}

//Syscall to get the number of times a blocked file has been accessed
SYSCALL_DEFINE1(fc421_query,const char *, filename){

	struct blockFile *flBlock;
//	struct blockFile *flBlockTemp;
	unsigned long tempfileID;
	int numAccessed;
	int checkMutext;
	int checkFile;


	//searching the ID file
	checkFile = searchFileID(filename, &tempfileID);
	if(checkFile != 0) {
		return -EINVAL;
	}

	//lock read mutex
	checkMutext = blockReadF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//list all the fileID to find a match if not errpr
	list_for_each_entry(flBlock, &AllfileBlock, list){

		//if we have same file id in our data structure
		//return the number of counts
		//unlock the mutex
		if(flBlock->fileID == tempfileID){
			numAccessed = flBlock->count;
			checkMutext = unblockReadF();
			if(checkMutext != 0){
				return -ENOLCK;
			}
			return numAccessed;

		}


	}

	//if file was not found send error and unlock the read mutex
	checkMutext = unblockReadF();
	if(checkMutext != 0){
		return -ENOLCK;
	}

  //return 0;
	return -ENOENT;
}

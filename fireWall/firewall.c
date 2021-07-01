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
#include <linux/syscalls.h>
#include <linux/mutex.h>

#include "firewall.h"
//the struct for the list
//with the corrresponding variables to hold information for
// int proto, int dir unsigned short port

//dont need this anymore because of header created
struct blockPort {

			int count;
			int typeProto;
			int dirPort;
			unsigned short portToBlock;
			struct list_head list;
};



// https://stackoverflow.com/questions/33932991/difference-between-mutex-init-and-define-mutex
// http://www.cs.otago.ac.nz/cosc440/labs/lab04.pdf
static DEFINE_MUTEX(mutRead);
static DEFINE_MUTEX(mutWrite);
static LIST_HEAD(listBlockPort);
static int counterRead;

/*
//dont need this anymore because of header
int blockWrite(void);
int unblockWrite(void);
int blockRead(void);
int unblockRead(void);*/

int blockWrite(void){

	//block write
	mutex_lock(&mutWrite);

	//if no problem then return 0
	return 0;
}

int unblockWrite(void){

	//unblock write
	mutex_unlock(&mutWrite);

	//if no problem then return 0
	return 0;
}

int blockRead(void) {
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

int unblockRead(void) {

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

/* //dnt need this anymore because of header file
int checkTCP_UDP (int proto);
int searchPort(int proto, int dirPort, struct sockaddr_storage *addrcheck);
int checkPort(int proto, int dir, unsigned short port);*/



//function to check if we are dealing with
//TCP or UDP then just return
int checkTCP_UDP (int proto){

	int temp;
	if(proto == SOCK_STREAM){
		temp = IPPROTO_TCP;
		return temp;
	}
	else if(proto == SOCK_DGRAM){
		temp = IPPROTO_UDP;
		return temp;
	}
	else{
			return 0;
	}
}

int searchPort(int proto, int dirPort, struct sockaddr_storage *addrcheck){

	struct sockaddr_in *addrForm; //https: //www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
	struct blockPort *tempPort;
	unsigned short portForm;
	int checkMutext;


	//need to check if address is NULL
	if(addrcheck == NULL){
		return -EINVAL;
	}

	//getting the right for for the address
	//https://linux.die.net/man/7/ip
	addrForm = (struct sockaddr_in*)addrcheck;

	//checking if  proto is TCP OR UDP
	proto = checkTCP_UDP(proto);
	//fixing some errors with checking
	//not same type
	if(proto == 0){
	 	return 0;
	}

	//get the port using the right form of the address related to lil indian
	//http://bruceblinn.com/linuxinfo/ByteOrder.html
	portForm = (short int)be16_to_cpup(&addrForm->sin_port);

	checkMutext= blockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//checking if we have an empty list and return 0
	//checking if unblock mutex failed
	if(list_empty(&listBlockPort)){
		checkMutext = unblockRead();
		if(checkMutext != 0){
			return -ENOLCK;
		}
		return 0;
	}

	list_for_each_entry(tempPort, &listBlockPort, list){

		/*loop to check if we have a similar port */
		if(tempPort->portToBlock == portForm){

			if(tempPort->typeProto == proto && tempPort->dirPort == dirPort ){

				//unlock read mutex
				checkMutext = unblockRead();
				if(checkMutext != 0){
					return -ENOLCK;
				}

				//lock write to add the times it was accessed
				checkMutext = blockWrite();
				if(checkMutext != 0){
					return -ENOLCK;
				}

				//adding count to the port accessed
				tempPort->count++;
				checkMutext = unblockWrite();
				if(checkMutext != 0){
					return -ENOLCK;
				}

				//since it got here it means the port was accessed and return invalid_params
				return -EINVAL;

			}

		}


	}

	//if port was not found in the list unlock read and return 0
	checkMutext = unblockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	return 0;
}


int checkPort(int proto, int dir, unsigned short port){

	int checkMutext;
	struct blockPort *tempPort;

	//lock the mutex for read
	checkMutext = blockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	//
	// if(list_empty(&listBlockPort)){
	//
	// 	return -EINVAL;
	// }


	list_for_each_entry(tempPort, &listBlockPort, list){

		if(tempPort->typeProto == proto && tempPort->portToBlock == port && tempPort->dirPort == dir) {

			checkMutext = unblockRead();
			if(checkMutext != 0){
				return -ENOLCK;
			}
			//returns error saying the port trying to block is already blocked
			return -EEXIST;

		}

	}

	//unlock read mutext and return 0 meaning it was not blocked
	checkMutext = unblockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	return 0;
}




SYSCALL_DEFINE0(fw421_reset){


	//TODO add mutex
	int checkMutext;
	struct blockPort *pBlock;
	struct blockPort *pBlockTemp;

	//cheching if valid user root
	if(current_uid().val != 0){
		return -EPERM;
	}

	//lock the mutex for write
	checkMutext = blockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//resetting the whole list
	list_for_each_entry_safe(pBlock, pBlockTemp, &listBlockPort, list){
		list_del(&pBlock->list);
		kfree(pBlock);
	}
	//printk("this passed 1");

	checkMutext = unblockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	return 0;
}

SYSCALL_DEFINE3(fw421_block_port, int, proto, int, dir, unsigned short, port){

	int checkMutext;
	int checkBlock;
	struct blockPort *portAdd;

	// checking valid user root
	if(current_uid().val != 0){
		return -EPERM;
	}
//	printk("this passed 1");


	// //checking cheching parameter proto its not TCP or UPD
	if(proto != IPPROTO_TCP && proto != IPPROTO_UDP ){
	 	return -EINVAL;
	}
	// printk("this passed 2");
 	// if(proto != IPPROTO_UDP){
	//  	return -EINVAL;
 	// }

	//printk("this passed 2");

	//making sure its not outside de range of 0-1
	if(dir != 0 && dir != 1){
		return -EINVAL;
	}

	// printk("this passed 4");
	// if(dir != 1){
	// 	return -EINVAL;
	// }

	//printk("this passed LOCKING");
	//checking if port is alredy blocked
	checkBlock = 	checkPort(proto, dir, port);
	if(checkBlock != 0){
		return -EINVAL;
	}
//printk("this passed 6");
	//creating the kmalloc struck to add to the list
	portAdd = (struct blockPort*)kmalloc(sizeof(struct blockPort), GFP_KERNEL);
	portAdd->count = 0; //initialize the count
	portAdd->typeProto = proto; //addint protocol
	portAdd->dirPort = dir; //adding direction
	portAdd->portToBlock = port; //adding port

	//block write mutex
	checkMutext = blockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	//adding it to the list of blocked port
	list_add(&portAdd->list, &listBlockPort);

	//unlock write mutex
	checkMutext = unblockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	return 0;
}

SYSCALL_DEFINE3(fw421_unblock_port,int, proto, int, dir, unsigned short, port){

	struct blockPort *unPort;
	struct blockPort *tempPort;
	int checkMutext;

	//checking if root return error if not error
	if(current_uid().val != 0){
		return -EPERM;
	}

	//printk("this passed 1");
	//we have to check the protocol proto
	//if its not TCP OR UDP return invalid errors
	if(proto != IPPROTO_TCP && proto != IPPROTO_UDP ){
		return -EINVAL;
	}

	//making sure its not outside de range of 0-1
	if(dir != 0 && dir != 1){
		return -EINVAL;
	}
/*

	if(proto != IPPROTO_TCP){
		return -EINVAL;
	}
	if(proto != IPPROTO_UDP){
		return -EINVAL;
	}

	//we have to check the direction int dir
	//it should be only 0 or 1
	//return error Otherwise
	if(dir != 0 && dir != 1){
		return -EINVAL;
	}*/

	//lock write mutex
	checkMutext = blockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	printk("this passed 2");

	list_for_each_entry_safe(unPort, tempPort, &listBlockPort, list){

		//checking if we find a math
		if(unPort->typeProto == proto && unPort->dirPort == dir){

			if(unPort->portToBlock == port){

				//deleating port that was blocked from the data structure
				list_del(&unPort->list);
				kfree(unPort);

				//releasing mutex for write
				checkMutext = unblockWrite();
				if(checkMutext != 0){
					return -ENOLCK;
				}

				return 0;

			}



		}
		//TODO checking if port not in the above return error?
		//else
			// {
			// 	checkMutext = unblockWrite();
			// 	if(checkMutext != 0){
			// 		return -ENOLCK;
			// 	}
			// 	return -ENOENT;
			// }
	}
	printk("this passed 3");
	//releasing mutex for write
	checkMutext = unblockWrite();
	if(checkMutext != 0){
		return -ENOLCK;
	}

	printk("this passed 4");
	//since it was not found in the list return error of file not found
	return -ENOENT;
}

SYSCALL_DEFINE3(fw421_query, int, proto, int, dir, unsigned short, port){

	int checkMutext;
	int totalCount;
	struct blockPort *portList;


	//checking if root return error if not error
	if(current_uid().val != 0){
		return -EPERM;
	}

	//printk("this passed 1");
	//we have to check the protocol proto
	//if its not TCP OR UDP return invalid errors
	if(proto != IPPROTO_TCP && proto != IPPROTO_UDP ){
		return -EINVAL;
	}

	//making sure its not outside de range of 0-1
	if(dir != 0 && dir != 1){
		return -EINVAL;
	}


/*
	//we have to check the protocol proto
	//if its not TCP OR UDP return invalid errors
	if(proto != IPPROTO_TCP){
		return -EINVAL;
	}
	if(proto != IPPROTO_UDP){
		return -EINVAL;
	}

	//we have to check the direction int dir
	//it should be only 0 or 1
	//return error Otherwise
	if(dir != 0 && dir != 1){
		return -EINVAL;
	}
*/
	//lock read mutex
	checkMutext = blockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}


	//loop to list the count of each port if we have a match
	list_for_each_entry(portList, &listBlockPort, list){

		//checking if we have a match
		if(portList->typeProto == proto && portList->dirPort ==dir){

			if(portList->portToBlock == port){

				//copying variable
				totalCount = portList->count;

				//release read mutex
				checkMutext = unblockRead();
				if(checkMutext != 0){
					return -ENOLCK;
				}

				//returning the total amount the file was accessed
				return totalCount;

			}
		}

	}

	//release read mutex
	checkMutext = unblockRead();
	if(checkMutext != 0){
		return -ENOLCK;
	}
	//if not port not found return error
	return -ENOENT;
}

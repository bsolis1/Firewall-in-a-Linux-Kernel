#ifndef _FIREWALL_H
#define _FIREWALL_H

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

int blockWrite(void);
int unblockWrite(void);
int blockRead(void);
int unblockRead(void);

int checkTCP_UDP (int proto);
int searchPort(int proto, int dirPort, struct sockaddr_storage *addrcheck);
int checkPort(int proto, int dir, unsigned short port);


#endif

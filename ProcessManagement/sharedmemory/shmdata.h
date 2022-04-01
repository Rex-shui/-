#ifndef _SHMDATA_H
#define _SHMDATA_H


#define TEXT_SZ 2048

struct shared_use_st
{
	int written;	//1: 可读	0: 可写
	char text[TEXT_SZ];
};




#endif

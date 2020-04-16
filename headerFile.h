#ifndef HDR
#define HDR
#include<stdio.h>
#include<unistd.h>

#define newline printf("\n");



// #define random 8349

// array of structures for storing the task sets



//extern int elements;
//extern int random1;

	
/*
extern struct tasks{
	int period;
	int wcet;
	int relativeDeadline;
} ;

extern struct tasks taskSet[10];

*/

// extern struct task taskSet[10];	



extern void printTaskSet();
extern int findHyperPeriod();
extern void initialize();

#endif
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int iloaded;
int maxf;
int runningf;

struct tasks{
	int tasknum;
	float period,deadline,execution,phase,freq;
};


struct granularity{
	int Gnum,numofjobs,starttime,endtime,freq;
	float slack;
	struct job *firstjob,*lastjob;
};

struct job {
	int job,oftask,executed,arrival,deadline;
	float exe,actualstart,actualstop,wctexe,slack;
	struct job *next,*prev;
};


struct taskutil{
	float runtime,period;
};


struct job *jobexecuting;
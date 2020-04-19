#include "datastr.h"
#include "func.h"

int main(int argc,char **argv){
	if(argc !=2){
		printf("Invalid argument type\n");
		return 0;
	}
	FILE *fptr;
	int n,nfreq;
	int *pi,*p,*e;
	float *freq,*volt;
	fptr = fopen(argv[1],"r");
	if(fptr == NULL)
	{
		printf("Please set the tasks in file names tasks.txt\n");
		return 0;
	}
	fscanf(fptr,"%d",&n);
	jobexecuting=NULL;
	pi = (int *)malloc(sizeof(int)*n);
	e = (int *)malloc(sizeof(int)*n);
	p = (int *)malloc(sizeof(int)*n);

	struct tasks *t=(struct tasks*)malloc(n*sizeof(struct tasks));
	int i =0;
	while(!feof(fptr)){
		fscanf(fptr,"%d %d %d",&pi[i],&p[i],&e[i]);
		t[i].tasknum = i+1;
		t[i].phase = pi[i];
		t[i].period = p[i];
		t[i].deadline = p[i];
		t[i].execution = e[i];
		i++;
	}

	fclose(fptr);

	fptr = fopen("freq.txt","r");
	if(fptr == NULL){
		printf("Please make sure the processors frequency is in the file freq.txt\n");
		return 0;
	}

	fscanf(fptr,"%d",&nfreq);
	freq = (float *)malloc(nfreq*sizeof(float));
	volt = (float *)malloc(nfreq*sizeof(float));
	i =0;
	while(!feof(fptr)){
		fscanf(fptr,"%f %f",&freq[i],&volt[i]);
		i++;
	}

	fclose(fptr);

	printf("\nSchedule done successfully\n");
	int hyperperiod = lcmn(p,n);
	int inphase = firstinphasetime(pi,p,n,hyperperiod);
	if(inphase==-1)
	{
		printf("\nSchedule not possible because jobs of all tasks never come in phase");
		exit(0);
	}
	int bmax = maxboundary(hyperperiod,inphase);
	//Creating the timeline.
	struct granularity *tl = inittimeline(bmax);
	int totalnumjobs = totaljobs(bmax,t,n);
	//Creating Jobs.
	struct job * newjoblist = createjoblist(t,n,totalnumjobs,bmax);
	sortjobonarrivaltime(newjoblist,totalnumjobs);
	// The inital taskutil can be generated dirctly from the tasks.
	struct taskutil * tu = gentaskutil(t,n);
	// Now creating the ready queue. This will be sorted based on absolute deadline.
	struct job* head = NULL;
	float optimalfreq = findfreq(freq,freq[nfreq-1],n,tu,nfreq);
	if(flageff==0)
	{
		printf("\nSchedule not possible because utilization>100%%");
		exit(0);
	}
	jobschedule(newjoblist,tl,bmax,head,totalnumjobs,optimalfreq,freq[nfreq-1],tu,freq,n,nfreq);

	fptr = fopen("output.txt","w");
	if(fptr == NULL)
	{
		printf("Error in opening file\n");
		exit(1);
	}

	fprintf(fptr ,"Hyperperiod=%d\n",hyperperiod);
	fprintf(fptr ,"First inphase time=%d\n",inphase);
	fprintf(fptr, "Scheduling Period: From=%d To=%d\n",inphase,bmax);
	fprintf(fptr,"Total number of jobs=%d\n",totalnumjobs);
	fprintf(fptr,"Number of context switches=%d\n",contSwit);
	fprintf(fptr,"%s","\n");
	fprintf(fptr,"%s","\n");
	fclose(fptr);

	printresponseTime(tl,bmax,n);
	return 0;
}

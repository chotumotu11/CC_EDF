#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int iloaded;
int maxf;
int runningf;

struct tasks{
	int tasknum;
	float period,deadline,execution,phase;
};


struct granularity{
	int Gnum,numofjobs,starttime,endtime;
	float slack;
	struct job *firstjob,*lastjob;
};

struct job {
	int job,oftask,executed,arrival,deadline;
	float exe,actualstart,actualstop;
	struct job *next,*prev;
};


struct taskutil{
	float runtime,period;
};


struct job *jobexecuting;

struct taskutil * gentaskutil(struct tasks *t,int numoftasks){
	struct taskutil* tu = (struct taskutil*)malloc(numoftasks*sizeof(struct taskutil));
	for(int i =0 ;i<numoftasks;i++){
		tu[i].runtime = t[i].execution;
		tu[i].period = t[i].period;
	}

	return tu;
}


void modifytaskexetime(struct taskutil * tu,int tasknum,float newexe){
	tu[tasknum].runtime = newexe;
}

float calefficiency(struct taskutil* tu, int numoftasks){
	float eff = 0;
	for(int i = 0;i<numoftasks;i++){
		eff=eff+(tu[i].runtime/tu[i].period);
	}

	return eff;

}


float findfreq(float *freq,float maxfreq,int numoftasks,struct taskutil *tu,int numfreq){
	for(int i = 0;i<numfreq;i++){
		//printf("The calfreq is %f present freq = %f maxfreq = %f\n",calefficiency(tu,numoftasks),freq[i],maxfreq);
		if(calefficiency(tu,numoftasks) <= (freq[i]/maxfreq))
			return freq[i];
	}
	return -1;
}	

int gcd(int a,int b){
	if(b==0){
		return a;
	}

	return gcd(b,a%b);
}


int lcmn(int *arr, int n){

	int lcm = arr[0];

	for(int i=0;i<n;i++){
		lcm = (arr[i]*lcm)/(gcd(arr[i],lcm));
	}

	return lcm;

}

struct granularity * inittimeline(int length){
	int i;
	struct granularity *st;
	st = (struct granularity *)malloc(length*sizeof(struct granularity));
	for(i=0;i<length;i++){
		st[i].numofjobs = 0;
		st[i].Gnum = i;
		st[i].slack = 1;
		st[i].starttime = i;
		st[i].endtime = i+1;
		st[i].firstjob = NULL;
		st[i].lastjob = NULL;
	}
	return st;
}


int firstinphasetime(int *pi,int *p,int n,int hyperperiod){
	int *allphases = (int *)malloc(n*sizeof(int));
	int threehyp = 3*hyperperiod;
	for(int i = 0 ;i<n;i++){
		allphases[i]=pi[i];
	}
	int highestallocate = 0;
	while(highestallocate<threehyp){
		int notequal = 0;
		for(int i =0;i<n-1;i++){
			if(allphases[i]!=allphases[i+1]){
				notequal=1;
			}
		}
		if(notequal == 0){
			return allphases[0];
		}else{
			int min=allphases[0],minpos=0,max=allphases[0],maxpos=0;
			for(int i =1;i<n;i++){
				if(min>allphases[i]){
					min = allphases[i];
					minpos = i;
				}
				if(max<allphases[i]){
					max= allphases[i];
					maxpos = i;
				}
			}
			highestallocate=max;
			allphases[minpos]+=p[minpos];
		}
	}
	return -1;
}

// Here in place of hyperperiod we will pass the bmax.
int totaljobs(int bmax,struct tasks *t,int n){
	int sum = 0;
	for(int i=0;i<n;i++){
		sum = sum + (bmax-t[i].phase)/t[i].period;
	}

	return sum;
}


int numjobs(int hyp,struct tasks t){
	return (hyp-t.phase)/t.period;
}



int calcmin(int a,int b){
	return a>b?b:a;
}

int maxboundary(int hyperperiod,int inphasetime){
	return calcmin(3*hyperperiod,inphasetime+hyperperiod);
}

void printerg(int bmax,struct granularity *timeline){
	for(int i=0;i<bmax;i++){
		printf("Gnum = %d, numberofjobs = %d, starttime=%d, endtime=%d,slack = %f\n",timeline[i].Gnum,timeline[i].numofjobs,timeline[i].starttime,timeline[i].endtime,timeline[i].slack);
	}
}


struct job * createjoblist(struct tasks *t,int n, int total, int hyp){
	struct job * s = (struct job *)malloc(total*sizeof(struct job));
	int k =0;
	for(int i = 0;i<n;i++){
		for(int j =0;j<numjobs(hyp,t[i]);j++){
			s[k].oftask = t[i].tasknum;
			s[k].job = j+1;
			s[k].exe = t[i].execution;
			s[k].arrival = t[i].phase+j*t[i].period;
			s[k].deadline =  s[k].arrival+t[i].deadline;
			s[k].next = NULL;
			s[k].prev = NULL;
			s[k].executed = 0;
			k++;
		}
	}

	return s;
}


void printerj(struct job *jb,int total){
	for(int i = 0;i<total;i++){
		printf("J%d%d exe = %f, arrival=%d, deadline=%d\n",jb[i].oftask,jb[i].job,jb[i].exe,jb[i].arrival,jb[i].deadline);
	}
}



void sortjobonarrivaltime(struct job *sj, int numofjobs){
    float min;
	int pos;
	for(int i=0;i<numofjobs-1;i++){
		min = sj[i].arrival;
		pos = i;
		for(int j=i+1;j<numofjobs;j++){
			if(min>sj[j].arrival){
				min = sj[j].arrival;
				pos = j;
			}
		}
		struct job a;
		a = sj[i];
		sj[i] = sj[pos];
		sj[pos] = a;
	}
}



void addreadyqueue(struct job r,struct job *head){
	struct job *co = (struct job*)malloc(sizeof(struct job));
	co->job = r.job;
	co->oftask = r.oftask;
	co->executed = r.executed;
	co->arrival = r.arrival;
	co->deadline = r.deadline;
	co->exe = r.exe;
	co->actualstart = r.actualstart;
	co->actualstop = r.actualstop;
	co->next = r.next;
	co->prev = r.prev;
	if(head==NULL){
        head = co;
        co->next=NULL;
        co->prev = NULL;
    }else{
        
        struct job *nextone = head;
        if((nextone->deadline)>(co->deadline)){
            
            co->next = nextone;
            nextone->prev = co;
            co->prev = NULL;
            head = co;
            return;
        }
    
        while(nextone->next!=NULL){
            
            struct job *test = nextone->next; 
            if((test->deadline)>(co->deadline)){
            
                co->next = test;
                co->prev = test->prev;
                (test->prev)->next = co;
                test->prev = co; 
                return;
            }
            nextone=nextone->next;
        }
        nextone->next = co;
        co->prev = nextone;
        co->next = NULL;
    }
}



void removejobfromqueue(struct job *new1,struct job * accepted){
    if(accepted == NULL){
        accepted = NULL; //nothing to remove.
    }else if(accepted->next == NULL){
        //free(accepted);
        accepted = NULL;
    }else{
        struct job * helper = accepted;
        if(accepted == new1){
            accepted = accepted->next;
            accepted->prev = NULL;
            //free(helper);
            return;
        }
        
        while(helper!=new1){
            helper=helper->next;
        }
        if(helper->next== NULL){
            (helper->prev)->next = NULL;
            //free(helper);
            return;
        }
        (helper->next)->prev = helper->prev;
        (helper->prev)->next = helper->next;
        //free(helper);
    }
}




struct job* loadtoreadyqueuewitharrivaltime(struct job *joblist,int arrivaltime,int totaljobs,struct job *head){
	for(int i = iloaded;i<totaljobs;i++){
		if(joblist[i].arrival==arrivaltime){
			addreadyqueue(joblist[i],head);
			iloaded=i;
		}
	}	
}


void wcetapply(struct job *j){
	srand(time(0));
	float ran = ((float)rand()/(float)RAND_MAX);
	if(ran<0.499)
		ran+=0.5;
	j->exe = (j->exe)*ran;
}

void applyalphatime(struct job *j,float freq,float maxfreq){
	float alpha = freq/maxfreq;
	j->exe=(j->exe)/alpha;
}

void insertintog(struct job *j,struct granularity *f){
	struct job * co = (struct job *)malloc(sizeof(struct job));
	co->job = j->job;
	co->oftask = j->oftask;
	co->executed = j->executed;
	co->arrival = j->arrival;
	co->deadline = j->deadline;
	co->exe = j->exe;
	co->actualstart = j->actualstart;
	co->actualstop = j->actualstop;
	co->firstjob = NULL;
	co->lastjob = NULL; 
	if(f->firstjob == NULL){
		f->firstjob = co;
		f->lastjob = co;
		co->next=NULL;
		co->prev=NULL;
	}else{
		f->lastjob->next = co;
		f->lastjob = co;
		co->prev = f->lastjob;
		co->next = NULL;
	}
}




void jobschedule(struct job * joblist,struct granularity *g,int bmax,struct job *head,int totaljobs,float freq,float maxfreq,struct taskutil *tu){
	for(int i =0;i<bmax;i++){
		loadtoreadyqueuewitharrivaltime(joblist,(g+i)->starttime,totaljobs,head);
		if(head!=NULL){
			if(jobexecuting==NULL){
				jobexecuting=head;
				float actualexecutiontime
				removejobfromqueue(head,head);
				if(jobexecuting->executed == 0){
					wcetapply(jobexecuting);
					actualexecutiontime = jobexecuting->exe;
					applyalphatime(jobexecuting,freq,maxfreq);
					jobexecuting->executed = 1;
				}
				if(g[i].slack<jobexecuting.exe){
					jobexecuting.exe -= g[i].slack;
					if(jobexecuting.exe!=0){
						addreadyqueue(*jobexecuting,head);
					}
					g[i].slack=0;
					insertintog(jobexecuting,g+i);
				}else{
					g[i].slack = g[i].slack - jobexecuting.exe;
					jobexecuting=NULL;
					tu[oftask].runtime = 
					insertintog(jobexecuting,g+i);
				}
			}
		}
	}
}


void printtaskutil(struct taskutil * t , int numtask){
	for(int i = 0;i<numtask;i++){
		printf("Task = %f %f\n",t[i].runtime,t[i].period);
	}

}



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


	int hyperperiod = lcmn(p,n);
	printf("The hyperperiod is %d\n",hyperperiod);
	int inphase = firstinphasetime(pi,p,n,hyperperiod);
	printf("The inphase time = %d\n",inphase);
	int bmax = maxboundary(hyperperiod,inphase);
	printf("The boundary is %d\n",bmax);
	//Creating the timeline.
	struct granularity *tl = inittimeline(bmax);
	int totalnumjobs = totaljobs(bmax,t,n);
	printerg(bmax,tl);
	printf("totaljobs is %d\n",totalnumjobs);
	//Creating Jobs.
	struct job * newjoblist = createjoblist(t,n,totalnumjobs,bmax);
	//printerj(newjoblist,totalnumjobs);
	sortjobonarrivaltime(newjoblist,totalnumjobs);
	printerj(newjoblist,totalnumjobs);
	// The inital taskutil can be generated dirctly from the tasks.
	struct taskutil * tu = gentaskutil(t,n);
	//printtaskutil(tu,n);
	// Now creating the ready queue. This will be sorted based on absolute deadline.
	struct job* head = NULL;
	float optimalfreq = findfreq(freq,freq[nfreq-1],n,tu,nfreq);
	printf("The static freq is %f\n",optimalfreq);
	//wcetapply();
	return 0;
}

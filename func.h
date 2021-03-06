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


int flageff=0;
float findfreq(float *freq,float maxfreq,int numoftasks,struct taskutil *tu,int numfreq){
	for(int i = 0;i<numfreq;i++){
		if(calefficiency(tu,numoftasks) <= (freq[i]/maxfreq))
		{
			flageff=1;
			return freq[i];
		}
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
		st[i].freq = 0;
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
			s[k].wctexe = t[i].execution;
			s[k].slack = t[i].execution;
			k++;
		}
	}

	return s;
}


void printerj(struct job *jb,int total){
	for(int i = 0;i<total;i++){
		printf("J%d %d exe = %f, wcet = %f ,arrival=%d, deadline=%d\n",jb[i].oftask,jb[i].job,jb[i].exe,jb[i].wctexe,jb[i].arrival,jb[i].deadline);
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



struct job * addreadyqueue(struct job r,struct job *head){
	struct job *co = (struct job*)malloc(sizeof(struct job));
	co->job = r.job;
	co->oftask = r.oftask;
	co->executed = r.executed;
	co->arrival = r.arrival;
	co->deadline = r.deadline;
	co->exe = r.exe;
	co->actualstart = r.actualstart;
	co->actualstop = r.actualstop;
	co->wctexe = r.wctexe;
	co->slack = r.slack;
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
            return head;
        }

        while(nextone->next!=NULL){

            struct job *test = nextone->next;
            if((test->deadline)>(co->deadline)){

                co->next = test;
                co->prev = test->prev;
                (test->prev)->next = co;
                test->prev = co;
                return head;
            }
            nextone=nextone->next;
        }
        nextone->next = co;
        co->prev = nextone;
        co->next = NULL;
    }

    return head;
}



struct job*  removeheadfromjob(struct job * accepted){

	if(accepted == NULL){
		return NULL;
	}

	if(accepted->next == NULL){
		accepted = NULL;
		return accepted;
	}

	struct job *helper = accepted;
	accepted = accepted->next;
	accepted->prev = NULL;
	return accepted;
}




struct job* loadtoreadyqueuewitharrivaltime(struct job *joblist,struct taskutil * tu,int arrivaltime,int totaljobs,struct job *head){
	for(int i = 0;i<totaljobs;i++){
		if(joblist[i].arrival==arrivaltime){
			if(joblist[i].job != 1){
				tu[joblist[i].oftask].runtime = joblist[i].wctexe;
			}
			head=addreadyqueue(joblist[i],head);
		}
	}
	return head;
}


void wcetapply(struct job *j){
	srand(time(0));
	float ran = ((float)rand()/(float)RAND_MAX);
	if(ran<0.499)
		ran+=0.5;
	j->wctexe = (j->wctexe)*ran;
	j->slack = (j->slack)*ran;
}

void wcetapplyalljobs(struct job *j,int numofjobs){
	for(int i = 0;i<numofjobs;i++){
		wcetapply(j+i);
	}
}

void applyalphatime(struct job *j,float freq,float maxfreq){
	float alpha = freq/maxfreq;
	j->slack = (j->slack)/alpha;
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
	co->wctexe = j->wctexe;
	co->slack = j->slack;
	co->next = NULL;
	co->prev = NULL;
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



struct job * inserttoreadyqueuewithoutcopy(struct job *co,struct job *head){
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
            return head;
        }
        while(nextone->next!=NULL){

            struct job *test = nextone->next;
            if((test->deadline)>(co->deadline)){

                co->next = test;
                co->prev = test->prev;
                (test->prev)->next = co;
                test->prev = co;
                return head;
            }
            nextone=nextone->next;
        }
        nextone->next = co;
        co->prev = nextone;
        co->next = NULL;
    }

    return head;

}

void printreadyqueue(struct job * head){
	struct job * helper = head;
	while(helper!=NULL){
		printf("J%d %d\n",helper->oftask,helper->job);
		helper=helper->next;
	}

}

int contSwit;
void jobschedule(struct job * joblist,struct granularity *g,int bmax,struct job *head,int totaljobs,float freq,float maxfreq,struct taskutil *tu,float *freqarray,int numoftasks,int numfreq){
	contSwit=0;
	for(int i =0 ;i<bmax;i++){
		head = loadtoreadyqueuewitharrivaltime(joblist,tu,(g+i)->starttime,totaljobs,head);
		while(g[i].slack!=0 && head!=NULL){
			if(jobexecuting == NULL){
				jobexecuting = head;
				if((jobexecuting->oftask)==1&&(jobexecuting->job)==1)
					contSwit=contSwit;
				else
					contSwit++;
				head=removeheadfromjob(head);
				jobexecuting->slack = (jobexecuting->slack)/(freq/maxfreq);
				if(g[i].slack < jobexecuting->slack){
					jobexecuting->actualstart=i+(1-g[i].slack);
					if(head==NULL)
					{
						jobexecuting->actualstop=i+jobexecuting->slack;
					}
					jobexecuting->slack = (jobexecuting->slack)-g[i].slack;
					g[i].slack = 0;
					jobexecuting->slack = (jobexecuting->slack)*(freq/maxfreq);
					insertintog(jobexecuting,g+i);
				}else{
					jobexecuting->actualstart=i+(1-g[i].slack);
					g[i].slack = g[i].slack-(jobexecuting->slack);
					jobexecuting->actualstop=i+jobexecuting->slack;
					jobexecuting->slack = 0;
					tu[jobexecuting->oftask].runtime = jobexecuting->wctexe;
					freq = findfreq(freqarray,maxfreq,numoftasks,tu,numfreq);
					insertintog(jobexecuting,g+i);
					jobexecuting = NULL;
				}

			}else{
				if((jobexecuting->deadline)>(head->deadline)){
					contSwit++;
					freq = findfreq(freqarray,maxfreq,numoftasks,tu,numfreq);
					inserttoreadyqueuewithoutcopy(jobexecuting,head);
					jobexecuting = head;
					jobexecuting->slack = (jobexecuting->slack)/(freq/maxfreq);
					if(g[i].slack < jobexecuting->slack){
						jobexecuting->actualstart=i+(1-g[i].slack);
						jobexecuting->slack = (jobexecuting->slack)-g[i].slack;
						g[i].slack = 0;
						jobexecuting->slack = (jobexecuting->slack)*(freq/maxfreq);
						insertintog(jobexecuting,g+i);
					}else{
						jobexecuting->actualstart=i+(1-g[i].slack);
						g[i].slack = g[i].slack-(jobexecuting->slack);
						jobexecuting->actualstop=i+jobexecuting->slack;
						jobexecuting->slack = 0;
						tu[jobexecuting->oftask].runtime = jobexecuting->wctexe;
						freq = findfreq(freqarray,maxfreq,numoftasks,tu,numfreq);
						insertintog(jobexecuting,g+i);
						jobexecuting = NULL;
					}

				}else{
					jobexecuting->slack = (jobexecuting->slack)/(freq/maxfreq);
					if(g[i].slack < jobexecuting->slack){
						jobexecuting->slack = (jobexecuting->slack)-g[i].slack;
						g[i].slack = 0;
						jobexecuting->slack = (jobexecuting->slack)*(freq/maxfreq);
						insertintog(jobexecuting,g+i);
					}else{
						g[i].slack = g[i].slack-(jobexecuting->slack);
						jobexecuting->actualstop=i+jobexecuting->slack;
						jobexecuting->slack = 0;
						tu[jobexecuting->oftask].runtime = jobexecuting->wctexe;
						freq = findfreq(freqarray,maxfreq,numoftasks,tu,numfreq);
						insertintog(jobexecuting,g+i);
						jobexecuting = NULL;
					}
				}
			}
		}
	}
}


// void printtaskutil(struct taskutil * t , int numtask){
// 	for(int i = 0;i<numtask;i++){
// 		printf("Task = %f %f\n",t[i].runtime,t[i].period);
// 	}

// }

// void printschedule(struct granularity *g,int bmax){
// 	FILE *fptr ;
// 	fptr = fopen("output.txt","a");
// 	if(fptr == NULL)
// 	{
// 		printf("Error in opening file\n");
// 		exit(1);
// 	}

// 	for(int i = 0;i<bmax;i++){
// 		struct job * co = g[i].firstjob;
// 		fprintf(fptr,"From time %d to %d\t",g[i].starttime,g[i].endtime);
// 		while(co!=NULL){
// 			fprintf(fptr,"Task: %d Job_no.: %d\t",co->oftask,co->job);
// 			co=co->next;
// 		}
// 		fprintf(fptr,"%s","\n");
// 	}
// 	fprintf(fptr,"%s","\n");
// 	fprintf(fptr,"%s","\n");
// 	fclose(fptr);
// }

void printresponseTime(struct granularity *g,int bmax ,int numtask){
	struct job * finaljoblist = NULL;
	for(int i = 0;i<bmax;i++){
		struct job * co = g[i].firstjob;
		while(co!=NULL){
			if(finaljoblist == NULL)
			{
				struct job * newNode = (struct job *)malloc(sizeof(struct job));
				newNode->job = co->job;
				newNode->oftask = co->oftask;
				newNode->executed = co->executed;
				newNode->arrival = co->arrival;
				newNode->deadline = co->deadline;
				newNode->exe = co->exe;
				newNode->actualstart = co->actualstart;
				newNode->actualstop = co->actualstop;
				newNode->wctexe = co->wctexe;
				newNode->slack = co->slack;
				newNode->prev = NULL; // single linked list
				newNode->next = NULL;
				finaljoblist = newNode;
			}
			else
			{
				struct job * temp = finaljoblist;
				while(temp != NULL)
				{
					if(temp->job == co->job && temp->oftask == co->oftask)
					{
						temp->exe = co->exe;
						temp->actualstop = co->actualstop;
						temp->wctexe = co->wctexe;
						temp->slack = co->slack;
						break;
					}
					temp = temp->next;
				}

				if(temp == NULL)
				{
				  temp = finaljoblist;
					while(temp->next != NULL)
					{
						temp = temp->next;
					}

					struct job * newNode = (struct job *)malloc(sizeof(struct job));
					newNode->job = co->job;
					newNode->oftask = co->oftask;
					newNode->executed = co->executed;
					newNode->arrival = co->arrival;
					newNode->deadline = co->deadline;
					newNode->exe = co->exe;
					newNode->actualstart = co->actualstart;
					newNode->actualstop = co->actualstop;
					newNode->wctexe = co->wctexe;
					newNode->slack = co->slack;
					newNode->prev = NULL; // single linked list
					newNode->next = NULL;
					temp->next = newNode;
				}

			}
			co=co->next;
		}
	}

	FILE *fptr ;
	fptr = fopen("output.txt","a");
	if(fptr == NULL)
	{
		printf("Error in opening file\n");
		exit(1);
	}


	struct job * travers_temp = finaljoblist;
	while(travers_temp != NULL)
	{
		fprintf(fptr,"Task=%d Job=%d Arrival=%d WCET=%.3f ActualStart=%.3f ActualStop=%.3f\n",travers_temp->oftask,travers_temp->job,travers_temp->arrival,travers_temp->exe,travers_temp->actualstart , travers_temp->actualstop);
		travers_temp= travers_temp->next;
	}

	fprintf(fptr,"%s","\n\n");

	float ovmaxResponseTime = -1;
	float ovminResponseTime = -1;
	float ovavgResponseTime =0;

	for(int i = 0 ; i < numtask ; i++)
	{
		float maxResponseTime = -1;
		float minResponseTime = -1;
		float avgResponseTime =-1;

	    struct job * travers_temp = finaljoblist;
		int totaljobs = 0;
		while(travers_temp != NULL)
		{
		    if(travers_temp->oftask == i+1)
			{
				float temp_response = travers_temp->actualstop - travers_temp->arrival;
				totaljobs++;
				if(avgResponseTime == -1 && temp_response >= 0)
				{
					avgResponseTime = temp_response;
				}
				else
				{
					if(temp_response >= 0)
					avgResponseTime = avgResponseTime + temp_response;
				}
	       		if(maxResponseTime == -1 && temp_response >= 0)
				{
					maxResponseTime = temp_response;
				//	ovmaxResponseTime=temp_response;
				}
				else
				{
				  	if(maxResponseTime < temp_response &&  temp_response >= 0)
						maxResponseTime = temp_response;
				}
				if(minResponseTime == -1 && temp_response >= 0)
				{
				    minResponseTime = temp_response;
				 //   ovminResponseTime=temp_response;
				}
				else
				{
					if(minResponseTime > temp_response && temp_response >= 0)
						minResponseTime = temp_response;
				}
			}
			travers_temp = travers_temp->next;
		}

		if(ovmaxResponseTime == -1)
		{
			ovmaxResponseTime =  maxResponseTime;
		}
		else
		{
			if(ovmaxResponseTime < maxResponseTime)
						ovmaxResponseTime = maxResponseTime;
		}

		if(ovminResponseTime == -1)
		{
			ovminResponseTime =  minResponseTime;
		}
		else
		{
			if(ovminResponseTime > minResponseTime)
						ovminResponseTime = minResponseTime;
		}

		//if(ovmaxResponseTime < maxResponseTime)
			//			ovmaxResponseTime=maxResponseTime;
		//if(ovminResponseTime > minResponseTime)
			//			ovminResponseTime=minResponseTime;
		ovavgResponseTime += (avgResponseTime/totaljobs);
  	fprintf(fptr,"Task %d Total jobs=%d\n",i+1,totaljobs);
  	fprintf(fptr,"Abs Response Time Jitter=%f\n",maxResponseTime-minResponseTime);
		fprintf(fptr,"MaxResponseTime=%f\n",maxResponseTime);
		fprintf(fptr,"MinResponseTime=%f\n",minResponseTime);
		fprintf(fptr,"AvgResponseTime=%f\n",(avgResponseTime/totaljobs));
		fprintf(fptr,"%s","\n\n");
	}
	fprintf(fptr,"Overall Schedule MaxResponseTime=%f\n",ovmaxResponseTime);
	fprintf(fptr,"Overall Schedule MinResponseTime=%f\n",ovminResponseTime);
	fprintf(fptr,"Overall Schedule AvgResponseTime=%f\n",ovavgResponseTime/numtask);
	fprintf(fptr,"%s","\n\n");
	fclose(fptr);
}

#include<stdio.h>
#include<stdlib.h>


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

int main(int argc,char **argv){
	if(argc !=2){
		printf("Invalid argument type\n");
		return 0;
	}
	FILE *fptr;
	int n;
	int *pi,*p,*e;
	fptr = fopen(argv[1],"r");
	if(fptr == NULL)
	{
		printf("Please set the tasks in file names tasks.txt\n");
		return 0;
	}
	fscanf(fptr,"%d",&n);
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
	printerj(newjoblist,totalnumjobs);
	return 0;
}

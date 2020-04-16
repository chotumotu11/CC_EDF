 
 #include"headerFile.h"




 struct tasks{
 	int arrivalTime;
	int period;
	float wcet;
	int relativeDeadline;
} ;

struct tasks taskSet[10];



static int elements = 0;

int gcd(int a, int b){
	if(a==0)
		return b;
	return gcd(b % a, a);
}


int findInphase(struct tasks *taskSet, int elements){
	int inphase[elements];
	for(int i=0; i<elements; i++){
		inphase[i] = taskSet[i].arrivalTime; // + taskSet[i].period;

	}
	// print initial arrival times
//	for(int i=0; i<elements; i++) printf(" %d  ", inphase[i]);
//	printf("\n");

	int inphasef = 10,  flag = 0,  FOUND = 1, it =0;

	while(flag == 0 && inphasef--){
		FOUND = 1;
		int val = inphase[0];
		//printf("val: %d\n", val);


		for(int i=1; i<elements; i++){
			if(val != inphase[i]){
				FOUND = 0;
				//flag = 1;
				break;

			}
		}
		if(FOUND)return val;
		else{
			int minp = inphase[0], index =0;
			for(int i=1; i<elements; i++){
				if(inphase[i] < minp){
					minp = inphase[i];
					index = i;

				} 

			}
			inphase[index] += taskSet[index].period;
			//printf("updated inphase value: %d\n", inphase[index]);

		}
		//printf("Iteration %d: ", it++);	
		//for(int i=0; i<elements; i++) printf(" %d  ", inphase[i]);
	    //   printf("\n");




	
}

}

int main(int argc, char *argv[]){

	// initialize extern variables
	//init();
	//printf("The value of random is %d and number of elements are %d \n", random1, elements);

	FILE *fp;
	char *filename;
	//char ch;

	// read TaskSet file values in integer variable i
	int i = 0, j =0,  hyperPeriod = 1; // elements : number of structure elements
	float maxExecutionTime = 0;


	// if number of arguments less than 2 that means file name is 
	// not provided
	if(argc < 2){
		printf("Filename not provided as argument\n");
		return 1;
	}	

	filename = argv[1];

	// opening the file in read-only mode
	fp = fopen(filename, "r");

	// initializing the extern variable
	//elements = 0;

	if(fp != NULL){
		//while((ch = fgetc(fp)) != EOF){
		int counter = 0;
		while(!feof(fp)){
			
			if(counter % 4 == 0){
				fscanf(fp, "%d", &i);
				//printf("%d", i );
				taskSet[j].arrivalTime = i;
			}
			else if(counter % 4 == 1){
				float temp;
				fscanf(fp, "%f", &temp);
				//printf("%f", temp );
				taskSet[j].period = temp;
			}
			else if(counter % 4 == 2){
				fscanf(fp, "%d", &i);
				//printf("%d", i );
				taskSet[j].wcet = i;
				
			}
			else if(counter % 4 == 3){
				fscanf(fp, "%d", &i);
				taskSet[j].relativeDeadline = i;
				j++;
				
				elements++;

			}
			counter++; // this was missing 

		}
		printf("\n"); // newline for readability
	}

	else{
		printf("File could not be opened\n");
	}

	fclose(fp);
	//printTaskSet();

//	printf("Number of elements in the TaskSet are: %d\n", elements);
	int periodArray[elements];


	// printing the taskSet


	for(int i=0; i<elements; i++){
	//	printf("%d %d %.2f %d\n", taskSet[i].arrivalTime, taskSet[i].period, taskSet[i].wcet, taskSet[i].relativeDeadline);
		periodArray[i] = taskSet[i].period;
		if(taskSet[i].wcet > maxExecutionTime){
			//printf("wcet %f is greater that %f \n", taskSet[i].wcet, maxExecutionTime );
			maxExecutionTime = taskSet[i].wcet;
		}
	}



  // calculating the in-phase time
	int inphaseTime = findInphase(taskSet,elements);
	printf("First In-phase time is: %d\n", inphaseTime);
	



    for(int i=0; i<elements; i++){
    	//hyperPeriod = lcm(hyperPeriod, periodArray[i]);
    	hyperPeriod = (hyperPeriod * periodArray[i]) / gcd(hyperPeriod, periodArray[i]);
    }


	printf("Hyperperiod of tasks is: %d\n", hyperPeriod);

	
	return 0;
}



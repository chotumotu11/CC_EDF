Dipayan Deb 2019H1030015G
Bhumika Joshi 2019H1030501G
Himanshu Sharma 2019H1030030G
Kumar Anand 2019H1030500G

FILE LIST

start.c - This is the file that contains starting code. It includes the main method.

datastr.h - This file also contains code. It contains all the data structure definaltions.

func.h - This file contains all the functions implementation. These functions are called from the main.

makefile - This is the file that contains the code for make command. When the make command is used it will compile the file

tasks.txt - This file contains the periodic tasks. The format is (phase , period , execution time). Make sure the inputs are all integer. The granularity for the program is 1. But its able to handle the decimal values of execution times generated because of the change in frequency and actual execution time calculation.

output.txt - This file contains the final schedule and all the details like different kinds of repsonse time , jitter etc about the simulation are printed.

freq.txt - Lists the number of frequency supported by the processor in the first line. 
The next few lines contains the frequency and the respective voltage. The list goes from minimun frequency at the top to the lowest frequency at the bottom. The frequency will have to be listed in that order for the progeram to work.


 Readme - This file.

HOW TO COMPILE THE PROGRAM

 On any Linux or Windows terminal use the "make" command. If there is any error in compiling then just remove the pre compiled file and run "make" again.

HOW TO EXECUTE YOUR PROGRAM

./start <filename>   eg ./start tasks.txt


STRUCTURE OF THE PROGRAM

The program is Structured into 3 files. The start.c is the starting point of the program. This is where the porgram starts executing. The func.h contains all the fucntions that are used to implement the schedule. The datastr.h contains the defination for all the data structures.

lcmn - calculates the hyperperiod of the program 
firstinphasetime - calulates the first in-phase time for all the out of phase jobs.
maxboundary - calcluates the time till which we would have to schedule . Uses the formula provided.
createjoblist - creates all the jobs for different tasks.
findfreq - finds the frequency based on the utilization.
jonschedule - schedules all the jobs the job.

THINGS NOT COMPLETED

1. Three types of overhead
2. Calculating Dynamic energy
3. Ideal times in schedule not mentioned seperately.
4. Inputs should be all integers.






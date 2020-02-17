 #include <stdio.h>
#include <unistd.h>
/*exe_time
when you compile it to job1 and execute it like ./job1 5 where 5 is 5 means this micro benchmark will run for approximately 5 seconds
you should make this job ocupy the processor for 5 seconds
and let the procssor excute  task implement in this job i.e., batch_job.

you should also consider a mechanism  to make the 5 as an argument rather than implemented as hard code. The same to Priority.
*/
int exe_time= 0;

int main(){
	int i,j,result;
	FILE *fp;
	fp = fopen("job1_output.txt", "w");
	fprintf(fp, "\n");
	for (i = 1; i < 100; i++){
		for(j = 1; j < 100; j++){
			result = i * j;
			fprintf(fp, "%d*%d=%-3d",i,j,result);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	/*sleep for */
    sleep(exe_time);
    exit(0);
}

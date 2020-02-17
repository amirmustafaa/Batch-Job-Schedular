#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "commandline.c"


struct job{
    char *name;
    int priority;
    int ex_time; //execution time
    char *entry; //job entry time
};

int count = 0;//count number of jobs
time_t curtime;//time tracking variable
clock_t t; //clock tracking
double totalT;//total time spent
struct job q[4];//job array
struct job testQ[4];//benchmark test array
double throughput;
double waiting;
struct commandline_parser; //cmd parser instance

pthread_mutex_t job_queue_lock;  /* Lock for critical sections */
pthread_cond_t job_queue_not_full; /* Condition variable for queue_not_full */
pthread_cond_t job_queue_not_empty;



void *scheduling_module( void *ptr );
void *dispatching_module( void *ptr );





int main() {
    pthread_t scheduling_thread, dispatching_thread; /* Two concurrent threads */
    char *message1 = "Command Thread";
    char *message2 = "dispatching_module Thread";
    int  iret1, iret2;


    /* Create two independent threads:command and dispatching_module*/
    iret1 = pthread_create(&scheduling_thread, NULL, scheduling_module, (void*) message1);
    pthread_join(scheduling_thread, NULL);
    iret2 = pthread_create(&dispatching_thread, NULL, dispatching_module, (void*) message2);

    /* Initialize the lock the two condition variables */
    pthread_mutex_init(&job_queue_lock, NULL);
    pthread_cond_init(&job_queue_not_full, NULL);
    pthread_cond_init(&job_queue_not_empty, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    //pthread_join(scheduling_thread, NULL);
    pthread_join(dispatching_thread, NULL);


   
    return(0);
}







void *scheduling_module(void *ptr) {
  char *buffer;
  size_t bufsize = 64;
  char *user[20];


  //main ui loop
  buffer = (char*) malloc(bufsize * sizeof(char));
  if (buffer == NULL) {
     perror("Unable to malloc buffer");
      exit(1);
    }

  while (count < 4) {
  printf("> [? for menu]: ");
  getline(&buffer, &bufsize, stdin);
  cmd_dispatch(buffer);
    }


    //hard coded jobs for testing
	 // q[0].name = "job1";
   //   q[0].priority = 1;
   //   q[0].ex_time = 2;
   //
	 // q[1].name = "job2";
   //   q[1].priority = 6;
   //   q[1].ex_time = 3;
   //
	 // q[2].name = "job3";
   //   q[2].priority = 4;
   //   q[2].ex_time = 4;
   //
	 // q[3].name = "job4";
   //   q[3].priority = 3;
   //   q[3].ex_time = 5;

        return 0;
}






void *dispatching_module(void *ptr) {

	int i;
	pid_t pid;

  for(i = 0; i < count; i++){//change count to 4 if doesnt work
    //printf("%s\n",*q[i].name);
    t = clock();
	  switch ((pid = fork()))
  	{
    case -1:
      /* Fork() has failed */
      perror("fork");
      break;
    case 0:
      /* This is processed by the child */
      execv(q[i].name, NULL);
      puts("Uh oh! If this prints, execv() must have failed");
      exit(EXIT_FAILURE);
      break;
    default:
      /* This is processed by the parent */
      puts ("This is a message from the parent");
      break;
      }

      printf("job done\n");
      sleep(q[i].ex_time);
    }
    t = clock() - t;
    totalT = ((double)t)/CLOCKS_PER_SEC; //time in seconds
    return 0;
  }


  /*
   * The run command - submit a job.
   */
   int cmd_run(int nargs, char **args) {
   int pri = -1;
   int ext = -1;
   char *nom = malloc(50);
   unsigned int_size = sizeof(int);
 	if (nargs != 4) {
    //count = 2;
     printf("enter job name('job1', 'job2', 'job3', 'job4')\n");
     scanf("%s", nom);
     q[count].name = nom;
     printf("enter job priority\n");
     scanf("%d", &pri);
     q[count].priority = pri;
     printf("enter job execution time\n");
     scanf("%d", &ext);
     q[count].ex_time = ext;
     time(&curtime);
     q[count].entry = ctime(&curtime);
     printf("Current time = %s",q[0].entry);
 		 //printf("Usage: run <job> <time> <priority>\n");
     count++;


     return 0;
 		//return EINVAL;
 	}

         /* Use execv to run the submitted job in csubatch */
 				printf("use execv to run the job in csubatch.\n");
       	return 0; /* if succeed */
 }

int cmd_list(){
  int j = 0;
  while(j < count){
    printf("%s has a priority of %d and will run %d\n", q[j].name,q[j].priority,j + 1);
    j++;
  }
  return 0;
}

//compare priority levels
int cmpfunc (const void *a, const void *b) {
    struct job *jobA = (struct job *)a;
    struct job *jobB = (struct job *)b;
    return ( jobB->priority - jobA->priority );
}

void cmd_priority(int urgency){
    int i, j, a, n = 3; //, number[30]

    qsort(q, count, sizeof(struct job), cmpfunc);

    printf("The numbers arranged in ascending order are given below \n");
    for (i = 0; i <= count; ++i){
        printf("%d  %s\n", q[i].priority, q[i].name);
    }

}

//compare execution times
int cmp_time (const void *a, const void *b) {
    struct job *jobA = (struct job *)a;
    struct job *jobB = (struct job *)b;
    return ( jobA->ex_time - jobB->ex_time );
}

void cmd_sjf(int time){

    int i, j, a, n = 3; //, number[30]

    qsort(q, count, sizeof(struct job), cmp_time);

    printf("The numbers arranged in ascending order are given below \n");
    for (i = 0; i <= count; ++i){
        printf("%d  %s\n", q[i].ex_time,q[i].name);
    }

}

int cmd_quit(int nargs, char **args) {
  printf("average waiting time: %f seconds\n",waiting );
  printf("average time per job: %f seconds\n",totalT/4);
  printf("average throughput this session: %f\n",throughput/4);
        exit(0);
}


int cmd_test(){
  char testtype = malloc(100);
  pid_t test_pid;

  //hard coded jobs for testing
 testQ[0].name = "job1";
   testQ[0].priority = 1;
   testQ[0].ex_time = 2;

 testQ[1].name = "job2";
   testQ[1].priority = 6;
   testQ[1].ex_time = 3;

 testQ[2].name = "job3";
   testQ[2].priority = 4;
   testQ[2].ex_time = 4;

 testQ[3].name = "job4";
   testQ[3].priority = 3;
   testQ[3].ex_time = 5;

   for(int i = 0; i < 4; i++){
     //printf("%s\n",*q[i].name);
     t = clock();
 	  switch ((test_pid = fork()))
   	{
     case -1:
       /* Fork() has failed */
       perror("fork");
       break;
     case 0:
       /* This is processed by the child */
       execv(testQ[i].name, NULL);
       puts("Uh oh! If this prints, execv() must have failed");
       exit(EXIT_FAILURE);
       break;
     default:
       /* This is processed by the parent */
       puts ("This is a message from the parent");
       break;
       }

       printf("job done\n");
       sleep(testQ[i].ex_time);
     }
     t = clock() - t;
     totalT = ((double)t)/CLOCKS_PER_SEC; //time in seconds
     return 0;
     printf("average waiting time: %f seconds\n",waiting );
     printf("average time per job: %f seconds\n",totalT/4);
     printf("average throughput this session: %f\n",throughput/4);
}

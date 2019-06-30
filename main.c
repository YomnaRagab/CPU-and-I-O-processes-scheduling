/*
Author:Yomna Ragab.
Process and I\O requests scheduling
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ELEMENTS_NUMBER 200
#define LSIZ 128
#define RSIZ 10
int tot_global;
typedef struct tstrQueue
{
    int     first;
    int     last;
    int     validItems;
    int     data[ELEMENTS_NUMBER];
} tstrQueue;
// new data type for queues
typedef enum en_errors
{
	empty=-1,
	full=1,
	NoError=0
}en_errors;
//new data types for error
typedef struct
{
    int num;
    int cpu_time;
    int io_time;
    int arr_time;
    int io_remaining_time;
    int cpu_remaining_time;
    int turnaround;
    int done;
    char status;
    char string_status[10];
}strprocess;
//struct to save the process elements
 strprocess arr[200]; //array of process structure with max number of processes which is 200
 void readfile (void);
 /*
        readfile is a function to open the file which contatins the processes data and save each line in an array of string after that it calls a segmentation func. which divide the line into words to adjust the times
        return:none.
        arguments:none.
        ex for the file contain
        2 3 5 2
        1 2 1 1
        the first coloumn is the process number
        the second coloumn is the CPU burst time
        the third coloumn is the I/O burst time
        the fourth one is the arrival time.

 */
 void segmentation(char phrase[],char **token);
/*
        function: divide the command from the user into words and save it in the array of pointer to character.
                  Determine of the status of process.
        arguments: array of character (line) ,
                   array of pointer to character to save the "segmented part" time in it.
        return:None.
*/
void initializeQueue(tstrQueue *theQueue);
/*
            function:initialize the queue with the number of element which is called elemnets number and adjust all elemnts with zero and it's first and last pointers.
            arguments: pointer to the queue.
            return:None.
*/

en_errors isEmpty(tstrQueue *theQueue);
/*
            function:check if the refered queue is empty or no.
            arguments:poiter to the queue.
            return: enum called en_errors // scroll up to check it

*/

en_errors putItem(tstrQueue *theQueue, int theItemValue);
/*
            function: push the items in the queue.
            arguments:pointer to the queue.
            return:enum called en_errors // scroll up to check it

*/

en_errors getItem(tstrQueue *theQueue, int *theItemValue);
/*
            function: pop the items out the queue.
            arguments:pointer to the queue.
            return:enum called en_errors // scroll up to check it

*/

void initializeQueue(tstrQueue *theQueue)
{
    int i;
    theQueue->validItems  =  0;
    theQueue->first       =  0;
    theQueue->last        =  0;
    for(i=0; i<ELEMENTS_NUMBER; i++)
    {
        theQueue->data[i] = 0;
    }
}

en_errors isEmpty(tstrQueue *theQueue)
{
    if(theQueue->validItems==0)
        return(-1); //queue is empty
    else
        return(0); // there are content
}
en_errors putItem(tstrQueue *theQueue, int theItemValue)
{
    if(theQueue->validItems>=ELEMENTS_NUMBER)
    {
        return(1); //Queue is full
    }
    else
    {
        theQueue->validItems++;
        theQueue->data[theQueue->last] = theItemValue;
        theQueue->last = (theQueue->last+1)%ELEMENTS_NUMBER;
        return(0);
    }
}
en_errors getItem(tstrQueue *theQueue, int *theItemValue)
{
    if(isEmpty(theQueue))
    {
        return(-1); //queue is empty
    }
    else
    {
        *theItemValue=theQueue->data[theQueue->first];
        theQueue->first=(theQueue->first+1)%ELEMENTS_NUMBER;
        theQueue->validItems--;
        return(0); //No errors
    }
}
/////////////////////////////////////////////////////////////////////
//initialize the three queues one for running process , one for read processes and the last for the blocking processes
tstrQueue ready_q;
tstrQueue running_q;
tstrQueue blocking_q;
//_____________________________________________________________________________________________________________________________
void readfile (void)
{
    char line[RSIZ][LSIZ];
	char fname[20];
    FILE *fptr = NULL;
    int i = 0;
    int tot = 0;
    printf("\n\n Read the file and store the lines into an array :\n");
	printf("------------------------------------------------------\n");
	printf(" Input the filename to be opened : ");
	scanf("%s",fname);

    fptr = fopen(fname, "r");
    if (fptr == NULL) // if the folder name isn't exist
            {
              printf("Error! Could not open file\n");
              exit(-1); // must include stdlib.h // Exit the programme if the name doesnot exist
              }
    while(fgets(line[i], LSIZ, fptr))
	{
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    tot = i;
	printf("\n The content of the file %s  are : \n",fname);
    for(i = 0; i < tot; ++i)
    {
        printf(" %s\n", line[i]);
    }
    printf("\n");
 char row;
 char *token[128]; // to save segmented words

 for (row=0; row <tot ; ++row)
    {
        segmentation(line[row],&token);
        arr[row].num = atoi(token[0]);
        arr[row].cpu_time = atoi(token[1]);
        arr[row].io_time = atoi(token[2]);
        arr[row].arr_time = atoi(token[3]);
    }
    tot_global=tot;
}
//_______________________________________________________________________________________________________________________

void segmentation(char phrase[],char **token)
{
    int i=0;
    token[i]=strtok(phrase," "); // take the phrase and search for the delemeter which is tthe space and replace it with back slash 0 /0 and save the word in the token[i]

    while (token[i]!=NULL) // if the word is not null which means it's not the end of the command
    {
        i++;                // increase the counter to move to the next pointer
       token[i]=strtok(NULL," "); // from the last /0 it will complete the segmentation
    }
}
//____________________________________________________________________________________________________________________________


void FCFS(void);
void FCFS(void)
{
        FILE *file  = fopen("FCFS_log_file", "w"); // read only

          // test for files not existing.
         /* if (file == NULL)
            {
              printf("Error! Could not open file\n");
              exit(-1); // must include stdlib.h
            }*/

         initializeQueue(&ready_q);
         initializeQueue(&running_q);
         initializeQueue(&blocking_q);
        int num;// to save the process number which is the return of the getItem function
        int busy=0; // to store number of busy cycle of processor
        int cycle,flag=0;
        int i,j;
        for (i=0; i<tot_global;i++)
        {
            arr[i].cpu_remaining_time=arr[i].cpu_time-1;
            arr[i].io_remaining_time=arr[i].io_time;
            arr[i].done=0;
            arr[i].turnaround=0;
        }

        for(cycle=0;;cycle++) // cycles counter
            {
                if (isEmpty(&ready_q)==0) // check if there are processes are ready
                    {
                        if (isEmpty(&running_q)==-1)//if the running queue is empty
                        {
                            getItem(&ready_q,&num);
                            for(i=0; i<tot_global; i++) //loop to get the arrange of the process in the array
                            {
                                if (arr[i].num==num) // when the process number equals the return number from the queue
                                   {
                                        break; // get out of the loop
                                    }
                            }
                            putItem(&running_q,arr[i].num);// put this process with this number in the running queue
                            arr[i].status='r';
                        }
                    }


                for(i=0; i<tot_global; i++) // check for all process if any one arrives at this cycle
                    {
                        if (arr[i].arr_time==cycle)
                            {

                                for (j=i+1;j<tot_global; j++) // check if another process arrives at same moment
                                    {

                                        if(arr[j].arr_time==cycle) //if yes check the lowest ID
                                            {
                                                flag=1;

                                                if(arr[i].num > arr[j].num) // if process of i has the lowest ID
                                                    {

                                                        if (isEmpty(&running_q)==-1) // check if the runnng queue is empty , if yes
                                                            {

                                                                putItem(&running_q,arr[j].num); // put the process of i in the running queue
                                                                arr[j].status='r';// status is running
                                                                putItem(&ready_q,arr[i].num); // put the process of j in the ready queue
                                                                arr[i].status='D'; //status is ready
                                                            }
                                                        else if  (isEmpty(&running_q)!=-1)      // if the runnining queue is not empty
                                                            {
                                                                putItem(&ready_q,arr[j].num); //put both of them in the ready queue w.r.t the lowest ID
                                                                arr[j].status='D';// status is ready
                                                                putItem(&ready_q,arr[i].num);
                                                                arr[i].status='D';// status is ready
                                                            }
                                                    }

                                                if (arr[i].num < arr[j].num) //if the process of j has the lowest ID make the inverse of the above.
                                                    {

                                                        if (isEmpty(&running_q)==-1)
                                                            {
                                                                putItem(&running_q,arr[i].num);
                                                                arr[i].status='r';// status is running
                                                                putItem(&ready_q,arr[j].num);
                                                                arr[j].status='D';// status is ready

                                                            }
                                                        else if (isEmpty(&running_q)!=-1)
                                                            {
                                                                putItem(&ready_q,arr[j].num);
                                                                arr[i].status='D';// status is ready
                                                                putItem(&ready_q,arr[i].num);
                                                                arr[j].status='D';// status is ready
                                                            }
                                                    }


                                            }

                                    }
                                if (flag==0)
                                {

                                    if (isEmpty(&running_q)==-1) // check if the runnng queue is empty , if yes
                                        {
                                            putItem(&running_q,arr[i].num); // put the process of i in the running queue
                                            arr[i].status='r';// status is running

                                        }
                                    else
                                        {
                                            putItem(&ready_q,arr[i].num); // put the process of i in the ready queue
                                            arr[i].status='D';// status is ready
                                        }
                                }

                            }


                    }

                    flag=0;
                    printf("Cycle number %d\n",cycle); // print the current cycle
                    for(i=0; i<tot_global; i++) // for the number of process check the status and convert it to word saving in the struct of the process
                    {
                        if (arr[i].status=='r')
                        {
                            arr[i].string_status[0]='R';
                            arr[i].string_status[1]='u';
                            arr[i].string_status[2]='n';
                            arr[i].string_status[3]='n';
                            arr[i].string_status[4]='i';
                            arr[i].string_status[5]='n';
                            arr[i].string_status[6]='g';
                            arr[i].string_status[7]='\0';
                        }
                        else if (arr[i].status=='D')
                        {
                            arr[i].string_status[0]='R';
                            arr[i].string_status[1]='e';
                            arr[i].string_status[2]='a';
                            arr[i].string_status[3]='d';
                            arr[i].string_status[4]='y';
                            arr[i].string_status[5]='\0';
                        }
                        else if (arr[i].status=='b')
                        {
                            arr[i].string_status[0]='B';
                            arr[i].string_status[1]='l';
                            arr[i].string_status[2]='o';
                            arr[i].string_status[3]='c';
                            arr[i].string_status[4]='k';
                            arr[i].string_status[5]='i';
                            arr[i].string_status[6]='n';
                            arr[i].string_status[7]='g';
                            arr[i].string_status[8]='\0';
                        }
                        if (arr[i].status=='F')
                        {
                            arr[i].string_status[0]='F';
                            arr[i].string_status[1]='i';
                            arr[i].string_status[2]='n';
                            arr[i].string_status[3]='i';
                            arr[i].string_status[4]='s';
                            arr[i].string_status[5]='h';
                            arr[i].string_status[6]='e';
                            arr[i].string_status[7]='d';
                            arr[i].string_status[8]='\0';
                        }
                        // write to file
                        fprintf(file, "Process number %d: status is %s\n",arr[i].num,arr[i].string_status); // write to file
                        printf("-->process number %d : status is %s\n",arr[i].num,arr[i].string_status);
                    }

                    ///////////////////////////////////////////////running queue////////////////////////////////////////////////

                    for (i=0; i <tot_global; i++)//loop for the running process
                    {
                        if(arr[i].status=='r') // if it gets a process in running state then
                        {
                            busy++; // means the CPU is busy now as a certaion process is running regardless its case
                            if(arr[i].cpu_remaining_time!=0) // check if its cpu remaining time not equal zero , means it doesn't fininsh yet
                            {
                                arr[i].cpu_remaining_time --; // so decrease its remaining time by one cycle

                            }
                            else if(arr[i].cpu_remaining_time==0) // if the process cpu remaining time finished
                            {
                                getItem(&running_q,&num);            // free the running queue

                                if(arr[i].io_remaining_time!=0) //check if the same process if it has io request
                                {
                                    putItem(&blocking_q,arr[i].num); //if it has , so put it in blocking queue
                                    arr[i].status='b'; // and changes the status to blocking
                                }
                                else if(arr[i].io_remaining_time!=0) //it it doesn't have io request
                                {
                                    arr[i].done++; //number of complete times
                                    if(arr[i].done >=2)
                                    {
                                        arr[i].status='F';
                                        arr[i].turnaround=cycle-arr[i].arr_time+1; //it finishes
                                    }
                                    else if (arr[i].done <2)
                                    {
                                        putItem(&ready_q,arr[i].num); // put it in the ready queue
                                        arr[i].status='D'; // change the process status to ready
                                    }
                                }
                                arr[i].cpu_remaining_time=arr[i].cpu_time-1;
                            }
                            break;
                        }
                    }
                    ////////////////////////////////////////////////////////////////////////////////////////////////

                    /////////////////////////////////////////////////Blocking queue//////////////////////////////////////

                    for(i=0; i<tot_global; i++)// loop for all blocked processes
                    {
                        if(arr[i].status=='b')
                        {
                            if(arr[i].io_remaining_time!=0)
                            {
                                arr[i].io_remaining_time--;
                            }
                            else if(arr[i].io_remaining_time==0)
                            {
                                getItem(&blocking_q,&num); // get it out of the blocking queue
                                arr[i].done++; // number of complete times
                                if (arr[i].done >= 2) // if it is done twice so it finishes and don't put it again in any queue
                                {
                                    arr[i].status='F';
                                    arr[i].turnaround=cycle-arr[i].arr_time+1; //it finishes
                                }
                                else if (arr[i].done <2) //if it less than 2 so complete as usual
                                {
                                    putItem(&ready_q,arr[i].num); // put it in the ready queue
                                    arr[i].status='D';
                                    arr[i].io_remaining_time=arr[i].io_time; //to reset the time
                                }
                            }
                        }
                    }
                    int count=0; // initialize the variable to count the processes which finished
                    for (i=0; i<tot_global; i++) // for all processes
                    {
                        if(arr[i].done==2) // check process finishing times if it's done
                        {
                            count=count+2; // increamet the count by two
                        }
                    }
                    if(count==tot_global*2) // after that if count equal all process * 2 which means all processes are finished
                    {
                        break; // break the cycles
                    }


}// cycle bracket
printf("finish time is %d\n",cycle); // number of cycles
printf("CPU busy time: %d cycles\n",busy); // number of cycles which cpu was busyy through them
printf("CPU utilization %f\n",(busy*1.0)/(cycle+1)); // utilization of CPU = cpu busy time / no of cycles +1
for(i=0; i<tot_global; i++)
{
    printf("Turnaround of process %d is %d\n",arr[i].num,arr[i].turnaround);
}


}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////RR/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RR(void);
void RR(void)
{
            FILE *file  = fopen("RR_log_file", "w"); // read only

          // test for files not existing.
         /* if (file == NULL)
            {
              printf("Error! Could not open file\n");
              exit(-1); // must include stdlib.h
            }*/
         initializeQueue(&ready_q);
         initializeQueue(&running_q);
         initializeQueue(&blocking_q);
        int num;// to save the process number which is the return of the getItem function
        int busy=0; // to store number of busy cycle of processor
        int cycle,flag=0;
        int i,j,quantum,temp;
        printf("Enter the quantum\n");
        scanf("%d",&quantum);
        temp=quantum-1;
if (quantum >0)
{

        for (i=0; i<tot_global;i++)
        {
            arr[i].cpu_remaining_time=arr[i].cpu_time-1;
            arr[i].io_remaining_time=arr[i].io_time;
            arr[i].done=0;
            arr[i].turnaround=0;
        }

        for(cycle=0;;cycle++) // cycles counter
            {
                if (isEmpty(&ready_q)==0) // check if there are processes are ready
                    {
                        if (isEmpty(&running_q)==-1)//if the running queue is empty
                        {
                            getItem(&ready_q,&num);
                            for(i=0; i<tot_global; i++) //loop to get the arrange of the process in the array
                            {
                                if (arr[i].num==num) // when the process number equals the return number from the queue
                                   {
                                        break; // get out of the loop
                                    }
                            }
                            putItem(&running_q,arr[i].num);// put this process with this number in the running queue
                            arr[i].status='r';
                        }
                    }


                for(i=0; i<tot_global; i++) // check for all process if any one arrives at this cycle
                    {
                        if (arr[i].arr_time==cycle)
                            {

                                for (j=i+1;j<tot_global; j++) // check if another process arrives at same moment
                                    {

                                        if(arr[j].arr_time==cycle) //if yes check the lowest ID
                                            {
                                                flag=1;

                                                if(arr[i].num > arr[j].num) // if process of i has the lowest ID
                                                    {

                                                        if (isEmpty(&running_q)==-1) // check if the runnng queue is empty , if yes
                                                            {

                                                                putItem(&running_q,arr[j].num); // put the process of i in the running queue
                                                                arr[j].status='r';// status is running
                                                                putItem(&ready_q,arr[i].num); // put the process of j in the ready queue
                                                                arr[i].status='D'; //status is ready
                                                            }
                                                        else if  (isEmpty(&running_q)!=-1)      // if the runnining queue is not empty
                                                            {
                                                                putItem(&ready_q,arr[j].num); //put both of them in the ready queue w.r.t the lowest ID
                                                                arr[j].status='D';// status is ready
                                                                putItem(&ready_q,arr[i].num);
                                                                arr[i].status='D';// status is ready
                                                            }
                                                    }

                                                if (arr[i].num < arr[j].num) //if the process of j has the lowest ID make the inverse of the above.
                                                    {

                                                        if (isEmpty(&running_q)==-1)
                                                            {
                                                                putItem(&running_q,arr[i].num);
                                                                arr[i].status='r';// status is running
                                                                putItem(&ready_q,arr[j].num);
                                                                arr[j].status='D';// status is ready

                                                            }
                                                        else if (isEmpty(&running_q)!=-1)
                                                            {
                                                                putItem(&ready_q,arr[j].num);
                                                                arr[i].status='D';// status is ready
                                                                putItem(&ready_q,arr[i].num);
                                                                arr[j].status='D';// status is ready
                                                            }
                                                    }


                                            }

                                    }
                                if (flag==0)
                                {

                                    if (isEmpty(&running_q)==-1) // check if the runnng queue is empty , if yes
                                        {
                                            putItem(&running_q,arr[i].num); // put the process of i in the running queue
                                            arr[i].status='r';// status is running

                                        }
                                    else
                                        {
                                            putItem(&ready_q,arr[i].num); // put the process of i in the ready queue
                                            arr[i].status='D';// status is ready
                                        }
                                }

                            }


                    }

                    flag=0;
                    printf("Cycle number %d\n",cycle);
                    fprintf(file, "Cycle number %d\n",cycle); // write to file
                    for(i=0; i<tot_global; i++)
                    {
                        if (arr[i].status=='r')
                        {
                            arr[i].string_status[0]='R';
                            arr[i].string_status[1]='u';
                            arr[i].string_status[2]='n';
                            arr[i].string_status[3]='n';
                            arr[i].string_status[4]='i';
                            arr[i].string_status[5]='n';
                            arr[i].string_status[6]='g';
                            arr[i].string_status[7]='\0';
                        }
                        else if (arr[i].status=='D')
                        {
                            arr[i].string_status[0]='R';
                            arr[i].string_status[1]='e';
                            arr[i].string_status[2]='a';
                            arr[i].string_status[3]='d';
                            arr[i].string_status[4]='y';
                            arr[i].string_status[5]='\0';
                        }
                        else if (arr[i].status=='b')
                        {
                            arr[i].string_status[0]='B';
                            arr[i].string_status[1]='l';
                            arr[i].string_status[2]='o';
                            arr[i].string_status[3]='c';
                            arr[i].string_status[4]='k';
                            arr[i].string_status[5]='i';
                            arr[i].string_status[6]='n';
                            arr[i].string_status[7]='g';
                            arr[i].string_status[8]='\0';
                        }
                        if (arr[i].status=='F')
                        {
                            arr[i].string_status[0]='F';
                            arr[i].string_status[1]='i';
                            arr[i].string_status[2]='n';
                            arr[i].string_status[3]='i';
                            arr[i].string_status[4]='s';
                            arr[i].string_status[5]='h';
                            arr[i].string_status[6]='e';
                            arr[i].string_status[7]='d';
                            arr[i].string_status[8]='\0';
                        }
                        // write to file
                        fprintf(file, "Process number %d: status is %s\n",arr[i].num,arr[i].string_status); // write to file
                        printf("-->process number %d : status is %s\n",arr[i].num,arr[i].string_status);
                    }
                    ///////////////////////////////////////////////running queue////////////////////////////////////////////////

                    for (i=0; i<tot_global; i++)//loop for the running process
                    {
                        if(arr[i].status=='r') // if it gets a process in running state then
                        {
                            busy++;
                            if(arr[i].cpu_remaining_time!=0) // check if its cpu remaining time not equal zero , means it doesn't fininsh yet
                            {
                                if(temp!=0)
                                {
                                    arr[i].cpu_remaining_time --; // so decrease its remaining time by one cycle
                                    temp--;
                                }
                                else if (temp==0) // the process quantum is finished
                                {
                                    arr[i].cpu_remaining_time --; // so decrease its remaining time by one cycle
                                    getItem(&running_q,&num);            // free the running queue
                                    putItem(&ready_q,arr[i].num); // put it in the ready queue
                                    arr[i].status='D';
                                    temp=quantum-1;
                                }
                            }
                            else if(arr[i].cpu_remaining_time==0) // if the process cpu remaining time finished
                            {
                                getItem(&running_q,&num);            // free the running queue
                                temp=quantum-1;
                                if(arr[i].io_remaining_time!=0) //check if the same process  has io request
                                {
                                    putItem(&blocking_q,arr[i].num); //if it has , so put it in blocking queue
                                    arr[i].status='b'; // and changes the status to blocking
                                }
                                else if(arr[i].io_remaining_time==0) //it it doesn't have io request
                                {
                                    arr[i].done++; //number of complete times
                                    if(arr[i].done >=2)
                                    {
                                        arr[i].status='F';
                                        arr[i].turnaround=cycle-arr[i].arr_time+1; //it finishes
                                    }
                                    else if (arr[i].done <2)
                                    {
                                        putItem(&ready_q,arr[i].num); // put it in the ready queue
                                        arr[i].status='D'; // change the process status to ready
                                    }
                                }
                                arr[i].cpu_remaining_time=arr[i].cpu_time-1;
                            }
                            break;
                        }
                    }
                    ////////////////////////////////////////////////////////////////////////////////////////////////

                    /////////////////////////////////////////////////Blocking queue//////////////////////////////////////

                    for(i=0; i<tot_global; i++)// loop for all blocked processes
                    {
                        if(arr[i].status=='b')
                        {
                            if(arr[i].io_remaining_time!=0)
                            {
                                arr[i].io_remaining_time--;
                            }
                            else if(arr[i].io_remaining_time==0)
                            {
                                getItem(&blocking_q,&num); // get it out of the blocking queue
                                arr[i].done++; // number of complete times
                                if (arr[i].done >= 2) // if it is done twice so it finishes and don't put it again in any queue
                                {
                                    arr[i].status='F';
                                    arr[i].turnaround=cycle-arr[i].arr_time+1; //it finishes
                                }
                                else if (arr[i].done <2) //if it less than 2 so complete as usual
                                {
                                    putItem(&ready_q,arr[i].num); // put it in the ready queue
                                    arr[i].status='D';
                                    arr[i].io_remaining_time=arr[i].io_time; //to reset the time
                                }
                            }
                        }
                    }
                    int count=0;
                    for (i=0; i<tot_global; i++)
                    {
                        if(arr[i].done==2)
                        {
                            count=count+2;
                        }
                    }
                    if(count==tot_global*2)
                    {
                        break;
                    }


}//cycle bracket
printf("finish time is %d\n",cycle);
printf("CPU busy time: %d cycles\n",busy);
printf("CPU utilization %f\n",(busy*1.0)/cycle);
for(i=0; i<tot_global; i++)
{
    printf("Turnaround of process %d is %d\n",arr[i].num,arr[i].turnaround);
}

}
else
{
    printf("The quantum must be greater than 0\n");
}
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    FCFS_sched=0,
    RR_sched=1
}enumsched;
int main(void)
{
    enumsched scanned;
    printf("Welcome :)\nPlease enter your scheduling algoirtm\n0)FCFS\n1)RR\nYour choice:");
    scanf("%d",&scanned);
    if((scanned==FCFS_sched)||(scanned==RR_sched)) // if the choice is right call readfile func
    {
        readfile();
        if(scanned==FCFS_sched)
            {FCFS();}
        else
        {
            RR();
        }
    }
    else
     {
     printf("Oprion error");
     }
return 0;

}

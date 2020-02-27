#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define NUM_P 10 
//Initial data will be stored in files(including list sizes and process stuff)
//No.of memory nodes is stored i the file i.e till file is not empty

//This part has structures for the memory nodes.....
typedef enum {FALSE,TRUE} bool;
typedef struct Mem_Node_tag
{
    int block_id;
    int block_size;
    bool alloted_status;
    struct Mem_Node_tag *next;
}Mem_Node;
//---------------------------------------------------

//Structures for process and its queue
typedef enum {ACTIVE,RUNNING,WAITING,COMPLETED} status_of_process;
typedef struct Process_tag
{
    int process_id;
    time_t timestamp;//Time when process was created,gives in seconds elapsed from a specific date
    int burst_time;//Time required by process to execute in seconds
    int memory_requirement;
    int priority;
    status_of_process status;
}Process;
typedef struct Process_node_tag
{
    Process P;
    int block_id;
    struct Process_node_tag *next;
}Process_Node;

typedef struct Queue_tag
{
    //Process_Node *lastPN;//Circular LL?
    Process_Node *front;
    Process_Node *rear;
}Process_Queue;

typedef struct PQ_tag
{
    Process_Queue PQ[NUM_P];//Priority queue
}Priority_Process_Queue;
//----------------------------------------------------------------------

//Structure for status _code--------------------------------------------
typedef enum {FAILURE,SUCCESS} status_code;
//----------------------------------------------------------------------

//-------------------------------Functions------------------------------
//---------For Memory Nodes---------------------------------------------
Mem_Node* MakeMemoryNode(int block_id,int block_size)//Tested==True
{
    //This function just creates a node of type Mem_Node
    Mem_Node *ptr;
    ptr=(Mem_Node*)malloc(sizeof(Mem_Node));
    if(ptr!=NULL)
    {
        ptr->alloted_status=FALSE;
        ptr->block_id=block_id;
        ptr->block_size=block_size;
        ptr->next=NULL;
    }
    return ptr;
}

status_code InitializeMemList(Mem_Node **mptr)//Tested==True
{
    FILE *fp;
    //The file is assumed to have info in form of id first and then size i.e.   id size
    int id,size,flag=0;
    status_code sc=SUCCESS;
    Mem_Node *lptr,*ptr,*nptr;
    lptr=MakeMemoryNode(0,0);//Dummy Node
    ptr=lptr;
    fp=fopen("MemoryBlocks.txt","r");
    while(((fscanf(fp,"%d%d",&id,&size))!=EOF)&&flag==0)
    {
        nptr=MakeMemoryNode(id,size);
        if(nptr!=NULL)
        {
            ptr->next=nptr;
            ptr=nptr;
        }
        else
        {
            sc=FAILURE;
            flag=1;
        }
    }
    ptr=lptr;
    lptr=lptr->next;
    free(ptr);
    fclose(fp);
    *mptr=lptr;
    return sc;
}

void DisplayMemList(Mem_Node *mptr)//Tested==True
{
    //This shows the status of the complete memory blocks
    while(mptr!=NULL)
    {
        printf("\nID:%d SIZE:%d ",mptr->block_id,mptr->block_size);
        if(mptr->alloted_status==FALSE)
        {
            printf("ALLOTED STATUS:FALSE");
        }
        else
        {
            printf("ALLOTED STATUS:TRUE");
        }
        mptr=mptr->next;
    }
}

void DisplayFreeBlocks(Mem_Node *mptr)
{
    printf("\nThe free blocks are:");
    while(mptr!=NULL)
    {
        if(mptr->alloted_status==FALSE)
        {
            printf("\nID:%d SIZE:%d ",mptr->block_id,mptr->block_size);
        }
        mptr=mptr->next;
    }
}
//-----------------------------------------

//----------------------------------------------------------
//Functions for Process and its queues
Process_Node* MakeProcessNode(int process_id,int burst_time,int memory_requirement,int priority)//Tested==True
{
    Process_Node *ptr;
    time_t lt;
    ptr=(Process_Node *)malloc(sizeof(Process_Node));
    if(ptr!=NULL)
    {
        (ptr->P).process_id=process_id;
        (ptr->P).burst_time=burst_time;
        (ptr->P).memory_requirement=memory_requirement;
        (ptr->P).priority=priority;
        
        (ptr->P).timestamp=time(NULL);
        //Doubtful for different meannings ,ask once
        (ptr->P).status=ACTIVE;
        //Include line for timestamp here
        //
        //--------------------------------
        ptr->block_id=-1;//To signify no block has been allocated yet
        ptr->next=NULL;
    }
    return ptr;
}

bool IsPQEmpty(Process_Queue P)//Tested==True
{
    bool retval=FALSE;
    if(P.front==NULL)
    {
        retval=TRUE;
    }
    return retval;
}

status_code InsertProcess(Priority_Process_Queue *qptr,Process_Node *ptr)//Tested==True
{
    status_code sc=SUCCESS;
    int p;
    p=(ptr->P).priority;
    if(IsPQEmpty(qptr->PQ[p])==TRUE)//That queue is empty
    {
        qptr->PQ[p].front=qptr->PQ[p].rear=ptr;
    }
    else
    {
        qptr->PQ[p].rear->next=ptr;
        qptr->PQ[p].rear=ptr;
    }
}

status_code InitializeProcessQueue(Priority_Process_Queue *qptr)//Tested==True
{
    Process_Node *ptr;
    int i=0,flag=0;
    int pid,burst_time,memory_requirement,priority;
    status_code sc=SUCCESS;
    FILE *fp;
    for(i=0;i<NUM_P;i++)
    {
        (qptr->PQ[i]).rear=(qptr->PQ[i]).front=NULL;
    }
    fp=fopen("ProcessInfo.txt","r");
    //The file will contain info in form of: pid burst_time memory_requirement priority
    /*
        NOTE:the type of burst time is not known yet....,it has to be adjusted as the timestamp stuff?
    */
    while((fscanf(fp,"%d%d%d%d",&pid,&burst_time,&memory_requirement,&priority))!=EOF&&flag==0)
    {
        ptr=MakeProcessNode(pid,burst_time,memory_requirement,priority);
        if(ptr!=NULL)
        {
            sc=InsertProcess(qptr,ptr);
            if(sc==FAILURE)
            {
                flag=1;
            }
        }
        else
        {
            sc=FAILURE;
            flag=1;
        }
    }  
    fclose(fp);
    return sc;
}

void DisplayProcessQueue(Priority_Process_Queue *qptr)//For my debugging purpose
{
    int i=0;
    Process_Node *ptr;
    for(i=0;i<NUM_P;i++)
    {
        printf("\nPriority Level:%d ",i);
        if(IsPQEmpty(qptr->PQ[i]))
        {
            printf(" Empty");
        }
        else
        {
            ptr=qptr->PQ[i].front;
            while(ptr!=NULL)
            {
                printf(" Process ID:%d MemoryRequired:%d Time Remaining;%d Timestamp:%ld ",ptr->P.process_id,ptr->P.memory_requirement,ptr->P.burst_time,ptr->P.timestamp);
                ptr=ptr->next;
            }
        }
    }
}
//-----------------------------------------------------------------------------------

//------------Functions to allot memory to a process-----------------------------------
status_code AllotMemoryToProcessFirstFit(Mem_Node *lptr,Process_Node *ptr)
{
    Mem_Node *tptr;
    status_code sc=SUCCESS;
    tptr=lptr;
    int flag=0;
    if(lptr!=NULL)
    {   
        while(tptr!=NULL&&flag==0)
        {
            if(tptr->block_size>=ptr->P.memory_requirement)
            {
                if(tptr->alloted_status==FALSE)
                {
                    ptr->P.status=RUNNING;
                    ptr->block_id=tptr->block_id;
                    tptr->alloted_status=TRUE;
                    flag=1;
                }
            }
            tptr=tptr->next;
        }
        if(ptr->P.status!=RUNNING)//Memory could not be alloted
        {
            sc=FAILURE;
        }      
    }
    else
    {
        sc=FAILURE;
    }
    return sc;
}

status_code AllotMemoryToProcessBestFit(Mem_Node *lptr,Process_Node *ptr)
{
    Mem_Node *tptr,*best;
    status_code sc=SUCCESS;
    tptr=lptr;
    best=NULL;
    int flag=0,size;
    size=ptr->P.memory_requirement;
    if(lptr!=NULL)
    {
        while(tptr!=NULL)
        {
            if(tptr->block_size>=size)
            {
                if(tptr->alloted_status==FALSE)
                {
                    if(best==NULL)
                    {
                        best=tptr;
                    }
                    else if(best->block_size>tptr->block_size)
                    {
                        best=tptr;
                    }
                }
            }
            tptr=tptr->next;
        }
        if(best!=NULL)//Some memory was present
        {
            ptr->P.status=RUNNING;
            ptr->block_id=best->block_id;
            best->alloted_status=TRUE;
        }
        else
        {
            sc=FAILURE;
        }
    }
    else
    {
        sc=FAILURE;
    }
    return sc;
}

status_code ReturnMemoryToMemList(Process_Node *ptr,Mem_Node *lptr)
{
    status_code sc=SUCCESS;
    int block_id,flag=0;
    Mem_Node *mptr;
    block_id=ptr->block_id;
    mptr=lptr;
    if(block_id!=-1)
    {
        while(mptr!=NULL&&flag==0)
        {
            if(mptr->block_id==block_id)
            {
                flag=1;
                mptr->alloted_status=FALSE;
                ptr->block_id=-1;
                //If Needed-------------
                //ptr->P.status=WAITING;
                //----------------------
            }
        }
    }
}
//-----------------------------------------------------------------------
//Functions to delete a process from the Priority Queue
void DeleteProcessFromQueue(Priority_Process_Queue *qptr,Process_Node *ptr)
{
    Process_Node *prev=NULL;
    prev=qptr->PQ[ptr->P.priority].front;
    if(prev!=ptr)
    {
        while(prev->next!=ptr)
        {
            prev=prev->next;
        }
    }
    else
    {
        prev=NULL;
    }
    if(prev==NULL)//Then the to be deleted node is the first node
    {
        qptr->PQ[ptr->P.priority].front=ptr->next;
        if(ptr->next==NULL)
        {
            qptr->PQ[ptr->P.priority].rear=ptr->next;
        }
        free(ptr);
    }
    else
    {
        prev->next=ptr->next;
        if(ptr->next==NULL)
        {
            qptr->PQ[ptr->P.priority].rear=prev;
        }
        free(ptr);
    }
}

Process_Node* GetProcessFromQueueToProcess(Priority_Process_Queue *qptr/*,Process_Node *current_highest*/)
{
    Process_Node *ptr,*prev,*retval;
    int i=0;
    while(i<NUM_P&&IsPQEmpty(qptr->PQ[i])==TRUE)
    {
        i++;
    }
    if(i==NUM_P)
    {
        retval=NULL;
        //sc=FAILURE;//No process to execute...
    }
    else
    {
        //Since it is in a queue fashion,we are currently assuming the process to be sorted according to the timestamps
        retval=qptr->PQ[i].front;
        
        /*if(current_highest->P.priority<retval->P.priority)
        {
            //Continue executing curr_highest
        }
        else
        {
            current_highest->P.status=WAITING;
            //ReturnMemoryToMemList
            retval->P.status=RUNNING;
            //AllocateMemory(Best/First Fit)
        }*/
    }
    return retval;
}
//-----------------------------------------------------------------------
time_t ProcessorStart(Mem_Node *mptr,Priority_Process_Queue *qptr,time_t prev_time)
{
    Process_Node *curr_highest=NULL;
    time_t curr_time=time(NULL);
    double elapsed_time; 
    int flag=0;
    elapsed_time=difftime(curr_time,prev_time);
    //Take the top priority process and start its execution
    while(elapsed_time!=0&&flag==0)
    {
        curr_highest=GetProcessFromQueueToProcess(qptr);
        if(curr_highest!=NULL)
        {
            if(elapsed_time>=curr_highest->P.burst_time)
            {
                elapsed_time-=curr_highest->P.burst_time;
                curr_highest->P.status=COMPLETED;
                printf("\n Process %d COMPLETED:",curr_highest->P.process_id);
                DeleteProcessFromQueue(qptr,curr_highest);
                //RemoveProcess
            }
            else if(elapsed_time<curr_highest->P.burst_time)
            {
                curr_highest->P.burst_time-=elapsed_time;
                curr_highest->P.status=RUNNING;
                elapsed_time=0;
            }
        }
        else
        {
            flag=1;
            printf("\nAll processes executed");
        }
    }  
    return curr_time;  
}
//-----------------------------------------------------------------------
//main to test Init of memnodes
void main()
{
    
    //For debugging Memlist
    Mem_Node *mptr;
    time_t prev_time;
    int flag=0,code;
    InitializeMemList(&mptr);
    DisplayMemList(mptr);
    //DisplayFreeBlocks(mptr);
    //For debugging Process queues
    Priority_Process_Queue q;
    InitializeProcessQueue(&q);
    DisplayProcessQueue(&q);
    prev_time=time(NULL);
    //int a;
    //printf("\nEnter");
    //scanf("%d",&a);
    //curr_time=time(NULL);
    //prev_time=ProcessorStart(mptr,&q,prev_time);
    //printf("\n---------------------------------------");
    //DisplayProcessQueue(&q);
    //AllotMemoryToProcessBestFit(mptr,q.PQ[1].front);
    //DisplayMemList(mptr);
    int pid,burst_time,memory_requirement,priority;
    Process_Node *ptr;
    status_code sc;
    //Basic UI
    while(flag==0)
    {
        printf("\nWhat do you want to do:");
        printf("\n1.Add Process");
        printf("\n2.View Status");
        printf("\n3.Exit");
        scanf("%d",&code);
        switch(code)
        {
            case 1: prev_time=ProcessorStart(mptr,&q,prev_time);//Execute from prev time to current
                    printf("\n---------------------------------------");
                    //Ask for process details
                    printf("\nAbove is the current state:");
                    printf("\nEnter the details:Process_ID,burst Time,memory,priority:");
                    scanf("%d%d%d%d",&pid,&burst_time,&memory_requirement,&priority);
                    ptr=MakeProcessNode(pid,burst_time,memory_requirement,priority);
                    sc=InsertProcess(&q,ptr);
                    if(sc==SUCCESS)
                    {
                        printf("\nSUCCESS IN INSERTING");
                    }
                    break;

            case 2: prev_time=ProcessorStart(mptr,&q,prev_time);
                    printf("\n---------------------------------------");
                    DisplayProcessQueue(&q);
                    break;

            default:
                    flag=1;
                    break;
        }
    }
    
}

//-------------------------------------------




#include<stdio.h>
#include<stdlib.h>
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
    int timestamp;//Time when process was created
    int burst_time;//Time required by process to execute
    int memory_requirement;
    int priority;
    status_of_process status;
}Process;
typedef struct Process_node_tag
{
    Process P;
    struct Process_node_tag *next;
}Process_Node;

typedef struct Queue_tag
{
    Process_Node *lastPN;//Circular LL?
    /*  
        if not circular,
        Process_Node *front;
        Process_Node *rear;
    */
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

//--------------------------------------------
//main to test Init of memnodes
void main()
{
    Mem_Node *mptr;
    InitializeMemList(&mptr);
    //DisplayMemList(mptr);
    DisplayFreeBlocks(mptr);
}

//-------------------------------------------




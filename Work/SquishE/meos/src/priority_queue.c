

/*+
UPDATE IDEA :
USING STATIC STRUCTURE BY ADDING size_t allocated_size; and a table of PriorityQueue as the final structure adapat the algorithm from that.

*/
#include <stdio.h>
#include <stdlib.h>


struct PriorityQueueElem
{
  void * point;
  double priority;
  struct PriorityQueueElem *next_entry;
};

typedef struct PriorityQueue
{
  struct PriorityQueueElem *head;
  size_t size;
} PriorityQueue;


struct PriorityQueue *
create_PriorityQueue(){
  struct PriorityQueue * result =  malloc(sizeof(struct PriorityQueue));
  result->head = NULL;
  result->size = 0;
  return result;
}


size_t size_queue(const PriorityQueue *Q)
{
    return Q->size;
}

struct PriorityQueueElem *remove_min(PriorityQueue *Q)
{
  struct PriorityQueueElem *result = Q->head;
  if(result != NULL){
    Q->head = Q->head->next_entry;
    Q->size--;
  }
  return result;
}

struct PriorityQueueElem *
remove_elem(void *p_i ,PriorityQueue *Q)
{
  struct PriorityQueueElem *before = NULL;
  struct PriorityQueueElem *current = Q->head;
  while(current != NULL){
    if(current->point == p_i){
        Q->size--;
        if(before != NULL){
            before->next_entry = current->next_entry;
        }
        else{
            Q->head = current->next_entry;
        }
        return current;
    }
    before = current;
    current = current->next_entry;
  }
  return current;
}


void
push(struct PriorityQueueElem * insert,PriorityQueue *Q)
{
  Q->size++;
  struct PriorityQueueElem *before = NULL;
  struct PriorityQueueElem *current = Q->head;
  while(current != NULL){
    if(current->priority >= insert->priority)
    {
        insert->next_entry = current;
        if(before != NULL)
        {
            before->next_entry = insert;
        }
        else
        {
            Q->head = insert;
        }
        return;
    }

    before = current;
    current = current->next_entry;
  }
  if(before != NULL)
  {
      before->next_entry = insert;
  }
  else
  {
      Q->head = insert;
  }
}


void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q)
{
  struct PriorityQueueElem * insert = remove_elem(p_i,Q);
  if(insert == NULL){
    insert = malloc(sizeof(struct PriorityQueueElem));
    insert->point = p_i;
    insert->priority = priority;
    insert->next_entry = NULL;
  }
  push(insert,Q);
}

void destroy_Queue(struct PriorityQueue *Q)
{
   struct PriorityQueueElem *current = Q->head;
   while (current != NULL)
   {
        struct PriorityQueueElem *temp = current;
        current = current->next_entry;
        free(temp);
   }
   free(Q);
}





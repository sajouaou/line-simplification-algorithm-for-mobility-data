

/*+
UPDATE IDEA :
USING STATIC STRUCTURE BY ADDING size_t allocated_size; and a table of PriorityQueue as the final structure adapat the algorithm from that.

*/
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

typedef struct PriorityQueueElem
{
  void * point;
  double priority;
  int index; //reference his own index
} PriorityQueueElem;

typedef void* IDict;

typedef struct PriorityQueue
{
  PriorityQueueElem **arr;
  IDict dict;
  size_t size;
  size_t capacity;
} PriorityQueue;

struct PriorityQueue *
create_PriorityQueue(int capacity);
size_t size_queue(const PriorityQueue *Q);
void minHeapify(PriorityQueue* Q, int index);
PriorityQueueElem *remove_min(PriorityQueue *Q);
struct PriorityQueueElem *
replace_elem(void *p_i,double priority ,PriorityQueue *Q);
void insertHelper(PriorityQueue* Q, int index);
void
push(PriorityQueueElem * insert,PriorityQueue *Q);
void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q);
void destroy_Queue(struct PriorityQueue *Q);

struct PriorityQueue *
create_PriorityQueue(int capacity){
    // Allocating memory to heap h
    PriorityQueue* h = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    // Checking if memory is allocated to h or not
    if (h == NULL) {
        //printf("Memory error");
        return NULL;
    }
    // set the values to size and capacity
    h->size = 0;
    h->capacity = capacity;

    // Allocating memory to array
    h->arr = (PriorityQueueElem**)malloc(capacity * sizeof(PriorityQueueElem*));

    // Checking if memory is allocated to h or not
    if (h->arr == NULL) {
        //printf("Memory error");
        return NULL;
    }
    return h;
}


size_t size_queue(const PriorityQueue *Q)
{
    return Q->size;
}

void minHeapify(PriorityQueue* Q, int index)
{
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    // Checking whether our left or child element
    // is at right index or not to avoid index error
    if (left >= Q->size || left < 0)
        left = -1;
    if (right >= Q->size || right < 0)
        right = -1;

    // store left or right element in min if
    // any of these is smaller that its parent
    if (left != -1 && Q->arr[left]->priority < Q->arr[index]->priority)
        min = left;
    if (right != -1 && Q->arr[right]->priority < Q->arr[index]->priority)
        min = right;

    // Swapping the nodes
    if (min != index) {
        PriorityQueueElem *temp = Q->arr[min];
        Q->arr[min] = Q->arr[index];
        Q->arr[index] = temp;

        Q->arr[min]->index = min;
        Q->arr[index]->index = index;

        // recursively calling for their child elements
        // to maintain min heap
        minHeapify(Q, min);
    }
}


int compar_index(const void *l, const void *r)
{
    const PriorityQueueElem *lm = l;
    const PriorityQueueElem *lr = r;
    return lm->point - lr->point;
}

PriorityQueueElem *remove_min(PriorityQueue *Q)
{
  PriorityQueueElem *result = NULL;
  if (Q->size != 0) {
        result = Q->arr[0];
        // Replace the deleted node with the last node
        Q->arr[0] = Q->arr[Q->size - 1];
        Q->arr[Q->size - 1] = NULL;
        if(Q->arr[0]){
            Q->arr[0]->index = 0;
        }

        // Decrement the size of heap
        Q->size--;

        tdelete(result,&Q->dict,compar_index);
        // Call minheapify_top_down for 0th index
        // to maintain the heap property
        minHeapify(Q, 0);

  }
  return result;
}


PriorityQueueElem *get_elem(void *p_i,IDict *dict)
{
    PriorityQueueElem find;
    find.point = p_i;
    PriorityQueueElem * result = tfind(&find, dict, compar_index);
    if(result){
        result = (*(PriorityQueueElem**)result);
    }
    return result;

}




struct PriorityQueueElem *
replace_elem(void *p_i,double priority ,PriorityQueue *Q)
{
  PriorityQueueElem *elem = get_elem(p_i,&Q->dict);
  if(elem ){
    //elog(NOTICE,"FOUND ELEM FOR SUR \n Index :  %i / SIZE : %i ",elem->index ,Q->size);
    if(Q->size > elem->index && elem->index != -1){
    //elog(NOTICE,"REPLACE ELEM FOR SUR");
        Q->arr[elem->index]->priority = priority;
        insertHelper(Q, elem->index);
        minHeapify(Q, elem->index);
    }
  }
  return elem;
}

void insertHelper(PriorityQueue* Q, int index)
{
    // Store parent of element at index
    // in parent variable
    int parent = (index - 1) / 2;

    if (index > 0 && index < Q->size && parent >= 0 && parent < Q->size) {
        if( Q->arr[parent]->priority > Q->arr[index]->priority){
            // Swapping when child is smaller
            // than parent element
            PriorityQueueElem *temp = Q->arr[parent];
            Q->arr[parent] = Q->arr[index];
            Q->arr[index] = temp;

            Q->arr[parent]->index = parent;
            Q->arr[index]->index = index;

            // Recursively calling insertHelper
            insertHelper(Q, parent);
        }
    }

}


void
push(PriorityQueueElem * insert,PriorityQueue *Q)
{
  Q->size++;
  if(Q->size > Q->capacity){
    Q->arr = realloc(Q->arr, Q->size * sizeof(PriorityQueueElem*));
  }
  Q->arr[Q->size-1] = insert;
  Q->arr[Q->size-1]->index = Q->size-1;
  insertHelper(Q, Q->size-1);
}


void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q)
{
  struct PriorityQueueElem * insert = replace_elem(p_i,priority,Q);
  if(insert == NULL){
    //elog(NOTICE,"INSERT ELEM FOR SUR");
    insert = malloc(sizeof(struct PriorityQueueElem));
    insert->point = p_i;
    insert->priority = priority;
    insert->index = -1;
    tsearch(insert, &Q->dict, compar_index); /* insert */
  }
  if(insert->index == -1){
    push(insert,Q);
  }
}

void i_free(void *l)
{
    free(l);
}

void destroy_Queue(struct PriorityQueue *Q)
{
   free(Q->arr);
   tdestroy(Q->dict,i_free);
   free(Q);
}








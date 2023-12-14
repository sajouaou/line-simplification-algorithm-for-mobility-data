

/*+
UPDATE IDEA :
USING STATIC STRUCTURE BY ADDING size_t allocated_size; and a table of PriorityQueue as the final structure adapat the algorithm from that.

*/

struct PriorityQueue
{
  void * point;
  double priority;
  struct PriorityQueue *next_entry;
  size_t size;
};


static struct PriorityQueue *
create_PriorityQueue(){
  struct PriorityQueue * result =  malloc(sizeof(struct PriorityQueue));
  result->next_entry = NULL;
  result->point = NULL;
  result->size = 0;
  return result;
}


size_t size_queue(const struct PriorityQueue *Q)
{
    return Q->size;
}

struct PriorityQueue* remove_min(struct PriorityQueue *Q)
{
  struct PriorityQueue *result = Q->next_entry;
  if(Q->point != NULL)
  {
     if(result == NULL)
     {
       result = create_PriorityQueue();

     }
     void *point = Q->point;
     Q->point = result->point;
     result->point = point;

     double c_priority = Q->priority;
     Q->priority = result->priority;
     result->priority = c_priority;

     Q->next_entry = result->next_entry;
     Q->size--;
  }

  return result;
}

void
set_priority_queue(void *p_i,double priority ,struct PriorityQueue *Q)
{
  Q->size++;
  // Check if p_i is already there and at the same time check position to put priority
  struct PriorityQueue * before_position = NULL;

  struct PriorityQueue * current = Q;
  struct PriorityQueue * before_current = NULL;
  struct PriorityQueue * insert = NULL;
  struct PriorityQueue * before_insert = NULL;


  //printf("Check if p_i is already there and at the same time check position to put priority \n");
  while(current != NULL && current->point != NULL &&  (insert == NULL || before_position == NULL) )
  {

    //printf("Value : %lf compare with %lf \n", current->priority,priority);
    if(current->point == p_i && insert == NULL)
    {
      Q->size--;
      insert = current;
      before_insert = before_current;
    }
    if(current->point != p_i && current->priority < priority && current->next_entry != NULL &&
      current->next_entry->priority > priority &&
      before_position == NULL)
    {

      //printf("INSERT POSITION : %lf , %lf \n",*(double *)(current->point), current->priority);
      before_position = current;
    }
    else if(current->next_entry == NULL && current->priority <= priority)
    {
      if(current == insert)
      {
        before_position = before_current;
      }
      else
      {
        before_position = current;
      }
      //printf("INSERT POSITION : %lf , %lf \n",*(double *)(current->point), current->priority);
    }

    before_current = current;
    current = current->next_entry;
  }

   //printf("Insert in a sorted list \n");
  // Insert in a sorted list

  if(before_insert != NULL)
  {
    // Case insert was found and remove it from queue
    //printf("Case insert was found and remove it from queue\n");
    before_insert->next_entry = insert->next_entry;
  }
  else if(insert == Q)
  {
    // Case insert was found and is the top of queue
    insert = remove_min(Q);
    Q->size++;
  }

  if(insert == NULL &&  Q->point != NULL)
  {
    // Case insert is not found
    //printf("Case insert is not found\n");
    insert = create_PriorityQueue();
  }

  if(insert != NULL)
  {
    insert->point = p_i;
    insert->priority = priority;
  }


  if(before_position != NULL)
  {
    // Case there is an entry before the position
    //printf("Case there is an entry before the position\n");
    struct PriorityQueue *next_entry = before_position->next_entry;
    before_position->next_entry = insert;
    if(next_entry != insert)
    {
        insert->next_entry = next_entry;
    }
  }
  else
  {
    // Case insert in beginning
    //printf("Case insert in beginning\n");
    if(Q->point == NULL) // Queue head is empty
    {
      // Case Queue head is not initialized
      //printf("Case Queue head is not initialized\n");

      current->point = p_i;
      current->priority = priority;

    }
    else
    {
      // Case insert before Queue head or at the end
      //printf("Case insert before Queue head \n");
      void *point = Q->point;
      Q->point = p_i;
      insert->point = point;

      double c_priority = Q->priority;
      Q->priority = priority;
      insert->priority = c_priority;

      struct PriorityQueue *next_entry = Q->next_entry;
      Q->next_entry = insert;
      insert->next_entry = next_entry;

    }
  }

}

void destroy_Queue(struct PriorityQueue *Q)
{
    while (Q != NULL)
    {
        struct PriorityQueue *temp = Q;
        Q = Q->next_entry;
        free(temp);
    }
}





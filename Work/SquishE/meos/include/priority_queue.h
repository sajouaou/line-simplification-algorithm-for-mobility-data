typedef struct PriorityQueueElem
{
  void * point;
  double priority;
  int index;
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
remove_elem(void *p_i ,PriorityQueue *Q);
void insertHelper(PriorityQueue* Q, int index);
void
push(PriorityQueueElem * insert,PriorityQueue *Q);
void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q);
void destroy_Queue(struct PriorityQueue *Q);
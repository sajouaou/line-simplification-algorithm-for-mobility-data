#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h> // Ajouter en haut du fichier



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
PriorityQueueElem *remove_min(PriorityQueue *Q);
struct PriorityQueueElem *
remove_elem(void *p_i ,PriorityQueue *Q);
void insertHelper(PriorityQueue* Q, int index);
void
push(PriorityQueueElem * insert,PriorityQueue *Q);
void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q);
void destroy_Queue(struct PriorityQueue *Q);

void printPriorityQueue(PriorityQueue *pq)
{
    printf("PRINT---------\n");
    for(int i=0;i < pq->size;i++)
    {

        struct PriorityQueueElem *current = pq->arr[i];
        printf("Point: %p, Priority: %lf\n", current->point, current->priority);
    }
}

void perf_test(PriorityQueue *pq,int num_insertions,int ecart_ )
{
   clock_t start, end;
   double cpu_time_used;
   start = clock();
   int ecart = ecart_;
   for (int i = 0; i < num_insertions; i++) {
        int priority = rand() % 100; // Génère une priorité aléatoire entre 0 et 99
        double *newElement = malloc(sizeof(double)); // Assurez-vous que votre type d'élément est correctement défini
        // Initialisez votre nouvel élément ici, si nécessaire

        set_priority_queue(newElement, priority,pq); // Remplacez cette fonction par votre fonction d'insertion

        if (i%ecart == 0){
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Insertion %d - Temps d'exécution : %f secondes - Execution by insertion : %f secondes \n",
             i+1, cpu_time_used,cpu_time_used/((i+1) - (i+1 - ecart)) );
            ecart = ecart *2;
        }
   }

   ecart = ecart_;
   for (int i = 0; i < num_insertions; i++) {
        struct PriorityQueueElem *min_entry = remove_min(pq);
        if (i%ecart == 0){
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Remove %d FROM SIZE %i -> TO SIZE %i \n - Temps d'exécution : %f secondes - Execution by remove : %f secondes \n",
             i+1,pq->size + i , pq->size, cpu_time_used,cpu_time_used/((i+1) - (i+1 - ecart)) );
            ecart = ecart *2;
        }

        free(min_entry);
   }
}



int main()
{
    // Créer une file de priorité vide
    struct PriorityQueue *queue = create_PriorityQueue(100);

    // Ajouter des éléments à la file de priorité
    double value1 = 3.14;
    double value2 = 2.718;
    double value3 = 1.618;
    // Mesurer le temps pour set_priority_queue
    clock_t start, end;
    double cpu_time_used;

    printf("BEGIN TO SET ___ - \n");
    set_priority_queue(&value1, 3.14, queue);
    set_priority_queue(&value2, 2.718, queue);
    set_priority_queue(&value3, 1.618, queue);
    printPriorityQueue(queue);
    printf("END TO SET ___ - \n");
    assert(queue->size == 3);
    assert(size_queue(queue) == 3);


    assert(queue->arr[0]->point == &value3 && queue->arr[0]->priority == 1.618);

    // Retirer l'élément avec la plus petite priorité
    struct PriorityQueueElem *min_entry = remove_min(queue);

    // Vérifier que l'élément avec la plus petite priorité a été correctement retiré
    assert(min_entry != NULL);

    assert(min_entry->point == &value3 && min_entry->priority == 1.618);
    assert(queue->size == 2);

    assert(queue->arr[0]->point == &value2 && queue->arr[0]->priority == 2.718);

    set_priority_queue(&value2, 8.5, queue);
    printPriorityQueue(queue);
    assert(queue->arr[0]->point == &value1 && queue->arr[0]->priority == 3.14);

    free(min_entry);

    set_priority_queue(&value2, 1.5, queue);
    printPriorityQueue(queue);
    assert(queue->arr[0]->point == &value2 && queue->arr[0]->priority == 1.5);

    // Vérifier que la file de priorité est mise à jour après la suppression de l'élément minimum
    assert(queue != NULL);

    destroy_Queue(queue);
    queue = create_PriorityQueue(100);

    // Libérer la mémoire de la file de priorité

    start = clock();
    perf_test(queue,1000000,10);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Temps d'exécution : %f secondes\n", cpu_time_used);

    printf("Tous les tests ont réussi !\n");
    destroy_Queue(queue);

    return 0;
}

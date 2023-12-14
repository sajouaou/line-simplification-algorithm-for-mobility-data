#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct PriorityQueue
{
    void *point;
    double priority;
    struct PriorityQueue *next_entry;
    size_t size;
};

// Définir vos fonctions ici
void set_priority_queue(void *p_i, double priority, struct PriorityQueue *Q);
struct PriorityQueue *remove_min(struct PriorityQueue *Q);
void destroyQueue(struct PriorityQueue *Q);

void printPriorityQueue(const struct PriorityQueue *queue)
{
    while (queue != NULL)
    {
        printf("Point: %lf, Priority: %lf\n", *(double *)(queue->point), queue->priority);
        queue = queue->next_entry;
    }
}


int main()
{
    // Créer une file de priorité vide
    struct PriorityQueue *queue = malloc(sizeof(struct PriorityQueue));

    // Ajouter des éléments à la file de priorité
    double value1 = 3.14;
    double value2 = 2.718;
    double value3 = 1.618;

    set_priority_queue(&value1, 3.14, queue);
    set_priority_queue(&value2, 2.718, queue);
    set_priority_queue(&value3, 1.618, queue);

    // Vérifier que les éléments ont été correctement ajoutés
    assert(queue != NULL);
    assert(queue->point == &value3 && queue->priority == 1.618);
    assert(queue->size == 3);

    // Retirer l'élément avec la plus petite priorité
    struct PriorityQueue *min_entry = remove_min(queue);

    // Vérifier que l'élément avec la plus petite priorité a été correctement retiré
    assert(min_entry != NULL);
    assert(min_entry->point == &value3 && min_entry->priority == 1.618);
    assert(queue->size == 2);

    // Libérer la mémoire des éléments retirés
    free(min_entry);

    // Vérifier que la file de priorité est mise à jour après la suppression de l'élément minimum
    assert(queue != NULL);
    assert(queue->point == &value2 && queue->priority == 2.718);

    // Libérer la mémoire de la file de priorité

    destroy_Queue(queue);
    printf("Tous les tests ont réussi !\n");

    return 0;
}

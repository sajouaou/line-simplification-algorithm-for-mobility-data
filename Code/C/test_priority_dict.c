#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef struct PriorityDict
{
  void * key;
  double priority;
} PriorityDict;


typedef void* PDict;

double
get_priority_dict(void *p_i,PDict *dict);

void
set_priority_dict(void * p_i,double priority,PDict *dict);
void destroy_PriorityDict(PDict dict);

int main()
{
    // Créer un dictionnaire de priorité vide
    PDict priorityDict =  0;


    // Ajouter des éléments au dictionnaire de priorité
    double priority1 = 3.14;
    double value1 = 3.14;
    double value2 = 2.718;
    double value3 = 1.618;
    set_priority_dict(&value1, priority1, &priorityDict);

    double priority2 = 2.718;
    set_priority_dict(&value2, priority2, &priorityDict);



    // Obtenir la priorité associée à une clé
    double retrievedPriority = get_priority_dict(&value1, &priorityDict);

    // Vérifier que la priorité a été correctement récupérée
    assert(retrievedPriority == priority1);

    // Modifier la priorité associée à une clé existante
    double newPriority = 1.0;
    set_priority_dict(&value2, newPriority, &priorityDict);


    // Libérer la mémoire des éléments du dictionnaire de priorité
    destroy_PriorityDict(priorityDict);

    printf("\nTous les tests ont réussi !\n");

    return 0;
}

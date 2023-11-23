#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct PriorityDict
{
    void *point;
    double priority;
    struct PriorityDict *next_entry;
};

// Définir vos fonctions ici
double get_priority_dict(void *p_i, struct PriorityDict *dict);
void set_priority_dict(void *p_i, double priority, struct PriorityDict *dict);

void printPriorityDict(const struct PriorityDict *dict)
{
    while (dict != NULL)
    {
        printf("Point: %p, Priority: %lf\n", dict->point, dict->priority);
        dict = dict->next_entry;
    }
}

int main()
{
    // Créer un dictionnaire de priorité vide
    struct PriorityDict *priorityDict =  malloc(sizeof(struct PriorityDict));


    // Ajouter des éléments au dictionnaire de priorité
    double priority1 = 3.14;
    double value1 = 3.14;
    double value2 = 2.718;
    double value3 = 1.618;
    set_priority_dict(&value1, priority1, priorityDict);

    double priority2 = 2.718;
    set_priority_dict(&value2, priority2, priorityDict);

    // Imprimer le contenu du dictionnaire de priorité
    printf("Contenu initial du dictionnaire de priorité :\n");
    printPriorityDict(priorityDict);

    // Obtenir la priorité associée à une clé
    double retrievedPriority = get_priority_dict(&value1, priorityDict);

    // Vérifier que la priorité a été correctement récupérée
    assert(retrievedPriority == priority1);

    // Modifier la priorité associée à une clé existante
    double newPriority = 1.0;
    set_priority_dict(&value2, newPriority, priorityDict);

    // Imprimer le contenu du dictionnaire de priorité après la modification
    printf("\nContenu du dictionnaire de priorité après modification :\n");
    printPriorityDict(priorityDict);

    // Libérer la mémoire des éléments du dictionnaire de priorité
    while (priorityDict != NULL)
    {
        struct PriorityDict *temp = priorityDict;
        priorityDict = priorityDict->next_entry;
        free(temp);
    }

    printf("\nTous les tests ont réussi !\n");

    return 0;
}

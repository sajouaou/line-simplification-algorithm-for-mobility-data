#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct PointDict
{
    void *key;
    void *value;
    struct PointDict *next_value;
};

// Définir vos fonctions ici
void *get_point_dict(void *p_i, struct PointDict *dict);
void set_point_dict(void *p_i, void *p_j, struct PointDict *dict);

void printPointDict(const struct PointDict *dict)
{
    while (dict != NULL)
    {
        printf("Key: %p, Value: %p\n", dict->key, dict->value);
        dict = dict->next_value;
    }
}


int main()
{
    // Créer un dictionnaire vide
    struct PointDict *dictionary = malloc(sizeof(struct PointDict));

    // Ajouter des éléments au dictionnaire
    int key1 = 42;
    int value1 = 100;
    set_point_dict(&key1, &value1, dictionary);

    char key2 = 'A';
    char value2 = 'Z';
    set_point_dict(&key2, &value2, dictionary);

    // Vérifier que les éléments ont été correctement ajoutés
    int *entry1 = get_point_dict(&key1, dictionary);
    int *entry2 = get_point_dict(&key2, dictionary);

    printPointDict(dictionary);
    assert(entry1 != NULL);
    assert(entry1 == &value1);

    assert(entry2 != NULL);
    assert(entry2 == &value2);

    // Modifier la valeur associée à une clé existante
    int newValue1 = 999;
    set_point_dict(&key1, &newValue1, dictionary);

    // Vérifier que la valeur a été correctement mise à jour
    entry1 = get_point_dict(&key1, dictionary);
    assert(entry1 != NULL);
    assert(entry1 == &newValue1);

    // Libérer la mémoire du dictionnaire
    while (dictionary != NULL)
    {
        struct PointDict *temp = dictionary;
        dictionary = dictionary->next_value;
        free(temp);
    }

    printf("Tous les tests ont réussi !\n");

    return 0;
}

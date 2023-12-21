#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <assert.h>

struct PointDict
{
    void *key;
    void *value;
} typedef PointDict;

typedef void * Dict;
// Définir vos fonctions ici
void *get_point_dict(void *p_i, Dict *dict);
void set_point_dict(void *p_i, void *p_j, Dict *dict);
void destroy_PointDict(Dict *dict);


int compar(const void *l, const void *r);



int main()
{
    // Créer un dictionnaire vide
    Dict dictionary = 0;
    int key = 2;
    int value = 2;



    // Ajouter des éléments au dictionnaire
    int key1 = 42;
    int value1 = 100;
    set_point_dict(&key1, &value1, &dictionary);

    char key2 = 'A';
    char value2 = 'Z';
    set_point_dict(&key2, &value2, &dictionary);

    // Vérifier que les éléments ont été correctement ajoutés
    int *entry1 = get_point_dict(&key1, &dictionary);
    int *entry2 = get_point_dict(&key2, &dictionary);

    //printPointDict(dictionary);
    assert(entry1 != NULL);
    assert(entry1 == &value1);

    assert(entry2 != NULL);
    assert(entry2 == &value2);

    // Modifier la valeur associée à une clé existante
    int newValue1 = 999;
    set_point_dict(&key1, &newValue1, &dictionary);

    // Vérifier que la valeur a été correctement mise à jour
    entry1 = get_point_dict(&key1, &dictionary);
    assert(entry1 != NULL);
    assert(entry1 == &newValue1);

    // Libérer la mémoire du dictionnaire
    destroy_PointDict(dictionary);

    printf("Tous les tests ont réussi !\n");

    return 0;
}

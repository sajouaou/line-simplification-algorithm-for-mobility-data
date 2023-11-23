


struct PointDict
{
    void *key;
    void *value;
    struct PointDict *next_value;
};


void*
get_point_dict(void *p_i,struct PointDict *dict)
{
    void * result = NULL;
    short loop = 1;
    while(loop && dict != NULL)
    {

        if(dict->key == p_i)
        {
            result = dict->value;
            loop = 0;
        }
        dict = dict->next_value;
    }
    return result;
}


void
set_point_dict(void * p_i,void * p_j,struct PointDict *dict)
{
    short loop = 1;

    while(loop && dict != NULL)
    {
        if(dict-> key == p_i)
        {
            dict->value = p_j;
            loop = 0;
        }
        else if( dict->key == NULL)
        {
            dict->key = p_i;
            dict->value = p_j;
            loop = 0;

        }
        else if(dict->next_value  == NULL && loop )
        {
            struct PointDict * entry = malloc(sizeof(struct PointDict));
            entry->key = p_i;
            entry->value = p_j;
            dict->next_value = entry;
            loop = 0;
        }
        dict = dict->next_value;
    }
}


void destroy_PointDict(struct PointDict *dict)
{
    while (dict != NULL)
    {
        struct PointDict *temp = dict;
        dict = dict->next_value;
        free(temp);
    }
}


void destroy_elem_PointDict(void * p_i,struct PointDict *dict)
{

    struct PointDict *temp = NULL;
    struct PointDict *d = dict;
    short loop = 1;
    while (d != NULL && loop)
    {
        if(d->key == p_i)
        {
            loop = 0;
            if(temp != NULL)
            {
                temp->next_value = d->next_value;
                free(d);
            }
            if(d == dict)
            {
                d->key = NULL;
                d->value = NULL;
                if(d->next_value != NULL)
                {
                    temp = d->next_value;
                    d->key = temp->key ;
                    d->value = temp->value ;
                    d->next_value = temp->next_value;
                    free(temp);
                }
            }
        }
        temp = dict;
        d = d->next_value;
    }
}
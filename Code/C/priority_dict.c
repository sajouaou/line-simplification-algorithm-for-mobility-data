


struct PriorityDict
{
  void * point;
  double priority;
  struct PriorityDict *next_entry;
};



double
get_priority_dict(void *p_i,struct PriorityDict *dict)
{
    double result = -1;
    short loop = 1;
    while(loop && dict != NULL)
    {

        if(dict->point == p_i)
        {
            result = dict->priority;
            loop = 0;
        }
        dict = dict->next_entry;
    }
    return result;
}


void
set_priority_dict(void * p_i,double priority,struct PriorityDict *dict)
{
    short loop = 1;

    while(loop && dict != NULL)
    {
        if(dict-> point == p_i)
        {
            dict->priority = priority;
            loop = 0;
        }
        else if( dict->point == NULL)
        {
            dict->point = p_i;
            dict->priority = priority;
            loop = 0;

        }
        else if(dict->next_entry  == NULL && loop )
        {
            struct PriorityDict * entry = malloc(sizeof(struct PriorityDict));
            entry->point = p_i;
            entry->priority = priority;
            dict->next_entry = entry;
            loop = 0;
        }
        dict = dict->next_entry;
    }
}


void destroy_PriorityDict(struct PriorityDict *dict)
{
    while (dict != NULL)
    {
        struct PriorityDict *temp = dict;
        dict = dict->next_entry;
        free(temp);
    }
}


void destroy_elem_PriorityDict(void * p_i,struct PriorityDict *dict)
{

    struct PriorityDict *temp = NULL;
    struct PriorityDict *d = dict;
    short loop = 1;
    while (d != NULL && loop)
    {
        if(d->point == p_i)
        {
            loop = 0;
            if(temp != NULL)
            {
                temp->next_entry = d->next_entry;
                free(d);
            }
            if(d == dict)
            {
                d->point = NULL;
                d->priority = 0;
                if(d->next_entry != NULL)
                {
                    temp = d->next_entry;
                    d->point = temp->point ;
                    d->priority = temp->priority ;
                    d->next_entry = temp->next_entry;
                    free(temp);
                }
            }
        }
        temp = dict;
        d = d->next_entry;
    }
}
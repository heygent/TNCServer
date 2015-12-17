//
// Created by root on 11/12/15.
//

#include <malloc.h>
#include <assert.h>
#include "list.h"

struct t_tnclist
{
    TNCList_Node first, last;
};

struct t_tnclist_node
{
    void *value;
    struct t_tnclist_node *next;
};

TNCList TNCList_new()
{
    TNCList this;
    this = malloc(sizeof(TNCList));
    this->first = this->last = NULL;
    return this;
}

void TNCList_add(TNCList this, void *val)
{
    assert(val != NULL);
    assert(this != NULL);

    TNCList_Node to_add = malloc(sizeof(TNCList_Node));

    to_add->value = val;
    to_add->next = NULL;

    if(NULL == this->first)
    {
        this->first = to_add;
        this->last = to_add;
    }
    else
    {
        this->last->next = to_add;
        this->last = to_add;
    }

}

void TNCList_delete(TNCList *this)
{
    TNCList to_del = *this;
    *this = NULL;

    TNCList_Node current_node = to_del->first;

    while(NULL != current_node)
    {
        TNCList_Node next_node = current_node->next;
        free(current_node->value);
        free(current_node);
        current_node = next_node;
    }

    free(to_del);
}

void TNCList_chain(TNCList list, TNCList *to_chain)
{
    list->last->next = (**to_chain).first;
    list->last = (**to_chain).last;
    free(*to_chain);
    *to_chain = NULL;
}

TNCList_Node TNCList_begin(TNCList list)
{
    return list->first;
}

TNCList_Node TNCList_next(TNCList_Node current)
{
    return current->next;
}

const void *TNCList_get_value(TNCList_Node node)
{
    return node->value;
}


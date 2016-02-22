#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "error.h"

struct _TNCList
{
    TNCListNode first;
    TNCListNode last;
    size_t num_of_elements;
};

struct _TNCListNode
{
    void *value;
    TNCListNode next;
    TNCListNode prev;
    TNCList owner;
};

static TNCListNode _TNCListNode_new(void *val, TNCList owner)
{
    TNCListNode ret = malloc(sizeof *ret);
    if(ret)
    {
        ret->value = val;
        ret->owner = owner;
    }

    return ret;
}

static inline void _TNCListNode_destroy(TNCListNode node)
{
    free(node);
}

TNCList TNCList_new()
{
    TNCList self;
    self = malloc(sizeof *self);

    if(self)
    {
        self->first = self->last = NULL;
        self->num_of_elements = 0;
    }

    return self;
}


void TNCList_destroy_and_free(TNCList self, TNCConsumer freefn)
{
    if(!self) return;

    TNCListNode current = self->first;

    while (current)
    {
        TNCListNode to_destroy = current;

        current = current->next;
        if(freefn) freefn(to_destroy->value);
        _TNCListNode_destroy(to_destroy);
    }

    free(self);
}

void TNCList_destroy(TNCList self)
{
    TNCList_destroy_and_free(self, NULL);
}

static int _TNCList_insert_first(TNCList restrict self, void *restrict val)
{
    TNCListNode node = _TNCListNode_new(val, self);
    if(!node) return TNCError_failed_alloc;

    node->prev = node->next = NULL;
    self->first = self->last = node;
    self->num_of_elements = 1;

    return TNCError_good;
}

int TNCList_insert_before(TNCListNode restrict ref, void *restrict val)
{
    assert(ref);
    assert(val);
    assert(ref != val);

    TNCListNode node = _TNCListNode_new(val, ref->owner);

    if(!node) return TNCError_failed_alloc;

    node->prev = ref->prev;
    node->next = ref;
    ref->prev = node;

    if(ref->owner->first == ref)
        ref->owner->first = node;

    ++ref->owner->num_of_elements;

    return TNCError_good;
}

int TNCList_insert_after(TNCListNode restrict ref, void *restrict val)
{
    assert(ref);
    assert(val);
    assert(ref != val);

    TNCListNode node = _TNCListNode_new(val, ref->owner);

    if(!node) return TNCError_failed_alloc;

    node->next = ref->next;
    node->prev = ref;
    ref->next = node;

    if(ref->owner->last == ref)
        ref->owner->last = node;

    ++ref->owner->num_of_elements;

    return TNCError_good;

}

void *TNCList_remove(TNCListNode node)
{
    assert(node);

    void *val = node->value;

    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        node->owner->first = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else
    {
        node->owner->last = node->prev;
    }

    _TNCListNode_destroy(node);

    --node->owner->num_of_elements;

    return val;

}

int TNCList_push_front(TNCList restrict self, void *restrict val)
{
    assert(self);
    assert(val);
    assert(self != val);


    if(self->first)
    {
        assert(self->first && self->last);

        return TNCList_insert_before(self->first, val);
    }
    else
    {
        assert(!self->first && !self->last);

        return _TNCList_insert_first(self,val);
    }
}

int TNCList_push_back(TNCList restrict self, void *restrict val)
{
    assert(self);
    assert(val);
    assert(self != val);

    if(self->first)
    {
        assert(self->first && self->last);

        return TNCList_insert_after(self->last, val);
    }
    else
    {
        assert(!self->first && !self->last);

        return _TNCList_insert_first(self, val);
    }
}

void *TNCList_pop_front(TNCList self)
{
    return TNCList_remove(self->first);
}

void *TNCList_pop_back(TNCList self)
{
    return TNCList_remove(self->last);
}

void TNCList_chain(TNCList list, TNCList to_chain)
{
    assert(list);
    assert(to_chain);
    assert(list != to_chain);

    list->last->next = to_chain->first;
    to_chain->first->prev = list->last;
    list->last = to_chain->last;

    list->num_of_elements += to_chain->num_of_elements;

    free(to_chain);
}

TNCListNode TNCList_first(TNCList list)
{
    assert(list);
    return list->first;
}

TNCListNode TNCList_last(TNCList list)
{
    assert(list);
    return list->last;
}

TNCListNode TNCList_next(TNCListNode current)
{
    assert(current);
    return current->next;
}

TNCListNode TNCList_previous(TNCListNode current)
{
    assert(current);
    return current->prev;
}

const void *TNCList_getvalue(TNCListNode node)
{
    assert(node);
    return node->value;
}

int TNCList_empty(TNCList self)
{
    return self->first == NULL;
}

size_t TNCList_length(TNCList self)
{
    assert(self);
    return self->num_of_elements;
}

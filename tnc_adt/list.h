//
// Created by root on 11/12/15.
//

#ifndef PROGETTOSISTEMI_STRINGLIST_H
#define PROGETTOSISTEMI_STRINGLIST_H

#include <stdbool.h>

typedef struct t_tnclist *TNCList;
typedef struct t_tnclist_node *TNCList_Node;

TNCList TNCList_new();
void TNCList_add(TNCList this, void *val);
void TNCList_delete(TNCList *to_delete);
void TNCList_chain(TNCList list, TNCList *to_chain);

TNCList_Node TNCList_begin(TNCList list);
TNCList_Node TNCList_next(TNCList_Node current);
const void * TNCList_get_value(TNCList_Node node);

#endif //PROGETTOSISTEMI_STRINGLIST_H

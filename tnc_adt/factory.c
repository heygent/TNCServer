//
// Created by jeff on 06/01/16.
//

#include "factory.h"

struct _TNCFactoryProto
{
    void  *proto;
    size_t protosize;
};

void *TNCFactory_make_one(TNCFactoryProto prototype)
{

}

void *TNCFactory_make_many(TNCFactoryProto prototype, size_t howmany)
{

}
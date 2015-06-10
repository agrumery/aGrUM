#ifndef SIGNALER3_H__
#define SIGNALER3_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom##3
#define LIST_DECL_CLASSES class type1, class type2, class type3
#define LIST_CLASSES type1, type2, type3
#define LIST_DECL_ARGS type1 arg1, type2 arg2, type3 arg3
#define LIST_ARGS arg1, arg2, arg3

#define GUM_EMIT3(signal, arg1, arg2, arg3) this->signal(this, arg1, arg2, arg3)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.pattern.h"

#endif // SIGNALER3_H__

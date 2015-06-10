#ifndef SIGNALER2_H__
#define SIGNALER2_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom##2
#define LIST_DECL_CLASSES class type1, class type2
#define LIST_CLASSES type1, type2
#define LIST_DECL_ARGS type1 arg1, type2 arg2
#define LIST_ARGS arg1, arg2

#define GUM_EMIT2(signal, arg1, arg2) this->signal(this, arg1, arg2)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.pattern.h"

#endif // SIGNALER2_H__

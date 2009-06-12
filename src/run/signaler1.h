#ifndef SIGNALER1_H__
#define SIGNALER1_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom ## 1
#define LIST_DECL_CLASSES class type1
#define LIST_CLASSES type1
#define LIST_DECL_ARGS type1 arg1
#define LIST_ARGS arg1

#define GUM_EMIT1(signal,arg1) this->signal(this,arg1)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.patron.h"

#endif // SIGNALER1_H__


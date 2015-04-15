#ifndef SIGNALER4_H__
#define SIGNALER4_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom##4
#define LIST_DECL_CLASSES class type1, class type2, class type3, class type4
#define LIST_CLASSES type1, type2, type3, type4
#define LIST_DECL_ARGS type1 arg1, type2 arg2, type3 arg3, type4 arg4
#define LIST_ARGS arg1, arg2, arg3, arg4

#define GUM_EMIT4(signal, arg1, arg2, arg3, arg4)                                   \
  this->signal(this, arg1, arg2, arg3, arg4)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.pattern.h"

#endif // SIGNALER4_H__

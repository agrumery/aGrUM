#ifndef SIGNALER5_H__
#define SIGNALER5_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom##5
#define LIST_DECL_CLASSES                                                           \
  class type1, class type2, class type3, class type4, class type5
#define LIST_CLASSES type1, type2, type3, type4, type5
#define LIST_DECL_ARGS type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5
#define LIST_ARGS arg1, arg2, arg3, arg4, arg5

#define GUM_EMIT5(signal, arg1, arg2, arg3, arg4, arg5)                             \
  this->signal(this, arg1, arg2, arg3, arg4, arg5)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.pattern.h"

#endif // SIGNALER5_H__

#ifndef SIGNALER6_H__
#define SIGNALER6_H__

#include "signaler.h"

#define MAKE_NAME(nom) nom##6
#define LIST_DECL_CLASSES                                                           \
  class type1, class type2, class type3, class type4, class type5, class type6
#define LIST_CLASSES type1, type2, type3, type4, type5, type6
#define LIST_DECL_ARGS                                                              \
  type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6
#define LIST_ARGS arg1, arg2, arg3, arg4, arg5, arg6

#define GUM_EMIT6(signal, arg1, arg2, arg3, arg4, arg5, arg6)                       \
  this->signal(this, arg1, arg2, arg3, arg4, arg5, arg6)

#define SIGNALER_PATRON_ACCEPTED
#include "signaler_with_args.pattern.h"

#endif // SIGNALER6_H__

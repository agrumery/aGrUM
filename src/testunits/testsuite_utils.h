#ifndef TESTSUITE_UTILS_H
#define TESTSUITE_UTILS_H

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(AGRUM_SRC_PATH) "/testunits/ressources/" #x

#endif //TESTSUITE_UTILS_H

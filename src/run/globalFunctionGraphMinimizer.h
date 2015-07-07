#ifndef GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H
#define GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H

#include <agrum/multidim/multiDimFunctionGraph.h>

namespace gum {

  class GlobalFunctionGraphMinimizer
  {
  public:
    GlobalFunctionGraphMinimizer( ) {
      GUM_CONSTRUCTOR(GlobalFunctionGraphMinimizer)
    }
    ~GlobalFunctionGraphMinimizer() {
      GUM_DESTRUCTOR(GlobalFunctionGraphMinimizer)
    }

    void loadFunctionGraphs(MultiDimFunctionGraph<double>* fg1, MultiDimFunctionGraph<double>* fg2);
    void minimize();

  private:

    void __extractVarList();
    void __orderSimillary();
    void __minimizeGlobally();

    MultiDimFunctionGraph<double>* __fg1;
    MultiDimFunctionGraph<double>* __fg2;

    Sequence<const DiscreteVariable*> __varList;
  };
}

#endif // GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H

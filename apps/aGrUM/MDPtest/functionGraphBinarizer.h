#ifndef GUM_FUNCTION_GRAPH_BINARIZER_H
#define GUM_FUNCTION_GRAPH_BINARIZER_H

#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum {
  class FunctionGraphBinarizer {

    public:
    FunctionGraphBinarizer()
        : __binarizedFG(nullptr)
        , __dest(nullptr)
        , __defaultNode(0) {
      GUM_CONSTRUCTOR(FunctionGraphBinarizer)
    }

    ~FunctionGraphBinarizer();

    void loadFunctionGraph(const MultiDimFunctionGraph< double >* src);
    MultiDimFunctionGraph< double >* binarize();

    private:
    void   __binarizeVariables();
    NodeId __recurBinarize(NodeId binarizedNode, Idx valueModa, Idx vPos);
    void __binarizeFunctionGraph();


    const MultiDimFunctionGraph< double >* __binarizedFG;
    MultiDimFunctionGraph< double >*       __dest;

    HashTable< const DiscreteVariable*, Sequence< const DiscreteVariable* >* >
      __varMap;
    HashTable< NodeId, NodeId > __src2dest;

    NodeId __defaultNode;
  };
}

#endif  // GUM_FUNCTION_GRAPH_BINARIZER_H

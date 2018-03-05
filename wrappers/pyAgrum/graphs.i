%define ADD_NODES_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *nodes() const {
    return PyAgrumHelper::PySetFromNodeSet(self->nodes());
    PyObject* q=PyList_New(0);

    for ( auto node : self->nodes()) {
      PyList_Append(q,PyInt_FromLong(node));
    }

    return q;
  };
};
%enddef
ADD_NODES_METHOD_TO_GRAPHCLASS(gum::DiGraph); // add for the sub-classes (including MixedGraph)
ADD_NODES_METHOD_TO_GRAPHCLASS(gum::UndiGraph);
ADD_NODES_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);
%ignore gum::EssentialGraph::nodes const;
ADD_NODES_METHOD_TO_GRAPHCLASS(gum::MarkovBlanket);
%ignore gum::MarkovBlanket::nodes const;
ADD_NODES_METHOD_TO_GRAPHCLASS(gum::IBayesNet);

%define ADD_DI_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *arcs() const { // add for the sub-classes (including MixedGraph)
    return PyAgrumHelper::PySetFromArcSet(self->arcs());
  };
  PyObject *parents(gum::NodeId id) {
    return PyAgrumHelper::PySetFromNodeSet(self->parents(id));
  };
  PyObject *children(gum::NodeId id) {
    return PyAgrumHelper::PySetFromNodeSet(self->children(id));
  };
};
%enddef
ADD_DI_METHOD_TO_GRAPHCLASS(gum::DiGraph); // add for the sub-classes (including MixedGraph)
ADD_DI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);
%ignore gum::EssentialGraph::arcs const;
%ignore gum::EssentialGraph::parents const;
%ignore gum::EssentialGraph::children const;
ADD_DI_METHOD_TO_GRAPHCLASS(gum::MarkovBlanket);
%ignore gum::MarkovBlanket::arcs const;
%ignore gum::MarkovBlanket::parents const;
%ignore gum::MarkovBlanket::children const;
ADD_DI_METHOD_TO_GRAPHCLASS(gum::IBayesNet<double>);
%ignore gum::IBayesNet<double>::arcs const;
%ignore gum::IBayesNet<double>::parents const;
%ignore gum::IBayesNet<double>::children const;

%define ADD_UNDI_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *edges() { // add for the sub-classes (including MixedGraph)
    return PyAgrumHelper::PySetFromEdgeSet(self->edges());
  };

  PyObject *neighbours(gum::NodeId id) {
    return PyAgrumHelper::PySetFromNodeSet(self->neighbours(id));
  };
};
%enddef
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::UndiGraph); // add for the sub-classes (including MixedGraph)
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);
%ignore gum::EssentialGraph::edges const;
%ignore gum::EssentialGraph::neighbours const;


%extend gum::CliqueGraph {
  PyObject *clique(const gum::NodeId clique) const {
    PyObject* q=PySet_New(0);

    for(auto node : self->clique(clique)) {
          PySet_Add(q,PyInt_FromLong(node));
        }

    return q;
  };

  %pythoncode {
  def toDotWithNames(self,bn):
      """
      Parameters
      ----------
      bn : pyAgrum.BayesNet
      a Bayesian network

      Returns
      -------
      str
        a friendly display of the graph in DOT format where ids have been changed according to their correspondance in the BN
      """
      def nameFromId(m):
        return " ".join([bn.variable(int(n)).name()
                         for n in m.group().split("-")])
      import re
      m = re.compile('(?<=label=\")\d+[\-\d+]*')
      return m.sub(nameFromId,self.toDot())
    }
};
%ignore gum::CliqueGraph::clique;
%ignore gum::CliqueGraph::addNode;
%ignore gum::CliqueGraph::addEdge(const gum::NodeId,const gum::NodeId);

%include "extensions/JTGenerator.h"

%{
#include "extensions/JTGenerator.h"
%}

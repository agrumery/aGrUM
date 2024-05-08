/**
 *
 *  Copyright 2005-2024 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

%extend gum::DAG {
   bool dSeparation(PyObject* X,PyObject* Y,PyObject* Z) {
     gum::NodeSet sX,sY,sZ;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX,X);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY,Y);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sZ,Z);
     return self->dSeparation(sX,sY,sZ);
   }

   bool dSeparation(PyObject* X,PyObject* Y) {
     gum::NodeSet sX,sY,sZ;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX,X);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY,Y);
     return self->dSeparation(sX,sY,sZ);
   }

   gum::UndiGraph moralizedAncestralGraph(PyObject* nodes) {
     gum::NodeSet sonodes;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sonodes,nodes);
     return self->moralizedAncestralGraph(sonodes);
   }
 }

%extend gum::PDAG {
   bool cSeparation(PyObject* X,PyObject* Y,PyObject* Z) {
     gum::NodeSet sX,sY,sZ;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX,X);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY,Y);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sZ,Z);
     return self->cSeparation(sX,sY,sZ);
   }

   bool cSeparation(PyObject* X,PyObject* Y) {
     gum::NodeSet sX,sY,sZ;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX,X);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY,Y);
     return self->cSeparation(sX,sY,sZ);
   }

   gum::UndiGraph moralizedAncestralGraph(PyObject* nodes) {
     gum::NodeSet sonodes;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sonodes,nodes);
     return self->moralizedAncestralGraph(sonodes);
   }
 }

%define ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(classname)
%extend classname {
  PyObject *nodes() const {
    return PyAgrumHelper::PySetFromNodeSet(self->nodes());
  };

%pythoncode {
  def connectedComponents(self):
    """ connected components from a graph/graphical models

    Compute the connected components of a pyAgrum's graph or graphical models
    (more generally an object that has `nodes`, `children`/`parents` or `neighbours` methods)

    The firstly visited node for each component is called a 'root' and is used as a key for the component.
    This root has been arbitrarily chosen during the algorithm.

    Returns
    -------
    dict(int,Set[int])
      dict of connected components (as set of nodeIds (int)) with a nodeId (root) of each component as key.

    """
    nodes=self.nodes()
    connected_components=dict()

    def parcours(node,orig):
        cc={node}
        nodes.discard(node)
        if hasattr(self,'children'):
            for chi in self.children(node):
                if chi!=orig:
                    if chi in nodes:
                        cc|=parcours(chi,node)

        if hasattr(self,'parents'):
            for par in self.parents(node):
                if par!=orig:
                    if par in nodes:
                        cc|=parcours(par,node)

        if hasattr(self,'neighbours'):
            for nei in self.neighbours(node):
                if nei!=orig:
                    if nei in nodes:
                        cc|=parcours(nei,node)
        return cc

    while (len(nodes)>0):
        root=nodes.pop()
        connected_components[root]=parcours(root,None)
    return connected_components

  def adjacencyMatrix(self):
    """ adjacency matrix from a graph/graphical models

    Compute the adjacency matrix of a pyAgrum's graph or graphical models
    (more generally an object that has `nodes`, `children`/`parents` or `neighbours` methods)

    Returns
    -------
    numpy.ndarray
      adjacency matrix (as numpy.ndarray) with nodeId as key.

    """
    import numpy as np
    nodes=self.nodes()
    n=self.size()
    am=np.zeros((n,n)).astype(int)

    for node in nodes:
        if hasattr(self,'children'):
            for children in self.children(node):
                am[node,children]=1
        if hasattr(self,'neighbours'):
            for neighbour in self.neighbours(node):
                am[node,neighbour]=1
    return am
}

};
%enddef // ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS

ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::DiGraph); // add for the sub-classes (including DAG, MixedGraph and pdag)
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::UndiGraph);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::EssentialGraph);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::MarkovBlanket);

%ignore gum::EssentialGraph::nodes const;
%ignore gum::MarkovBlanket::nodes const;

%define ADD_METHOD_TO_GRAPH_ONLY_CLASS(classname)
%ignore classname::addNodes(gum::Size n);
%extend classname {
  PyObject *addNodes(gum::Size n) const {
    PyObject* q=PySet_New(0);

    PyObject* pyval;
    for(auto node : const_cast<classname *>(self)->addNodes(n)) {
      pyval=PyInt_FromLong(node);
      PySet_Add(q,pyval);
      Py_DecRef(pyval);
    }

    return q;
  };


%pythoncode {
  def __iter__(self):
    """
    Iterate over the nodes of the graph

    Yield
    -----
    int
      The index of the node
    """
    for i in self.nodes():
      yield i

  def __getstate__(self):
      state=dict()
      if hasattr(self,'arcs'):
          state['arcs']=self.arcs()
      if hasattr(self,'edges'):
        state['edges']=self.edges()
      return state

  def __setstate__(self,state):
      self.__init__()
      if 'arcs' in state:
          for x,y in state['arcs']:
            if not self.existsNode(x):
              self.addNodeWithId(x)
            if not self.existsNode(y):
              self.addNodeWithId(y)
            self.addArc(x,y)
      if 'edges' in state:
          for x,y in state['edges']:
            if not self.existsNode(x):
              self.addNodeWithId(x)
            if not self.existsNode(y):
              self.addNodeWithId(y)
            self.addEdge(x,y)
      return self
   }
};
%enddef
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::DiGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::DAG);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::UndiGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::MixedGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::PDAG);
// automatically done for subclass
//ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::DAG);

%define ADD_DI_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *arcs() const { // add for the sub-classes (including MixedGraph and PDAG)
    return PyAgrumHelper::PySetFromArcSet(self->arcs());
  };
  PyObject *parents(gum::NodeId id) const {
    return PyAgrumHelper::PySetFromNodeSet(self->parents(id));
  };
  PyObject *children(gum::NodeId id) const {
    return PyAgrumHelper::PySetFromNodeSet(self->children(id));
  };
};
%enddef
ADD_DI_METHOD_TO_GRAPHCLASS(gum::DiGraph);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::DAG);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::PDAG);

ADD_DI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);
%ignore gum::EssentialGraph::arcs const;
%ignore gum::EssentialGraph::parents const;
%ignore gum::EssentialGraph::children const;
ADD_DI_METHOD_TO_GRAPHCLASS(gum::MarkovBlanket);
%ignore gum::MarkovBlanket::arcs const;
%ignore gum::MarkovBlanket::parents const;
%ignore gum::MarkovBlanket::children const;

%define ADD_UNDI_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *edges() const { // add for the sub-classes (including MixedGraph)
    return PyAgrumHelper::PySetFromEdgeSet(self->edges());
  };

  PyObject *neighbours(gum::NodeId id) const {
    return PyAgrumHelper::PySetFromNodeSet(self->neighbours(id));
  };
};
%ignore classname::edges const;
%ignore classname::neighbours const;
%enddef
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::UndiGraph);
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::PDAG);

ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);


%define ADD_MIXED_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *boundary(gum::NodeId id) const {
    return PyAgrumHelper::PySetFromNodeSet(self->boundary(id));
  };
  PyObject *mixedOrientedPath(NodeId node1, NodeId node2) const {
    return PyAgrumHelper::PyListFromNodeVect(self->mixedOrientedPath(node1,node2));
  };
  PyObject* mixedUnorientedPath(NodeId node1, NodeId node2) const {
    return PyAgrumHelper::PyListFromNodeVect(self->mixedUnorientedPath(node1,node2));
  };
};
%ignore classname::boundary const;
%ignore classname::mixedOrientedPath const;
%ignore classname::mixedUnorientedPath const;
%enddef
ADD_MIXED_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_MIXED_METHOD_TO_GRAPHCLASS(gum::PDAG);


%extend gum::CliqueGraph {
  PyObject *clique(const gum::NodeId clique) const {
    PyObject* q=PySet_New(0);

    PyObject* pyval;
    for(auto node :self->clique(clique)) {
      pyval=PyInt_FromLong(node);
      PySet_Add(q,pyval);
      Py_DecRef(pyval);
    }

    return q;
  };

  PyObject* separator(const gum::NodeId cliq1,const gum::NodeId cliq2) const {
    PyObject* q=PySet_New(0);

    PyObject* pyval;
    for(auto node : self->separator(cliq1,cliq2)) {
      pyval=PyInt_FromLong(node);
      PySet_Add(q,pyval);
      Py_DecRef(pyval);
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
      m = re.compile(r'(?<=label=\")\d+[\-\d+]*')
      return m.sub(nameFromId,self.toDot())
    }
};
%rename ( __map_str__ ) gum::CliqueGraph::mapToDot const;
%ignore gum::CliqueGraph::clique;
%ignore gum::CliqueGraph::separator;
%ignore gum::CliqueGraph::addNodeWithId;
%ignore gum::CliqueGraph::addEdge(const gum::NodeId,const gum::NodeId);

%include "extensions/JunctionTreeGenerator.h"

%{
#include "extensions/JunctionTreeGenerator.h"
%}

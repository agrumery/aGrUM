/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
 }

%define ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(classname)
%extend classname {
  const gum::NodeGraphPart& nodes() const {
    return self->nodes();
  };

%pythoncode {
  def connectedComponentsList(self):
    """ connected components as a dict of sets

    Returns
    -------
    dict(int, set[int])
      dict of connected components (as sets of nodeIds) keyed by an arbitrary root nodeId per component.

    """
    cc = self.connectedComponents()
    result = {}
    for node, root in cc.items():
      if root not in result:
        result[root] = set()
      result[root].add(node)
    return result

  def connectedComponentsCount(self):
    """ number of connected components

    Returns
    -------
    int
      the number of connected components in the graph.

    """
    return len(set(self.connectedComponents().values()))

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


%define ADD_METHOD_TO_GRAPH_ONLY_CLASS(classname)
%ignore classname::addNodes(gum::Size n);
%extend classname {
  PyObject *addNodes(gum::Size n) const {
    return PyAgrumHelper::PySetFromNodeVect(const_cast<classname *>(self)->addNodes(n));
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
  gum::ArcSet arcs() const { // add for the sub-classes (including MixedGraph and PDAG)
    return self->arcs();
  };
  gum::NodeSet parents(gum::NodeId id) const {
    return self->parents(id);
  };
  gum::NodeSet children(gum::NodeId id) const {
    return self->children(id);
  };
  gum::NodeSet descendants(gum::NodeId id) const {
    return self->descendants(id);
  };
  gum::NodeSet ancestors(gum::NodeId id) const {
    return self->ancestors(id);
  };
};
%enddef
ADD_DI_METHOD_TO_GRAPHCLASS(gum::DiGraph);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::DAG);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_DI_METHOD_TO_GRAPHCLASS(gum::PDAG);

%define ADD_UNDI_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  gum::EdgeSet edges() const { // add for the sub-classes (including MixedGraph)
    return self->edges();
  };

  gum::NodeSet neighbours(gum::NodeId id) const {
    return self->neighbours(id);
  };
};
%ignore classname::edges const;
%ignore classname::neighbours const;
%enddef
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::UndiGraph);
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::PDAG);

%define ADD_MIXED_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  gum::NodeSet boundary(gum::NodeId id) const {
    return self->boundary(id);
  };
  std::vector<gum::NodeId> mixedOrientedPath(NodeId node1, NodeId node2) const {
    return self->mixedOrientedPath(node1,node2);
  };
  std::vector<gum::NodeId> mixedUnorientedPath(NodeId node1, NodeId node2) const {
    return self->mixedUnorientedPath(node1,node2);
  };

%pythoncode {
  def connectedComponentsList(self):
    """ connected components as a dict of sets

    Returns
    -------
    dict(int, set[int])
      dict of connected components (as sets of nodeIds) keyed by an arbitrary root nodeId per component.

    """
    cc = self.connectedComponents()
    result = {}
    for node, root in cc.items():
      if root not in result:
        result[root] = set()
      result[root].add(node)
    return result

  def connectedComponentsCount(self):
    """ number of connected components

    Returns
    -------
    int
      the number of connected components in the graph.

    """
    return len(set(self.connectedComponents().values()))
}

};
%ignore classname::boundary const;
%ignore classname::mixedOrientedPath const;
%ignore classname::mixedUnorientedPath const;
%enddef
ADD_MIXED_METHOD_TO_GRAPHCLASS(gum::MixedGraph);
ADD_MIXED_METHOD_TO_GRAPHCLASS(gum::PDAG);


%extend gum::CliqueGraph {
  gum::NodeSet clique(const gum::NodeId clique) const {
    return self->clique(clique);
  };

  gum::NodeSet separator(const gum::NodeId cliq1,const gum::NodeId cliq2) const {
    return self->separator(cliq1,cliq2);
  };

  %pythoncode {
  def toDotWithNames(self,bn):
      """
      Parameters
      ----------
      bn : pyagrum.BayesNet
      a Bayesian network

      Returns
      -------
      str
        a friendly display of the graph in DOT format where ids have been changed according to their correspondance in the BN
      """
      def local_nameFromId(m):
        return " ".join([bn.variable(int(n)).name()
                         for n in m.group().split("-")])
      import re
      m = re.compile(r'(?<=label=\")\d+[\-\d+]*')
      return m.sub(local_nameFromId,self.toDot())
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

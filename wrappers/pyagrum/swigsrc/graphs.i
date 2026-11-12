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


// the separation methods (method(X, Y[, Z])) with nodes or sets of nodes for X, Y and Z (None for Z:
// nothing observed); the C++ overloads are replaced by this one
%define ADD_SEPARATION_API(classname, method)
%ignore classname::method(NodeId, NodeId, const NodeSet&) const;
%ignore classname::method(const NodeSet&, const NodeSet&, const NodeSet&) const;
%extend classname {
   bool method(PyObject* X, PyObject* Y, PyObject* Z = Py_None) const {
     gum::NodeSet sX,sY,sZ;
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX,X);
     PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY,Y);
     if (Z != Py_None) PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sZ,Z);
     return self->method(sX,sY,sZ);
   }
 }
%enddef

// iteration over the nodes of the immutable graph classes
%define ADD_NODE_ITER(classname)
%extend classname {
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
}
}
%enddef

ADD_SEPARATION_API(gum::DAG, dSeparation)

// the moralization and the c-separation (LWF) are defined for chain graphs: deprecated on PDAG,
// kept as Python shims on the same free functions as the deprecated C++ methods (no wrapping of
// these methods, which would raise deprecation warnings in the generated code)
%{
#include <agrum/base/graphs/algorithms/generic/moralization.h>
#include <agrum/base/graphs/algorithms/generic/pathFinding.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>
%}
%ignore gum::PDAG::moralGraph;
%ignore gum::PDAG::moralizedAncestralGraph;
%ignore gum::PDAG::cSeparation;
%ignore gum::PDAG::hasMixedReallyOrientedPath;
%ignore gum::PAG::toMixedGraph;
// an %ignore is inherited by the methods of the same name in the subclasses: restore them on ChainGraph
%rename("%s") gum::ChainGraph::moralGraph;
%rename("%s") gum::ChainGraph::moralizedAncestralGraph;
%rename("%s") gum::ChainGraph::cSeparation;
%rename("%s") gum::ChainGraph::hasMixedReallyOrientedPath;
%extend gum::PDAG {
  gum::UndiGraph _moralGraph() const { return gum::graph::moralGraph(*self); }
  bool _hasMixedReallyOrientedPath(gum::NodeId n1, gum::NodeId n2) const {
    return gum::graph::hasMixedWalkWithArc(*self, n1, n2);
  }
  gum::UndiGraph _moralizedAncestralGraph(const gum::NodeSet& nodes) const {
    return gum::graph::moralizedAncestralGraph(*self, nodes);
  }
  bool _cSeparation(PyObject* X, PyObject* Y, PyObject* Z) const {
    gum::NodeSet sX, sY, sZ;
    PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sX, X);
    PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sY, Y);
    if (Z != Py_None) PyAgrumHelper::populateNodeSetFromIntOrPySequenceOfInt(sZ, Z);
    return gum::graph::cSeparated(*self, sX, sY, sZ);
  }
%pythoncode {
  def moralGraph(self):
    """
    Returns the moral graph of the PDAG (LWF interpretation of chain graphs).

    Returns
    -------
    pyagrum.UndiGraph
      the moral graph

    Warnings
    --------
    PDAG.moralGraph is deprecated after 3.3.2, use ChainGraph.moralGraph instead.
    """
    import warnings
    warnings.warn("PDAG.moralGraph is deprecated after 3.3.2, use ChainGraph.moralGraph instead",
                  DeprecationWarning, stacklevel=2)
    return self._moralGraph()

  def moralizedAncestralGraph(self, nodes):
    """
    Compute the moral graph (LWF interpretation of chain graphs) of the smallest ancestral set
    containing the nodes.

    Parameters
    ----------
    nodes : set[int]
      the nodes

    Returns
    -------
    pyagrum.UndiGraph
      the moral graph of this ancestral set

    Warnings
    --------
    PDAG.moralizedAncestralGraph is deprecated after 3.3.2, use ChainGraph.moralizedAncestralGraph
    instead.
    """
    import warnings
    warnings.warn("PDAG.moralizedAncestralGraph is deprecated after 3.3.2, use "
                  "ChainGraph.moralizedAncestralGraph instead", DeprecationWarning, stacklevel=2)
    return self._moralizedAncestralGraph(nodes)

  def cSeparation(self, X, Y, Z=None):
    """
    Check if the sets of nodes X and Y are c-separated (by the set of nodes Z if given).

    Parameters
    ----------
    X : int or set[int]
      a node or a set of nodes
    Y : int or set[int]
      a node or a set of nodes
    Z : int or set[int], optional
      the observed node or nodes (none by default)

    Returns
    -------
    bool
      True if X and Y are c-separated by Z

    Warnings
    --------
    PDAG.cSeparation is deprecated after 3.3.2, use ChainGraph.cSeparation instead.
    """
    import warnings
    warnings.warn("PDAG.cSeparation is deprecated after 3.3.2, use ChainGraph.cSeparation instead",
                  DeprecationWarning, stacklevel=2)
    return self._cSeparation(X, Y, Z)

  def hasMixedReallyOrientedPath(self, n1, n2):
    """
    Check if a walk from n1 to n2 exists, following the arcs in their direction and the edges in
    either direction, with at least one arc. A walk may visit a node several times.

    Parameters
    ----------
    n1 : int
      the id of the first node of the walk
    n2 : int
      the id of the last node of the walk

    Returns
    -------
    bool
      True if such a walk exists

    Warnings
    --------
    PDAG.hasMixedReallyOrientedPath is deprecated after 3.3.2, use
    ChainGraph.hasMixedReallyOrientedPath instead.
    """
    import warnings
    warnings.warn("PDAG.hasMixedReallyOrientedPath is deprecated after 3.3.2, use "
                  "ChainGraph.hasMixedReallyOrientedPath instead", DeprecationWarning, stacklevel=2)
    return self._hasMixedReallyOrientedPath(n1, n2)
}
}

ADD_SEPARATION_API(gum::ChainGraph, cSeparation)

// CPDAG, MPDAG and MAG: immutable values; graph() returns a copy (the Python object must not
// refer to the internal graph of an immutable graph that may be destroyed before it)
%pythonappend gum::ImmutableGraph::graph %{
  val = type(val)(val)
%}
%ignore gum::MPDAG::fromCPDAG;
%pythonappend gum::MAG::markAt %{
  val = EdgeMark(val)
%}

ADD_SEPARATION_API(gum::CPDAG, dSeparation)
ADD_SEPARATION_API(gum::MPDAG, dSeparation)
ADD_SEPARATION_API(gum::MAG, mSeparation)
ADD_NODE_ITER(gum::CPDAG)
ADD_NODE_ITER(gum::MPDAG)
ADD_NODE_ITER(gum::MAG)


%extend gum::MPDAG {
  static gum::MPDAG _fromCPDAGArcs(const gum::CPDAG& cpdag,
                                   const std::vector<int>& tails,
                                   const std::vector<int>& heads) {
    gum::ArcSet arcs;
    for (std::size_t i = 0; i < tails.size(); ++i)
      arcs.insert(gum::Arc(gum::NodeId(tails[i]), gum::NodeId(heads[i])));
    return gum::MPDAG::fromCPDAG(cpdag, arcs);
  }

%pythoncode {
  @staticmethod
  def fromCPDAG(cpdag: "CPDAG", arcs) -> "MPDAG":
    """
    The MPDAG of a CPDAG with some arcs imposed by background knowledge.

    Parameters
    ----------
    cpdag : pyagrum.CPDAG
      the Markov equivalence class
    arcs : iterable of tuple[int, int]
      the imposed arcs (tail, head): each one must orient an edge of the CPDAG (or be one of its arcs)

    Returns
    -------
    pyagrum.MPDAG
      the maximal orientation of the CPDAG with these arcs

    Raises
    ------
    pyagrum.InvalidArc
      If an arc does not orient an edge of the CPDAG.
    pyagrum.InvalidArgument
      If no DAG of the Markov equivalence class contains these arcs.
    """
    arcs = list(arcs)
    return MPDAG._fromCPDAGArcs(cpdag, [t for t, _ in arcs], [h for _, h in arcs])
}
}

// nodes() and adjacencyMatrix(): for graphs and for graph-like objects (MarkovBlanket)
%define ADD_NODES_METHODS_TO_GRAPHCLASS(classname)
%extend classname {
  const gum::NodeGraphPart& nodes() const {
    return self->nodes();
  };

%pythoncode {
  def adjacencyMatrix(self):
    """ adjacency matrix from a graph/graphical models

    Compute the adjacency matrix of a pyAgrum's graph or graphical models
    (more generally an object that has ``nodes``, ``children``/``parents`` or ``neighbours`` methods; for
    a MAG, the symmetric matrix of ``adjacents``)

    Returns
    -------
    numpy.ndarray
      adjacency matrix (as numpy.ndarray) with nodeId as key.

    """
    import numpy  # this code is also generated in the submodules (id, mrf, ...), which do not import numpy

    nodes=self.nodes()
    n=self.size()
    am=numpy.zeros((n,n)).astype(int)

    for node in nodes:
        if hasattr(self,'adjacents'):
            for adjacent in self.adjacents(node):
                am[node,adjacent]=1
            continue
        if hasattr(self,'children'):
            for children in self.children(node):
                am[node,children]=1
        if hasattr(self,'neighbours'):
            for neighbour in self.neighbours(node):
                am[node,neighbour]=1
    return am
}
};
%enddef // ADD_NODES_METHODS_TO_GRAPHCLASS

// ADD_NODES_METHODS_TO_GRAPHCLASS + connected components (the class must have connectedComponents())
%define ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(classname)
ADD_NODES_METHODS_TO_GRAPHCLASS(classname)
%extend classname {
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
%enddef // ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS

ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::DiGraph); // add for the sub-classes (including DAG, MixedGraph and pdag)
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::UndiGraph);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::MarkedGraph);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::CPDAG);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::MPDAG);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::MAG);

%define ADD_NODENAME_METHODS(classname)
%extend classname {
  std::string nameFromId(gum::NodeId id) const { return self->nameFromId(id); };
  std::optional<gum::NodeId> idFromName(const std::string& name) const { return self->idFromName(name); };
  void setName(gum::NodeId id, const std::string& name) { self->setName(id, name); };
  bool hasName(gum::NodeId id) const { return self->hasName(id); };
};
%enddef

ADD_NODENAME_METHODS(gum::DiGraph)
ADD_NODENAME_METHODS(gum::UndiGraph)
ADD_NODENAME_METHODS(gum::MarkedGraph)


%define ADD_METHOD_TO_GRAPH_ONLY_CLASS(classname)
%ignore classname::addNodes(gum::Size n);
%extend classname {
  // const on purpose: a non-const signature would match the %ignore above, which then hides this %extend too
  PyAgrumSetOfInt *addNodes(gum::Size n) const {
    return PyAgrumHelper::PySetFromNodeVect(const_cast<classname *>(self)->addNodes(n));
  };
};
ADD_NODE_ITER(classname)
%enddef
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::DiGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::DAG);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::UndiGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::MixedGraph);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::PDAG);
ADD_METHOD_TO_GRAPH_ONLY_CLASS(gum::MarkedGraph); // and PAG

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
ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::MarkedGraph);

// the deprecated C++ method, called by its deprecated Python shim only (without the deprecation
// warning of the compiler, in a verbatim block)
%{
#include <agrum/base/graphs/PAG.h>
#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif
static gum::MixedGraph pyagrumPAGToMixedGraph(const gum::PAG& pag) { return pag.toMixedGraph(); }
#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif
%}

%extend gum::PAG {
  gum::MixedGraph _toMixedGraph() const { return pyagrumPAGToMixedGraph(*self); }

%pythoncode {
  def toMixedGraph(self):
    """
    Approximate conversion into a mixed graph: tail-arrowhead becomes an arc, arrowhead-arrowhead two
    arcs, anything else an undirected edge.

    Warnings
    --------
    PAG.toMixedGraph is deprecated after 3.3.2, use MAG.fromPAG instead (a representative MAG).

    Returns
    -------
    pyagrum.MixedGraph
    """
    import warnings
    warnings.warn("PAG.toMixedGraph is deprecated after 3.3.2, use MAG.fromPAG instead",
                  DeprecationWarning, stacklevel=2)
    return self._toMixedGraph()
}
}

// the marks are given as pyagrum.EdgeMark (an IntEnum: the constants EdgeMark_Circle... still work)
// only the values of gum::EdgeMark are accepted (an int stored as EdgeMark is never checked afterwards)
%typemap(in) gum::EdgeMark {
  const long _mark = PyLong_AsLong($input);
  if (_mark == -1 && PyErr_Occurred()) SWIG_fail;
  if (_mark < static_cast< long >(gum::EdgeMark::Circle)
      || _mark > static_cast< long >(gum::EdgeMark::Arrowhead)) {
    PyErr_SetString(PyExc_ValueError, "an edge mark must be a pyagrum.EdgeMark (Circle, Tail or Arrowhead)");
    SWIG_fail;
  }
  $1 = static_cast< gum::EdgeMark >(_mark);
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_INTEGER) gum::EdgeMark {
  $1 = PyLong_Check($input);
}
%apply gum::EdgeMark { EdgeMark };

%pythonappend gum::MarkedGraph::markAt %{
  val = EdgeMark(val)
%}
%pythoncode %{
import enum as _enum


class EdgeMark(_enum.IntEnum):
  """
  The mark at an end of an edge of a marked graph (`pyagrum.MarkedGraph`, `pyagrum.PAG`).

  An edge x-y carries one mark at x and one at y: ``x->y`` has a tail at x and an arrowhead at y,
  ``x<->y`` an arrowhead at both ends, ``x--y`` a tail at both ends; a circle is an end whose mark
  is not determined (in a PAG, it varies across the equivalence class).
  """

  # the values of the C++ enumeration gum::EdgeMark
  Circle = _pyagrumcpp.EdgeMark_Circle
  Tail = _pyagrumcpp.EdgeMark_Tail
  Arrowhead = _pyagrumcpp.EdgeMark_Arrowhead
%}

%define ADD_MIXED_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  gum::NodeSet boundary(gum::NodeId id) const {
    return self->boundary(id);
  };
  std::optional<std::vector<gum::NodeId>> mixedOrientedPath(NodeId node1, NodeId node2) const {
    return self->mixedOrientedPath(node1,node2);
  };
  std::optional<std::vector<gum::NodeId>> mixedUnorientedPath(NodeId node1, NodeId node2) const {
    return self->mixedUnorientedPath(node1,node2);
  };
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

  // pickling: a clique graph is not a graph of the GUM format (see ADD_GUM_IO_TO_GRAPH)
  void _addCliqueWithId(const gum::NodeId id, const gum::NodeSet& clique) {
    self->addNodeWithId(id, clique);
  }
  void _addCliqueEdge(const gum::NodeId cliq1, const gum::NodeId cliq2) {
    self->addEdge(cliq1, cliq2);
  }

  %pythoncode {
  def __getstate__(self):
    return {"cliques": {n: set(self.clique(n)) for n in self.nodes()}, "edges": list(self.edges())}

  def __setstate__(self, state):
    self.__init__()
    for n, clique in state["cliques"].items():
      self._addCliqueWithId(n, set(clique))
    for x, y in state["edges"]:
      self._addCliqueEdge(x, y)
    return self

  def saveGUM(self, name: str, binary: bool | None = None, indent: int | None = None) -> None:
    """
    Not available: a clique graph is not a graph of the GUM format (the cliques would be lost).

    Raises
    ------
    NotImplementedError
      always
    """
    raise NotImplementedError("a CliqueGraph can not be saved in the GUM format (the cliques would be lost)")

  def saveGUMstring(self, indent: int | None = None) -> str:
    """
    Not available: a clique graph is not a graph of the GUM format (the cliques would be lost).

    Raises
    ------
    NotImplementedError
      always
    """
    raise NotImplementedError("a CliqueGraph can not be saved in the GUM format (the cliques would be lost)")

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

%include "extensions/JunctionTreeGenerator.h"

%{
#include "extensions/JunctionTreeGenerator.h"
%}

// the GUM format (jgum/bgum) of the graphs, and their pickling through it
%{
#include <agrum/base/graphs/io/graphGumIO.h>
%}
%define ADD_GUM_IO_TO_GRAPH(classname)
%extend classname {
  std::string _saveGUMstring(int indent = gum::defaultJgumIndent()) const {
    return gum::graphToGUMString(*self, indent);
  }
  void _saveGUM(const std::string& name, PyObject* binary, int indent = gum::defaultJgumIndent()) const {
    gum::saveGraphGUM(*self, name, PyAgrumHelper::isBinaryGUMFile(name, binary), indent);
  }

%pythoncode {
  def saveGUM(self, name: str, binary: bool | None = None, indent: int | None = None) -> None:
    """
    Save the graph as a .jgum (text) or .bgum (binary) file (its class, its nodes with their names, its arcs,
    edges and marks).

    Parameters
    ----------
    name : str
      the name of the file
    binary : bool, optional
      True for the msgpack-based .bgum binary format, False for the JSON-based .jgum text format. By
      default, the format is given by the extension of name (binary for an unknown extension).
    indent : int, optional
      JSON indentation for the .jgum text format (ignored for the binary format): -1 for the most compact
      output, N>=0 to pretty-print with N spaces. Defaults to ``pyagrum.config["core","jgumIndent"]``.

    Raises
    ------
    pyagrum.InvalidArgument
      if binary contradicts the extension (``.jgum`` or ``.bgum``) of name
    """
    if indent is None:
      self._saveGUM(name, binary)
    else:
      self._saveGUM(name, binary, indent)

  def saveGUMstring(self, indent: int | None = None) -> str:
    """
    Save the graph as a jgum string (see `pyagrum.graphFromGUMString` to read it back).

    Parameters
    ----------
    indent : int, optional
      JSON indentation: -1 for the most compact output, N>=0 to pretty-print with N spaces.
      Defaults to ``pyagrum.config["core","jgumIndent"]``.

    Returns
    -------
    str
      the jgum string
    """
    return self._saveGUMstring() if indent is None else self._saveGUMstring(indent)

  def __getstate__(self):
    return self.saveGUMstring(-1)

  def __setstate__(self, state):
    if isinstance(state, dict):
      _setstate_from_former_pickle(self, state)
    else:
      self.__init__(graphFromGUMString(state))
    return self
}
};
%enddef

ADD_GUM_IO_TO_GRAPH(gum::DiGraph)
ADD_GUM_IO_TO_GRAPH(gum::DAG)
ADD_GUM_IO_TO_GRAPH(gum::UndiGraph)
ADD_GUM_IO_TO_GRAPH(gum::MixedGraph)
ADD_GUM_IO_TO_GRAPH(gum::PDAG)
ADD_GUM_IO_TO_GRAPH(gum::ChainGraph)
ADD_GUM_IO_TO_GRAPH(gum::MarkedGraph)
ADD_GUM_IO_TO_GRAPH(gum::PAG)
ADD_GUM_IO_TO_GRAPH(gum::CPDAG)
ADD_GUM_IO_TO_GRAPH(gum::MPDAG)
ADD_GUM_IO_TO_GRAPH(gum::MAG)

// typed readers, used by graphFromGUMString and loadGraph
%define ADD_GUM_GRAPH_READERS(classname, suffix)
%inline %{
  classname _graphFromGUMString_##suffix(const std::string& content) {
    return gum::graphFromGUMString< classname >(content);
  }
  classname _loadGraphGUM_##suffix(const std::string& path, bool binary) {
    return gum::loadGraphGUM< classname >(path, binary);
  }
%}
%enddef

ADD_GUM_GRAPH_READERS(gum::DiGraph, DiGraph)
ADD_GUM_GRAPH_READERS(gum::DAG, DAG)
ADD_GUM_GRAPH_READERS(gum::UndiGraph, UndiGraph)
ADD_GUM_GRAPH_READERS(gum::MixedGraph, MixedGraph)
ADD_GUM_GRAPH_READERS(gum::PDAG, PDAG)
ADD_GUM_GRAPH_READERS(gum::ChainGraph, ChainGraph)
ADD_GUM_GRAPH_READERS(gum::MarkedGraph, MarkedGraph)
ADD_GUM_GRAPH_READERS(gum::PAG, PAG)
ADD_GUM_GRAPH_READERS(gum::CPDAG, CPDAG)
ADD_GUM_GRAPH_READERS(gum::MPDAG, MPDAG)
ADD_GUM_GRAPH_READERS(gum::MAG, MAG)

%inline %{
  std::string _graphGUMType(const std::string& content) { return gum::graphGUMType(content); }
  std::string _graphGUMTypeOfFile(const std::string& path, bool binary) {
    return gum::graphGUMTypeOfFile(path, binary);
  }
%}

%pythoncode %{
def _setstate_from_former_pickle(graph, state):
  # the pickles of the graphs up to pyAgrum 3.3.2: a dict of arcs and edges
  import warnings

  warnings.warn(
    "Reading a graph pickled by pyAgrum 3.3.2 or before is deprecated after 3.3.2: pickle it again",
    DeprecationWarning,
    stacklevel=3,
  )
  graph.__init__()
  for x, y in state.get("arcs", []):
    for n in (x, y):
      if not graph.existsNode(n):
        graph.addNodeWithId(n)
    graph.addArc(x, y)
  for x, y in state.get("edges", []):
    for n in (x, y):
      if not graph.existsNode(n):
        graph.addNodeWithId(n)
    graph.addEdge(x, y)


def graphFromGUMString(content: str):
  """
  Read a graph from a jgum string (see the method ``saveGUMstring`` of the graphs).

  Parameters
  ----------
  content : str
    the jgum string

  Returns
  -------
  pyagrum.DiGraph or pyagrum.DAG or pyagrum.UndiGraph or pyagrum.MixedGraph or pyagrum.PDAG or pyagrum.ChainGraph or pyagrum.MarkedGraph or pyagrum.PAG or pyagrum.CPDAG or pyagrum.MPDAG or pyagrum.MAG or pyagrum.EssentialGraph
    the graph, of the class written in the string

  Raises
  ------
  pyagrum.GumIOError
    if the string is not a graph in the GUM format, or if the graph does not respect the
    properties of its class (e.g. a cycle in a DAG)
  """
  graphType = _graphGUMType(content)
  reader = globals().get("_graphFromGUMString_" + graphType)
  if reader is None:
    raise GumIOError(f"Unknown graph class '{graphType}' in the jgum string")
  return reader(content)


def loadGraph(filename: str):
  """
  Read a graph from a file, its format being given by its extension: .jgum (GUM, text), .bgum (GUM,
  binary) or .pkl (pickle).

  Parameters
  ----------
  filename : str
    the name of the file (see :func:`pyagrum.availableGraphExts`)

  Returns
  -------
  pyagrum.DiGraph or pyagrum.DAG or pyagrum.UndiGraph or pyagrum.MixedGraph or pyagrum.PDAG or pyagrum.ChainGraph or pyagrum.MarkedGraph or pyagrum.PAG or pyagrum.CPDAG or pyagrum.MPDAG or pyagrum.MAG or pyagrum.EssentialGraph
    the graph, of the class written in the file

  Raises
  ------
  pyagrum.GumIOError
    if the file can not be read or is not a graph in the GUM format, or if the graph does not
    respect the properties of its class (e.g. a cycle in a DAG)
  pyagrum.InvalidArgument
    if the extension is unknown (see :func:`pyagrum.availableGraphExts`)
  """
  name = str(filename)
  suffix = ospath.splitext(name)[1][1:]
  extension = suffix.upper()
  if extension == "PKL":
    return _gum_pickle_load(name)
  if extension not in ("JGUM", "BGUM"):
    raise InvalidArgument(f"extension '{suffix}' unknown. Please use among {availableGraphExts()}")
  binary = extension == "BGUM"
  graphType = _graphGUMTypeOfFile(name, binary)
  reader = globals().get("_loadGraphGUM_" + graphType)
  if reader is None:
    raise GumIOError(f"Unknown graph class '{graphType}' in {filename}")
  return reader(name, binary)


def saveGraph(graph, filename: str) -> None:
  """
  Save a graph in a file, its format being given by its extension: .jgum (GUM, text), .bgum (GUM,
  binary) or .pkl (pickle).

  Parameters
  ----------
  graph : pyagrum.DiGraph or pyagrum.DAG or pyagrum.UndiGraph or pyagrum.MixedGraph or pyagrum.PDAG or pyagrum.ChainGraph or pyagrum.MarkedGraph or pyagrum.PAG or pyagrum.CPDAG or pyagrum.MPDAG or pyagrum.MAG or pyagrum.EssentialGraph
    the graph (any class of graph; pickle only for `pyagrum.CliqueGraph`)
  filename : str
    the name of the file (see :func:`pyagrum.availableGraphExts`)

  Raises
  ------
  pyagrum.InvalidArgument
    if the extension is unknown (see :func:`pyagrum.availableGraphExts`)
  NotImplementedError
    if graph is a `pyagrum.CliqueGraph` (a junction tree included) and the format is GUM (a
    `pyagrum.CliqueGraph` can be pickled)
  """
  name = str(filename)
  suffix = ospath.splitext(name)[1][1:]
  extension = suffix.upper()
  if extension == "PKL":
    _gum_pickle_save(graph, name)
  elif extension in ("JGUM", "BGUM"):
    graph.saveGUM(name, binary=extension == "BGUM")
  else:
    raise InvalidArgument(f"extension '{suffix}' unknown. Please use among {availableGraphExts()}")


def availableGraphExts() -> str:
  """ Give the list of all formats known by pyAgrum to save a graph.

  :return: a string which lists all suffixes for supported graph file formats.
  """
  return "jgum|bgum|pkl"

%}


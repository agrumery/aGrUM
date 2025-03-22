/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




 // defines some macro for PGM classes


%pythonappend gum::DAGmodel::dag %{
  from pyagrum.base import DAG
  val = DAG(val) # copying the DAG
%}

%define IMPROVE_GRAPHICAL_MODEL_API(classname)
%extend classname {
  PyObject* names() const {
     PyObject* q=PySet_New(0);

     PyObject* pyval;
     for ( auto node : self->nodes()) {
       pyval=PyString_FromString(self->variable(node).name().c_str());
       PySet_Add(q,pyval);
       Py_DecRef(pyval);
     }
     return q;
  };


%pythoncode {
  def __iter__(self):
    """
    Iterate over the variables of the model

    Yield
    -----
    Tuple[int,str]
      The index of the variable and its name
    """
    for i in self.nodes():
      yield i,self.variable(i).name()

  def __getitem__(self, key):
    if isinstance(key, int):
      return self.variable(key)
    elif isinstance(key, str):
      return self.variableFromName(key)
    else:
      raise TypeError("key must be an int or a string")
}
};
// macro from graphs.i
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(classname);
%enddef

%define IMPROVE_DIRECTED_GRAPHICAL_MODEL_API(classname)

IMPROVE_GRAPHICAL_MODEL_API(classname);

%extend classname {
   PyObject *arcs() const {
     return PyAgrumHelper::PySetFromArcSet(self->arcs());
   };

   PyObject *parents(PyObject* norid) const {
     return PyAgrumHelper::PySetFromNodeSet(self->parents(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
   };
   PyObject *children(PyObject* norid) const {
     return PyAgrumHelper::PySetFromNodeSet(self->children(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
   };
   PyObject *family(PyObject* norid) const {
     return PyAgrumHelper::PySetFromNodeSet(self->family(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
   };
   PyObject *descendants(PyObject* norid) const {
     return PyAgrumHelper::PySetFromNodeSet(self->descendants(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
   };
   PyObject *ancestors(PyObject* norid) const {
     return PyAgrumHelper::PySetFromNodeSet(self->ancestors(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
   };

   gum::UndiGraph moralizedAncestralGraph(PyObject* nodes) {
     gum::NodeSet sonodes;
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sonodes,nodes,self->variableNodeMap());
     return self->moralizedAncestralGraph(sonodes);
   };
}
%enddef


%define IMPROVE_UNDIRECTED_GRAPHICAL_MODEL_API(classname)

IMPROVE_GRAPHICAL_MODEL_API(classname);

%extend classname {
  PyObject* neighbours(PyObject * norid) const {
    return PyAgrumHelper::PySetFromNodeSet(self->neighbours(
       PyAgrumHelper::nodeIdFromNameOrIndex(norid, self->variableNodeMap())));
  };

  PyObject* edges() const {
    return PyAgrumHelper::PySetFromEdgeSet(self->graph().edges());
  };
}
%enddef

/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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
 // defines some macro for PGM classes


%pythonappend gum::DAGmodel::dag %{
  val = DAG(val) # copying the DAG
%}

%define IMPROVE_GRAPHICAL_MODEL_API(classname)
%extend classname {
  PyObject* names() const {
     PyObject* q=PySet_New(0);

     for ( auto node : self->nodes()) {
       PySet_Add(q,PyString_FromString(self->variable(node).name().c_str()));
     }
     return q;
  };
}

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
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
 
%ignore gum::InfluenceDiagram::variableNodeMap;
%ignore gum::InfluenceDiagram::insertArc;
%ignore gum::InfluenceDiagram::getPartialTemporalOrder;
%ignore gum::InfluenceDiagram:: addUtilityNode ( const gum::DiscreteVariable& variable,gum::MultiDimImplementation<GUM_SCALAR>* aContent, NodeId id = 0 );

%typemap(out) const gum::Sequence<gum::NodeId>& {
  PyObject *q=PyList_New(0);
  for(auto i : *$1) {
    PyList_Append(q,PyInt_FromLong(i));
  }
  $result=q;
}

%include "extensions/IDGenerator.h"

%{
#include "extensions/IDGenerator.h"
%}


%extend gum::InfluenceDiagram {
    PyObject *names() const {
      PyObject* q=PyList_New(0);

      const gum::DAG& dag=self->dag();
      for ( gum::NodeGraphPartIterator node_iter = dag.nodes().begin();node_iter != dag.nodes().end(); ++node_iter ) {
        PyList_Append(q,PyString_FromString(self->variable(*node_iter).name().c_str()));
      }
      return q;
    };

    PyObject *nodes() {
      PyObject* q=PyList_New(0);

      const gum::DAG& dag=self->dag();
      for ( gum::NodeGraphPartIterator  node_iter = dag.nodes().begin();node_iter != dag.nodes().end(); ++node_iter ) {
        PyList_Append(q,PyInt_FromLong(*node_iter));
      }

      return q;
    };

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

   bool loadBIFXML(std::string name, PyObject *l=(PyObject*)0) {
    try {
        gum::BIFXMLIDReader<GUM_SCALAR> reader(self,name);
        reader.proceed(); 
        return true;
    } catch (gum::IOError& e) {
        throw(e);
    }

    return false;
  };

  void saveBIFXML(std::string name) {
      gum::BIFXMLIDWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };
}


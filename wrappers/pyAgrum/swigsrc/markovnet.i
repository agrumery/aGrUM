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

// macro from graphs.i
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::IMarkovNet);

%define IMPROVE_MARKOVNET_API(classname)
%extend classname {
  PyObject *names() const {
    PyObject* q=PyList_New(0);

    for ( auto node : self->graph().nodes()) {
      PyList_Append(q,PyString_FromString(self->variable(node).name().c_str()));
    }
    return q;
  };

  PyObject *neighbours(PyObject* norid) const {
    return PyAgrumHelper::PySetFromNodeSet(self->neighbours(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
  };
  PyObject *edges() const {
    return PyAgrumHelper::PySetFromEdgeSet(self->graph().edges());
  };

  const gum::Potential<double>& factor(PyObject* nodeseq) const {
    gum::NodeSet idx;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(idx,nodeseq,self->variableNodeMap());

    return self->factor(idx);
  };
}
%enddef
IMPROVE_MARKOVNET_API(gum::IMarkovNet);
IMPROVE_MARKOVNET_API(gum::MarkovNet);

// for gum::IMarkovNet::factors
%typemap(out) const gum::FactorTable<double> & {
  $result = PyList_New(0);

  for (auto kv : *$1) {
    PyList_Append($result, PyAgrumHelper::PySetFromNodeSet(kv.first));
  }
}

%define IMPROVE_CONCRETEMARKOVNET_API(classname)
%extend classname {
%pythoncode {
def addStructureListener(self,whenNodeAdded=None,whenNodeDeleted=None,whenEdgeAdded=None,whenedgeDeleted=None):
    """
    Add the listeners in parameters to the list of existing ones.

    Parameters
    ----------
    whenNodeAdded : lambda expression
      a function for when a node is added
    whenNodeDeleted : lambda expression
      a function for when a node is removed
    whenEdgeAdded : lambda expression
      a function for when an edge is added
    whenEdgeDeleted : lambda expression
      a function for when an edge is removed
    """
    if [whenNodeAdded,whenNodeDeleted,whenEdgeAdded,whenEdgeDeleted]==[None,None,None,None]:
      return

    if not hasattr(self,"_listeners"):
      self._listeners=[]

    nl = PythonMNListener(self, self.variableNodeMap())
    if whenNodeAdded is not None:
      nl.setWhenNodeAdded(whenNodeAdded)
    if whenNodeDeleted is not None:
      nl.setWhenNodeDeleted(whenNodeDeleted)
    if whenEdgeAdded is not None:
      nl.setWhenEdgeAdded(whenEdgeAdded)
    if whenEdgeDeleted is not None:
      nl.setWhenArcDeleted(whenEdgeDeleted)

    self._listeners.append(nl)
}
}
%enddef
IMPROVE_CONCRETEBAYESNET_API(gum::MarkovNet);

%extend gum::MarkovNet {
   std::string loadUAI(std::string name, PyObject *l=(PyObject*)0)
   {
     std::stringstream stream;
     std::vector<PythonLoadListener> py_listener;

     try {
       gum::UAIMNReader<GUM_SCALAR> reader(self,name);
       int l_size=__fillLoadListeners(py_listener,l);
       for(int i=0 ; i<l_size ; i++) {
         GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
       }

       auto nbErr=reader.proceed();
       reader.showElegantErrorsAndWarnings(stream);
       if (nbErr>0) {
         reader.showErrorCounts(stream);
         GUM_ERROR(gum::FatalError,stream.str());
       } else {
         return stream.str();
       }
     } catch (gum::IOError& e) {
       throw(e);
     }
     return "";
   };

   void saveUAI(std::string name) {
     gum::UAIMNWriter<GUM_SCALAR> writer;
     writer.write( name, *self );
   };

%pythonappend gum::UGmodel::graph %{
    val = UndiGraph(val) # copying the graph
%}
}

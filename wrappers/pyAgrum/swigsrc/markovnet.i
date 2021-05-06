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

%define IMPROVE_MARKOVNET_API(classname)
IMPROVE_UNDIRECTED_GRAPHICAL_MODEL_API(classname)

%extend classname {
  PyObject *minimalCondSet(gum::NodeId target,PyObject* list) const {
    gum::NodeSet soids;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soids,list,self->variableNodeMap());
    return PyAgrumHelper::PySetFromNodeSet(self->minimalCondSet(target, soids));
  };


  PyObject *minimalCondSet(PyObject* targets,PyObject* list) const {
    gum::NodeSet sotargets;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sotargets,targets,self->variableNodeMap());

    gum::NodeSet soids;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soids,list,self->variableNodeMap());
    return PyAgrumHelper::PySetFromNodeSet(self->minimalCondSet(sotargets, soids));
  };


  const gum::Potential<double>& factor(PyObject* nodeseq) const {
    gum::NodeSet idx;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(idx,nodeseq,self->variableNodeMap());

    return self->factor(idx);
  };

   bool isIndependent(PyObject* X,PyObject* Y,PyObject* Z) {
     gum::NodeSet nx,ny,nz;
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nx,X,self->variableNodeMap());
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(ny,Y,self->variableNodeMap());
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nz,Z,self->variableNodeMap());
     return self->isIndependent(nx,ny,nz);
   }
   bool isIndependent(PyObject* X,PyObject* Y) {
     gum::NodeSet nx,ny,nz;
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(nx,X,self->variableNodeMap());
     PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(ny,Y,self->variableNodeMap());
     return self->isIndependent(nx,ny,nz);
   }
}
%enddef
IMPROVE_MARKOVNET_API(gum::IMarkovNet);
IMPROVE_MARKOVNET_API(gum::MarkovNet);

%define IMPROVE_CONCRETEMARKOVNET_API(classname)

%ignore gum::classname::addFactor(const std::vector<std::string,std::allocator< std::string > > const &);
%ignore gum::classname::addFactor(const gum::NodeSet &);
%ignore gum::classname::addFactor(const gum::Potential<double> &);
%ignore gum::classname::eraseFactor(const std::vector<std::string,std::allocator< std::string > > const &);
%ignore gum::classname::eraseFactor(const gum::NodeSet &);

%extend gum::classname {
    const Potential<double>& addFactor(PyObject *seq) {
      gum::NodeSet son;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(son,seq,self->variableNodeMap());
      return self->addFactor(son);
    };
    void eraseFactor(PyObject *seq) {
      gum::NodeSet son;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(son,seq,self->variableNodeMap());
      self->eraseFactor(son);
    };

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
IMPROVE_CONCRETEMARKOVNET_API(MarkovNet<double>);

%extend gum::MarkovNet {
   std::string loadUAI(std::string name, PyObject *l=(PyObject*)0)
   {
     std::stringstream stream;
     std::vector<PythonLoadListener> py_listener;

     try {
       gum::UAIMNReader<GUM_SCALAR> reader(self,name);
       int l_size=fillLoadListeners__(py_listener,l);
       for(int i=0 ; i<l_size ; i++) {
         GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
       }

       auto nbErr=reader.proceed();
       reader.showElegantErrorsAndWarnings(stream);
       if (nbErr>0) {
         reader.showErrorCounts(stream);
         GUM_ERROR(gum::FatalError,stream.str())
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

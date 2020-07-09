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
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::IBayesNet);

 %ignore gum::BayesNet<GUM_SCALAR>::add(const gum::DiscreteVariable& variable, gum::MultiDimImplementation<GUM_SCALAR> * aContent);

%ignore *::beginNodes;
%ignore *::endNodes;
%ignore *::beginNodesSafe;
%ignore *::endNodesSafe;
%ignore *::beginArcs;
%ignore *::endArcs;
%ignore *::beginEdges;
%ignore *::endEdges;

%include "extensions/BNGenerator.h"

%{
#include "extensions/BNGenerator.h"
%}

%define IMPROVE_BAYESNET_API(classname)
%extend classname {
  PyObject *names() const {
    PyObject* q=PyList_New(0);

    for ( auto node : self->dag().nodes()) {
      PyList_Append(q,PyString_FromString(self->variable(node).name().c_str()));
    }
    return q;
  };

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

  PyObject *parents(PyObject* norid) const {
    return PyAgrumHelper::PySetFromNodeSet(self->parents(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
  };
  PyObject *children(PyObject* norid) const {
    return PyAgrumHelper::PySetFromNodeSet(self->children(PyAgrumHelper::nodeIdFromNameOrIndex(norid,self->variableNodeMap())));
  };
  PyObject *arcs() const {
    return PyAgrumHelper::PySetFromArcSet(self->arcs());
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
IMPROVE_BAYESNET_API(gum::IBayesNet);
IMPROVE_BAYESNET_API(gum::BayesNet);
IMPROVE_BAYESNET_API(gum::BayesNetFragment);

%define IMPROVE_CONCRETEBAYESNET_API(classname)
%extend classname {
%pythoncode {
def addStructureListener(self,whenNodeAdded=None,whenNodeDeleted=None,whenArcAdded=None,whenArcDeleted=None):
    """
    Add the listeners in parameters to the list of existing ones.

    Parameters
    ----------
    whenNodeAdded : lambda expression
      a function for when a node is added
    whenNodeDeleted : lambda expression
      a function for when a node is removed
    whenArcAdded : lambda expression
      a function for when an arc is added
    whenArcDeleted : lambda expression
      a function for when an arc is removed
    """
    if [whenNodeAdded,whenNodeDeleted,whenArcAdded,whenArcDeleted]==[None,None,None,None]:
      return

    if not hasattr(self,"_listeners"):
      self._listeners=[]

    nl = PythonBNListener(self, self.variableNodeMap())
    if whenNodeAdded is not None:
      nl.setWhenNodeAdded(whenNodeAdded)
    if whenNodeDeleted is not None:
      nl.setWhenNodeDeleted(whenNodeDeleted)
    if whenArcAdded is not None:
      nl.setWhenArcAdded(whenArcAdded)
    if whenArcDeleted is not None:
      nl.setWhenArcDeleted(whenArcDeleted)

    self._listeners.append(nl)
}
}
%enddef
IMPROVE_CONCRETEBAYESNET_API(gum::BayesNet);
IMPROVE_CONCRETEBAYESNET_API(gum::BayesNetFragment);

%extend gum::BayesNet {
  std::string loadBIF(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFReader<GUM_SCALAR> reader(self,name);
          int l_size=fillLoadListeners__(py_listener,l);
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

  void saveBIF(std::string name) {
      gum::BIFWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadDSL(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::DSLReader<GUM_SCALAR> reader(self,name);
          int l_size=fillLoadListeners__(py_listener,l);
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

  void saveDSL(std::string name) {
      gum::DSLWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadNET(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::NetReader<GUM_SCALAR> reader(self,name);
          int l_size=fillLoadListeners__(py_listener,l);
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

  void saveNET(std::string name) {
      gum::NetWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadO3PRM(std::string name, std::string system="",std::string classpath="",PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::O3prmBNReader<GUM_SCALAR> reader(self,name,system,classpath);

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

  void saveO3PRM(std::string name) {
      gum::O3prmBNWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadBIFXML(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFXMLBNReader<GUM_SCALAR> reader(self,name);

          int isOK=reader.proceed();// for BIFXML, proceed() returns 0 or 1
          if (isOK==1) {
              GUM_ERROR(gum::FatalError,"Errors found in XML file");
          } else {
              return "";
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveBIFXML(std::string name) {
      gum::BIFXMLBNWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadUAI(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::UAIBNReader<GUM_SCALAR> reader(self,name);
          int l_size=fillLoadListeners__(py_listener,l);
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
      gum::UAIBNWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };
}

%pythonappend gum::DAGmodel::dag %{
    val = DAG(val) # copying the DAG
%}

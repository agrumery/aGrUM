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

// macro from graphs.i
%ignore gum::BayesNet<GUM_SCALAR>::add(const gum::DiscreteVariable& variable, gum::MultiDimImplementation<GUM_SCALAR> * aContent);

%ignore *::beginNodes;
%ignore *::endNodes;
%ignore *::beginNodesSafe;
%ignore *::endNodesSafe;
%ignore *::beginArcs;
%ignore *::endArcs;
%ignore *::beginEdges;
%ignore *::endEdges;
%ignore *::variables;

%include "extensions/BNGenerator.h"
%{
#include "extensions/BNGenerator.h"
%}

%define IMPROVE_BAYESNET_API(classname)
%extend classname {
  PyObject*ids(const std::vector<std::string>& names) {
    return PyAgrumHelper::PyListFromNodeVect(self->ids(names));
  }

  PyObject*nodeset(const std::vector<std::string>& names) {
    return PyAgrumHelper::PySetFromNodeVect(self->ids(names));
  }

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

IMPROVE_DIRECTED_GRAPHICAL_MODEL_API(classname);
%enddef

IMPROVE_BAYESNET_API(gum::IBayesNet);
IMPROVE_BAYESNET_API(gum::BayesNet);
IMPROVE_BAYESNET_API(gum::BayesNetFragment);

%define IMPROVE_CONCRETEBAYESNET_API(classname)
%extend classname {
%pythoncode {
def addVariables(self,listFastVariables,default_nbr_mod=2):
   """
   Add a list of variable in the form of 'fast' syntax.

   Parameters
   ----------
   listFastVariables: List[str]
     the list of variables in 'fast' syntax.
   default_nbr_mod: int
     the number of modalities for the variable if not specified following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`. Note that default_nbr_mod=1 is
     mandatory to create variables with only one modality (for utility for instance).

   Returns
   -------
   List[int]
     the list of created ids.
   """
   return [self.add(descr,default_nbr_mod) for descr in listFastVariables]

def addArcs(self,listArcs):
  """
  add a list of arcs in te model.

  Parameters
  ----------
  listArcs : List[Tuple[int|str,int|str]]
    the list of arcs
  """
  self.beginTopologyTransformation()
  for arc in listArcs:
    self.addArc(*arc)
  self.endTopologyTransformation()

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
  std::string loadBIF(std::string name, PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFReader<GUM_SCALAR> reader(self,name);
          int l_size=_fillLoadListeners_(py_listener,l);
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

  void saveBIF(std::string name,bool allowModificationWhenSaving=false) {
      gum::BIFWriter<GUM_SCALAR> writer;
      writer.setAllowModification(allowModificationWhenSaving);
      writer.write( name, *self );
  };

  std::string loadDSL(std::string name, PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::DSLReader<GUM_SCALAR> reader(self,name);
          int l_size=_fillLoadListeners_(py_listener,l);
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
      return "plop";
  };

  std::string loadXDSL(std::string name, PyObject *l=nullptr)
  {
      try {
          gum::XDSLBNReader<GUM_SCALAR> reader(self,name);

          int isOK=reader.proceed();// for BIFXML, proceed() returns 0 or 1
          if (isOK==1) {
              GUM_ERROR(gum::FatalError,"Errors found in XML file")
          } else {
              return "";
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveDSL(std::string name,bool allowModificationWhenSaving=false) {
    gum::DSLWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  void saveXDSL(std::string name,bool allowModificationWhenSaving=false) {
    gum::XDSLBNWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  std::string loadNET(std::string name, PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::NetReader<GUM_SCALAR> reader(self,name);
          int l_size=_fillLoadListeners_(py_listener,l);
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

  void saveNET(std::string name,bool allowModificationWhenSaving=false) {
    gum::NetWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  std::string loadO3PRM(std::string name, std::string system="",std::string classpath="",PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::O3prmBNReader<GUM_SCALAR> reader(self,name,system,classpath);

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

  void saveO3PRM(std::string name,bool allowModificationWhenSaving=false) {
    gum::O3prmBNWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  std::string loadBIFXML(std::string name, PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFXMLBNReader<GUM_SCALAR> reader(self,name);

          int isOK=reader.proceed();// for BIFXML, proceed() returns 0 or 1
          if (isOK==1) {
              GUM_ERROR(gum::FatalError,"Errors found in XML file")
          } else {
              return "";
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveBIFXML(std::string name,bool allowModificationWhenSaving=false) {
    gum::BIFXMLBNWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  std::string loadUAI(std::string name, PyObject *l=nullptr)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::UAIBNReader<GUM_SCALAR> reader(self,name);
          int l_size=_fillLoadListeners_(py_listener,l);
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

  void saveUAI(std::string name,bool allowModificationWhenSaving=false) {
    gum::UAIBNWriter<GUM_SCALAR> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *self );
  };

  %pythoncode {
def __getstate__(self):
    pyAgrum.base._gum_add_properties_while_getstate_(self)
    state={"nodes":[self.variable(i).toFast() for i in self.nodes()],
           "parents":{self.variable(i).name():list(self.cpt(i).names)[1:] for i in self.nodes()},
           "cpt":{self.variable(i).name():self.cpt(i)[:].flatten().tolist() for i in self.nodes()},
           "properties":{k:self.property(k) for k in self.properties()}
          }
    return state

def __setstate__(self,state):
    self.__init__()
    for fastvar in state['nodes']:
        self.add(fastvar)
    self.beginTopologyTransformation()
    for son in state['parents']:
        for father in state['parents'][son]:
            self.addArc(father,son)
    self.endTopologyTransformation()
    for node in state['cpt']:
        self.cpt(node).fillWith(state['cpt'][node])
    for prop in state['properties']:
        self.setProperty(prop,state['properties'][prop])
    return self

def toFast(self, filename: str = None) -> str:
  """
  Export the Bayesian network as *fast* syntax (in a string or in a python file)

  Parameters
  ----------
  filename : Optional[str]
    the name of the file (including the prefix), if None , use sys.stdout
  """

  def _toFastBN(bn,pythoncode=False):
    res = []
    sovars = set()
    for x, y in bn.arcs():
      if x in sovars:
        src = bn.variable(x).name()
      else:
        src = bn.variable(x).toFast()
        sovars.add(x)
      if y in sovars:
        dst = bn.variable(y).name()
      else:
        dst = bn.variable(y).toFast()
        sovars.add(y)
      res.append(f"{src}->{dst}")

    for x in bn.nodes():
      if x not in sovars:
        res .append(bn.variable(x).toFast())

    if pythoncode:
      return 'model=gum.fastBN("""'+';\n     '.join(res)+'""")'
    else:
      return ';'.join(res)

  if filename is None:
    return _toFastBN(self)
  else:
    with open(filename, "w") as pyfile:
      print(_toFastBN(self,pythoncode=True), file=pyfile)
  }
}

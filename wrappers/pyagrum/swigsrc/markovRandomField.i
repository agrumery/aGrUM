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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







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


  const gum::Tensor<double>& factor(PyObject* nodeseq) const {
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
IMPROVE_MARKOVNET_API(gum::IMarkovRandomField);
IMPROVE_MARKOVNET_API(gum::MarkovRandomField);

%define IMPROVE_CONCRETEMARKOVNET_API(classname)

%ignore gum::classname::addFactor(const std::vector<std::string,std::allocator< std::string > > const &);
%ignore gum::classname::addFactor(const gum::NodeSet &);
%ignore gum::classname::addFactor(const gum::Tensor<double> &);
%ignore gum::classname::eraseFactor(const std::vector<std::string,std::allocator< std::string > > const &);
%ignore gum::classname::eraseFactor(const gum::NodeSet &);

%extend gum::classname {
    const Tensor<double>& addFactor(PyObject *seq) {
      if (PyList_Check(seq)) {
        std::vector<std::string> names;
        PyAgrumHelper::populateStrVectorFromPySequenceOfIntOrString(names,seq,self->variableNodeMap());
        return self->addFactor(names);
      } else {
        gum::NodeSet son;
        PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(son,seq,self->variableNodeMap());
        return self->addFactor(son);
      }
    };
    void eraseFactor(PyObject *seq) {
      gum::NodeSet son;
      PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(son,seq,self->variableNodeMap());
      self->eraseFactor(son);
    };

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

    nl = PythonBNListener(self, self.variableNodeMap())
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
IMPROVE_CONCRETEMARKOVNET_API(MarkovRandomField<double>);

%extend gum::MarkovRandomField {
   std::string loadUAI(std::string name, PyObject *l=(PyObject*)0)
   {
     std::stringstream stream;
     std::vector<PythonLoadListener> py_listener;

     try {
       gum::UAIMRFReader<GUM_SCALAR> reader(self,name);
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

   void saveUAI(std::string name) {
     gum::UAIMRFWriter<GUM_SCALAR> writer;
     writer.write( name, *self );
   };

  %pythoncode {
def __getstate__(self):
    _gum_add_properties_while_getstate_(self)
    state={"nodes":[self.variable(i).toFast() for i in self.nodes()],
           "factors":[[n for n in self.factor(factor).names] for factor in self.factors()],
           "potential":{"-".join(self.factor(factor).names):self.factor(factor)[:].flatten().tolist() for factor in self.factors()},
           "properties":{k:self.property(k) for k in self.properties()}
          }
    return state

def __setstate__(self,state):
    self.__init__()
    for fastvar in state['nodes']:
        self.add(fastvar)
    self.beginTopologyTransformation()
    for factor in state['factors']:
         self.addFactor(factor)
    self.endTopologyTransformation()
    for cliq in state['potential']:
        self.factor(cliq.split("-")).fillWith(state['potential'][cliq])
    for prop in state['properties']:
        self.setProperty(prop,state['properties'][prop])
    return self

def toFast(self, filename: str = None) -> str:
  """
  Export the MRF as *fast* syntax (in a string or in a python file)

  Parameters
  ----------
  filename : Optional[str]
    the name of the file (including the prefix), if None , use sys.stdout
  """

  def _toFastMRF(model,pythoncode=False):
    res = []
    sovars = set()
    first = True
    for f in model.factors():
      l = []
      for x in f:
          if x in sovars:
            src = model.variable(x).name()
          else:
            src = model.variable(x).toFast()
            sovars.add(x)
          l.append(src)
      res.append("--".join(l))

    for x in model.nodes():
      if x not in sovars:
        res.append(model.variable(x).toFast())

    if pythoncode:
      return 'model=gum.fastMRF("""'+';\n     '.join(res)+'""")'
    else:
      return ';'.join(res)

  if filename is None:
    return _toFastMRF(self)
  else:
    with open(filename, "w") as pyfile:
      print(_toFastBN(self,pythoncode=True), file=pyfile)
  }

%pythonappend gum::UGmodel::graph %{
    val = UndiGraph(val) # copying the graph
%}
}

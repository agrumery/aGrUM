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








%define IMPROVE_MARKOVNET_API(classname)
IMPROVE_UNDIRECTED_GRAPHICAL_MODEL_API(classname)

%extend classname {
  PyAgrumSetOfInt *minimalCondSet(gum::NodeId target,PyObject* list) const {
    gum::NodeSet soids;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soids,list,self->variableNodeMap());
    return PyAgrumHelper::PySetFromNodeSet(self->minimalCondSet(target, soids));
  };


  PyAgrumSetOfInt *minimalCondSet(PyObject* targets,PyObject* list) const {
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


%extend gum::MarkovRandomField {
  void loadGUM(std::string name, bool binary=false) {
    std::stringstream stream;
    try {
      gum::GumMRFReader<GUM_SCALAR> reader(self,name,binary);
      auto nbErr=reader.proceed();
      reader.showElegantErrorsAndWarnings(stream);
      if (nbErr>0) {
        reader.showErrorCounts(stream);
        GUM_ERROR(gum::FatalError,stream.str())
      }
    } catch (gum::IOError& e) { throw(e); }
  }

  void saveGUM(std::string name, bool binary=false, int indent=2) {
    gum::GumMRFWriter<GUM_SCALAR> writer(binary,indent);
    writer.write(name,*self);
  }

  std::string saveGUMstring(int indent=2) {
    gum::GumMRFWriter<GUM_SCALAR> writer(false,indent);
    self->updateMetaData();
    return writer.toString(*self);
  }

  void loadGUMstring(const std::string& content) {
    std::stringstream stream;
    try {
      gum::GumMRFReader<GUM_SCALAR> reader(self);
      auto nbErr=reader.proceedFromString(content);
      reader.showElegantErrorsAndWarnings(stream);
      if (nbErr>0) {
        reader.showErrorCounts(stream);
        GUM_ERROR(gum::FatalError,stream.str())
      }
    } catch (gum::IOError& e) { throw(e); }
  }

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
    return self.saveGUMstring()

def __setstate__(self,state):
    self.__init__()
    self.loadGUMstring(state)
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

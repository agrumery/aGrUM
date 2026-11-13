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








// the public toFast (with a filename) is defined below in %pythoncode
%rename(_toFast) gum::MarkovRandomField::toFast;
%include "keepModelAlive.i"
GUM_KEEP_MODEL_ALIVE(gum::IMarkovRandomField<double>::factor)
GUM_KEEP_MODEL_ALIVE(gum::MarkovRandomField<double>::factor)
GUM_KEEP_MODEL_ALIVE(gum::MarkovRandomField<double>::variable)
GUM_KEEP_MODEL_ALIVE(gum::MarkovRandomField<double>::variableFromName)

%define IMPROVE_MARKOVNET_API(classname)
IMPROVE_UNDIRECTED_GRAPHICAL_MODEL_API(classname)

%extend classname {
  gum::NodeSet minimalCondSet(gum::NodeId target,PyObject* list) const {
    gum::NodeSet soids;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soids,list,self->variableNodeMap());
    return self->minimalCondSet(target, soids);
  };


  gum::NodeSet minimalCondSet(PyObject* targets,PyObject* list) const {
    gum::NodeSet sotargets;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(sotargets,targets,self->variableNodeMap());

    gum::NodeSet soids;
    PyAgrumHelper::populateNodeSetFromPySequenceOfIntOrString(soids,list,self->variableNodeMap());
    return self->minimalCondSet(sotargets, soids);
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

// Renamed so the public saveGUM/saveGUMstring (added below in %pythoncode) can keep named arguments
// and default indent to None: the library then uses pyagrum.config["core","jgumIndent"]
// (see gum::defaultJgumIndent()), while an explicit indent is always used as is.
%rename(_saveGUM) gum::MarkovRandomField::saveGUM;
%rename(_saveGUMstring) gum::MarkovRandomField::saveGUMstring;

%extend gum::MarkovRandomField {
  void loadGUM(std::string name, PyObject* binary=nullptr) {
    std::stringstream stream;
    try {
      gum::GumMRFReader<GUM_SCALAR> reader(self,name,PyAgrumHelper::isBinaryGUMFile(name,binary));
      auto nbErr=reader.proceed();
      reader.showElegantErrorsAndWarnings(stream);
      if (nbErr>0) {
        reader.showErrorCounts(stream);
        GUM_ERROR(gum::FatalError,stream.str())
      }
    } catch (gum::IOError& e) { throw(e); }
  }

  void saveGUM(std::string name, PyObject* binary=nullptr, int indent=gum::defaultJgumIndent()) {
    gum::GumMRFWriter<GUM_SCALAR> writer(PyAgrumHelper::isBinaryGUMFile(name,binary),indent);
    writer.write(name,*self);
  }

  std::string saveGUMstring(int indent=gum::defaultJgumIndent()) {
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

def saveGUM(self, name: str, binary: bool | None = None, indent: int | None = None) -> None:
  """
  Save the Markov random field as a .jgum (text) or .bgum (binary) file.

  Parameters
  ----------
  name : str
    the name of the file
  binary : bool, optional
    True for the msgpack-based .bgum binary format, False for the JSON-based .jgum text format. By
    default, the format is given by the extension of name (binary for an unknown extension).
  indent : int, optional
    JSON indentation for the .jgum text format (ignored for the binary format): -1 for the most
    compact output, N>=0 to pretty-print with N spaces. Defaults to
    ``pyagrum.config["core","jgumIndent"]``.

  Raises
  ------
  pyagrum.InvalidArgument
    if binary contradicts the extension (``.jgum`` or ``.bgum``) of name
  """
  if indent is None:
    self._saveGUM(name, binary)
  else:
    self._saveGUM(name, binary, indent)

def saveGUMstring(self, indent: int = None) -> str:
  """
  Save the Markov random field as a jgum string.

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
  if indent is None:
    return self._saveGUMstring()
  return self._saveGUMstring(indent)

def toFast(self, filename: str = None) -> str:
  """
  Export the Markov random field in *fast* syntax (the structure and the variables, not the parameters), read back by
  `pyagrum.fastMRF`. A name is quoted if it would not be read back as is otherwise (see `pyagrum.quoteFastName`).

  Parameters
  ----------
  filename : str, optional
    if given, a Python file which builds the model with `pyagrum.fastMRF` is written

  Returns
  -------
  str
    the fast description of the model (if filename is None)
  """
  if filename is None:
    return self._toFast()
  with open(filename, "w") as pyfile:
    print(f"import pyagrum\n\nmodel = pyagrum.fastMRF({self._toFast()!r})", file=pyfile)
  }

%pythonappend gum::UGmodel::graph %{
    val = UndiGraph(val) # copying the graph
%}
}

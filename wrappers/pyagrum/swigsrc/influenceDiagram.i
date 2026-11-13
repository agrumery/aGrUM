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
%rename(_toFast) gum::InfluenceDiagram::toFast;
%include "keepModelAlive.i"
GUM_KEEP_MODEL_ALIVE(gum::InfluenceDiagram<double>::variable)
GUM_KEEP_MODEL_ALIVE(gum::InfluenceDiagram<double>::variableFromName)
GUM_KEEP_MODEL_ALIVE(gum::InfluenceDiagram<double>::cpt)
GUM_KEEP_MODEL_ALIVE(gum::InfluenceDiagram<double>::utility)

%ignore gum::InfluenceDiagram::variableNodeMap;
%ignore gum::InfluenceDiagram::insertArc;
%ignore gum::InfluenceDiagram::getPartialTemporalOrder;
// a signature with a default argument: each generated variant needs its own %ignore
%ignore gum::InfluenceDiagram<double>::addUtilityNode(const DiscreteVariable&, MultiDimImplementation<double>*, NodeId);
%ignore gum::InfluenceDiagram<double>::addUtilityNode(const DiscreteVariable&, MultiDimImplementation<double>*);

%include "extensions/IDGenerator.h"

%{
#include "extensions/IDGenerator.h"
%}


IMPROVE_DIRECTED_GRAPHICAL_MODEL_API(gum::InfluenceDiagram);

// Renamed so the public saveGUM/saveGUMstring (added below in %pythoncode) can keep named arguments
// and default indent to None: the library then uses pyagrum.config["core","jgumIndent"]
// (see gum::defaultJgumIndent()), while an explicit indent is always used as is.
%rename(_saveGUM) gum::InfluenceDiagram::saveGUM;
%rename(_saveGUMstring) gum::InfluenceDiagram::saveGUMstring;

%extend gum::InfluenceDiagram {
  void loadGUM(std::string name, PyObject* binary=nullptr) {
    std::stringstream stream;
    try {
      gum::GumIDReader<GUM_SCALAR> reader(self,name,PyAgrumHelper::isBinaryGUMFile(name,binary));
      auto nbErr=reader.proceed();
      reader.showElegantErrorsAndWarnings(stream);
      if (nbErr>0) {
        reader.showErrorCounts(stream);
        GUM_ERROR(gum::FatalError,stream.str())
      }
    } catch (gum::IOError& e) { throw(e); }
  }

  void saveGUM(std::string name, PyObject* binary=nullptr, int indent=gum::defaultJgumIndent()) {
    gum::GumIDWriter<GUM_SCALAR> writer(PyAgrumHelper::isBinaryGUMFile(name,binary),indent);
    writer.write(name,*self);
  }

  std::string saveGUMstring(int indent=gum::defaultJgumIndent()) {
    gum::GumIDWriter<GUM_SCALAR> writer(false,indent);
    self->updateMetaData();
    return writer.toString(*self);
  }

  void loadGUMstring(const std::string& content) {
    std::stringstream stream;
    try {
      gum::GumIDReader<GUM_SCALAR> reader(self);
      auto nbErr=reader.proceedFromString(content);
      reader.showElegantErrorsAndWarnings(stream);
      if (nbErr>0) {
        reader.showErrorCounts(stream);
        GUM_ERROR(gum::FatalError,stream.str())
      }
    } catch (gum::IOError& e) { throw(e); }
  }

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

  %pythoncode {
def addVariables(self,listFastVariables,default_nbr_mod=2):
   """
   Add a list of variable in the form of 'fast' syntax.

   Parameters
   ----------
   listFastVariables: list[str]
     the list of variables following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>` extended for :func:`pyagrum.fastID`.
   default_nbr_mod: int
     the number of modalities for the variable if not specified in the fast description. Note that default_nbr_mod=1 is
     mandatory to create variables with only one modality (for utility for instance).

   Returns
   -------
   list[int]
     the list of created ids.
   """
   return [self.add(descr,default_nbr_mod) for descr in listFastVariables]

def addArcs(self,listArcs):
  """
  add a list of arcs in te model.

  Parameters
  ----------
  listArcs : list[tuple[int,int]]
    the list of arcs
  """
  for arc in listArcs:
    self.addArc(*arc)


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

  Returns
  -------
  pyagrum.PythonBNListener | None
    the new listener (kept alive by the model), on which other functions can be set; None if no function is given
  """
  if [whenNodeAdded,whenNodeDeleted,whenArcAdded,whenArcDeleted]==[None,None,None,None]:
    return None

  if not hasattr(self,"_listeners"):
    self._listeners=[]

  nl = pyagrum.PythonBNListener(self, self._variableNodeMap())
  if whenNodeAdded is not None:
    nl.setWhenNodeAdded(whenNodeAdded)
  if whenNodeDeleted is not None:
    nl.setWhenNodeDeleted(whenNodeDeleted)
  if whenArcAdded is not None:
    nl.setWhenArcAdded(whenArcAdded)
  if whenArcDeleted is not None:
    nl.setWhenArcDeleted(whenArcDeleted)

  self._listeners.append(nl)
  return nl


def __getstate__(self):
    return self.saveGUMstring()

def __setstate__(self,state):
    self.__init__()
    self.loadGUMstring(state)
    return self


def saveGUM(self, name: str, binary: bool | None = None, indent: int | None = None) -> None:
  """
  Save the influence diagram as a .jgum (text) or .bgum (binary) file.

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
  Save the influence diagram as a jgum string.

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
  Export the influence diagram in *fast* syntax (the structure and the variables, not the parameters), read back by
  `pyagrum.fastID`. A name is quoted if it would not be read back as is otherwise (see `pyagrum.quoteFastName`).

  Parameters
  ----------
  filename : str, optional
    if given, a Python file which builds the model with `pyagrum.fastID` is written

  Returns
  -------
  str
    the fast description of the model (if filename is None)
  """
  if filename is None:
    return self._toFast()
  with open(filename, "w") as pyfile:
    print(f"import pyagrum\n\nmodel = pyagrum.fastID({self._toFast()!r})", file=pyfile)
  }
}

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




%module(package="pyagrum.prm", docstring="pyagrum.prm module") prm

// %import only carries typemap/type declarations into this module's own
// generated .cxx, not raw %{ %} code blocks -- these must be re-declared
// here too (see mrf.i for the long-form explanation).
%{
#include "extensions/helpers.h"
#include "extensions/pyagrumSentinelTypes.h"
#include "extensions/pyagrumExceptionHandling.h"
#include "extensions/PythonLoadListener.h"
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmBNWriter.h>
%}

%import "pyagrum.i"

/////////////////////////////////
/////// PRM submodule ///////////
/////////////////////////////////
%include "PRMexplorer.i"

// loadO3PRM/saveO3PRM: moved from core bayesnet.i (PRM totally split out).
//
// NOT a %extend: gum::BayesNet<double> is %template'd in the CORE module,
// not here, and SWIG silently drops %extend on a class reached only via
// %import (confirmed empirically -- it compiles with no error/warning, but
// the method never appears in the generated .py and never reaches the real
// pyagrum.BayesNet class). Instead: two plain %inline functions taking the
// BayesNet as an explicit argument, wrapped by *this* module as ordinary
// free functions, then bound onto the real pyagrum.BayesNet class from
// Python (%pythoncode below) the moment pyagrum.prm is imported.
%inline %{
namespace gum {
  std::string _o3prm_loadBN(gum::BayesNet<double>* bn, std::string name, std::string system, std::string classpath, PyObject* l) {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::O3prmBNReader<double> reader(bn,name,system,classpath);

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
  }

  void _o3prm_saveBN(gum::BayesNet<double>* bn, std::string name, bool allowModificationWhenSaving) {
    gum::O3prmBNWriter<double> writer;
    writer.setAllowModification(allowModificationWhenSaving);
    writer.write( name, *bn );
  }
}
%}

%pythoncode %{
def _bn_loadO3PRM(self, name, system="", classpath="", listeners=None):
  return _o3prm_loadBN(self, name, system, classpath, listeners)

def _bn_saveO3PRM(self, name, allowModificationWhenSaving=False):
  return _o3prm_saveBN(self, name, allowModificationWhenSaving)

# Monkeypatch onto the real core class, not the one SWIG would have generated
# for its own (unused) type reference in this module -- see the note above.
pyagrum.BayesNet.loadO3PRM = _bn_loadO3PRM
pyagrum.BayesNet.saveO3PRM = _bn_saveO3PRM
%}

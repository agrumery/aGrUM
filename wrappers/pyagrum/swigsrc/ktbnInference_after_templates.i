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

// This file contains %extend blocks that must appear AFTER the %template
// instantiation of gum::KTBNInference<double> in aGrUM_wrap_KTBN.i.

// ---------------------------------------------------------------------------
// KTBNInference<double>: addIntervention / addObservation (hard forms) and
// their batch overloads.
//
// KTBNModality (implicit int-or-str) and NodeKey (std::variant<std::string,
// std::pair<std::string,int>>) are never exposed to Python. The single-node
// hard-value overloads are rebuilt here taking a plain PyObject* value
// (int or str, via ktbnModalityFromPyObject); the batch overloads take a
// Python dict {node_key: value} (via populateKTBNNodeModalityVectorFromPyDict),
// node_key being either a str (engine name) or a (str,int) tuple.
//
// The *soft* addObservation(base, slice, likelihood) / (node_name, likelihood)
// overloads are NOT re-declared here: `likelihood` is a plain
// `const std::vector<double>&`, already handled by gum_typemaps.i, so the
// native overloads %include'd from KTBNInference.h wrap directly. SWIG
// disambiguates a Python sequence[float] (matches the vector<double>
// typecheck) from a bare int/str (falls through to the PyObject* overload
// below) by typecheck precedence, same as for KTBN::fillCPT's distribution
// argument.
// ---------------------------------------------------------------------------
%extend gum::KTBNInference<double> {
  void addIntervention(std::string base, int slice, PyObject* value) {
    self->addIntervention(base, slice, PyAgrumHelper::ktbnModalityFromPyObject(value));
  }
  void addIntervention(std::string node_name, PyObject* value) {
    self->addIntervention(node_name, PyAgrumHelper::ktbnModalityFromPyObject(value));
  }
  void addIntervention(PyObject* interventions) {
    std::vector<std::pair<gum::KTBNInference<double>::NodeKey, gum::KTBNModality>> v;
    PyAgrumHelper::populateKTBNNodeModalityVectorFromPyDict(v, interventions);
    self->addIntervention(v);
  }

  void addObservation(std::string base, int slice, PyObject* value) {
    self->addObservation(base, slice, PyAgrumHelper::ktbnModalityFromPyObject(value));
  }
  void addObservation(std::string node_name, PyObject* value) {
    self->addObservation(node_name, PyAgrumHelper::ktbnModalityFromPyObject(value));
  }
  void addObservations(PyObject* observations) {
    std::vector<std::pair<gum::KTBNInference<double>::NodeKey, gum::KTBNModality>> v;
    PyAgrumHelper::populateKTBNNodeModalityVectorFromPyDict(v, observations);
    self->addObservations(v);
  }

  // Replaces the native posteriors(base) (%ignore'd pre-template in
  // ktbnInference.i): returns a plain Python list of (copied) Tensor
  // objects instead of the raw std::vector<Tensor<double>>&, whose nested
  // template return type passForPyType.py cannot render as a valid Python
  // type hint. SWIGTYPE_p_gum__TensorT_double_t is already defined earlier
  // in this same generated file (KTBN's own cpt()/posterior() accessors use
  // gum::Tensor<double> throughout), so no extra type lookup is needed.
  //
  // Named _posteriorsAsList, NOT posteriors: combining %ignore and %extend
  // for the exact same (qualified, instantiated) name suppresses BOTH --
  // confirmed empirically, contrary to swigsrc/causal.i's %ignore-only
  // precedent (which has no %extend replacement to collide with). The
  // public `posteriors` name is reclaimed just below via %pythoncode,
  // mirroring markovRandomField.i's %rename(_saveGUM)+%pythoncode saveGUM
  // idiom (there via %rename; here via %ignore, since there's no overload
  // ambiguity to preserve on the C++ side, only the Python-visible name).
  PyAgrumListOfTensor* _posteriorsAsList(std::string base) {
    const std::vector<gum::Tensor<double>>& v = self->posteriors(base);
    PyObject* list = PyList_New(v.size());
    for (std::size_t i = 0; i < v.size(); ++i) {
      gum::Tensor<double>* copy = new gum::Tensor<double>(v[i]);
      PyList_SET_ITEM(list, i, SWIG_NewPointerObj(SWIG_as_voidptr(copy), SWIGTYPE_p_gum__TensorT_double_t, SWIG_POINTER_OWN));
    }
    return (PyAgrumListOfTensor*)list;
  }

  %pythoncode {
def posteriors(self, base: str) -> list:
    """
    The whole marginal time-series of a targeted base variable.

    Parameters
    ----------
    base : str
      the base name of a temporal process or an atemporal variable

    Returns
    -------
    list[pyagrum.Tensor]
      one Tensor per slice (a single-element list for an atemporal base)
    """
    return self._posteriorsAsList(base)
  }
}

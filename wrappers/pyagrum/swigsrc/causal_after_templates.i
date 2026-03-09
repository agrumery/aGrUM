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
// instantiations in aGrUM_wrap_*.i files. SWIG requires %extend on a template
// specialisation (e.g. gum::CausalModel<double>) to come after the matching
// %template directive; %rename/%feature directives that must precede the class
// declaration stay in the module-specific swigsrc/*.i files as usual.

// ---------------------------------------------------------------------------
// CausalModel<double>: Python-friendly constructor from list[tuple[str,list[str]]]
//
// The C++ LatentDescriptorVector uses NodeId for children, so a plain typemap
// cannot convert string names to ids without access to the BN (first argument).
// This constructor calls addLatentVariable (string overload), which resolves
// names to NodeIds internally.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CausalModel<double>: Python-friendly constructor from list[tuple[str,list[str]]]
//
// The C++ LatentDescriptorVector uses NodeId for children, so a plain typemap
// cannot convert string names to ids without access to the BN (first argument).
// This constructor calls addLatentVariable (string overload), which resolves
// names to NodeIds internally.
//
// %feature("kwargs") is NOT used: it causes SIGSEGV on inline-constructed
// temporaries (e.g. gum.fastBN("...") passed as first arg) because the
// **kwargs dispatch drops the refcount before the C++ call completes.
// Keyword argument support (keepArcs=) is provided by the %pythoncode block below.
// ---------------------------------------------------------------------------
%extend gum::CausalModel<double> {
  CausalModel(const gum::BayesNet<double>& bn, PyObject* latents, bool assumeNonSpurious=false) {
    auto* cm = new gum::CausalModel<double>(bn);
    if (!PySequence_Check(latents)) {
      delete cm;
      PyErr_SetString(PyExc_TypeError, "CausalModel: latents must be a sequence of (name, children) pairs");
      return nullptr;
    }
    Py_ssize_t n = PySequence_Size(latents);
    for (Py_ssize_t i = 0; i < n; ++i) {
      PyObject* item = PySequence_GetItem(latents, i);
      if (!PySequence_Check(item) || PySequence_Size(item) != 2) {
        Py_DecRef(item);
        delete cm;
        PyErr_SetString(PyExc_TypeError, "CausalModel: each latent descriptor must be a (name, children) pair");
        return nullptr;
      }
      PyObject* pyname     = PySequence_GetItem(item, 0);
      PyObject* pychildren = PySequence_GetItem(item, 1);
      Py_DecRef(item);
      std::string              name = PyAgrumHelper::stringFromPyObject(pyname);
      Py_DecRef(pyname);
      std::vector<std::string> childNames;
      Py_ssize_t               nc = PySequence_Size(pychildren);
      for (Py_ssize_t j = 0; j < nc; ++j) {
        PyObject* child = PySequence_GetItem(pychildren, j);
        childNames.push_back(PyAgrumHelper::stringFromPyObject(child));
        Py_DecRef(child);
      }
      Py_DecRef(pychildren);
      cm->addLatentVariable(name, childNames, assumeNonSpurious);
    }
    return cm;
  }
}

// Python-level __init__ replacement for CausalModel, exposing 'keepArcs' as the
// keyword argument for assumeNonSpurious. Must appear after the class definition
// (i.e. after %template and %extend above), hence its placement in this file.
%pythoncode %{
_CausalModel_swig_init = CausalModel.__init__

def _CausalModel_init(self, bn, latents=None, keepArcs=False):
    if latents is None:
        _CausalModel_swig_init(self, bn)
    else:
        _CausalModel_swig_init(self, bn, latents, keepArcs)

CausalModel.__init__ = _CausalModel_init
del _CausalModel_init

def _enumerateBackdoorSets_wrap(dag, X, Y, *, excluded_nodes=None, max_cardinality=0,
                                              only_minimal=True, stopAtFirst=False):
    return DoorCriteria._enumerateBackdoorSets(dag, X, Y,
                                               excluded_nodes if excluded_nodes is not None else set(),
                                               max_cardinality, only_minimal, stopAtFirst)

def _enumerateFrontdoorSets_wrap(dag, X, Y, *, excluded_nodes=None, max_cardinality=0,
                                               only_minimal=True, stopAtFirst=False):
    return DoorCriteria._enumerateFrontdoorSets(dag, X, Y,
                                                excluded_nodes if excluded_nodes is not None else set(),
                                                max_cardinality, only_minimal, stopAtFirst)

DoorCriteria.enumerateBackdoorSets  = staticmethod(_enumerateBackdoorSets_wrap)
DoorCriteria.enumerateFrontdoorSets = staticmethod(_enumerateFrontdoorSets_wrap)
del _enumerateBackdoorSets_wrap, _enumerateFrontdoorSets_wrap

DoorCriteria.enumerateBackdoorSets.__doc__ = """
Enumerate valid backdoor adjustment sets for the causal effect of X on Y.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the treatment variable.
Y : int
    NodeId of the outcome variable.
excluded_nodes : set of int, optional
    Nodes that cannot appear in any adjustment set. Default is empty.
max_cardinality : int, optional
    Maximum size of returned sets. 0 means no limit. Default is 0.
only_minimal : bool, optional
    If True, return only minimal adjustment sets (no redundant variables).
    Default is True.
stopAtFirst : bool, optional
    If True, stop after finding the first valid set. Default is False.

Returns
-------
list of set of int
    All valid backdoor adjustment sets (as NodeId sets).
"""

DoorCriteria.enumerateFrontdoorSets.__doc__ = """
Enumerate valid frontdoor adjustment sets for the causal effect of X on Y.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the treatment variable.
Y : int
    NodeId of the outcome variable.
excluded_nodes : set of int, optional
    Nodes that cannot appear in any adjustment set. Default is empty.
max_cardinality : int, optional
    Maximum size of returned sets. 0 means no limit. Default is 0.
only_minimal : bool, optional
    If True, return only minimal adjustment sets. Default is True.
stopAtFirst : bool, optional
    If True, stop after finding the first valid set. Default is False.

Returns
-------
list of set of int
    All valid frontdoor adjustment sets (as NodeId sets).
"""
%}

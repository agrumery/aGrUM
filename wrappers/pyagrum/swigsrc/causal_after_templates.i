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
// DoorCriteria::enumerateBackdoorSets / enumerateFrontdoorSets
//
// These C++ static methods have multiple overloads (full signature + shorthand)
// and the full overload has four default parameters. SWIG expands those defaults
// into separate Python overloads, which prevents %feature("shadow") from working
// (SWIG silently disables shadow on overloaded methods).
//
// Solution: inject the keyword-only Python wrappers via %extend %pythoncode.
// Methods defined in a %pythoncode block inside %extend appear after the C++
// binding methods in the generated class body and therefore shadow them.
// The C-level dispatch function _pyagrum.DoorCriteria_enumerateBackdoorSets is
// always available because the C++ functions are not %ignore'd; we call it with
// all arguments explicitly, so the 7-argument overload is always selected.
// ---------------------------------------------------------------------------
%extend gum::DoorCriteria {
%pythoncode %{
    @staticmethod
    def enumerateBackdoorSets(dag, X, Y, *, excluded_nodes=None, max_cardinality=0,
                                            only_minimal=True, stopAtFirst=False):
        """
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
        return _pyagrum.DoorCriteria_enumerateBackdoorSets(
            dag, X, Y,
            excluded_nodes if excluded_nodes is not None else set(),
            max_cardinality, only_minimal, stopAtFirst)

    @staticmethod
    def enumerateFrontdoorSets(dag, X, Y, *, excluded_nodes=None, max_cardinality=0,
                                             only_minimal=True, stopAtFirst=False):
        """
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
        return _pyagrum.DoorCriteria_enumerateFrontdoorSets(
            dag, X, Y,
            excluded_nodes if excluded_nodes is not None else set(),
            max_cardinality, only_minimal, stopAtFirst)
%}
}

// ---------------------------------------------------------------------------
// CausalImpact<double>::toDict() — convert the identified AST to a Python dict
//
// The traversal helper PyAgrumHelper::PyDictFromASTtree is defined in
// extensions/helpers.h, included at the top of pyagrum.i, so it is always
// available to wrapper functions generated here.
// Placed here (after_templates) because %extend on a template specialisation
// must come after the matching %template directive.
// ---------------------------------------------------------------------------
%extend gum::CausalImpact<double> {
  PyAgrumDict* toDict() const {
    if (!$self->isIdentified()) Py_RETURN_NONE;
    return PyAgrumHelper::PyDictFromASTtree($self->root());
  }
}

// ---------------------------------------------------------------------------
// CausalModel<double>: Python-friendly constructor from list[tuple[str,list[str]]]
//
// The C++ LatentDescriptorVector uses NodeId for children, so a plain typemap
// cannot convert string names to ids without access to the BN (first argument).
// This constructor calls addLatentVariable (string overload), which resolves
// names to NodeIds internally.
//
// latents=None (Py_None) is accepted and treated as an empty list, so that
// CausalModel(bn, None) is equivalent to CausalModel(bn).
//
// %feature("kwargs") is NOT used: it causes SIGSEGV on inline-constructed
// temporaries (e.g. gum.fastBN("...") passed as first arg) because the
// **kwargs dispatch drops the refcount before the C++ call completes.
// ---------------------------------------------------------------------------
%extend gum::CausalModel<double> {
  CausalModel(const gum::BayesNet<double>& bn, PyObject* latents, bool assumeNonSpurious=false) {
    auto* cm = new gum::CausalModel<double>(bn);
    if (latents == Py_None) return cm;
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

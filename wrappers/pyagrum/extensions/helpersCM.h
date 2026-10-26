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

/**
 * @file
 * @brief C++ helper functions for the pyagrum.causal_model wrapper only (not included but
 * imported in the wrapper). Split out of extensions/helpers.h -- unlike that file,
 * this one is included only from cm.i: it references gum::CM/tools/doAST.h types
 * (ASTplus, ASTjointProba, ...), which live in the CM module. Including it from
 * the generic helpers.h (shared by every pyAgrum SWIG module) made every module,
 * including core, require CM's typeinfo to be linkable.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <memory>
#include <Python.h>
#include <string>
#include <vector>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/doAST.h>

#ifndef PYAGRUM_HELPER_CM
#  define PYAGRUM_HELPER_CM

namespace PyAgrumHelper {

  // Convert a CausalImpact AST node to a nested Python dict.
  // Returns a new reference. Each node dict has an "op" key identifying its
  // type, plus type-specific keys (op1/op2 for binary ops, vars/knowing for
  // probability nodes, var/term for sum nodes).
  PyObject* PyDictFromASTtree(const gum::ASTtree< double >& node) {
    PyObject* d = PyDict_New();
    if (!d) return nullptr;

    auto setStr = [&](const char* key, const std::string& val) {
      PyObject* s = PyUnicode_FromString(val.c_str());
      PyDict_SetItemString(d, key, s);
      Py_DECREF(s);
    };
    auto setObj = [&](const char* key, PyObject* obj) {
      PyDict_SetItemString(d, key, obj);
      Py_XDECREF(obj);
    };
    auto makeStrList = [](const gum::Set< std::string >& names) {
      PyObject* list = PyList_New(0);
      for (const auto& n: names) {
        PyObject* s = PyUnicode_FromString(n.c_str());
        PyList_Append(list, s);
        Py_DECREF(s);
      }
      return list;
    };

    if (auto* p = dynamic_cast< const gum::ASTplus< double >* >(&node)) {
      setStr("op", "+");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast< const gum::ASTminus< double >* >(&node)) {
      setStr("op", "-");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast< const gum::ASTmult< double >* >(&node)) {
      setStr("op", "*");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast< const gum::ASTdiv< double >* >(&node)) {
      setStr("op", "/");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast< const gum::ASTposteriorProba< double >* >(&node)) {
      setStr("op", "P");
      setObj("vars", makeStrList(p->vars()));
      setObj("knowing", makeStrList(p->knw()));
    } else if (auto* p = dynamic_cast< const gum::ASTjointProba< double >* >(&node)) {
      setStr("op", "P");
      setObj("vars", makeStrList(p->varNames()));
    } else if (auto* p = dynamic_cast< const gum::ASTsum< double >* >(&node)) {
      setStr("op", "sum");
      setStr("var", p->var());
      setObj("term", PyDictFromASTtree(p->term()));
    }
    return d;
  }

  // Add to cm the latent variables described by latents, a sequence of (name, children) pairs
  // (None means no latent variable). Returns false, with a Python TypeError set, if latents is
  // malformed.
  bool addLatentsFromPyObject(gum::CausalModel< double >& cm,
                              PyObject*                   latents,
                              bool                        assumeNonSpurious) {
    if (latents == Py_None) return true;
    if (!PySequence_Check(latents)) {
      PyErr_SetString(PyExc_TypeError,
                      "CausalModel: latents must be a sequence of (name, children) pairs");
      return false;
    }
    const Py_ssize_t n = PySequence_Size(latents);
    for (Py_ssize_t i = 0; i < n; ++i) {
      PyOwnedRef item(PySequence_GetItem(latents, i));
      if (!PySequence_Check(item.get()) || PySequence_Size(item.get()) != 2) {
        PyErr_SetString(PyExc_TypeError,
                        "CausalModel: each latent descriptor must be a (name, children) pair");
        return false;
      }
      PyOwnedRef pyname(PySequence_GetItem(item.get(), 0));
      PyOwnedRef pychildren(PySequence_GetItem(item.get(), 1));
      if (!PySequence_Check(pychildren.get())) {
        PyErr_SetString(
            PyExc_TypeError,
            "CausalModel: the children of a latent variable must be a sequence of names");
        return false;
      }
      std::vector< std::string > childNames;
      const Py_ssize_t           nc = PySequence_Size(pychildren.get());
      for (Py_ssize_t j = 0; j < nc; ++j) {
        PyOwnedRef child(PySequence_GetItem(pychildren.get(), j));
        childNames.push_back(stringFromPyObject(child.get()));
      }
      cm.addLatentVariable(stringFromPyObject(pyname.get()), childNames, assumeNonSpurious);
    }
    return true;
  }

}   // namespace PyAgrumHelper
#endif   // PYAGRUM_HELPER_CM

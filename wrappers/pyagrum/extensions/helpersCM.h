/***************************************************************************
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief C++ helper functions for the pyagrum.cm wrapper only (not included but
 * imported in the wrapper). Split out of extensions/helpers.h -- unlike that file,
 * this one is included only from cm.i: it references gum::CM/tools/doAST.h types
 * (ASTplus, ASTjointProba, ...), which live in the CM module. Including it from
 * the generic helpers.h (shared by every pyAgrum SWIG module) made every module,
 * including core, require CM's typeinfo to be linkable.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>

#include <agrum/CM/tools/doAST.h>

#ifndef PYAGRUM_HELPER_CM
#  define PYAGRUM_HELPER_CM

namespace PyAgrumHelper {

  // Convert a CausalImpact AST node to a nested Python dict.
  // Returns a new reference. Each node dict has an "op" key identifying its
  // type, plus type-specific keys (op1/op2 for binary ops, vars/knowing for
  // probability nodes, var/term for sum nodes).
  PyObject* PyDictFromASTtree(const gum::ASTtree<double>& node) {
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
    auto makeStrList = [](const gum::Set<std::string>& names) {
      PyObject* list = PyList_New(0);
      for (const auto& n : names) {
        PyObject* s = PyUnicode_FromString(n.c_str());
        PyList_Append(list, s);
        Py_DECREF(s);
      }
      return list;
    };

    if (auto* p = dynamic_cast<const gum::ASTplus<double>*>(&node)) {
      setStr("op", "+");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast<const gum::ASTminus<double>*>(&node)) {
      setStr("op", "-");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast<const gum::ASTmult<double>*>(&node)) {
      setStr("op", "*");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast<const gum::ASTdiv<double>*>(&node)) {
      setStr("op", "/");
      setObj("op1", PyDictFromASTtree(p->op1()));
      setObj("op2", PyDictFromASTtree(p->op2()));
    } else if (auto* p = dynamic_cast<const gum::ASTposteriorProba<double>*>(&node)) {
      setStr("op", "P");
      setObj("vars",    makeStrList(p->vars()));
      setObj("knowing", makeStrList(p->knw()));
    } else if (auto* p = dynamic_cast<const gum::ASTjointProba<double>*>(&node)) {
      setStr("op", "P");
      setObj("vars", makeStrList(p->varNames()));
    } else if (auto* p = dynamic_cast<const gum::ASTsum<double>*>(&node)) {
      setStr("op",  "sum");
      setStr("var", p->var());
      setObj("term", PyDictFromASTtree(p->term()));
    }
    return d;
  }

}   // namespace PyAgrumHelper
#endif   // PYAGRUM_HELPER_CM

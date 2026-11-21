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
 * @brief This file contains c++ helper functions for wrappers (not included but
 * imported in the wrapper)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <cstring>
#include <iterator>
#include <limits>

#include <agrum/base/core/set.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/parts/nodeGraphPart.h>
#include <agrum/base/io/GumBinaryIO.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNet.h>

#include "extensions/pyagrumExceptionHandling.h"

#include <numpy/arrayobject.h>

#ifndef PYAGRUM_HELPER
#  define PYAGRUM_HELPER

namespace PyAgrumHelper {

  /// the format of a GUM file (gum::isBinaryGUMFile): binary is None (or absent) or a bool
  bool isBinaryGUMFile(const std::string& path, PyObject* binary) {
    if (binary == nullptr || binary == Py_None) return gum::isBinaryGUMFile(path);
    if (!PyBool_Check(binary)) GUM_ERROR(gum::InvalidArgument, "binary must be a bool or None")
    return gum::isBinaryGUMFile(path, binary == Py_True);
  }

  // true if stringFromPyObject reads o as a string (otherwise it returns "")
  bool isStringPyObject(PyObject* o) { return PyUnicode_Check(o) || PyBytes_Check(o); }

  std::string stringFromPyObject(PyObject* o) {
    std::string name = "";
    if (PyUnicode_Check(o)) {   // python3 string
      PyObject* asbytes = PyUnicode_AsUTF8String(o);
      name              = PyBytes_AsString(asbytes);
      Py_DECREF(asbytes);
    } else if (PyBytes_Check(o)) {   // other python3 string
      name = PyBytes_AsString(o);
    }
    return name;
  }

  // filling a Set of DiscreteVariable* from a list of string, in the context of a
  // tensor.
  void fillDVSetFromPyObject(const gum::Tensor< double >*              pot,
                             gum::Set< const gum::DiscreteVariable* >& s,
                             PyObject*                                 varnames) {
    gum::Set< std::string > names;
    if (PyList_Check(varnames)) {
      const Py_ssize_t siz = PyList_Size(varnames);
      for (Py_ssize_t i = 0; i < siz; i++) {
        std::string name = stringFromPyObject(PyList_GetItem(varnames, i));

        if (name == "") GUM_ERROR(gum::InvalidArgument, "Argument is not a list of string")

        names << name;
      }
    } else {
      std::string name = stringFromPyObject(varnames);
      if (name == "") {
        GUM_ERROR(gum::InvalidArgument, "Argument is not a list or a string")
      } else {
        names << name;
      }
    }

    for (const auto v: pot->variablesSequence())
      if (names.contains(v->name())) s << v;

    if (s.size() == 0) GUM_ERROR(gum::InvalidArgument, "No relevant dimension in the argument")
  }

  // owns a new reference and releases it at scope exit, including when a GUM_ERROR is thrown
  class PyOwnedRef {
    public:
    explicit PyOwnedRef(PyObject* obj) : obj_(obj) {}

    ~PyOwnedRef() { Py_XDECREF(obj_); }

    PyOwnedRef(const PyOwnedRef&)            = delete;
    PyOwnedRef& operator=(const PyOwnedRef&) = delete;

    PyObject* get() const { return obj_; }

    /// gives the reference back to the caller
    PyObject* release() {
      PyObject* obj = obj_;
      obj_          = nullptr;
      return obj;
    }

    private:
    PyObject* obj_;
  };

  // ---- building Python objects -------------------------------------------------
  // Every function building a Python object returns a new reference, or nullptr with the Python
  // error set (never a partially filled object).
  // The *Stolen functions take the reference to the item(s) they add, which may be nullptr when
  // their creation failed (the Python error is then set). They return false, with the Python error
  // set, on failure; the items are released in any case.

  inline bool appendStolen(PyObject* list, PyObject* item) {
    const int err = (item == nullptr) ? -1 : PyList_Append(list, item);
    Py_XDECREF(item);
    return err == 0;
  }

  inline bool addStolen(PyObject* set, PyObject* item) {
    const int err = (item == nullptr) ? -1 : PySet_Add(set, item);
    Py_XDECREF(item);
    return err == 0;
  }

  inline bool setItemStolen(PyObject* dict, PyObject* key, PyObject* val) {
    const int err = (key == nullptr || val == nullptr) ? -1 : PyDict_SetItem(dict, key, val);
    Py_XDECREF(key);
    Py_XDECREF(val);
    return err == 0;
  }

  inline bool setItemStringStolen(PyObject* dict, const char* key, PyObject* val) {
    const int err = (val == nullptr) ? -1 : PyDict_SetItemString(dict, key, val);
    Py_XDECREF(val);
    return err == 0;
  }

  inline bool setTupleItemStolen(PyObject* tuple, Py_ssize_t i, PyObject* item) {
    if (item == nullptr) return false;
    return PyTuple_SetItem(tuple, i, item) == 0;   // steals item, even on failure
  }

  /// a Python str from a C++ string (nullptr with UnicodeDecodeError if it is not valid UTF-8)
  inline PyObject* PyStr(const std::string& str) {
    return PyUnicode_FromStringAndSize(str.data(), static_cast< Py_ssize_t >(str.size()));
  }

  namespace detail {
    template < typename RANGE, typename CONV >
    PyObject* fillCollection(PyObject*    q,
                             const RANGE& range,
                             CONV&        conv,
                             bool (*add)(PyObject*, PyObject*)) {
      if (q == nullptr) return nullptr;
      for (const auto& x: range) {
        if (!add(q, conv(x))) {
          Py_DECREF(q);
          return nullptr;
        }
      }
      return q;
    }
  }   // namespace detail

  /// list [conv(x) for x in range]; conv returns a new reference or nullptr (Python error set)
  template < typename RANGE, typename CONV >
  PyObject* PyListFrom(const RANGE& range, CONV conv) {
    return detail::fillCollection(PyList_New(0), range, conv, &appendStolen);
  }

  /// set {conv(x) for x in range}; conv returns a new reference or nullptr (Python error set)
  template < typename RANGE, typename CONV >
  PyObject* PySetFrom(const RANGE& range, CONV conv) {
    return detail::fillCollection(PySet_New(nullptr), range, conv, &addStolen);
  }

  /// frozenset {conv(x) for x in range}; conv returns a new reference or nullptr (Python error set)
  template < typename RANGE, typename CONV >
  PyObject* PyFrozenSetFrom(const RANGE& range, CONV conv) {
    return detail::fillCollection(PyFrozenSet_New(nullptr), range, conv, &addStolen);
  }

  /// tuple (conv(x) for x in range); conv returns a new reference or nullptr (Python error set)
  template < typename RANGE, typename CONV >
  PyObject* PyTupleFrom(const RANGE& range, CONV conv) {
    PyObject* q = PyTuple_New(static_cast< Py_ssize_t >(std::size(range)));
    if (q == nullptr) return nullptr;
    Py_ssize_t i = 0;
    for (const auto& x: range) {
      if (!setTupleItemStolen(q, i++, conv(x))) {
        Py_DECREF(q);
        return nullptr;
      }
    }
    return q;
  }

  /// dict {convKey(x): convVal(x) for x in range}; both return a new reference or nullptr (Python
  /// error set)
  template < typename RANGE, typename CONVKEY, typename CONVVAL >
  PyObject* PyDictFrom(const RANGE& range, CONVKEY convKey, CONVVAL convVal) {
    PyObject* q = PyDict_New();
    if (q == nullptr) return nullptr;
    for (const auto& x: range) {
      if (!setItemStolen(q, convKey(x), convVal(x))) {
        Py_DECREF(q);
        return nullptr;
      }
    }
    return q;
  }

  inline PyObject* PyIntFromNodeId(gum::NodeId node) { return PyLong_FromSize_t(node); }

  void fillEdgeSetFromPyObject(gum::EdgeSet& s, PyObject* sofedge) {
    PyOwnedRef iter(PyObject_GetIter(sofedge));
    if (iter.get() != NULL) {
      while (PyObject* next = PyIter_Next(iter.get())) {
        PyOwnedRef item(next);
        if (PyTuple_Check(item.get())) {
          if (PyTuple_Size(item.get()) == 2) {
            s.insert(gum::Edge(PyLong_AsLong(PyTuple_GetItem(item.get(), 0)),
                               PyLong_AsLong(PyTuple_GetItem(item.get(), 1))));
          } else {
            GUM_ERROR(gum::InvalidArgument, "An element in the set is not a tuple of size 2")
          }
        } else {
          GUM_ERROR(gum::InvalidArgument, "An element in the set is not a tuple")
        }
      }
      if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a list nor a set")
    }
  }

  // filling a Instantiation from a dictionnary<string,int> and a Tensor (to find variable
  // and labels) and vice-versa

  PyObject* instantiationToDict(const gum::Instantiation& inst, bool withLabels = true) {
    PyObject* res = PyDict_New();
    if (res == nullptr) return nullptr;
    for (gum::Idx i = 0; i < inst.nbrDim(); i++) {
      PyObject* val = withLabels ? PyStr(inst.variable(i).label(inst.val(i)))
                                 : PyLong_FromSize_t(inst.val(i));
      if (!setItemStolen(res, PyStr(inst.variable(i).name()), val)) {
        Py_DECREF(res);
        return nullptr;
      }
    }
    return res;
  }

  void fillInstantiationFromPyObject(const gum::Tensor< double >* pot,
                                     gum::Instantiation&          inst,
                                     PyObject*                    dict) {
    if (!PyDict_Check(dict)) { GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary") }

    gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
    for (gum::Idx i = 0; i < pot->nbrDim(); i++)
      namesToVars.insert(pot->variable(i).name(), &(pot->variable(i)));


    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    inst.clear();
    while (PyDict_Next(dict, &pos, &key, &value)) {
      std::string name = stringFromPyObject(key);
      if (name == "") { GUM_ERROR(gum::InvalidArgument, "A key is not a string"); }
      if (!namesToVars.exists(name)) {
        // not relevant name. we just skip it
        continue;
      }
      std::string label = stringFromPyObject(value);
      gum::Idx    v;
      if (label == "") {
        if (!(PyLong_Check(value))) {
          GUM_ERROR(gum::InvalidArgument, "A value is neither an int nor a string")
        }
        v = gum::Idx(PyLong_AsLong(value));
      } else {
        v = namesToVars[name]->index(label);
      }

      if (v >= namesToVars[name]->domainSize()) {
        GUM_ERROR(gum::InvalidArgument, "The value " << v << " is not in the domain of " << name);
      }

      inst.add(*(namesToVars[name]));
      inst.chgVal(*namesToVars[name], v);
    }
  }

  // filling a Instantiation from a dictionnary<string,int> and a BayesNet (to find variable and
  // labels)
  void fillInstantiationFromPyObjectAndBN(const gum::BayesNet< double >* map,
                                          gum::Instantiation&            inst,
                                          PyObject*                      dict) {
    if (!PyDict_Check(dict)) { GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary") }

    inst.clear();
    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value)) {
      std::string name = stringFromPyObject(key);
      if (name == "") { GUM_ERROR(gum::InvalidArgument, "A key is not a string"); }

      const auto& variable = map->variable(name);

      std::string label = stringFromPyObject(value);
      gum::Idx    v;
      if (label == "") {
        if (!(PyLong_Check(value))) {
          GUM_ERROR(gum::InvalidArgument, "A value is neither an int nor a string")
        }
        v = gum::Idx(PyLong_AsLong(value));
      } else {
        v = variable.index(label);
      }

      if (v >= variable.domainSize()) {
        GUM_ERROR(gum::InvalidArgument, "The value " << v << " is not in the domain of " << name);
      }

      inst.add(variable);
      inst.chgVal(variable, v);
    }
  }

  gum::NodeId nodeIdFromNameOrIndex(PyObject* n, const gum::VariableNodeMap& map) {
    const std::string name = PyAgrumHelper::stringFromPyObject(n);
    if (name != "") {
      return map.idFromName(name);
    } else if (PyLong_Check(n)) {
      return gum::NodeId(PyLong_AsLong(n));
    } else {
      GUM_ERROR(gum::InvalidArgument, "A value is neither a node name nor an node id");
    }
  }

  std::string nameFromNameOrIndex(PyObject* n, const gum::VariableNodeMap& map) {
    const std::string name = PyAgrumHelper::stringFromPyObject(n);
    if (name != "") {
      return name;
    } else if (PyLong_Check(n)) {
      return map.name(gum::NodeId(PyLong_AsLong(n)));
    } else {
      GUM_ERROR(gum::InvalidArgument, "A value is neither a node name nor an node id");
    }
  }

  void populateStrVectorFromPySequenceOfIntOrString(std::vector< std::string >& names,
                                                    PyObject*                   seq,
                                                    const gum::VariableNodeMap& map) {
    // if seq is just a string
    const std::string name = PyAgrumHelper::stringFromPyObject(seq);
    if (name != "") {
      names.push_back(name);
      return;
    }

    // if seq is just a nodeId
    if (PyLong_Check(seq)) {
      names.push_back(map.name(gum::NodeId(PyLong_AsLong(seq))));
      return;
    }

    // seq really is a sequence
    PyOwnedRef iter(PyObject_GetIter(seq));
    if (iter.get() != NULL) {
      while (PyObject* next = PyIter_Next(iter.get())) {
        PyOwnedRef item(next);
        names.push_back(nameFromNameOrIndex(item.get(), map));
      }
      if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument <seq> is not a list nor a set")
    }
  }

  void populateHashTableStrStrFromPyDict(gum::HashTable< std::string, std::string >& hash,
                                         PyObject*                                   dict) {
    if (!PyDict_Check(dict)) { GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary") }

    PyObject*  key;
    PyObject*  value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict, &pos, &key, &value)) {
      const std::string name = stringFromPyObject(key);
      if (name == "") { GUM_ERROR(gum::InvalidArgument, "A key is not a string"); }

      std::string val = stringFromPyObject(value);
      if (val == "") {
        if (PyLong_Check(value)) {
          val = std::to_string(PyLong_AsLong(value));
        } else {
          GUM_ERROR(gum::InvalidArgument, "A value is neither a string nor an integer");
        }
      }

      hash.insert(name, val);
    }
  }

  PyObject* PyDictFromHashTableStrStr(const gum::HashTable< std::string, std::string >& hash) {
    return PyDictFrom(
        hash,
        [](const auto& kv) { return PyStr(kv.first); },
        [](const auto& kv) { return PyStr(kv.second); });
  }

  void populateNodeSetFromIntOrPySequenceOfInt(gum::NodeSet& nodeset, PyObject* seq) {
    // if seq is just a nodeId
    if (PyLong_Check(seq)) {
      nodeset.insert(gum::NodeId(PyLong_AsLong(seq)));
      return;
    }

    // seq really is a sequence
    PyOwnedRef iter(PyObject_GetIter(seq));
    if (iter.get() != NULL) {
      while (PyObject* next = PyIter_Next(iter.get())) {
        PyOwnedRef item(next);
        if (PyLong_Check(item.get())) {
          nodeset.insert(gum::NodeId(PyLong_AsLong(item.get())));
        } else {
          GUM_ERROR(gum::InvalidArgument, "An elmement in the sequence is not a int nor a long")
        }
      }
      if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument <seq> is not an int, a list nor a set")
    }
  }

  void populateNodeSetFromPySequenceOfIntOrString(gum::NodeSet&               nodeset,
                                                  PyObject*                   seq,
                                                  const gum::VariableNodeMap& map) {
    // if seq is just a string
    const std::string name = PyAgrumHelper::stringFromPyObject(seq);
    if (name != "") {
      nodeset.insert(map.idFromName(name));
      return;
    }

    // if seq is just a nodeId
    if (PyLong_Check(seq)) {
      nodeset.insert(gum::NodeId(PyLong_AsLong(seq)));
      return;
    }

    // seq really is a sequence
    PyOwnedRef iter(PyObject_GetIter(seq));
    if (iter.get() != NULL) {
      while (PyObject* next = PyIter_Next(iter.get())) {
        PyOwnedRef item(next);
        nodeset.insert(nodeIdFromNameOrIndex(item.get(), map));
      }
      if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument <seq> is not a list nor a set")
    }
  }

  // ---- ids <-> names in Python containers (namesFromIds/idsFromNames) ----------------------
  namespace detail {
    /// o itself, or PythonErrorAlreadySet if o is nullptr (the Python error is then set)
    inline PyObject* checkedPyObject(PyObject* o) {
      if (o == nullptr) throw PythonErrorAlreadySet();
      return o;
    }

    [[noreturn]] inline void raisePyTypeError(const std::string& msg) {
      PyErr_SetString(PyExc_TypeError, msg.c_str());
      throw PythonErrorAlreadySet();
    }

    /// the name of the type of o (for the error messages)
    inline std::string pyTypeName(PyObject* o) {
      PyOwnedRef type(PyObject_Type(o));
      PyOwnedRef name(type.get() == nullptr ? nullptr
                                            : PyObject_GetAttrString(type.get(), "__name__"));
      PyErr_Clear();
      return (name.get() == nullptr) ? std::string("?") : stringFromPyObject(name.get());
    }

    /// the text of o (str(o))
    inline std::string pyStr(PyObject* o) {
      PyOwnedRef str(PyObject_Str(o));
      PyErr_Clear();
      return (str.get() == nullptr) ? std::string("?") : stringFromPyObject(str.get());
    }

    /// the name of the node id of g; NotFound if g has no such node. G: a graph class (nameFromId
    /// does not check the id), a graphical model or a causal model (both throw NotFound)
    template < typename G >
    std::string nodeName(const G& g, gum::NodeId id) {
      if constexpr (requires {
                      g.existsNode(id);
                      g.nameFromId(id);
                    }) {
        if (!g.existsNode(id)) GUM_ERROR(gum::NotFound, "namesFromIds: no node with id " << id)
        return g.nameFromId(id);
      } else {
        try {
          if constexpr (requires { g.nameFromId(id); }) return g.nameFromId(id);
          else return g.variable(id).name();
        } catch (gum::NotFound&) {
          GUM_ERROR(gum::NotFound, "namesFromIds: no node with id " << id)
        }
      }
    }

    /// the id of the node named name in g; NotFound if g has no such node
    template < typename G >
    gum::NodeId nodeId(const G& g, const std::string& name) {
      if constexpr (requires { g.idFromName(name).has_value(); }) {   // graph classes: optional
        const auto id = g.idFromName(name);
        if (!id) GUM_ERROR(gum::NotFound, "idsFromNames: no node named '" << name << "'")
        return *id;
      } else {
        try {
          return g.idFromName(name);
        } catch (gum::NotFound&) {
          GUM_ERROR(gum::NotFound, "idsFromNames: no node named '" << name << "'")
        }
      }
    }

    /// the node id given by o (an int, bool excluded); NotFound if negative or too large
    inline gum::NodeId nodeIdFromPyInt(PyObject* o) {
      if (PyBool_Check(o) || !PyIndex_Check(o) || isStringPyObject(o))
        raisePyTypeError("namesFromIds: a node id (int) was expected, got " + pyTypeName(o));
      PyOwnedRef               index(checkedPyObject(PyNumber_Index(o)));
      const unsigned long long val = PyLong_AsUnsignedLongLong(index.get());
      if (PyErr_Occurred()) {   // negative or too large: not a node anyway
        PyErr_Clear();
        GUM_ERROR(gum::NotFound, "namesFromIds: no node with id " << pyStr(index.get()))
      }
      if (val > std::numeric_limits< gum::NodeId >::max())
        GUM_ERROR(gum::NotFound, "namesFromIds: no node with id " << val)
      return static_cast< gum::NodeId >(val);
    }

    /// a copy of x where every node is converted by convertNode (which returns a new reference or
    /// throws), keeping the type of the containers: list, tuple, set, frozenset (a subclass gives
    /// its base type) and dict (keys and values converted), recursively
    template < typename CONVERT >
    PyObject* convertNodesKeepingContainers(PyObject* x, const CONVERT& convertNode) {
      const auto convert = [&convertNode](PyObject* item) {
        return convertNodesKeepingContainers(item, convertNode);
      };
      if (PyDict_Check(x)) {
        PyOwnedRef res(checkedPyObject(PyDict_New()));
        PyObject*  key;
        PyObject*  value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(x, &pos, &key, &value)) {
          PyOwnedRef k(convert(key));
          PyOwnedRef v(convert(value));
          if (PyDict_SetItem(res.get(), k.get(), v.get()) != 0) throw PythonErrorAlreadySet();
        }
        return res.release();
      }
      if (PyList_Check(x) || PyTuple_Check(x) || PyAnySet_Check(x)) {
        PyOwnedRef items(checkedPyObject(PyList_New(0)));
        PyOwnedRef iter(checkedPyObject(PyObject_GetIter(x)));
        while (PyObject* next = PyIter_Next(iter.get())) {
          PyOwnedRef item(next);
          PyOwnedRef converted(convert(item.get()));
          if (PyList_Append(items.get(), converted.get()) != 0) throw PythonErrorAlreadySet();
        }
        if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
        if (PyList_Check(x)) return items.release();
        if (PyTuple_Check(x)) return checkedPyObject(PySequence_Tuple(items.get()));
        if (PyFrozenSet_Check(x)) return checkedPyObject(PyFrozenSet_New(items.get()));
        return checkedPyObject(PySet_New(items.get()));
      }
      return convertNode(x);
    }
  }   // namespace detail

  /// the names of the nodes of g given by ids (a node id, or a container of node ids, see
  /// detail::convertNodesKeepingContainers); G: a graph class, a graphical model or a causal model
  template < typename G >
  PyObject* namesFromIds(const G& g, PyObject* ids) {
    return detail::convertNodesKeepingContainers(ids, [&g](PyObject* o) {
      return detail::checkedPyObject(PyStr(detail::nodeName(g, detail::nodeIdFromPyInt(o))));
    });
  }

  /// the ids of the nodes of g given by names (a node name, or a container of node names, see
  /// detail::convertNodesKeepingContainers); G: a graph class, a graphical model or a causal model
  template < typename G >
  PyObject* idsFromNames(const G& g, PyObject* names) {
    return detail::convertNodesKeepingContainers(names, [&g](PyObject* o) {
      if (!isStringPyObject(o))
        detail::raisePyTypeError("idsFromNames: a node name (str) was expected, got "
                                 + detail::pyTypeName(o));
      return detail::checkedPyObject(PyIntFromNodeId(detail::nodeId(g, stringFromPyObject(o))));
    });
  }

  PyObject* PyListFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    return PyListFrom(nodevect, PyIntFromNodeId);
  }

  PyObject* PyTupleFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    return PyTupleFrom(nodevect, PyIntFromNodeId);
  }

  PyObject* PySetFromNodeSet(const gum::NodeSet& nodeset) {
    return PySetFrom(nodeset, PyIntFromNodeId);
  }

  PyObject* PySetFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    return PySetFrom(nodevect, PyIntFromNodeId);
  }

  PyObject* PySetFromNodeSet(const gum::NodeGraphPart& nodeset) {
    return PySetFrom(nodeset, PyIntFromNodeId);
  }

  inline PyObject* PyTupleFromArc(const gum::Arc& arc) {
    return Py_BuildValue("(nn)",
                         static_cast< Py_ssize_t >(arc.tail()),
                         static_cast< Py_ssize_t >(arc.head()));
  }

  PyObject* PyListFromArcVect(const std::vector< gum::Arc >& arcseq) {
    return PyListFrom(arcseq, PyTupleFromArc);
  }

  PyObject* PySetFromArcSet(const gum::ArcSet& arcset) { return PySetFrom(arcset, PyTupleFromArc); }

  PyObject* PySetFromEdgeSet(const gum::EdgeSet& edgeset) {
    return PySetFrom(edgeset, [](const gum::Edge& edg) {
      return Py_BuildValue("(nn)",
                           static_cast< Py_ssize_t >(edg.first()),
                           static_cast< Py_ssize_t >(edg.second()));
    });
  }

  PyObject* PySetFromStringSet(const gum::Set< std::string >& strset) {
    return PySetFrom(strset, PyStr);
  }

  void populateStringSetFromPySequence(gum::Set< std::string >& strset, PyObject* seq) {
    // if seq is just a string
    const std::string name = stringFromPyObject(seq);
    if (name != "") {
      strset.insert(name);
      return;
    }

    // seq really is a sequence (set or list)
    PyOwnedRef iter(PyObject_GetIter(seq));
    if (iter.get() != NULL) {
      while (PyObject* next = PyIter_Next(iter.get())) {
        PyOwnedRef        item(next);
        const std::string str = stringFromPyObject(item.get());
        if (str == "") GUM_ERROR(gum::InvalidArgument, "An element in the sequence is not a string")
        strset.insert(str);
      }
      if (PyErr_Occurred()) throw PythonErrorAlreadySet();   // the iteration raised
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a string, a list nor a set")
    }
  }

  PyObject* PyDictFromInstantiation(const gum::Instantiation& inst) {
    return PyDictFrom(
        inst.variablesSequence(),
        [](const gum::DiscreteVariable* k) { return PyStr(k->name()); },
        [&inst](const gum::DiscreteVariable* k) { return PyLong_FromSize_t(inst.val(*k)); });
  }

  PyObject* PyDictFromPairMeanVar(std::pair< double, double > mv) {
    return Py_BuildValue("{s:d,s:d}", "mean", mv.first, "variance", mv.second);
  }

  PyObject* PySeqFromSetOfInstantiation(const gum::Set< gum::Instantiation >& soi) {
    return PyListFrom(soi, PyDictFromInstantiation);
  }

  template < typename T >
  PyObject* PyListOfSetsFromSetOfNodeSets(const T& setOfNodeSets) {
    return PyListFrom(setOfNodeSets, [](const gum::NodeSet& ns) { return PySetFromNodeSet(ns); });
  }

  // ---- numpy interoperability helpers ----------------------------------------

  // Zero-copy view: creates a numpy array pointing to the Tensor's buffer.
  // self_pyobj becomes the numpy base object (keeps the Tensor alive).
  PyObject* tensorAsNpArrayRaw(gum::Tensor< double >* self, PyObject* self_pyobj) {
    double* raw = self->content()->data();
    if (raw == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "[pyAgrum] Tensor has no contiguous data buffer");
      return nullptr;
    }
    npy_intp  dims[1] = {static_cast< npy_intp >(self->domainSize())};
    PyObject* arr     = PyArray_New(&PyArray_Type,
                                    1,
                                    dims,
                                    NPY_DOUBLE,
                                    nullptr,
                                    raw,
                                    0,
                                    NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_WRITEABLE,
                                    nullptr);
    if (arr == nullptr) return nullptr;
    Py_INCREF(self_pyobj);   // SetBaseObject steals the reference
    if (PyArray_SetBaseObject(reinterpret_cast< PyArrayObject* >(arr), self_pyobj) < 0) {
      Py_DECREF(arr);
      return nullptr;
    }
    return arr;
  }

  // Copy: allocates a new numpy array and memcpys the Tensor's data into it.
  PyObject* tensorToArray(const gum::Tensor< double >* self) {
    npy_intp  dims[1] = {static_cast< npy_intp >(self->domainSize())};
    PyObject* arr     = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
    if (arr == nullptr) return nullptr;
    const double* src = self->content()->data();
    if (src != nullptr && dims[0] > 0)
      std::memcpy(PyArray_DATA(reinterpret_cast< PyArrayObject* >(arr)),
                  src,
                  dims[0] * sizeof(double));
    return arr;
  }

  // Fill: single memcpy from a C-contiguous float64 numpy array into the
  // Tensor's buffer. Returns 0 on success, -1 on error (exception set).
  int tensorFillWithNpArray(gum::Tensor< double >* self, PyObject* arr) {
    if (!PyArray_Check(arr)) {
      PyErr_SetString(PyExc_TypeError, "[pyAgrum] argument is not a numpy array");
      return -1;
    }
    PyArrayObject* nparr = reinterpret_cast< PyArrayObject* >(arr);
    if (!PyArray_IS_C_CONTIGUOUS(nparr)) {
      PyErr_SetString(PyExc_ValueError, "[pyAgrum] numpy array must be C-contiguous");
      return -1;
    }
    if (PyArray_TYPE(nparr) != NPY_DOUBLE) {
      PyErr_SetString(PyExc_TypeError, "[pyAgrum] numpy array must have dtype float64");
      return -1;
    }
    npy_intp  np_size = PyArray_SIZE(nparr);
    gum::Size t_size  = self->domainSize();
    if (static_cast< gum::Size >(np_size) != t_size) {
      PyErr_Format(PyExc_ValueError,
                   "[pyAgrum] numpy array size %zd does not match "
                   "Tensor domainSize %zu",
                   static_cast< Py_ssize_t >(np_size),
                   static_cast< std::size_t >(t_size));
      return -1;
    }
    double* dst = self->content()->data();
    if (dst == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "[pyAgrum] Tensor has no contiguous data buffer");
      return -1;
    }
    std::memcpy(dst, PyArray_DATA(nparr), t_size * sizeof(double));
    return 0;
  }

  // PyDictFromASTtree (CausalImpact AST -> nested Python dict) moved to
  // extensions/helpersCM.h: it references gum::ASTplus/ASTjointProba/... which
  // live in CM, and this header is included by every pyAgrum SWIG module
  // (core included) -- see helpersCM.h for why that made core require CM to
  // be linkable.

  PyObject* PyDictFromNodePropertyNodeId(const gum::NodeProperty< gum::NodeId >& prop) {
    return PyDictFrom(
        prop,
        [](const auto& kv) { return PyLong_FromSize_t(kv.first); },
        [](const auto& kv) { return PyLong_FromSize_t(kv.second); });
  }

  // nested std::vector< double > (any depth) -> nested Python list of floats.
  // Returns nullptr (Python error set) on failure.
  inline PyObject* PyListFromNestedVector(double val) { return PyFloat_FromDouble(val); }

  template < typename T >
  PyObject* PyListFromNestedVector(const std::vector< T >& vec) {
    return PyListFrom(vec, [](const T& elt) { return PyListFromNestedVector(elt); });
  }

  // nested Python sequences of numbers -> nested std::vector< double > (any depth).
  // Returns false (Python TypeError set) on failure.
  inline bool NestedVectorFromPyObject(PyObject* obj, double& val) {
    val = PyFloat_AsDouble(obj);
    return !(val == -1.0 && PyErr_Occurred());
  }

  template < typename T >
  bool NestedVectorFromPyObject(PyObject* obj, std::vector< T >& vec) {
    if (!PySequence_Check(obj) || PyUnicode_Check(obj)) {
      PyErr_SetString(PyExc_TypeError, "expected a (nested) sequence of floats");
      return false;
    }
    const Py_ssize_t n = PySequence_Size(obj);
    if (n < 0) return false;
    vec.assign(static_cast< std::size_t >(n), T{});
    for (Py_ssize_t i = 0; i < n; ++i) {
      PyObject* item = PySequence_GetItem(obj, i);
      if (item == nullptr) return false;
      const bool ok = NestedVectorFromPyObject(item, vec[static_cast< std::size_t >(i)]);
      Py_DecRef(item);
      if (!ok) return false;
    }
    return true;
  }

  // NodeProperty< nested std::vector< double > > -> Python dict {node id: nested list of floats}.
  // Returns nullptr (Python error set) on failure.
  template < typename T >
  PyObject* PyDictFromNodePropertyNestedVector(const gum::NodeProperty< T >& prop) {
    return PyDictFrom(
        prop,
        [](const auto& kv) { return PyLong_FromSize_t(kv.first); },
        [](const auto& kv) { return PyListFromNestedVector(kv.second); });
  }

}   // namespace PyAgrumHelper
#endif   // PYAGRUM_HELPER

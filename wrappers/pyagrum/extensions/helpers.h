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
 * @brief This file contains c++ helper functions for wrappers (not included but
 * imported in the wrapper)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/base/core/set.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/parts/nodeGraphPart.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNet.h>

#ifndef PYAGRUM_HELPER
#  define PYAGRUM_HELPER

namespace PyAgrumHelper {

  std::string stringFromPyObject(PyObject* o) {
    std::string name = "";
    if (PyUnicode_Check(o)) {   // python3 string
      PyObject* asbytes = PyUnicode_AsUTF8String(o);
      name              = PyBytes_AsString(asbytes);
      Py_DECREF(asbytes);
    } else if (PyString_Check(o)) {   // python2 string
      name = PyString_AsString(o);
    } else if (PyBytes_Check(o)) {    // other python3 string
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
      auto siz = PyList_Size(varnames);
      for (int i = 0; i < siz; i++) {
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

  // filling a vector of DiscreteVariable* from a list of string, in the context of
  // a tensor.
  void fillDVVectorFromPyObject(const gum::Tensor< double >*                 pot,
                                std::vector< const gum::DiscreteVariable* >& s,
                                PyObject*                                    varnames) {
    if (PyList_Check(varnames)) {
      gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
      for (gum::Idx i = 0; i < pot->nbrDim(); i++)
        namesToVars.insert(pot->variable(i).name(), &(pot->variable(i)));

      auto siz = PyList_Size(varnames);
      s.clear();

      for (int i = 0; i < siz; i++) {
        std::string name = stringFromPyObject(PyList_GetItem(varnames, i));
        if (name == "") { GUM_ERROR(gum::InvalidArgument, "Argument is not a list of string") }
        if (!namesToVars.exists(name)) {
          GUM_ERROR(gum::InvalidArgument, "Argument is not a name of a variable in this tensor");
        }
        s.push_back(namesToVars[name]);
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a list")
    }
  }

  // filling a DiscreteVariable* from a string, in the context of a tensor.
  void fillDVFromPyObject(const gum::Tensor< double >*  pot,
                          const gum::DiscreteVariable*& pvar,
                          PyObject*                     varname) {
    const std::string name = stringFromPyObject(varname);
    if (name == "") { GUM_ERROR(gum::InvalidArgument, "Argument is not a string") }

    bool isOK = false;
    for (gum::Idx i = 0; i < pot->nbrDim(); i++) {
      if (pot->variable(i).name() == name) {
        pvar = &(pot->variable(i));
        isOK = true;
        break;
      }
    }
    if (!isOK) {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a name of a variable in this tensor");
    }
  }

  void fillEdgeSetFromPyObject(gum::EdgeSet& s, PyObject* sofedge) {
    PyObject* iter = PyObject_GetIter(sofedge);
    if (iter != NULL) {
      PyObject* item;
      while ((item = PyIter_Next(iter))) {
        if (PyTuple_Check(item)) {
          if (PyTuple_Size(item) == 2) {
            s.insert(gum::Edge(PyLong_AsLong(PyTuple_GetItem(item, 0)),
                               PyLong_AsLong(PyTuple_GetItem(item, 1))));
          } else {
            GUM_ERROR(gum::InvalidArgument, "An element in the set is not a tuple of size 2")
          }
        } else {
          GUM_ERROR(gum::InvalidArgument, "An element in the set is not a tuple")
        }
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a list nor a set")
    }
  }

  // filling a Instantiation from a dictionnary<string,int> and a Tensor (to find variable
  // and labels) and vice-versa

  PyObject* instantiationToDict(const gum::Instantiation& inst, bool withLabels = true) {
    auto res = PyDict_New();
    for (gum::Idx i = 0; i < inst.nbrDim(); i++) {
      PyObject* key = PyString_FromString(inst.variable(i).name().c_str());
      PyObject* val;
      if (withLabels) {
        val = PyString_FromString(inst.variable(i).label(inst.val(i)).c_str());
      } else {
        val = PyLong_FromUnsignedLong(inst.val(i));
      }
      PyDict_SetItem(res, key, val);
      Py_DecRef(key);
      Py_DecRef(val);
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
        if (!(PyInt_Check(value))) {
          GUM_ERROR(gum::InvalidArgument, "A value is neither an int nor a string")
        }
        v = gum::Idx(PyInt_AsLong(value));
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
        if (!(PyInt_Check(value))) {
          GUM_ERROR(gum::InvalidArgument, "A value is neither an int nor a string")
        }
        v = gum::Idx(PyInt_AsLong(value));
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
    } else if (PyInt_Check(n)) {
      return gum::NodeId(PyInt_AsLong(n));
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
    } else if (PyInt_Check(n)) {
      return map.name(gum::NodeId(PyInt_AsLong(n)));
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
    if (PyInt_Check(seq) || PyLong_Check(seq)) {
      names.push_back(map.name(gum::NodeId(PyLong_AsLong(seq))));
      return;
    }

    // seq really is a sequence
    PyObject* iter = PyObject_GetIter(seq);
    if (iter != NULL) {
      PyObject* item;
      while ((item = PyIter_Next(iter))) {
        names.push_back(nameFromNameOrIndex(item, map));
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument <seq> is not a list nor a set")
    }
  }

  void populateNodeSetFromIntOrPySequenceOfInt(gum::NodeSet& nodeset, PyObject* seq) {
    // if seq is just a nodeId
    if (PyInt_Check(seq) || PyLong_Check(seq)) {
      nodeset.insert(gum::NodeId(PyLong_AsLong(seq)));
      return;
    }

    // seq really is a sequence
    PyObject* iter = PyObject_GetIter(seq);
    if (iter != NULL) {
      PyObject* item;
      while ((item = PyIter_Next(iter))) {
        if (PyInt_Check(item)) {
          nodeset.insert(gum::NodeId(PyInt_AsLong(item)));
        } else if (PyLong_Check(item)) {
          nodeset.insert(gum::NodeId(PyLong_AsLong(item)));
        } else {
          GUM_ERROR(gum::InvalidArgument, "An elmement in the sequence is not a int nor a long")
        }
      }
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
    if (PyInt_Check(seq) || PyLong_Check(seq)) {
      nodeset.insert(gum::NodeId(PyLong_AsLong(seq)));
      return;
    }

    // seq really is a sequence
    PyObject* iter = PyObject_GetIter(seq);
    if (iter != NULL) {
      PyObject* item;
      while ((item = PyIter_Next(iter))) {
        nodeset.insert(nodeIdFromNameOrIndex(item, map));
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument <seq> is not a list nor a set")
    }
  }

  PyObject* PyListFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    PyObject* q = PyList_New(0);

    PyObject* pyval;
    for (auto node: nodevect) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }

    return q;
  }

  PyObject* PyListFromNodeSet(const gum::NodeSet& nodeset) {
    PyObject* q = PyList_New(0);

    PyObject* pyval;
    for (auto node: nodeset) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }

    return q;
  }

  PyObject* PyTupleFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    PyObject* q = PyTuple_New(nodevect.size());

    int       i = 0;
    PyObject* pyval;
    for (auto node: nodevect) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PyTuple_SET_ITEM(q, i, pyval);
      Py_DecRef(pyval);
      i++;
    }

    return q;
  }

  PyObject* PyTupleFromNodeSet(const gum::NodeSet& nodeset) {
    PyObject* q = PyTuple_New(nodeset.size());

    int       i = 0;
    PyObject* pyval;
    for (auto node: nodeset) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PyTuple_SET_ITEM(q, i, pyval);
      Py_DecRef(pyval);
      i++;
    }

    return q;
  }

  PyObject* PySetFromNodeSet(const gum::NodeSet& nodeset) {
    PyObject* q = PySet_New(0);

    PyObject* pyval;
    for (auto node: nodeset) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PySet_Add(q, pyval);
      Py_DecRef(pyval);
    }

    return q;
  }

  PyObject* PySetFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    PyObject* q = PySet_New(0);

    PyObject* pyval;
    for (auto node: nodevect) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PySet_Add(q, pyval);
      Py_DecRef(pyval);
    }

    return q;
  }

  PyObject* PySetFromNodeSet(const gum::NodeGraphPart& nodeset) {
    PyObject* q = PySet_New(0);

    PyObject* pyval;
    for (auto node: nodeset) {
      pyval = PyLong_FromUnsignedLong((unsigned long)node);
      PySet_Add(q, pyval);
      Py_DecRef(pyval);
    }

    return q;
  }

  PyObject* PyListFromArcVect(const std::vector< gum::Arc >& arcseq) {
    PyObject* q = PyList_New(0);

    PyObject* pyval;
    for (const auto& arc: arcseq) {
      pyval = Py_BuildValue("(i,i)", arc.tail(), arc.head());
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  std::vector< gum::Arc > populateArcVectFromPyList(PyObject* arcseq) {
    std::vector< gum::Arc > res;
    if (PyList_Check(arcseq)) {
      auto siz = PyList_Size(arcseq);
      for (int i = 0; i < siz; i++) {
        PyObject* item = PyList_GetItem(arcseq, i);
        if (PyTuple_Check(item)) {
          if (PyTuple_Size(item) == 2) {
            res.push_back(gum::Arc(PyLong_AsLong(PyTuple_GetItem(item, 0)),
                                   PyLong_AsLong(PyTuple_GetItem(item, 1))));
          } else {
            GUM_ERROR(gum::InvalidArgument, "An element in the list is not a tuple of size 2")
          }
        } else {
          GUM_ERROR(gum::InvalidArgument, "An element in the list is not a tuple")
        }
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a list")
    }
    return res;
  }

  PyObject* PyListFromSequenceOfInt(const gum::Sequence< int >& seq) {
    PyObject* q = PyList_New(0);
    PyObject* pyval;
    for (const auto& val: seq) {
      pyval = PyLong_FromLong(val);
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  PyObject* PyListFromSequenceOfDouble(const gum::Sequence< double >& seq) {
    PyObject* q = PyList_New(0);
    PyObject* pyval;
    for (const auto& val: seq) {
      pyval = PyFloat_FromDouble(val);
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  PyObject* PySetFromArcSet(const gum::ArcSet& arcset) {
    PyObject* q = PySet_New(0);

    PyObject* pyval;
    for (const auto& arc: arcset) {
      pyval = Py_BuildValue("(i,i)", arc.tail(), arc.head());
      PySet_Add(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  PyObject* PySetFromEdgeSet(const gum::EdgeSet& edgeset) {
    PyObject* q = PySet_New(0);
    PyObject* pyval;

    for (const auto& edg: edgeset) {
      pyval = Py_BuildValue("(i,i)", edg.first(), edg.second());
      PySet_Add(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  PyObject* PyDictFromInstantiation(const gum::Instantiation& inst) {
    PyObject* q = PyDict_New();
    PyObject* pyval;

    for (const auto& k: inst.variablesSequence()) {
      pyval = PyLong_FromUnsignedLong((unsigned long)inst.val(*k));
      PyDict_SetItemString(q, k->name().c_str(), pyval);
      Py_DecRef(pyval);
    }
    return q;
  }

  PyObject* PyDictFromPairMeanVar(std::pair< double, double > mv) {
    PyObject* q = PyDict_New();
    PyObject* pyval;

    pyval = PyFloat_FromDouble(mv.first);
    PyDict_SetItemString(q, "mean", PyFloat_FromDouble(mv.first));
    Py_DecRef(pyval);

    pyval = PyFloat_FromDouble(mv.second);
    PyDict_SetItemString(q, "variance", PyFloat_FromDouble(mv.second));
    Py_DecRef(pyval);

    return q;
  }

  PyObject* PySeqFromSetOfInstantiation(const gum::Set< gum::Instantiation >& soi) {
    PyObject* q = PyList_New(0);
    PyObject* pyval;
    for (const auto& inst: soi) {
      pyval = PyDictFromInstantiation(inst);
      PyList_Append(q, pyval);
      Py_DecRef(pyval);
    }
    return q;
  }
}   // namespace PyAgrumHelper
#endif   // PYAGRUM_HELPER

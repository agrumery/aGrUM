/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
#include <agrum/BN/IBayesNet.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/multidim/potential.h>

namespace PyAgrumHelper {

  std::string stringFromPyObject(PyObject* o) {
    std::string name = "";
    if (PyUnicode_Check(o)) {   // python3 string
      PyObject* asbytes = PyUnicode_AsASCIIString(o);
      name = PyBytes_AsString(asbytes);
      Py_DECREF(asbytes);
    } else if (PyString_Check(o)) {   // python2 string
      name = PyString_AsString(o);
    } else if (PyBytes_Check(o)) {   // other python3 string
      name = PyBytes_AsString(o);
    }
    return name;
  }

  // filling a Set of DiscreteVariable* from a list of string, in the context of a
  // potential.
  void fillDVSetFromPyObject(const gum::Potential< double >*           pot,
                             gum::Set< const gum::DiscreteVariable* >& s,
                             PyObject* varnames) {

    gum::Set< std::string > names;
    if (PyList_Check(varnames)) {
      auto                    siz = PyList_Size(varnames);
      for (int i = 0; i < siz; i++) {
        std::string name = stringFromPyObject(PyList_GetItem(varnames, i));

        if (name == "")
          GUM_ERROR(gum::InvalidArgument, "Argument is not a list of string");

        names << name;
      }
    } else {
      std::string name=stringFromPyObject(varnames);
      if (name=="") {
        GUM_ERROR(gum::InvalidArgument, "Argument is not a list or a string");
      } else {
        names<<name;
      }
    }

    for (const auto v : pot->variablesSequence())
      if (names.contains(v->name())) s << v;

    if (s.size() == 0)
      GUM_ERROR(gum::InvalidArgument, "No relevant dimension in the argument");
  }

  // filling a vector of DiscreteVariable* from a list of string, in the context of
  // a potential.
  void fillDVVectorFromPyObject(const gum::Potential< double >*              pot,
                                std::vector< const gum::DiscreteVariable* >& s,
                                PyObject* varnames) {
    if (PyList_Check(varnames)) {
      gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
      for (gum::Idx i = 0; i < pot->nbrDim(); i++)
        namesToVars.insert(pot->variable(i).name(), &(pot->variable(i)));

      auto siz = PyList_Size(varnames);
      s.clear();

      for (int i = 0; i < siz; i++) {
        std::string name = stringFromPyObject(PyList_GetItem(varnames, i));
        if (name == "") {
          GUM_ERROR(gum::InvalidArgument, "Argument is not a list of string");
        }
        if (!namesToVars.exists(name)) {
          GUM_ERROR(gum::InvalidArgument,
                    "Argument is a not a name of a variable in this potential");
        }
        s.push_back(namesToVars[name]);
      }
    } else {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a list");
    }
  }

  // filling a DiscreteVariable* from a string, in the context of a potential.
  void fillDVFromPyObject(const gum::Potential< double >* pot,
                          const gum::DiscreteVariable*&   pvar,
                          PyObject*                       varname) {
    const std::string name = stringFromPyObject(varname);
    if (name == "") {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a string");
    }

    bool isOK = false;
    for (gum::Idx i = 0; i < pot->nbrDim(); i++) {
      if (pot->variable(i).name() == name) {
        pvar = &(pot->variable(i));
        isOK = true;
        break;
      }
    }
    if (!isOK) {
      GUM_ERROR(gum::InvalidArgument,
                "Argument is a not a name of a variable in this potential");
    }
  }

  // filling a Instantiation from a dictionnary<string,int>
  void fillInstantiationFromPyObject(const gum::Potential< double >* pot,
                                     gum::Instantiation&             inst,
                                     PyObject*                       dict) {
    if (!PyDict_Check(dict)) {
      GUM_ERROR(gum::InvalidArgument, "Argument is not a dictionary");
    }

    gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
    for (gum::Idx i = 0; i < pot->nbrDim(); i++)
      namesToVars.insert(pot->variable(i).name(), &(pot->variable(i)));


    PyObject * key, *value;
    Py_ssize_t pos = 0;
    inst.clear();
    while (PyDict_Next(dict, &pos, &key, &value)) {
      std::string name = stringFromPyObject(key);
      if (name == "") { GUM_ERROR(gum::InvalidArgument, "A key is not a string"); }
      if (!namesToVars.exists(name)) {
        GUM_ERROR(gum::InvalidArgument,
                  "The key "
                    << name << " is a not a name of a variable in this potential");
      } 
      if (!(PyInt_Check(value))) {
        GUM_ERROR(gum::InvalidArgument, "A value is not an int");
      }
      gum::Idx v = gum::Idx(PyInt_AsLong(value));
      if (v >= namesToVars[name]->domainSize()) {
        GUM_ERROR(gum::InvalidArgument,
                  "The value " << v << " is not in the domain of " << name);
      }
      inst.add(*(namesToVars[name]));
      inst.chgVal(namesToVars[name], v);
    }
  }

  gum::NodeId nodeIdFromNameOrIndex(PyObject*                       n,
                                    const gum::VariableNodeMap& map) {
    const std::string name = PyAgrumHelper::stringFromPyObject(n);
    if (name != "") {
      return map.idFromName(name);
    } else if (PyInt_Check(n)) {
        return gum::NodeId(PyInt_AsLong(n));
    } else if (PyLong_Check(n)) {
      return gum::NodeId(PyLong_AsLong(n));
    } else {
        GUM_ERROR(gum::InvalidArgument,
                  "A value is neither a node name nor an node id");
    }
  }

  void populateNodeSetFromPySequenceOfIntOrString(
    gum::NodeSet&                   nodeset,
    PyObject*                       seq,
    const gum::VariableNodeMap& map) {
    // if seq is just a string
    const std::string name = PyAgrumHelper::stringFromPyObject(seq);
    if (name != "") {
      nodeset.insert(map.idFromName(name));
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
      GUM_ERROR(gum::InvalidArgument,
                "Argument <seq> is not a list nor a set");
    }
  }

  PyObject* PyListFromNodeVect(const std::vector< gum::NodeId >& nodevect) {
    PyObject* q = PyList_New(0);

    for (auto node : nodevect) {
      PyList_Append(q, PyLong_FromUnsignedLong((unsigned long)node));
    }

    return q;
  }

  PyObject* PyListFromNodeSet(const gum::NodeSet& nodeset) {
    PyObject* q = PyList_New(0);

    for (auto node : nodeset) {
      PyList_Append(q, PyLong_FromUnsignedLong((unsigned long)node));
    }

    return q;
  }

  PyObject* PySetFromNodeSet(const gum::NodeSet& nodeset) {
    PyObject* q = PySet_New(0);

    for (auto node : nodeset) {
      PySet_Add(q, PyLong_FromUnsignedLong((unsigned long)node));
    }

    return q;
  }

  PyObject* PySetFromNodeSet(const gum::NodeGraphPart& nodeset) {
    PyObject* q = PySet_New(0);

    for (auto node : nodeset) {
      PySet_Add(q, PyLong_FromUnsignedLong((unsigned long)node));
    }

    return q;
  }

  PyObject* PyListFromArcVect(const std::vector< gum::Arc >& arcseq) {
    PyObject* q = PyList_New(0);
    for (const auto& arc : arcseq) {
      PyList_Append(q, Py_BuildValue("(i,i)", arc.tail(), arc.head()));
    }
    return q;
  }

  PyObject* PySetFromArcSet(const gum::ArcSet& arcset) {
    PyObject* q = PySet_New(0);
    for (const auto& arc : arcset) {
      PySet_Add(q, Py_BuildValue("(i,i)", arc.tail(), arc.head()));
    }
    return q;
  }

  PyObject* PySetFromEdgeSet(const gum::EdgeSet& edgeset) {
    PyObject* q = PySet_New(0);
    for (const auto& edg : edgeset) {
      PySet_Add(q, Py_BuildValue("(i,i)", edg.first(), edg.second()));
    }
    return q;
  }

  PyObject* PyDictFromInstantiation(const gum::Instantiation& inst) {
    PyObject* q = PyDict_New();
    for (const auto& k : inst.variablesSequence()) {
      PyDict_SetItemString(q,
                           k->name().c_str(),
                           PyLong_FromUnsignedLong((unsigned long)inst.val(*k)));
    }
    return q;
  }

  PyObject*
    PySeqFromSetOfInstantiation(const gum::Set< gum::Instantiation >& soi) {
    PyObject* q = PyList_New(0);
    for (const auto& inst : soi) {
      PyList_Append(q, PyDictFromInstantiation(inst));
    }
    return q;
  }
}   // namespace PyAgrumHelper

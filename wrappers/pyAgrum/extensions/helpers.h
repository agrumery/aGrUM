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
 * @brief This file contains c++ helper functions for wrappers (not included but imported in the wrapper)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <agrum/core/set.h>
#include <agrum/multidim/potential.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/graphs/graphElements.h>

class PyAgrumHelper {
  public:

  static std::string stringFromPyObject(PyObject *o) {
    std::string name="";
    if (PyUnicode_Check(o)) { //python3 string
      PyObject* asbytes=PyUnicode_AsASCIIString(o);
      name=PyBytes_AsString(asbytes);
      Py_DECREF(asbytes);
    } else if (PyString_Check(o)) {// python2 string
      name=PyString_AsString(o);
    } else if (PyBytes_Check(o)) {//other python3 string
      name=PyBytes_AsString(o);
    }
    return name;
  }

  // filling a Set of DiscreteVariable* from a list of string, in the context of a potential.
  static void fillDVSetFromPyObject(const gum::Potential<double>* pot,gum::Set<const gum::DiscreteVariable*>& s,PyObject* varnames) {
    if (PyList_Check(varnames)) {
      gum::Set<std::string> names;
      auto siz=PyList_Size(varnames);
      for (int i=0 ;i<siz ;i++) {
        std::string name=stringFromPyObject(PyList_GetItem(varnames,i));

        if (name=="")
          GUM_ERROR(gum::InvalidArgument,"Argument is not a list of string");

        names<<name;
      }

      for(const auto v : pot->variablesSequence())
        if (names.contains(v->name())) s<<v;

      if (s.size()==0)
        GUM_ERROR(gum::InvalidArgument,"No relevant dimension in the argument");
    } else {
      GUM_ERROR(gum::InvalidArgument,"Argument is not a list");
    }
  }

    // filling a vector of DiscreteVariable* from a list of string, in the context of a potential.
    static void fillDVVectorFromPyObject(const gum::Potential<double>* pot,std::vector<const gum::DiscreteVariable*>& s,PyObject* varnames) {
      if (PyList_Check(varnames)) {
        gum::HashTable<std::string,const gum::DiscreteVariable*> namesToVars;
        for(gum::Idx i=0;i<pot->nbrDim();i++)
          namesToVars.insert(pot->variable(i).name(),&(pot->variable(i)));

        auto siz=PyList_Size(varnames);
        s.clear();

        for (int i=0 ;i<siz ;i++) {
          std::string name=stringFromPyObject(PyList_GetItem(varnames,i));
          if (name=="") {
            GUM_ERROR(gum::InvalidArgument, "Argument is not a list of string");
          }
          if (! namesToVars.exists(name)) {
            GUM_ERROR(gum::InvalidArgument, "Argument is a not a name of a variable in this potential");
          }
          s.push_back(namesToVars[name]);
        }
      } else {
        GUM_ERROR(gum::InvalidArgument,"Argument is not a list");
      }
    }

    // filling a vector of DiscreteVariable* from a list of string, in the context of a potential.
    static void fillDVFromPyObject(const gum::Potential<double>* pot,const gum::DiscreteVariable*& pvar,PyObject* varname) {
      std::string name = stringFromPyObject(varname);
      if (name == "") {
        GUM_ERROR(gum::InvalidArgument, "Argument is not a string");
      }

      bool isOK=false;
      for (gum::Idx i = 0; i < pot->nbrDim(); i++) {
        if (pot->variable(i).name() == name) {
          pvar=&(pot->variable(i));
          isOK=true;
          break;
        }
      }
      if (!isOK) {
        GUM_ERROR(gum::InvalidArgument, "Argument is a not a name of a variable in this potential");
      }
    }

    // filling a Instantiation from a dictionnary<string,int>
    static void fillInstantiationFromPyObject(const gum::Potential<double>* pot,gum::Instantiation& inst,PyObject* dict) {
      gum::HashTable<std::string,const gum::DiscreteVariable*> namesToVars;
      for(gum::Idx i=0;i<pot->nbrDim();i++)
        namesToVars.insert(pot->variable(i).name(),&(pot->variable(i)));

      if (! PyDict_Check(dict)) {
        GUM_ERROR(gum::InvalidArgument,"Argument is not a dictionnary");
      }

      PyObject *key, *value;
      Py_ssize_t pos = 0;
      inst.clear();
      while (PyDict_Next(dict, &pos, &key, &value)) {
        std::string name= stringFromPyObject(key);
        if (name==""){
          GUM_ERROR(gum::InvalidArgument, "A key is not a string");
        }
        if (! namesToVars.exists(name)) {
          GUM_ERROR(gum::InvalidArgument, "The key "<<name<<" is a not a name of a variable in this potential");
        }
        if (!PyInt_Check(value)) {
          GUM_ERROR(gum::InvalidArgument, "A value is not an int");
        }
        gum::Idx v=gum::Idx(PyInt_AsLong(value));
        if (v>=namesToVars[name]->domainSize()) {
          GUM_ERROR(gum::InvalidArgument, "The value "<<v<<" is not in the domain of "<<name);
        }
        inst.add(*(namesToVars[name]));
        inst.chgVal(namesToVars[name],v);
      }
    }

    static gum::NodeId idFromNameOrInt(PyObject *n,const gum::IBayesNet<double>& bn) {
      std::string name= PyAgrumHelper::stringFromPyObject(n);
      if (name!=""){
        return bn.idFromName(name);
      } else {
        if (!PyInt_Check(n)) {
          GUM_ERROR(gum::InvalidArgument, "A value is neither a node name nor an node id");
        } else {
          return gum::NodeId(PyInt_AsLong(n));
        }
      }
    }
    
    static void populateNodeSetFromPySequenceOfIntOrString(gum::NodeSet& nodeset,PyObject* seq,const gum::IBayesNet<double>& bn) {
        /*
      if (PySequence_Check(list)) {
        auto siz=PySequence_Size(list);
        for (int i=0 ;i<siz ;i++) 
          nodeset.insert(idFromNameOrInt(PySequence_GetItem(list,i),bn));
      }
      if (PyAnySet_Check(list)) {
        auto siz=PySet_Size(list);
        for (int i=0 ;i<siz ;i++) 
          nodeset.insert(idFromNameOrInt(PySequence_GetItem(list,i),bn));
        
      }*/
      PyObject *iter = PyObject_GetIter(seq);
      if (iter!=NULL) {
        PyObject *item;
        while((item=PyIter_Next(iter))) {
          nodeset.insert(idFromNameOrInt(item,bn));          
        }
      } else {
        GUM_ERROR(gum::InvalidArgument,"Argument <list> is not a sequence nor a set");
      }  
    }

    static PyObject* PyListFromNodeSet(const gum::NodeSet& nodeset) {
      PyObject* q = PyList_New( 0 );

      for ( auto node : nodeset) {
        PyList_Append( q, PyLong_FromUnsignedLong((unsigned long)node));
      }

      return q;
    }
    static PyObject* PySetFromNodeSet(const gum::NodeSet& nodeset) {
      PyObject* q = PySet_New( 0 );

      for ( auto node : nodeset) {
        PySet_Add( q, PyLong_FromUnsignedLong((unsigned long)node));
      }

      return q;
    }
};

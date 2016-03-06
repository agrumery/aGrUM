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

class PyAgrumHelper {
  public:
  
  // filling a Set of DiscreteVariable* from a list of string, in the context of a potential.
  static void fillDVSetFromPyObject(const gum::Potential<double>* pot,gum::Set<const gum::DiscreteVariable*>& s,PyObject* varnames) {
    if (PyList_Check(varnames)) {
      gum::Set<std::string> names;
      auto siz=PyList_Size(varnames);
      for (int i=0 ; 
            i<siz ; 
            i++) {
        PyObject *o = PyList_GetItem(varnames,i);
      
        if (PyUnicode_Check(o)) { //python3 string
          PyObject* asbytes=PyUnicode_AsASCIIString(o);
          names<<PyBytes_AsString(asbytes);
          Py_DECREF(asbytes);
          continue;
        }
        if (PyString_Check(o)) {// python2 string
          names<<PyString_AsString(o);
          continue;
        }
        if (PyBytes_Check(o)) {//other python3 string
          names<<PyBytes_AsString(o);
          continue;
        }
        GUM_ERROR(gum::FatalError,"Argument is not a list of string");
      }
      
      for(const auto v : pot->variablesSequence()) 
        if (names.contains(v->name())) s<<v;
        
    } else {
      GUM_ERROR(gum::FatalError,"Argument is not a list");
    }     
  }
    
};
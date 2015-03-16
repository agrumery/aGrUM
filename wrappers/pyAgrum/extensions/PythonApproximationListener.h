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
#include <Python.h>

#include <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>
#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>
#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

class PythonApproximationListener: public gum::ApproximationSchemeListener {
  private:

    PyObject* __pyWhenProgress;
    PyObject* __pyWhenStop;

    void __checkCallable ( PyObject* pyfunc ) {
      if ( !PyCallable_Check ( pyfunc ) ) {
        PyErr_SetString ( PyExc_TypeError, "Need a callable object!" );
      }
    };

  public:
    PythonApproximationListener ( gum::IApproximationSchemeConfiguration& algo ) : gum::ApproximationSchemeListener ( algo ) {
      __pyWhenProgress = __pyWhenStop = ( PyObject* ) 0;
    };

    ~PythonApproximationListener() {
      if ( __pyWhenProgress ) Py_DECREF ( __pyWhenProgress );

      if ( __pyWhenStop ) Py_DECREF ( __pyWhenStop );
    };

    void whenProgress ( const void* src, gum::Size step, double error, double duration ) {
      if ( __pyWhenProgress ) {
        PyObject* arglist = Py_BuildValue ( "(ldd)", step, error, duration );
        PyEval_CallObject ( __pyWhenProgress, arglist );
        Py_DECREF ( arglist );
      }
    };

    void whenStop ( const void* src, std::string message ) {
      if ( __pyWhenStop ) {
        PyObject* arglist = Py_BuildValue ( "(s)", message.c_str() );
        PyEval_CallObject ( __pyWhenStop, arglist );
        Py_DECREF ( arglist );
      }
    };

    void setWhenProgress ( PyObject* pyfunc ) {
      __checkCallable ( pyfunc );

      if ( __pyWhenProgress ) Py_DECREF ( __pyWhenProgress );

      __pyWhenProgress = pyfunc;
      Py_INCREF ( pyfunc );
    };

    void setWhenStop ( PyObject* pyfunc ) {
      __checkCallable ( pyfunc );

      if ( __pyWhenStop ) Py_DECREF ( __pyWhenStop );

      __pyWhenStop = pyfunc;
      Py_INCREF ( pyfunc );
    };
};


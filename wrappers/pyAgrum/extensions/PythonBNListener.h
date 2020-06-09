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
 * @brief This file contains definition for a BN listener in python
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <Python.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/parts/listeners/diGraphListener.h>


class PythonBNListener : public gum::DiGraphListener {
  private:
    PyObject* pyWhenNodeAdded__;
    PyObject* pyWhenNodeDeleted__;
    PyObject* pyWhenArcAdded__;
    PyObject* pyWhenArcDeleted__;

    const gum::VariableNodeMap* map__;

    void checkCallable__ ( PyObject* pyfunc ) {
      if ( !PyCallable_Check ( pyfunc ) ) {
        PyErr_SetString ( PyExc_TypeError, "Need a callable object!" );
      }
    }

  public:
    PythonBNListener ( const gum::BayesNet<double>* bn, const gum::VariableNodeMap* vnm ) :
      gum::DiGraphListener ( &(bn->dag())), map__ ( vnm ) {
      pyWhenArcAdded__ = pyWhenArcDeleted__ = ( PyObject* ) 0;
      pyWhenNodeAdded__ = pyWhenNodeDeleted__ = ( PyObject* ) 0;
    }

    ~PythonBNListener() {
      if ( pyWhenArcAdded__ ) Py_DECREF ( pyWhenArcAdded__ );

      if ( pyWhenArcDeleted__ ) Py_DECREF ( pyWhenArcDeleted__ );

      if ( pyWhenNodeAdded__ ) Py_DECREF ( pyWhenNodeAdded__ );

      if ( pyWhenNodeDeleted__ ) Py_DECREF ( pyWhenNodeDeleted__ );
    }

    virtual void whenNodeAdded ( const void* source, gum::NodeId id ) {
      // we could check if source==_digraph !!!
      if ( pyWhenNodeAdded__ ) {
        PyObject* arglist = Py_BuildValue ( "(ls)", id,
                                            ( *map__ ) [id].name().c_str() );
        PyEval_CallObject ( pyWhenNodeAdded__, arglist );
        Py_DECREF ( arglist );
      }
    }

    virtual void whenNodeDeleted ( const void*, gum::NodeId id ) {
      // we could check if source==_digraph !!!
      if ( pyWhenNodeDeleted__ ) {
        PyObject* arglist = Py_BuildValue ( "(l)", id );
        PyEval_CallObject ( pyWhenNodeDeleted__, arglist );
        Py_DECREF ( arglist );
      }
    }

    virtual void whenArcAdded ( const void*, gum::NodeId src, gum::NodeId dst ) {
      // we could check if source==_digraph !!!
      if ( pyWhenArcAdded__ ) {
        PyObject* arglist = Py_BuildValue ( "(ll)", src, dst );
        PyEval_CallObject ( pyWhenArcAdded__, arglist );
        Py_DECREF ( arglist );
      }
    }

    virtual void whenArcDeleted ( const void*, gum::NodeId src, gum::NodeId dst ) {
      // we could check if source==_digraph !!!
      if ( pyWhenArcDeleted__ ) {
        PyObject* arglist = Py_BuildValue ( "(ll)", src, dst );
        PyEval_CallObject ( pyWhenArcDeleted__, arglist );
        Py_DECREF ( arglist );
      }
    }

    void setWhenArcAdded ( PyObject* pyfunc ) {
      checkCallable__ ( pyfunc );
      pyWhenArcAdded__ = pyfunc;
      Py_INCREF ( pyfunc );
    }

    void setWhenArcDeleted ( PyObject* pyfunc ) {
      checkCallable__ ( pyfunc );
      pyWhenArcDeleted__ = pyfunc;
      Py_INCREF ( pyfunc );
    }

    void setWhenNodeAdded ( PyObject* pyfunc ) {
      checkCallable__ ( pyfunc );
      pyWhenNodeAdded__ = pyfunc;
      Py_INCREF ( pyfunc );
    }

    void setWhenNodeDeleted ( PyObject* pyfunc ) {
      checkCallable__ ( pyfunc );
      pyWhenNodeDeleted__ = pyfunc;
      Py_INCREF ( pyfunc );
    }
};

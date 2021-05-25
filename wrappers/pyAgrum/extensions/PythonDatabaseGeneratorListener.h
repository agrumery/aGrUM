/***************************************************************************
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <agrum/tools/core/progressNotification.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>

class PythonDatabaseGeneratorListener : public gum::ProgressListener {
  private:
  PyObject* _pyWhenProgress_;
  PyObject* _pyWhenStop_;

  void _checkCallable_(PyObject* pyfunc) {
    if (!PyCallable_Check(pyfunc)) {
      PyErr_SetString(PyExc_TypeError, "Need a callable object!");
    }
  };

  public:
  explicit PythonDatabaseGeneratorListener(gum::learning::BNDatabaseGenerator<double>& notif)
      : gum::ProgressListener(notif) {
    _pyWhenProgress_ = _pyWhenStop_ = (PyObject*)0;
  };

  ~PythonDatabaseGeneratorListener() {
    if (_pyWhenProgress_) Py_DECREF(_pyWhenProgress_);

    if (_pyWhenStop_) Py_DECREF(_pyWhenStop_);
  };

  void whenProgress(const void* src, const gum::Size step, const double duration) {
    if (_pyWhenProgress_) {
      PyObject* arglist = Py_BuildValue("(ld)", step, duration);
      PyObject_Call(_pyWhenProgress_, arglist, NULL);
      Py_DECREF(arglist);
    }
  };

  void whenStop(const void* src, const std::string& message) {
    if (_pyWhenStop_) {
      PyObject* arglist = Py_BuildValue("(s)", message.c_str());
      PyObject_Call(_pyWhenStop_, arglist, NULL);
      Py_DECREF(arglist);
    }
  };

  void setWhenProgress(PyObject* pyfunc) {
    _checkCallable_(pyfunc);

    if (_pyWhenProgress_) Py_DECREF(_pyWhenProgress_);

    _pyWhenProgress_ = pyfunc;
    Py_INCREF(pyfunc);
  };

  void setWhenStop(PyObject* pyfunc) {
    _checkCallable_(pyfunc);

    if (_pyWhenStop_) Py_DECREF(_pyWhenStop_);

    _pyWhenStop_ = pyfunc;
    Py_INCREF(pyfunc);
  };
};

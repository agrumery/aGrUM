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
 * @brief gum:: -> Python exception translation used by the global
 * %exception SWIG feature declared in exceptions.i. Shared via #include
 * (not SWIG %import, which does not carry raw %{ %} code blocks across
 * modules) by every generated SWIG module (core pyagrum + each split
 * submodule): %import does propagate the %exception feature itself, so
 * every module's wrapper call sites call SetPythonizeAgrumException(), but
 * each module's own translation unit still needs this definition.
 *
 * The gum::Exception -> Python exception type mapping needs no per-class
 * catch here: every gum::Exception subclass overrides pythonClassName_()
 * (see GUM_MAKE_ERROR in exceptions.h) with its own exact C++ class name,
 * which is enough to look up the matching SWIG-wrapped Python type.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>
#include <string>

#include <agrum/agrum.h>

static void SetPythonizeAgrumException() {
  try {
    throw;
  } catch (std::bad_cast&) {
    PyErr_SetString(PyExc_RuntimeError, "C++ Bad Cast");
  } catch (gum::SyntaxError& e) {
    PyErr_SetString(PyExc_SyntaxError, e.errorContent().c_str());
    PyErr_SyntaxLocationEx(e.filename().c_str(), e.line(), e.col());
  } catch (gum::Exception& e) {
    const std::string swigTypeName = std::string("gum::") + e.pythonClassName_() + " *";
    PyErr_SetString(SWIG_Python_ExceptionType(SWIG_TypeQuery(swigTypeName.c_str())), e.what());
  } catch (std::exception& e) { PyErr_SetString(PyExc_Exception, e.what()); }
}

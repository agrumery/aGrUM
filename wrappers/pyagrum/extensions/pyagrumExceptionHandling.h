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
 * @author Pierre-Henri WUILLEMIN
 */
#pragma once

#include <Python.h>

#include <agrum/agrum.h>

#define PYGUM_CATCH(GUMEXCEPTION)                                               \
  catch (gum::GUMEXCEPTION & e) {                                               \
     PyErr_SetString (SWIG_Python_ExceptionType(SWIG_TypeQuery("gum::" #GUMEXCEPTION " *")), e.what()); \
  }

static void SetPythonizeAgrumException() {
  try {
    throw;
  }
  catch ( std::bad_cast& ) {
    PyErr_SetString ( PyExc_RuntimeError, "C++ Bad Cast" );
  }
  catch (gum::SyntaxError & e) {
    PyErr_SetString ( PyExc_SyntaxError, e.errorContent().c_str());
    PyErr_SyntaxLocationEx(e.filename().c_str(), e.line(), e.col());
  }
  PYGUM_CATCH(DefaultInLabel)
  PYGUM_CATCH(DuplicateElement)
  PYGUM_CATCH(DuplicateLabel)
  PYGUM_CATCH(FatalError)
  PYGUM_CATCH(FormatNotFound)
  PYGUM_CATCH(InvalidArc)
  PYGUM_CATCH(InvalidArgument)
  PYGUM_CATCH(InvalidArgumentsNumber)
  PYGUM_CATCH(InvalidDirectedCycle)
  PYGUM_CATCH(InvalidEdge)
  PYGUM_CATCH(InvalidNode)
  PYGUM_CATCH(DatabaseError)
  PYGUM_CATCH(MissingValueInDatabase)
  PYGUM_CATCH(MissingVariableInDatabase)
  PYGUM_CATCH(NoChild)
  PYGUM_CATCH(NoNeighbour)
  PYGUM_CATCH(NoParent)
  PYGUM_CATCH(GraphError)
  PYGUM_CATCH(NotFound)
  PYGUM_CATCH(NullElement)
  PYGUM_CATCH(OperationNotAllowed)
  PYGUM_CATCH(OutOfBounds)
  PYGUM_CATCH(ArgumentError)
  PYGUM_CATCH(SizeError)
  PYGUM_CATCH(IOError)
  PYGUM_CATCH(UndefinedElement)
  PYGUM_CATCH(UndefinedIteratorKey)
  PYGUM_CATCH(UndefinedIteratorValue)
  PYGUM_CATCH(UnknownLabelInDatabase)
  PYGUM_CATCH(CPTError)
  catch ( std::exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what() );
  }
}

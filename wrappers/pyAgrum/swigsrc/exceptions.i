/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
 /* EXCEPTION HANDLING */
%{
#define PYGUM_CATCH(GUMEXCEPTION)                                               \
  catch (gum::GUMEXCEPTION & e) {                                               \
     PyErr_SetString (SWIG_Python_ExceptionType(SWIGTYPE_p_gum__##GUMEXCEPTION),e.what().c_str()); \
  }

static void SetPythonizeAgrumException() {
  try {
    throw;
  }
  catch ( std::bad_cast& ) {
    PyErr_SetString ( PyExc_RuntimeError, "C++ Bad Cast" );
  }
  PYGUM_CATCH(DefaultInLabel)
  PYGUM_CATCH(DuplicateElement)
  PYGUM_CATCH(DuplicateLabel)
  PYGUM_CATCH(EmptyBSTree)
  PYGUM_CATCH(EmptySet)
  PYGUM_CATCH(FatalError)
  PYGUM_CATCH(FormatNotFound)
  PYGUM_CATCH(IdError)
  PYGUM_CATCH(InvalidArc)
  PYGUM_CATCH(InvalidArgument)
  PYGUM_CATCH(InvalidArgumentsNumber)
  PYGUM_CATCH(InvalidDirectedCycle)
  PYGUM_CATCH(InvalidEdge)
  PYGUM_CATCH(InvalidNode)
  PYGUM_CATCH(MissingValueInDatabase)
  PYGUM_CATCH(MissingVariableInDatabase)
  PYGUM_CATCH(NoChild)
  PYGUM_CATCH(NoNeighbour)
  PYGUM_CATCH(NoParent)
  PYGUM_CATCH(GraphError)
  PYGUM_CATCH(NotFound)
  PYGUM_CATCH(NullElement)
  PYGUM_CATCH(OperationNotAllowed)
  PYGUM_CATCH(OutOfLowerBound)
  PYGUM_CATCH(OutOfUpperBound)
  PYGUM_CATCH(OutOfBounds)
  PYGUM_CATCH(ReferenceError)
  PYGUM_CATCH(SizeError)
  PYGUM_CATCH(SyntaxError)
  PYGUM_CATCH(IOError)
  PYGUM_CATCH(UndefinedElement)
  PYGUM_CATCH(UndefinedIteratorKey)
  PYGUM_CATCH(UndefinedIteratorValue)
  PYGUM_CATCH(UnknownLabelInDatabase)
  catch ( gum::Exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what().c_str() );
  }
  catch ( std::exception& e ) {
    PyErr_SetString ( PyExc_Exception, e.what() );
  }
}
%}

%exception {
  try {
    $action
  } catch (...) {
    SetPythonizeAgrumException();
    SWIG_fail;
  }
}


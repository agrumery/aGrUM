/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





 /* EXCEPTION HANDLING */
%{
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
%}

%exception {
  try {
    $action
  } catch (...) {
    SetPythonizeAgrumException();
    SWIG_fail;
  }
}

/**************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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
/** @file
 * @brief aGrUM's exceptions
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef GUM_EXCEPTIONS_H
#define GUM_EXCEPTIONS_H

#include <iostream>
#include <iomanip>
#include <string>


#ifdef NDEBUG
#define GUM_ERROR_IN_EXPR(type,msg) throw(type(msg))
#define GUM_ERROR(type,msg) {std::ostringstream __error__str;__error__str<<msg;throw(type(__error__str.str()));}
#define GUM_SHOWERROR(e) {std::cerr << (e).content() << std::endl;}
#else
#define GUM_ERROR_IN_EXPR(type,msg) throw(type(msg))
#define GUM_ERROR(type,msg) {std::ostringstream __error__str;__error__str<<msg;throw(type(gum::__createMsg(__FILE__,__FUNCTION__,__LINE__,__error__str.str())));}
#define GUM_SHOWERROR(e) {std::cerr << __FILE__ << ":" << __LINE__ << ": Exception " << (e).content() << std::endl;std::cerr<<(e).callStack()<<std::endl;}
#endif //NDEBUG

#define GUM_MAKE_ERROR(TYPE,SUPERCLASS,MSG)                             \
  class TYPE : public SUPERCLASS { public: TYPE(std::string aMsg,std::string aType=MSG) : SUPERCLASS(aMsg,aType) {}; };


namespace gum {
  const std::string __createMsg ( const std::string filename,
                                  const std::string function,
                                  const int line, const std::string msg );
  /**
   * base class for all aGrUM's exceptions
   */

  class Exception {

    protected:
      std::string _msg;
      std::string _type;
      std::string _callstack;

    public:
      /** @name constructors & destructors
       * @{
       **/
      Exception ( const std::string aMsg = "",
                  const std::string aType = "Generic error" );

      ~Exception() {}

      /// @}
      /**
       * @return return the message content
       */
      const std::string content() const {
        return _type + " " + _msg;
      }

      const std::string type() const {
        return _type;
      }

      const std::string callStack() const {
        return _callstack;
      }
  };

/// Exception : there is a problem with an ID
  GUM_MAKE_ERROR ( IdError, Exception, "potential problem with an ID" )
/// Exception : erreur (inconnue ?) fatale
  GUM_MAKE_ERROR ( FatalError, Exception, "Fatal error" )
/// Exception : iterator does not point to any valid value
  GUM_MAKE_ERROR ( UndefinedIteratorValue, Exception, "Iterator's value is undefined" )
/// Exception : iterator does not point to any valid key
  GUM_MAKE_ERROR ( UndefinedIteratorKey, Exception, "Iterator's key is undefined" )
/// Exception : a pointer or a reference on a NULL (0) object
  GUM_MAKE_ERROR ( NullElement, Exception, "Null Element" )
/// Exception : a looked-for element could not be found
  GUM_MAKE_ERROR ( UndefinedElement, Exception, "could not find the so-called element" )
/// Exception : problem with the size of a HashTable
  GUM_MAKE_ERROR ( HashSize, Exception, "size not admissible in a HashTable" )
/// Exception : problem with size
  GUM_MAKE_ERROR ( SizeError, Exception, "size not admissible in a HashTable" )
/// Exception: an empty set is found, where it should not be
  GUM_MAKE_ERROR ( EmptySet, Exception, "an empty set has been found where it should not be" )

/// Exception: the number of arguments passed to a function is not what was expected
  GUM_MAKE_ERROR ( InvalidArgumentsNumber, Exception, "the number of arguments passed differs from what was expected" )
/// Exception: at least one argument passed to a function is not what was expected
  GUM_MAKE_ERROR ( InvalidArgument, Exception, "at least one argument passed differs from what was expected" )

/// Exception : input/output problem
  GUM_MAKE_ERROR ( IOError, Exception, "an input/output error occurred" )
/// Exception : a I/O format was not found
  GUM_MAKE_ERROR ( FormatNotFound, IOError, "could not find the specified format" )

/// Exception : operation not allowed
  GUM_MAKE_ERROR ( OperationNotAllowed, Exception, "this operation is not allowed" )
/// Exception : the element we looked for cannot be found
  GUM_MAKE_ERROR ( NotFound, Exception , "could not find this object" )
/// Syntax Error while parsing
  GUM_MAKE_ERROR ( SyntaxError, Exception, "Syntax error" )
///////////////////////////////////
/// Exception base for reference errro
  GUM_MAKE_ERROR ( ReferenceError, Exception, "Reference error" )
/// Exception : out of bound
  GUM_MAKE_ERROR ( OutOfBounds, ReferenceError , "Out of bound" )
/// Exception : out of lower bound
  GUM_MAKE_ERROR ( OutOfLowerBound, OutOfBounds , "Out of bound" )
/// Exception : out of upper bound
  GUM_MAKE_ERROR ( OutOfUpperBound, OutOfBounds , "Out of bound" )
/// Exception : a similar element already exists
  GUM_MAKE_ERROR ( DuplicateElement, ReferenceError , "A similar element already exists" )
/// Exception : a similar label already exists
  GUM_MAKE_ERROR ( DuplicateLabel, ReferenceError , "A similar label already exists" )
///////////////////////////////////
/// Exception base for graph error
  GUM_MAKE_ERROR ( GraphError, Exception, "Graph error" )
/// Exception : no neighbour to a given node was found
  GUM_MAKE_ERROR ( Noneighbour, GraphError, "No neighbour can be found to the given node" )
/// Exception : no parent for a given node was found
  GUM_MAKE_ERROR ( NoParent, GraphError, "No parent can be found w.r.t the given node" )
/// Exception : no child for a given node was found
  GUM_MAKE_ERROR ( NoChild, GraphError, "No child can be found w.r.t the given node" )
/// Exception : there is something wrong with an edge
  GUM_MAKE_ERROR ( InvalidEdge, GraphError, "the edge is not correct" )
/// Exception : there is something wrong with an arc
  GUM_MAKE_ERROR ( InvalidArc, GraphError, "the arc is not correct" )
/// Exception : node does not exist
  GUM_MAKE_ERROR ( InvalidNode, GraphError, "the node does not exist" )
/// Exception : the binary search tree is empty
  GUM_MAKE_ERROR ( EmptyBSTree, GraphError, "the binary search tree is empty" )
/// Exception : default in label
  GUM_MAKE_ERROR ( DefaultInLabel, GraphError, "Error on label" )
/// Exception : existence of a directed cycle in a graph
  GUM_MAKE_ERROR ( InvalidCircuit, GraphError, "the graph contains a directed cycle" )

/// Exception base for factory error
  GUM_MAKE_ERROR ( FactoryError, Exception, "factory error" )
  GUM_MAKE_ERROR ( FactoryInvalidState, FactoryError, "invalid state error" )
  GUM_MAKE_ERROR ( WrongType, FactoryError, "wrong type for this operation" )
  GUM_MAKE_ERROR ( WrongClassElement, FactoryError, "wrong ClassElement for this operation" )
  GUM_MAKE_ERROR ( TypeError, FactoryError, "wrong subtype or subclass" )
} /* namespace gum */


#endif /* GUM_EXCEPTIONS_H */

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

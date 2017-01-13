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
// WARNING : Do not include this file directlty : instead include
// <agrum/config.h>

#include <iomanip>
#include <iostream>
#include <string>

#include <agrum/core/types.h>

#ifdef NDEBUG
#define GUM_ERROR_IN_EXPR( type, msg ) throw( type( msg ) )
#define GUM_ERROR( type, msg )                                  \
  {                                                             \
    std::ostringstream __error__str;                            \
    __error__str << __FILE__ << ":" << __LINE__ << ": " << msg; \
    throw( type( __error__str.str() ) );                        \
  }
#define GUM_SHOWERROR( e )                                               \
  {                                                                      \
    std::cout << std::endl                                               \
              << __FILE__ << ":" << __LINE__ << " " << ( e ).errorType() \
              << " from " << std::endl                                   \
              << ( e ).errorContent() << std::endl;                      \
  }
#else
#define GUM_ERROR_IN_EXPR( type, msg ) throw( type( msg ) )
#define GUM_ERROR( type, msg )                                      \
  {                                                                 \
    std::ostringstream __error__str;                                \
    __error__str << msg;                                            \
    throw( type( gum::__createMsg(                                  \
        __FILE__, __FUNCTION__, __LINE__, __error__str.str() ) ) ); \
  }
#define GUM_SHOWERROR( e )                                               \
  {                                                                      \
    std::cout << std::endl                                               \
              << __FILE__ << ":" << __LINE__ << " " << ( e ).errorType() \
              << " from " << std::endl                                   \
              << ( e ).errorContent() << std::endl;                      \
    std::cout << ( e ).errorCallStack() << std::endl;                    \
  }
#endif  // NDEBUG

#define GUM_MAKE_ERROR( TYPE, SUPERCLASS, MSG )       \
  class TYPE : public SUPERCLASS {                    \
    public:                                           \
    TYPE( std::string aMsg, std::string aType = MSG ) \
        : SUPERCLASS( aMsg, aType ){};                \
  };

#define GUM_SYNTAX_ERROR( msg, line, column )                      \
  {                                                                \
    std::ostringstream __error__str;                               \
    __error__str << msg;                                           \
    throw( gum::SyntaxError( __error__str.str(), line, column ) ); \
  }

namespace gum {

  /**
   * @brief Base class for all aGrUM's exceptions.
   */
  class Exception {

    protected:
    std::string _msg;
    std::string _type;
    std::string _callstack;

    public:
    // ====================================================================
    /// @name Class constructors & destructors
    // ====================================================================
    /// @{
    Exception( const std::string aMsg = "",
               const std::string aType = "Generic error" );

    Exception( const Exception& e );

    ~Exception() {}

    /// @}

    const std::string toString() const { return _msg; }

    /**
     * @brief Returns the message content.
     * @return Returns the message content.
     */
    const std::string errorContent() const { return _msg; }

    /**
     * @brief Returns the error type.
     * @return Returns the error type.
     */
    const std::string errorType() const { return _type; }

    /**
     * @brief Returns the error call stack.
     * @return Returns the error call stack.
     */
    const std::string errorCallStack() const { return _callstack; }
  };


  /**
   * @class gum::IdError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : there is a problem with an ID
   */
  class IdError;

  /**
   * @class gum::FatalError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : erreur (inconnue ?) fatale
   */
  class FatalError;

  /**
   * @class gum::UndefinedIteratorValue agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : iterator does not point to any valid value
   */
  class UndefinedIteratorValue;


  /**
   * @class gum::UndefinedIteratorKey agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : iterator does not point to any valid key
   */
  class UndefinedIteratorKey;

  /**
   * @class gum::NullElement agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : a pointer or a reference on a nullptr (0) object
   */
  class NullElement;

  /**
   * @class gum::UndefinedElement agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : a looked-for element could not be found
   */
  class UndefinedElement;

  /**
   * @class gum::SizeError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : problem with size
   */
  class SizeError;

  /**
   * @class gum::EmptySet agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception: an empty set is found, where it should not be
   */
  class EmptySet;

  /**
   * @class gum::InvalidArgumentsNumber agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception: the number of arguments passed to a function is not what was
   * expected
   */
  class InvalidArgumentsNumber;

  /**
   * @class gum::InvalidArgument agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception: at least one argument passed to a function is not what was
   * expected
   */
  class InvalidArgument;

  /**
   * @class gum::IOError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : input/output problem
   */
  class IOError;


  /**
   * @class gum::FormatNotFound agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : a I/O format was not found
   */
  class FormatNotFound;

  /**
   * @class gum::OperationNotAllowed agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : operation not allowed
   */
  class OperationNotAllowed;

  /**
   * @class gum::NotFound agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception : the element we looked for cannot be found
   */
  class NotFound;

  /**
   * @class gum::ReferenceError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception base for reference errro
   */
  class ReferenceError;

  /**
   * @class gum::OutOfBounds agrum/core/exceptions.h
   * @extends gum::ReferenceError
   * Exception : out of bound
   */
  class OutOfBounds;

  /**
   * @class gum::OutOfLowerBound agrum/core/exceptions.h
   * @extends OutOfBounds
   * Exception : out of lower bound
   */
  class OutOfLowerBound;

  /**
   * @class gum::OutOfUpperBound agrum/core/exceptions.h
   * @extends OutOfBounds
   * Exception : out of upper bound
   */
  class OutOfUpperBound;

  /**
   * @class gum::DuplicateElement agrum/core/exceptions.h
   * @extends gum::ReferenceError
   * Exception : a similar element already exists
   */
  class DuplicateElement;

  /**
   * @class gum::DuplicateLabel agrum/core/exceptions.h
   * @extends gum::ReferenceError
   * Exception : a similar label already exists
   */
  class DuplicateLabel;

  ///////////////////////////////////
  /**
   * @class gum::GraphError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception base for graph error
   */
  class GraphError;

  /**
   * @class gum::NoNeighbour agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no neighbour to a given node was found
   */
  class NoNeighbour;

  /**
   * @class gum::NoParent agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no parent for a given node was found
   */
  class NoParent;

  /**
   * @class gum::NoChild agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no child for a given node was found
   */
  class NoChild;

  /**
   * @class gum::InvalidEdge agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : there is something wrong with an edge
   */
  class InvalidEdge;

  /**
   * @class gum::InvalidArc agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : there is something wrong with an arc
   */
  class InvalidArc;

  /**
   * @class gum::InvalidNode agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : node does not exist
   */
  class InvalidNode;

  /**
   * @class gum::EmptyBSTree agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : the binary search tree is empty
   */
  class EmptyBSTree;

  /**
   * @class gum::DefaultInLabel agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : default in label
   */
  class DefaultInLabel;

  /**
   * @class gum::InvalidDirectedCycle agrum/core/exceptions.h
   * @extends gum::GraphError
   * Exception : existence of a directed cycle in a graph
   */
  class InvalidDirectedCycle;


  ///////////////////////////////////
  /**
   * @class gum::CPTError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception base for CPT error
   */
  class CPTError;

  /**
   * @class gum::CPTNoSumTo1 agrum/core/exceptions.h
   * @extends gum::CPTError
   * Exception : the CPT does not sum to 1
   */
  class CPTNoSumTo1;

  /**
   * @class gum::IncompatibleEvidence agrum/core/exceptions.h
   * Exception : several evidence are incompatible together (proba=0)
   */
  class IncompatibleEvidence;

  /**
   * @class gum::FactoryError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exception base for factory error
   */
  class FactoryError;

  /**
   * @class gum::FactoryInvalidState agrum/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : invalid state error
   */
  class FactoryInvalidState;

  /**
   * @class gum::WrongType agrum/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : wrong type for this operation
   */
  class WrongType;

  /**
   * @class gum::WrongClassElement agrum/core/exceptions.h
   * @extends gum::FactoryError
   * Exception: wrong PRMClassElement for this operation
   */
  class WrongClassElement;

  /**
   * @class gum::TypeError agrum/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : wrong subtype or subclass
   */
  class TypeError;

  /**
   * @class gum::LearningError agrum/core/exceptions.h
   * @extends gum::Exception
   * Exceptions for learning
   */
  class LearningError;

  /**
   * @class gum::IncompatibleScoreApriori agrum/core/exceptions.h
   * @extends gum::LearningError
   * Error: The score already contains a different 'implicit' apriori.
   */
  class IncompatibleScoreApriori;

  /**
   * @class gum::PossiblyIncompatibleScoreApriori agrum/core/exceptions.h
   * @extends gum::LearningError
   * Error: Due to its weight, the apriori is currently compatible with the
   *        score but if you change the weight, it will become incompatible"
   */
  class PossiblyIncompatibleScoreApriori;

  /**
   * @class gum::DatabaseError agrum/core/exceptions.h
   * @extends gum::LearningError
   * Error: An unknown error occured while accessing a database
   */
  class DatabaseError;

  /**
   * @class gum::MissingVariableInDatabase agrum/core/exceptions.h
   * @extends gum::LearningError
   * Error: A name of variable is not found in the database.
   */
  class MissingVariableInDatabase;

  /**
   * @class gum::UnknownLabelInDatabase agrum/core/exceptions.h
   * @extends gum::LearningError
   * Error: An unknown label is found in the database
   */
  class UnknownLabelInDatabase;

  /**
   * @class gum::SyntaxError exceptions.h <agrum/core/exceptions.h>
   * @extends gum::IOError
   * Special exception for syntax errors in files.
   */
  class SyntaxError;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  const std::string __createMsg( const std::string& filename,
                                 const std::string& function,
                                 const int          line,
                                 const std::string& msg );
  GUM_MAKE_ERROR( IdError, Exception, "ID error" )
  GUM_MAKE_ERROR( FatalError, Exception, "Fatal error" )
  GUM_MAKE_ERROR( UndefinedIteratorValue, Exception, "Undefined iterator" )
  GUM_MAKE_ERROR( UndefinedIteratorKey, Exception, "Undefined iterator's key" )
  GUM_MAKE_ERROR( NullElement, Exception, "Null element" )
  GUM_MAKE_ERROR( UndefinedElement, Exception, "Undefined element" )
  GUM_MAKE_ERROR( SizeError, Exception, "incorrect size" )
  GUM_MAKE_ERROR( EmptySet, Exception, "Empty set" )
  GUM_MAKE_ERROR( InvalidArgumentsNumber, Exception, "Invalid argument number" )
  GUM_MAKE_ERROR( InvalidArgument, Exception, "Invalid argument" )
  GUM_MAKE_ERROR( IOError, Exception, "I/O Error" )
  GUM_MAKE_ERROR( FormatNotFound, IOError, "Format not found" )
  GUM_MAKE_ERROR( OperationNotAllowed, Exception, "Operation not allowed" )
  GUM_MAKE_ERROR( NotFound, Exception, "Object not found" )
  GUM_MAKE_ERROR( ReferenceError, Exception, "Reference error" )
  GUM_MAKE_ERROR( OutOfBounds, ReferenceError, "Out of bound error" )
  GUM_MAKE_ERROR( OutOfLowerBound, OutOfBounds, "Out of lower bound error" )
  GUM_MAKE_ERROR( OutOfUpperBound, OutOfBounds, "Out of upper bound error" )
  GUM_MAKE_ERROR( DuplicateElement, ReferenceError, "Duplicate element" )
  GUM_MAKE_ERROR( DuplicateLabel, ReferenceError, "Duplicate label" )
  GUM_MAKE_ERROR( GraphError, Exception, "Graph error" )
  GUM_MAKE_ERROR( NoNeighbour, GraphError, "No neighbour found" )
  GUM_MAKE_ERROR( NoParent, GraphError, "No parent found" )
  GUM_MAKE_ERROR( NoChild, GraphError, "No child found" )
  GUM_MAKE_ERROR( InvalidEdge, GraphError, "Edge invalid" )
  GUM_MAKE_ERROR( InvalidArc, GraphError, "Arc invalid" )
  GUM_MAKE_ERROR( InvalidNode, GraphError, "Node invalid" )
  GUM_MAKE_ERROR( EmptyBSTree, GraphError, "Empty binary search tree" )
  GUM_MAKE_ERROR( DefaultInLabel, GraphError, "Error on label" )
  GUM_MAKE_ERROR( InvalidDirectedCycle, GraphError, "Directed cycle detected" )
  GUM_MAKE_ERROR( CPTError, Exception, "CPT error" )
  GUM_MAKE_ERROR( CPTNoSumTo1, CPTError, "CPT does not sum to 1" )
  GUM_MAKE_ERROR( IncompatibleEvidence,
                  Exception,
                  "several evidence/CPT are incompatible together (proba=0)" )
  GUM_MAKE_ERROR( FactoryError, Exception, "factory error" )
  GUM_MAKE_ERROR( FactoryInvalidState, FactoryError, "invalid state error" )
  GUM_MAKE_ERROR( WrongType, FactoryError, "wrong type for this operation" )
  GUM_MAKE_ERROR( WrongClassElement, FactoryError, "Wrong ClassElement" )
  GUM_MAKE_ERROR( TypeError, FactoryError, "Wrong subtype or subclass" )
  GUM_MAKE_ERROR( LearningError, Exception, "Factory error" )
  GUM_MAKE_ERROR( IncompatibleScoreApriori,
                  LearningError,
                  "Incompatbile (maybe implicit) priors" )
  GUM_MAKE_ERROR( PossiblyIncompatibleScoreApriori,
                  LearningError,
                  "Possible incompatibilty between score and prior" )
  GUM_MAKE_ERROR( DatabaseError, LearningError, "Database error" )
  GUM_MAKE_ERROR( MissingVariableInDatabase,
                  LearningError,
                  "Missing variable name in database" )
  GUM_MAKE_ERROR( UnknownLabelInDatabase,
                  LearningError,
                  "Unknown label found in database" )

  class SyntaxError : public IOError {
    protected:
    Size _noLine;
    Size _noCol;

    public:
    SyntaxError( const std::string& aMsg,
                 Size               nol,
                 Size               noc,
                 std::string        aType = "Syntax Error" )
        : IOError( aMsg, aType )
        , _noLine( nol )
        , _noCol( noc ){

          };

    Size col() const { return _noCol; };
    Size line() const { return _noLine; };
  };
#endif  // DOXYGEN_SHOULD_SKIP_THIS
} /* namespace gum */

#endif /* GUM_EXCEPTIONS_H */

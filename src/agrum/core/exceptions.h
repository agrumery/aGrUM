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

#include <iostream>
#include <iomanip>
#include <string>

#include <agrum/core/types.h>

#ifdef NDEBUG
#define GUM_ERROR_IN_EXPR( type, msg ) throw( type( msg ) )
#define GUM_ERROR( type, msg )           \
{                                      \
  std::ostringstream __error__str;     \
  __error__str << msg;                 \
  throw( type( __error__str.str() ) ); \
}
#define GUM_SHOWERROR( e )                                                \
{                                                                       \
  std::cerr << __FILE__ << ":" << __LINE__ << ": " << ( e ).errorType() \
  << " - " << ( e ).errorContent() << std::endl;              \
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
#define GUM_SHOWERROR( e )                                                \
{                                                                       \
  std::cerr << __FILE__ << ":" << __LINE__ << ": " << ( e ).errorType() \
  << " - " << ( e ).errorContent() << std::endl;              \
  std::cerr << ( e ).errorCallStack() << std::endl;                     \
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
  const std::string __createMsg( const std::string& filename,
      const std::string& function,
      const int line,
      const std::string& msg );
  /**
   * Base class for all aGrUM's exceptions
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
      Exception( const std::string aMsg = "",
          const std::string aType = "Generic error" );

      ~Exception() {}

      /// @}
      /**
       * @return return the message content
       */
      const std::string errorContent() const { return _type + " : " + _msg; }

      const std::string errorType() const { return _type; }

      const std::string errorCallStack() const { return _callstack; }
  };

  /*!
   * \class gum::IdError agrum/core/exceptions.h
   * \extends gum::Exception
   * Exception : there is a problem with an ID
   */
  class IdError;
  GUM_MAKE_ERROR( IdError, Exception, "potential problem with an ID" )

    /*!
     * \class gum::FatalError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : erreur (inconnue ?) fatale
     */
    class FatalError;
  GUM_MAKE_ERROR( FatalError, Exception, "Fatal error" )

    /*!
     * \class gum::UndefinedIteratorValue agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : iterator does not point to any valid value
     */
    class UndefinedIteratorValue;
  GUM_MAKE_ERROR( UndefinedIteratorValue,
      Exception,
      "Iterator's value is undefined" )

    /*!
     * \class gum::UndefinedIteratorKey agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : iterator does not point to any valid key
     */
    class UndefinedIteratorKey;
  GUM_MAKE_ERROR( UndefinedIteratorKey,
      Exception,
      "Iterator's key is undefined" )

    /*!
     * \class gum::NullElement agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : a pointer or a reference on a nullptr (0) object
     */
    class NullElement;
  GUM_MAKE_ERROR( NullElement, Exception, "Null Element" )

    /*!
     * \class gum::UndefinedElement agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : a looked-for element could not be found
     */
    class UndefinedElement;
  GUM_MAKE_ERROR( UndefinedElement,
      Exception,
      "could not find the so-called element" )

    /*!
     * \class gum::HashSize agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : problem with the size of a HashTable
     */
    class HashSize;
  GUM_MAKE_ERROR( HashSize, Exception, "size not admissible in a HashTable" )

    /*!
     * \class gum::SizeError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : problem with size
     */
    class SizeError;
  GUM_MAKE_ERROR( SizeError, Exception, "incorrect size" )

    /*!
     * \class gum::EmptySet agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception: an empty set is found, where it should not be
     */
    class EmptySet;
  GUM_MAKE_ERROR( EmptySet,
      Exception,
      "an empty set has been found where it should not be" )

    /*!
     * \class gum::InvalidArgumentsNumber agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception: the number of arguments passed to a function is not what was expected
     */
    class InvalidArgumentsNumber;
  GUM_MAKE_ERROR( InvalidArgumentsNumber,
      Exception,
      "the number of arguments passed differs from what was expected" )

    /*!
     * \class gum::InvalidArgument agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception: at least one argument passed to a function is not what was expected
     */
    class InvalidArgument;
  GUM_MAKE_ERROR( InvalidArgument,
      Exception,
      "at least one argument passed differs from what was expected" )

    /*!
     * \class gum::IOError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : input/output problem
     */
    class IOError;
  GUM_MAKE_ERROR( IOError, Exception, "an input/output error occurred" )


    /*!
     * \class gum::FormatNotFound agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : a I/O format was not found
     */
    class FormatNotFound;
  GUM_MAKE_ERROR( FormatNotFound,
      IOError,
      "could not find the specified format" )

    /*!
     * \class gum::OperationNotAllowed agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : operation not allowed
     */
    class OperationNotAllowed;
  GUM_MAKE_ERROR( OperationNotAllowed,
      Exception,
      "this operation is not allowed" )

    /*!
     * \class gum::NotFound agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception : the element we looked for cannot be found
     */
    class NotFound;
  GUM_MAKE_ERROR( NotFound, Exception, "could not find this object" )

    /*!
     * \class gum::ReferenceError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception base for reference errro
     */
    class ReferenceError;
  GUM_MAKE_ERROR( ReferenceError, Exception, "Reference error" )

    /*!
     * \class gum::OutOfBounds agrum/core/exceptions.h
     * \extends gum::ReferenceError
     * Exception : out of bound
     */
    class OutOfBounds;
  GUM_MAKE_ERROR( OutOfBounds, ReferenceError, "Out of bound" )

    /*!
     * \class gum::OutOfLowerBound agrum/core/exceptions.h
     * \extends OutOfBounds
     * Exception : out of lower bound
     */
    class OutOfLowerBound;
  GUM_MAKE_ERROR( OutOfLowerBound, OutOfBounds, "Out of lower bound" )

    /*!
     * \class gum::OutOfUpperBound agrum/core/exceptions.h
     * \extends OutOfBounds
     * Exception : out of upper bound
     */
    class OutOfUpperBound;
  GUM_MAKE_ERROR( OutOfUpperBound, OutOfBounds, "Out of upper bound" )

    /*!
     * \class gum::DuplicateElement agrum/core/exceptions.h
     * \extends gum::ReferenceError
     * Exception : a similar element already exists
     */
    class DuplicateElement;
  GUM_MAKE_ERROR( DuplicateElement,
      ReferenceError,
      "A similar element already exists" )

    /*!
     * \class gum::DuplicateLabel agrum/core/exceptions.h
     * \extends gum::ReferenceError
     * Exception : a similar label already exists
     */
    class DuplicateLabel;
  GUM_MAKE_ERROR( DuplicateLabel,
      ReferenceError,
      "A similar label already exists" )

    ///////////////////////////////////
    /*!
     * \class gum::GraphError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception base for graph error
     */
    class GraphError;
  GUM_MAKE_ERROR( GraphError, Exception, "Graph error" )

    /*!
     * \class gum::NoNeighbour agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : no neighbour to a given node was found
     */
    class NoNeighbour;
  GUM_MAKE_ERROR( NoNeighbour,
      GraphError,
      "No neighbour can be found to the given node" )

    /*!
     * \class gum::NoParent agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : no parent for a given node was found
     */
    class NoParent;
  GUM_MAKE_ERROR( NoParent,
      GraphError,
      "No parent can be found w.r.t the given node" )

    /*!
     * \class gum::NoChild agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : no child for a given node was found
     */
    class NoChild;
  GUM_MAKE_ERROR( NoChild,
      GraphError,
      "No child can be found w.r.t the given node" )

    /*!
     * \class gum::InvalidEdge agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : there is something wrong with an edge
     */
    class InvalidEdge;
  GUM_MAKE_ERROR( InvalidEdge, GraphError, "the edge is not correct" )

    /*!
     * \class gum::InvalidArc agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : there is something wrong with an arc
     */
    class InvalidArc;
  GUM_MAKE_ERROR( InvalidArc, GraphError, "the arc is not correct" )

    /*!
     * \class gum::InvalidNode agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : node does not exist
     */
    class InvalidNode;
  GUM_MAKE_ERROR( InvalidNode, GraphError, "the node does not exist" )

    /*!
     * \class gum::EmptyBSTree agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : the binary search tree is empty
     */
    class EmptyBSTree;
  GUM_MAKE_ERROR( EmptyBSTree, GraphError, "the binary search tree is empty" )

    /*!
     * \class gum::DefaultInLabel agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : default in label
     */
    class DefaultInLabel;
  GUM_MAKE_ERROR( DefaultInLabel, GraphError, "Error on label" )

    /*!
     * \class gum::InvalidDirectedCycle agrum/core/exceptions.h
     * \extends gum::GraphError
     * Exception : existence of a directed cycle in a graph
     */
    class InvalidDirectedCycle;
  GUM_MAKE_ERROR( InvalidDirectedCycle,
      GraphError,
      "the graph contains a directed cycle" )


    ///////////////////////////////////
    /*!
     * \class gum::CPTError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception base for CPT error
     */
    class CPTError;
  GUM_MAKE_ERROR( CPTError, Exception, "CPT error" )

    /*!
     * \class gum::CPTNoSumTo1 agrum/core/exceptions.h
     * \extends gum::CPTError
     * Exception : no neighbour to a given node was found
     */
    class CPTNoSumTo1;
  GUM_MAKE_ERROR( CPTNoSumTo1, CPTError, "CPT does not sum to 1" )

    /*!
     * \class gum::FactoryError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exception base for factory error
     */
    class FactoryError;
  GUM_MAKE_ERROR( FactoryError, Exception, "factory error" )

    /*!
     * \class gum::FactoryInvalidState agrum/core/exceptions.h
     * \extends gum::FactoryError
     * Exception : invalid state error
     */
    class FactoryInvalidState;
  GUM_MAKE_ERROR( FactoryInvalidState, FactoryError, "invalid state error" )

    /*!
     * \class gum::WrongType agrum/core/exceptions.h
     * \extends gum::FactoryError
     * Exception : wrong type for this operation
     */
    class WrongType;
  GUM_MAKE_ERROR( WrongType, FactoryError, "wrong type for this operation" )

    /*!
     * \class gum::WrongClassElement agrum/core/exceptions.h
     * \extends gum::FactoryError
     * Exception: wrong ClassElement for this operation
     */
    class WrongClassElement;
  GUM_MAKE_ERROR( WrongClassElement,
      FactoryError,
      "wrong ClassElement for this operation" )

    /*!
     * \class gum::TypeError agrum/core/exceptions.h
     * \extends gum::FactoryError
     * Exception : wrong subtype or subclass
     */
    class TypeError;
  GUM_MAKE_ERROR( TypeError, FactoryError, "wrong subtype or subclass" )

    /*!
     * \class gum::LearningError agrum/core/exceptions.h
     * \extends gum::Exception
     * Exceptions for learning
     */
    class LearningError;
  GUM_MAKE_ERROR( LearningError, Exception, "factory error" )

    /*!
     * \class gum::IncompatibleScoreApriori agrum/core/exceptions.h
     * \extends gum::LearningError
     * Error: The score already contains a different 'implicit' apriori.
     */
    class IncompatibleScoreApriori;
  GUM_MAKE_ERROR( IncompatibleScoreApriori,
      LearningError,
      "The score already "
      "contains a different 'implicit' apriori" )

    /*!
     * \class gum::PossiblyIncompatibleScoreApriori agrum/core/exceptions.h
     * \extends gum::LearningError
     * Error: Due to its weight, the apriori is currently compatible with the 
     *        score but if you change the weight, it will become incompatible"
     */
    class PossiblyIncompatibleScoreApriori;
  GUM_MAKE_ERROR(
      PossiblyIncompatibleScoreApriori,
      LearningError,
      "Due to its weight, the apriori is currently compatible with the "
      "score but if you change the weight, it will become incompatible" )

    /*!
     * \class gum::MissingVariableInDatabase agrum/core/exceptions.h
     * \extends gum::LearningError
     * Error: A name of variable is not found in the database.
     */
    class MissingVariableInDatabase;
  GUM_MAKE_ERROR( MissingVariableInDatabase,
      LearningError,
      "A name of variable is not found in the database" )

    /*!
     * \class gum::UnknownLabelInDatabase agrum/core/exceptions.h
     * \extends gum::LearningError
     * Error: An unknown label is found in the database
     */
    class UnknownLabelInDatabase;
  GUM_MAKE_ERROR( UnknownLabelInDatabase,
      LearningError,
      "An unknown label is found in the database" )

    /// special exception for syntax errors in files
    class SyntaxError : public IOError {
      protected:
        Size _noLine;
        Size _noCol;

      public:
        SyntaxError( const std::string& aMsg,
            Size nol,
            Size noc,
            std::string aType = "Syntax Error" )
          : IOError( aMsg, aType )
            , _noLine( nol )
            , _noCol( noc ){

            };

        Size col() const { return _noCol; };
        Size line() const { return _noLine; };
    };
} /* namespace gum */

#endif /* GUM_EXCEPTIONS_H */

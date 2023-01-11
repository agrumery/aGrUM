/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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


/** @file
 * @brief aGrUM's exceptions
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_EXCEPTIONS_H
#define GUM_EXCEPTIONS_H
// WARNING : Do not include this file directly : instead include
// <agrum/config.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include <agrum/tools/core/types.h>

#define GUM_ERROR_IN_EXPR(type, msg) throw(type(msg))

#ifdef GUM_FOR_SWIG
#  define GUM_ERROR(type, msg)         \
    {                                  \
      std::ostringstream error_stream; \
      error_stream << msg;             \
      throw(type(error_stream.str())); \
    }
#  define GUM_SHOWERROR(e) \
    { std::cout << std::endl << (e).what() << std::endl; }
#else   // GUM_FOR_SWIG
#  define GUM_ERROR(type, msg)                                                             \
    {                                                                                      \
      std::ostringstream error_stream;                                                     \
      error_stream << msg;                                                                 \
      throw(type(gum::_createMsg_(__FILE__, __FUNCTION__, __LINE__, error_stream.str()))); \
    }
#  ifdef GUM_DEBUG_MODE
#    define GUM_SHOWERROR(e)                                                                     \
      {                                                                                          \
        std::cout << std::endl << __FILE__ << ":" << __LINE__ << " " << (e).what() << std::endl; \
        std::cout << (e).errorCallStack() << std::endl;                                          \
      }
#  else   // GUM_DEBUG_MODE
#    define GUM_SHOWERROR(e) \
      { std::cout << std::endl << __FILE__ << ":" << __LINE__ << " " << (e).what() << std::endl; }
#  endif   // GUM_DEBUG_MODE
#endif     //  GUM_FOR_SWIG

#define GUM_MAKE_ERROR(TYPE, SUPERCLASS, MSG)                                \
  class TYPE: public SUPERCLASS {                                            \
    public:                                                                  \
    explicit TYPE(const std::string& aMsg, const std::string& aType = MSG) : \
        SUPERCLASS(aMsg, aType){};                                           \
    TYPE(const TYPE& src) : SUPERCLASS(src){};                               \
  };

#ifdef GUM_FOR_SWIG
#  define GUM_SYNTAX_ERROR(msg, filename, line, column)                    \
    {                                                                      \
      std::ostringstream error_stream;                                     \
      error_stream << msg;                                                 \
      throw(gum::SyntaxError(error_stream.str(), filename, line, column)); \
    }
#else   // GUM_FOR_SWIG
#  define GUM_SYNTAX_ERROR(msg, filename, line, column)                    \
    {                                                                      \
      std::ostringstream error_stream;                                     \
      error_stream << msg;                                                 \
      throw(gum::SyntaxError(error_stream.str(), filename, line, column)); \
    }
#endif   // GUM_FOR_SWIG

namespace gum {

  /**
   * @brief Base class for all aGrUM's exceptions.
   */
  class Exception: public std::exception {
    protected:
    std::string msg_;
    std::string type_;
    std::string callstack_;
    std::string what_;

    public:
    // ====================================================================
    /// @name Class constructors & destructors
    // ====================================================================
    /// @{
    explicit Exception(std::string aMsg = "", std::string aType = "Generic error");

    Exception(const Exception& e);

    virtual ~Exception() = default;

    /// @}

    GUM_NODISCARD const char* what() const noexcept override { return what_.c_str(); };

    /**
     * @brief Returns the message content.
     * @return Returns the message content.
     */
    GUM_NODISCARD std::string errorContent() const { return msg_; };

    /**
     * @brief Returns the error type.
     * @return Returns the error type.
     */
    GUM_NODISCARD std::string errorType() const { return type_; };

    /**
     * @brief Returns the error call stack.
     * @return Returns the error call stack.
     */
    GUM_NODISCARD std::string errorCallStack() const { return callstack_; };
  };


  /**
   * @class gum::FatalError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : fatal (unknown ?) error
   */
  class FatalError;

  /**
   * @class gum::UndefinedIteratorValue agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : generic error on iterator
   */
  class IteratorError;

  /**
   * @class gum::UndefinedIteratorValue agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : iterator does not point to any valid value
   */
  class UndefinedIteratorValue;

  /**
   * @class gum::UndefinedIteratorKey agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : iterator does not point to any valid key
   */
  class UndefinedIteratorKey;

  /**
   * @class gum::NullElement agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : a pointer or a reference on a nullptr (0) object
   */
  class NullElement;

  /**
   * @class gum::UndefinedElement agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : a looked-for element could not be found
   */
  class UndefinedElement;

  /**
   * @class gum::SizeError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : problem with size
   */
  class SizeError;


  /**
   * @class gum::InvalidArgumentsNumber agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception: the number of arguments passed to a function is not what was
   * expected
   */
  class InvalidArgumentsNumber;

  /**
   * @class gum::InvalidArgument agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception: at least one argument passed to a function is not what was
   * expected
   */
  class InvalidArgument;

  /**
   * @class gum::IOError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : input/output problem
   */
  class IOError;

  /**
   * @class gum::FormatNotFound agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : a I/O format was not found
   */
  class FormatNotFound;

  /**
   * @class gum::OperationNotAllowed agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : operation not allowed
   */
  class OperationNotAllowed;

  /**
   * @class gum::NotFound agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : the element we looked for cannot be found
   */
  class NotFound;

  /**
   * @class gum::ArgumentError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for argument error
   */
  class ArgumentError;

  /**
   * @class gum::OutOfBounds agrum/tools/core/exceptions.h
   * @extends gum::ArgumentError
   * Exception : out of bound
   */
  class OutOfBounds;

  /**
   * @class gum::DuplicateElement agrum/tools/core/exceptions.h
   * @extends gum::ArgumentError
   * Exception : a similar element already exists
   */
  class DuplicateElement;

  /**
   * @class gum::DuplicateLabel agrum/tools/core/exceptions.h
   * @extends gum::ArgumentError
   * Exception : a similar label already exists
   */
  class DuplicateLabel;

  ///////////////////////////////////
  /**
   * @class gum::GraphError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for graph error
   */
  class GraphError;

  /**
   * @class gum::NoNeighbour agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no neighbour to a given node was found
   */
  class NoNeighbour;

  /**
   * @class gum::NoParent agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no parent for a given node was found
   */
  class NoParent;

  /**
   * @class gum::NoChild agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : no child for a given node was found
   */
  class NoChild;

  /**
   * @class gum::InvalidEdge agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : there is something wrong with an edge
   */
  class InvalidEdge;

  /**
   * @class gum::InvalidArc agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : there is something wrong with an arc
   */
  class InvalidArc;

  /**
   * @class gum::InvalidNode agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : node does not exist
   */
  class InvalidNode;

  /**
   * @class gum::DefaultInLabel agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : default in label
   */
  class DefaultInLabel;

  /**
   * @class gum::InvalidDirectedCycle agrum/tools/core/exceptions.h
   * @extends gum::GraphError
   * Exception : existence of a directed cycle in a graph
   */
  class InvalidDirectedCycle;

  ///////////////////////////////////
  /**
   * @class gum::CPTError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for CPT error
   */
  class CPTError;

  ///////////////////////////////////
  /**
   * @class gum::ScheduleMultiDimError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for ScheduleMultiDim errors
   */
  class ScheduleMultiDimError;

  /**
   * @class gum::AbstractScheduleMultiDim agrum/tools/core/exceptions.h
   * @extends gum::ScheduleMultiDimError
   * Exception : The Schedule MultiDim Table is abstract
   */
  class AbstractScheduleMultiDim;

  /**
   * @class gum::UnknownScheduleMultiDim agrum/tools/core/exceptions.h
   * @extends gum::ScheduleMultiDimError
   * Exception : The Schedule MultiDim Table is unknown
   */
  class UnknownScheduleMultiDim;

  /**
   * @class gum::DuplicateScheduleMultiDim agrum/tools/core/exceptions.h
   * @extends gum::ScheduleMultiDimError
   * Exception : There exists another identical Schedule MultiDim Table
   */
  class DuplicateScheduleMultiDim;

  /**
   * @class gum::ScheduleOperationError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for ScheduleOperator errors
   */
  class ScheduleOperationError;

  /**
   * @class gum::UnknownScheduleOperation agrum/tools/core/exceptions.h
   * @extends gum::ScheduleOperationError
   * Exception : The Schedule Operation is unknown
   */
  class UnknownScheduleOperation;

  /**
   * @class gum::UnavailableScheduleOperation agrum/tools/core/exceptions.h
   * @extends gum::ScheduleOperationError
   * Exception : The Schedule Operation is not available yet
   */
  class UnavailableScheduleOperation;

  /**
   * @class gum::UnexecutedScheduleOperation agrum/tools/core/exceptions.h
   * @extends gum::ScheduleOperationError
   * Exception : The Schedule Operation has not been executed yet
   */
  class UnexecutedScheduleOperation;

  /**
   * @class gum::IncompatibleEvidence agrum/tools/core/exceptions.h
   * Exception : several evidence are incompatible together (proba=0)
   */
  class IncompatibleEvidence;

  /**
   * @class gum::FactoryError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception base for factory error
   */
  class FactoryError;

  /**
   * @class gum::FactoryInvalidState agrum/tools/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : invalid state error
   */
  class FactoryInvalidState;

  /**
   * @class gum::TypeError agrum/tools/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : wrong type for this operation
   */
  class TypeError;

  /**
   * @class gum::WrongClassElement agrum/tools/core/exceptions.h
   * @extends gum::FactoryError
   * Exception: wrong PRMClassElement for this operation
   */
  class WrongClassElement;

  /**
   * @class gum::PRMTypeError agrum/tools/core/exceptions.h
   * @extends gum::FactoryError
   * Exception : wrong subtype or subclass
   */
  class PRMTypeError;

  /**
   * @class gum::LearningError agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exceptions for learning
   */
  class LearningError;

  /**
   * @class gum::IncompatibleScorePrior agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: The score already contains a different 'implicit' prior.
   */
  class IncompatibleScorePrior;

  /**
   * @class gum::PossiblyIncompatibleScorePrior agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: Due to its weight, the prior is currently compatible with the
   *        score but if you change the weight, it will become incompatible"
   */
  class PossiblyIncompatibleScorePrior;

  /**
   * @class gum::DatabaseError agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: An unknown error occurred while accessing a database
   */
  class DatabaseError;

  /**
   * @class gum::MissingVariableInDatabase agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: A name of variable is not found in the database.
   */
  class MissingVariableInDatabase;

  /**
   * @class gum::UnknownLabelInDatabase agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: An unknown label is found in the database
   */
  class UnknownLabelInDatabase;

  /**
   * @class gum::MissingValueInDatabase agrum/tools/core/exceptions.h
   * @extends gum::LearningError
   * Error: The database contains some missing values
   */
  class MissingValueInDatabase;


  /**
   * @class gum::SyntaxError
   * @headerfile exceptions.h <agrum/tools/core/exceptions.h>
   * @extends gum::IOError
   * Special exception for syntax errors in files.
   */
  class SyntaxError;

  /**
   * @class gum::NotImplementedYet agrum/tools/core/exceptions.h
   * @extends gum::Exception
   * Exception : there is something wrong with an implementation
   */
  class NotImplementedYet;


#ifndef DOXYGEN_SHOULD_SKIP_THIS
  std::string _createMsg_(const std::string& filename,
                          const std::string& function,
                          int                line,
                          const std::string& msg);
  GUM_MAKE_ERROR(FatalError, Exception, "Fatal error")
  GUM_MAKE_ERROR(NotImplementedYet, Exception, "Not implemented yet")
  GUM_MAKE_ERROR(IteratorError, Exception, "Error in iterator")
  GUM_MAKE_ERROR(UndefinedIteratorValue, IteratorError, "Undefined iterator")
  GUM_MAKE_ERROR(UndefinedIteratorKey, IteratorError, "Undefined iterator's key")
  GUM_MAKE_ERROR(NullElement, Exception, "Null element")
  GUM_MAKE_ERROR(UndefinedElement, Exception, "Undefined element")
  GUM_MAKE_ERROR(SizeError, Exception, "incorrect size")
  GUM_MAKE_ERROR(ArgumentError, Exception, "Argument error")
  GUM_MAKE_ERROR(InvalidArgumentsNumber, ArgumentError, "Invalid argument number")
  GUM_MAKE_ERROR(InvalidArgument, ArgumentError, "Invalid argument")
  GUM_MAKE_ERROR(IOError, Exception, "I/O Error")
  GUM_MAKE_ERROR(FormatNotFound, IOError, "Format not found")
  GUM_MAKE_ERROR(OperationNotAllowed, Exception, "Operation not allowed")
  GUM_MAKE_ERROR(NotFound, Exception, "Object not found")
  GUM_MAKE_ERROR(OutOfBounds, ArgumentError, "Out of bound error")
  GUM_MAKE_ERROR(DuplicateElement, ArgumentError, "Duplicate element")
  GUM_MAKE_ERROR(DuplicateLabel, ArgumentError, "Duplicate label")
  GUM_MAKE_ERROR(GraphError, Exception, "Graph error")
  GUM_MAKE_ERROR(NoNeighbour, GraphError, "No neighbour found")
  GUM_MAKE_ERROR(NoParent, GraphError, "No parent found")
  GUM_MAKE_ERROR(NoChild, GraphError, "No child found")
  GUM_MAKE_ERROR(InvalidEdge, GraphError, "Invalid edge")
  GUM_MAKE_ERROR(InvalidArc, GraphError, "Invalid arc")
  GUM_MAKE_ERROR(InvalidNode, GraphError, "Invalid node")
  GUM_MAKE_ERROR(DefaultInLabel, GraphError, "Error on label")
  GUM_MAKE_ERROR(InvalidDirectedCycle, GraphError, "Directed cycle detected")
  GUM_MAKE_ERROR(InvalidPartiallyDirectedCycle, GraphError, "Partially directed cycle detected")
  GUM_MAKE_ERROR(CPTError, Exception, "CPT error")
  GUM_MAKE_ERROR(ScheduleMultiDimError, Exception, "ScheduleMultiDim error")
  GUM_MAKE_ERROR(AbstractScheduleMultiDim,
                 ScheduleMultiDimError,
                 "The Schedule MultiDim Table is abstract")
  GUM_MAKE_ERROR(UnknownScheduleMultiDim,
                 ScheduleMultiDimError,
                 "The Schedule MultiDim Table is unknown")
  GUM_MAKE_ERROR(DuplicateScheduleMultiDim,
                 ScheduleMultiDimError,
                 "There exists another identical Schedule MultiDim Table")
  GUM_MAKE_ERROR(ScheduleOperationError, Exception, "ScheduleOperator error")
  GUM_MAKE_ERROR(UnknownScheduleOperation,
                 ScheduleOperationError,
                 "The Schedule operation is unknown")
  GUM_MAKE_ERROR(UnavailableScheduleOperation,
                 ScheduleOperationError,
                 "The Schedule operation is not available yet")
  GUM_MAKE_ERROR(UnexecutedScheduleOperation,
                 ScheduleOperationError,
                 "The Schedule operation has not been executed yet")
  GUM_MAKE_ERROR(IncompatibleEvidence,
                 Exception,
                 "Several evidence/CPT are incompatible together (proba=0)")
  GUM_MAKE_ERROR(FactoryError, Exception, "Factory error")
  GUM_MAKE_ERROR(FactoryInvalidState, FactoryError, "Invalid state error")
  GUM_MAKE_ERROR(TypeError, FactoryError, "Wrong type")
  GUM_MAKE_ERROR(WrongClassElement, FactoryError, "Wrong ClassElement")
  GUM_MAKE_ERROR(PRMTypeError, FactoryError, "Wrong subtype or subclass")
  GUM_MAKE_ERROR(LearningError, Exception, "Factory error")
  GUM_MAKE_ERROR(IncompatibleScorePrior, LearningError, "Incompatible (maybe implicit) priors")
  GUM_MAKE_ERROR(PossiblyIncompatibleScorePrior,
                 LearningError,
                 "Possible incompatibility between score and prior")
  GUM_MAKE_ERROR(DatabaseError, LearningError, "Database error")
  GUM_MAKE_ERROR(MissingVariableInDatabase, LearningError, "Missing variable name in database")
  GUM_MAKE_ERROR(MissingValueInDatabase, LearningError, "The database contains some missing values")
  GUM_MAKE_ERROR(UnknownLabelInDatabase, LearningError, "Unknown label found in database")

  class SyntaxError: public IOError {
    protected:
    Size        noLine_;
    Size        noCol_;
    std::string filename_;

    public:
    SyntaxError(const std::string& aMsg,
                std::string        aFilename,
                Size               nol,
                Size               noc,
                const std::string& aType = "Syntax Error") :
        IOError(aMsg, aType),
        noLine_(nol), noCol_(noc), filename_(std::move(aFilename)) {
#  ifdef GUM_FOR_SWIG
      what_ = "[pyAgrum] " + msg_;
#  else    // GUM_FOR_SWIG
      std::ostringstream error_stream;
      error_stream << type_ << ":" << std::endl;
      error_stream << filename() << ":" << line() << "," << col() << " : " << msg_;
      what_ = error_stream.str();
#  endif   // GUM_FOR_SWIG
    }

    GUM_NODISCARD Size col() const { return noCol_; };
    GUM_NODISCARD Size line() const { return noLine_; };
    GUM_NODISCARD std::string filename() const { return filename_; };
  };
#endif   // DOXYGEN_SHOULD_SKIP_THIS
} /* namespace gum */

#endif /* GUM_EXCEPTIONS_H */

/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Errors container (at least) for parser.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_ERRORS_CONTAINERS_H
#define GUM_ERRORS_CONTAINERS_H

#include <fstream>
#include <iostream>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/core/cocoR/common.h>

namespace gum {

  /**
   * @class ParseError
   * @headerfile errorsContainer.h <agrum/tools/core/errorsContainer.h>
   * @brief This class is used to represent parsing errors for the different
   * parser implemented in aGrUM.
   * @ingroup basicstruct_group
   *
   */
  class ParseError {
    public:
    // ============================================================================
    /// @name Class constructors and destructors
    // ============================================================================
    /// @{

    /**
     * @brief Class constructor.
     * @param is_error True if this is an error.
     * @param msg The parsing error message.
     * @param line The line where the parsing error occured.
     */
    ParseError(bool is_error, const std::string& msg, Idx line);

    /**
     * @brief Class constructor.
     * @param is_error If false, then this ParseError is a warning.
     * @param msg The parsing error message.
     * @param filename The file where the parsing error occured.
     * @param line The line where the parsing error occured.
     * @param col The column where the parsing error occured.
     */
    ParseError(bool               is_error,
               const std::string& msg,
               const std::string& filename,
               Idx                line,
               Idx                col = 0);

    /**
     * @brief Class constructor.
     * @param is_error If false, then this ParseError is a warning.
     * @param msg The parsing error message.
     * @param filename The file where the parsing error occured.
     * @param code The code of the parsing error.
     * @param line The line where the parsing error occured.
     * @param col The column where the parsing error occured.
     */
    ParseError(bool               is_error,
               const std::string& msg,
               const std::string& filename,
               const std::string& code,
               Idx                line,
               Idx                col = 0);

    /**
     * @brief Copy constructor.
     * @param cont The gum::ParseError to copy.
     */
    ParseError(const ParseError& cont);

    /// @}
    // ============================================================================
    /// @name Class operator
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param cont The gum::ParseError to copy.
     * @return Returns this gum::ParseError.
     */
    ParseError operator=(const ParseError& cont);

    /// @}
    // ============================================================================
    /// @name Class operator
    // ============================================================================
    /// @{

    /// If false, this gum::ParseError is a warning.
    bool is_error;

    /// The line of this gum::ParseError.
    Idx line;

    /// The column of this gum::ParseError, default is 0.
    Idx column;

    /// The gum::ParseError message.
    std::string msg;

    /// The file of this gum::ParseError, default is "".
    std::string filename;

    /// The code of this gum::ParseError, default is "".
    mutable std::string code;   // default ""

    /**
     * @brief Return a std::string representation of this gum::ParseError.
     * @return Return a std::string representation of this gum::ParseError.
     */
    std::string toString() const;

    /**
     * @brief Return an elegant std::string representation of this
     * gum::ParseError.
     * @return Return an elegant std::string representation of this
     * gum::ParseError.
     */
    std::string toElegantString() const;

    /// @}
  };

  /**
   * @class ErrorsContainer
   * @headerfile errorsContainer.h <agrum/tools/core/errorsContainer.h>
   * @brief This class is used contain and manipulate gum::ParseError.
   * @ingroup basicstruct_group
   */
  class ErrorsContainer {
    /// The list of gum::ParseError contained in this gum::ErrorsContainer.
    mutable std::vector< ParseError > errors;

    public:
    /// Number of errors detected.
    Size error_count;

    // Number of warnings detected.
    Size warning_count;

    // ============================================================================
    /// @name Class Constructor
    // ============================================================================
    /// @{

    /**
     * @brief Class Constructor.
     */
    ErrorsContainer();

    /**
     * @brief Copy constructor.
     * @param cont The ErrorsContainer to copy.
     */
    ErrorsContainer(const ErrorsContainer& cont);

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Add an error object to the container.
     * @param error The gum::ParseError to add.
     */
    void add(ParseError error);

    /**
     * @brief Returns the i-th error.
     * @param i The error to return.
     * @return Returns the i-th error.
     * @throw OutOfBounds Raised if there is less than i errors.
     */
    ParseError error(Idx i) const;

    /**
     * @brief Returns the last added error.
     * @return Returns the last added error.
     * @throw OutOfBounds Raised if is no error to return.
     */
    ParseError last() const;

    /**
     * @brief Adds an error.
     * @param msg The error's message.
     * @param filename The error's file.
     * @param line The error's line.
     * @param col The error's column.
     */
    void addError(const std::string& msg, const std::string& filename, Idx line, Idx col);

    /**
     * @brief Adds a warning.
     * @param msg The warning's message.
     * @param filename The warning's file.
     * @param line The warning's line.
     * @param col The warning's column.
     */
    void addWarning(const std::string& msg, const std::string& filename, Idx line, Idx col);

    /**
     * @brief Add an exception.
     * @param msg The exception's message.
     * @param filename The exception's file.
     */
    void addException(const std::string& msg, const std::string& filename);

    /**
     * @brief Returns the number of errors and warnings.
     * @return Returns the number of errors and warnings.
     */
    Size count() const;

    /**
     * @brief Print errors on output stream.
     * @param o The output strem to send results.
     */
    void syntheticResults(std::ostream& o) const;

    /**
     * @brief Print errors on output stream.
     * @param o The output strem to send results.
     */
    void simpleErrors(std::ostream& o) const;

    /**
     * @brief Print errors on output stream.
     * @param o The output strem to send results.
     */
    void simpleErrorsAndWarnings(std::ostream& o) const;

    /**
     * @brief Print errors on output stream.
     * @param o The output strem to send results.
     */
    void elegantErrors(std::ostream& o) const;

    /**
     * @brief Print errors on output stream.
     * @param o The output strem to send results.
     */
    void elegantErrorsAndWarnings(std::ostream& o) const;

    /// @}
    // ============================================================================
    /// @name Coco/R helpers
    // ============================================================================
    /// @{

    /**
     * @brief For adding errors.
     * @param filename The error's file.
     * @param line The error's line.
     * @param col The error's column.
     * @param msg The error's message.
     */
    void Error(const std::wstring& filename, Idx line, Idx col, const wchar_t* msg);

    /**
     * @brief For adding warnings.
     * @param filename The warning's file.
     * @param line The warning's line.
     * @param col The warning's column.
     * @param msg The warning's message.
     */
    void Warning(const std::wstring& filename, Idx line, Idx col, const wchar_t* msg);

    /**
     * @brief For adding exceptions.
     * @param filename The exception's file.
     * @param msg The exception's message.
     */
    void Exception(const std::wstring& filename, const wchar_t* msg);

    /// @}
    // ============================================================================
    /// @name Class operators
    // ============================================================================
    /// @{

    /**
     * @brief Return the sum of two gum::ErrorsContainer.
     * @param cont The gum::ErrorsContainer to add.
     * @return Return the sum of two gum::ErrorsContainer.
     */
    ErrorsContainer operator+(const ErrorsContainer& cont) const;

    /**
     * @brief Copy Operator.
     * @param cont The gum::ErrorsContainer to copy.
     * @return Returns this gum::ErrorsContainer.
     */
    ErrorsContainer operator=(const ErrorsContainer& cont);

    /**
     * @brief Add the content of a gum::ErrorsContainer to this
     * gum::ErrorsContainer.
     * @param cont The gum::ErrorsContainer to add to this.
     * @return Returns this gum::ErrorsContainer.
     */
    ErrorsContainer operator+=(const ErrorsContainer& cont);

    /// @}

  };   // ErrorsContainer

}   // namespace gum

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/errorsContainer_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // GUM_ERRORS_CONTAINERS_H


/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
 * @brief Definition file for UAI exportation class
 *
 * Reads a markov net in UAI format
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES
 */

#ifndef UAI_MN_READER_H
#define UAI_MN_READER_H

#include <agrum/MN/MarkovNet.h>
#include <agrum/MN/io/MNReader.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#  undef COCO_PARSER_H__
#  undef COCO_SCANNER_H__
#  include <agrum/MN/io/UAI/cocoR/Parser.h>
#endif   // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS === */
  /* =========================================================================*/
  /**
   * @class UAIMNReader
   * @headerfile UAIMNReader.h <agrum/MN/io/UAI/UAIMNReader.h>
   * @ingroup bn_io
   * @brief Pure virtual class for reading a MN from a file.
   *
   * Every class used to read the content of a Markov Network from a stream,
   * or a file must be a subclass of UAIMNReader.
   */
  template < typename GUM_SCALAR >
  class UAIMNReader: public MNReader< GUM_SCALAR > {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the MN has to be built outside the reader. There is no
     * delegation to
     * create/destroy
     * the MN from inside the reader.
     */
    UAIMNReader(MarkovNet< GUM_SCALAR >* MN, const std::string& filename);

    /**
     * Default destructor.
     */
    ~UAIMNReader() final;

    /// Direct access to DSL scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    UAIMN::Scanner& scanner();

    /// name of read file
    const std::string& streamName() const;

    /// accessor to trace function (just write the number of parser line)
    bool trace() const;
    void trace(bool b);

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    Size proceed() final;

    void buildFromQuartets(
       std::vector< std::tuple< float, int, int, int > > quartets);

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors();
    /// # of errors
    Size warnings();

    /// line of ith error or warning
    Idx errLine(Idx i);
    /// col of ith error or warning
    Idx errCol(Idx i);
    /// type of ith error or warning
    bool errIsError(Idx i);
    /// message of ith error or warning
    std::string errMsg(Idx i);

    /// send on std::cerr the list of errorswith contents
    void showElegantErrors(std::ostream& o = std::cerr);

    /// send on std::cerr the list of errors or warnings with contents
    void showElegantErrorsAndWarnings(std::ostream& o = std::cerr);

    /// send on std::cerr the list of errors or warnings
    void showErrorsAndWarnings(std::ostream& o = std::cerr);

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream& o = std::cerr);
    /// @}

    protected:
    MarkovNet< GUM_SCALAR >* __mn;
    UAIMN::Scanner*          __scanner;
    UAIMN::Parser*           __parser;

    std::string __streamName;
    bool        __traceScanning;
    bool        __parseDone;

    // a boolean to throw the ioerror not in the constructor but in the
    // proceed()
    bool __ioerror;

    void __addFatalError(Idx                lig,
                         Idx                col,
                         const std::string& s);   // throw an exception
    void __addError(Idx lig, Idx col, const std::string& s);
    void __addWarning(Idx lig, Idx col, const std::string& s);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class UAIMNReader< double >;
#endif

} /* namespace gum */

#include "UAIMNReader_tpl.h"

#endif   // UAI_MN_READER_H
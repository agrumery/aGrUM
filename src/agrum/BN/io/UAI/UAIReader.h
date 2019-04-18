
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


#ifndef UAIREADER_H
#define UAIREADER_H

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>
#include <agrum/BN/io/BNReader.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#  undef COCO_PARSER_H__
#  undef COCO_SCANNER_H__
#  include <agrum/BN/io/UAI/cocoR/Parser.h>
#endif   // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS === */
  /* =========================================================================*/
  /**
   * @class UAIReader
   * @headerfile UAIReader.h <agrum/BN/io/UAI/UAIReader.h>
   * @ingroup bn_io
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian Network from a stream,
   * or a file must be a subclass of UAIReader.
   */
  template < typename GUM_SCALAR >
  class UAIReader : public BNReader< GUM_SCALAR > {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the BN has to be built outside the reader. There is no
     * delegation to
     * create/destroy
     * the BN from inside the reader.
     */
    UAIReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename);

    /**
     * Default destructor.
     */
    ~UAIReader() final;

    /// Direct access to DSL scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    UAI::Scanner& scanner();

    /// name of readen file
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
    BayesNet< GUM_SCALAR >*        __bn;
    BayesNetFactory< GUM_SCALAR >* __factory;
    UAI::Scanner*                  __scanner;
    UAI::Parser*                   __parser;

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
  extern template class UAIReader< double >;
#endif

} /* namespace gum */

#include "UAIReader_tpl.h"

#endif   // UAIREADER_H

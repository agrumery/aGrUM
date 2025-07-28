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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef NETREADER_H
#define NETREADER_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/io/BNReader.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#  undef _COCO_PARSER_H_
#  undef _COCO_SCANNER_H_
#  include <agrum/BN/io/net/cocoR/Parser.h>
#endif   // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS === */
  /* =========================================================================*/
  /**
   * @class NetReader
   * @headerfile netReader.h <agrum/BN/io/net/netReader.h>
   * @ingroup bn_io
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian network from a stream,
   * or a file must be a subclass of NetReader.
   */
  template < typename GUM_SCALAR >
  class NetReader: public BNReader< GUM_SCALAR > {
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
    NetReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename);

    /**
     * Default destructor.
     */
    ~NetReader();

    /// Direct access to DSL scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    net::Scanner& scanner();

    /// name of readen file
    const std::string& streamName() const;

    /// accessor to trace function (just write the number of parser line)
    bool trace() const;
    void trace(bool b);

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    Size proceed() final;

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
    BayesNet< GUM_SCALAR >*        _bn_;
    BayesNetFactory< GUM_SCALAR >* _factory_;
    net::Scanner*                  _scanner_;
    net::Parser*                   _parser_;

    std::string _streamName_;
    bool        _traceScanning_;
    bool        _parseDone_;

    // a boolean to throw the ioerror not in the constructor but in the
    // proceed()
    bool _ioerror_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class NetReader< double >;
#endif

} /* namespace gum */

#include "netReader_tpl.h"

#endif   // NETREADER_H

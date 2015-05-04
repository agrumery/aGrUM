/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
#ifndef NETREADER_H
#define NETREADER_H

#include <iostream>
#include <string>
#include <agrum/config.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BNReader.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#undef COCO_PARSER_H__
#undef COCO_SCANNER_H__
#include <agrum/BN/io/net/cocoR/Parser.h>
#endif // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /* ============================================================================ */
  /* ===                               READERS                                === */
  /* ============================================================================ */
  /**
   * @class NetReader
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian Network from a stream,
   * or a file must be a subclass of NetReader.
   */
  template <typename GUM_SCALAR>

  class NetReader : public BNReader<GUM_SCALAR> {

    public:
    /**
     * Constructor
    * A reader is defined for reading a defined file. Hence the 2 args of the
    * constructor.
    * Note that the BN has to be built outside the reader. There is no delegation to
    * create/destroy
    * the BN from inside the reader.
     */
    NetReader(BayesNet<GUM_SCALAR> *bn, const std::string &filename);

    /**
     * Default destructor.
     */
    ~NetReader();

    /// Direct access to DSL scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    net::Scanner &scanner();

    /// name of readen file
    const std::string &streamName() const;

    /// accessor to trace function (just write the number of parser line)
    bool trace(void) const;
    void trace(bool b);

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    int proceed(void);

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors();
    /// # of errors
    Size warnings();

    /// line of ith error or warning
    unsigned int errLine(unsigned int i);
    /// col of ith error or warning
    unsigned int errCol(unsigned int i);
    /// type of ith error or warning
    bool errIsError(unsigned int i);
    /// message of ith error or warning
    std::string errMsg(unsigned int i);

    /// send on std::cerr the list of errorswith contents
    void showElegantErrors(std::ostream &o = std::cerr);

    /// send on std::cerr the list of errors or warnings with contents
    void showElegantErrorsAndWarnings(std::ostream &o = std::cerr);

    /// send on std::cerr the list of errors or warnings
    void showErrorsAndWarnings(std::ostream &o = std::cerr);

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream &o = std::cerr);
    /// @}

    protected:
    BayesNet<GUM_SCALAR> *__bn;
    BayesNetFactory<GUM_SCALAR> *__factory;
    net::Scanner *__scanner;
    net::Parser *__parser;

    std::string __streamName;
    bool __traceScanning;
    bool __parseDone;

    // a boolean to throw the ioerror not in the constructor but in the proceed()
    bool __ioerror;
  };

  extern template class NetReader<float>;
  extern template class NetReader<double>;
} /* namespace gum */

#include "netReader.tcc"

#endif // NETREADER_H

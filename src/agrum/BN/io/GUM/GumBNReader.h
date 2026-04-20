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


#ifndef GUMREADER_H
#define GUMREADER_H
#include <agrum/base/core/errorsContainer.h>
#include <agrum/BN/io/BNReader.h>

namespace gum {
  /**
   * @class BNGumReader
   * @headerfile BNGumReader.h <agrum/BN/io/GUM/BNGumReader.h>
   * @ingroup bn_io
   * @brief Class for reading a Bayesian network from a GUM (json) file.
   *
   * This class is used to read a Bayesian network from a GUM file format.
   */
  template < GUM_Numeric GUM_SCALAR >
  class GumBNReader: public BNReader< GUM_SCALAR >, ErrorsContainer {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the BN has to be built outside the reader. There is no
     * delegation to create/destroy the BN from inside the reader.
     *
     * @param bn The BayesNet to fill with the read data.
     * @param filename The name of the file to read.
     * @param binary If true, the input is in binary format, otherwise in text format
     */
    GumBNReader(BayesNet< GUM_SCALAR >* bn, std::string_view filename, bool binary = false);

    /// Constructor for string-based parsing only (no file).
    /// Use exclusively with proceedFromString(); calling proceed() will fail.
    explicit GumBNReader(BayesNet< GUM_SCALAR >* bn);

    /**
     * Default destructor.
     */
    ~GumBNReader() override;

    /// Parse the file given at construction.
    /// @return the number of detected errors
    Size proceed() final;

    /// Parse a jgum JSON string directly (no file I/O).
    /// @return the number of detected errors
    Size proceedFromString(std::string_view content);

    void showElegantErrorsAndWarnings(std::ostream& stream = std::cerr) const;

    void showErrorCounts(std::ostream& stream = std::cerr) const;


    private:
    // Forward declaration: json is defined in the _tpl.h translation unit.
    template < typename JsonType >
    Size _proceedFromJson_(const JsonType& content);

    BayesNet< GUM_SCALAR >* _bn_;
    std::string             _streamName_;
    bool                    _parseDone_;
    bool _binary_;   ///< If true, the input is in binary format, otherwise in text format.
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GumBNReader< double >;
#endif
} /* namespace gum */

#include <agrum/BN/io/GUM/GumBNReader_tpl.h>

#endif   // GUM_BN_READER_H

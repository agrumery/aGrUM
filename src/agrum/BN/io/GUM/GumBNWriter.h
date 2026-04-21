/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Definition of classe for GUM (json) file output manipulation
 *
 * This class servers to write the content of a Bayesian network in
 * the gum (json) format.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BN_WRITER_H
#define GUM_BN_WRITER_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/io/BNWriter.h>

namespace gum {
  /**
   * @class GumBNWriter
   * @headerfile GumBNWriter.h <agrum/BN/io/GUM/GumBNWriter.h>
   * @brief Writes a IBayesNet in the GUM json format.
   * @ingroup bn_io
   *
   * This class to write the content of a Bayesian network in
   * the GUM json format.
   *
   */
  template < GUM_Numeric GUM_SCALAR >
  class GumBNWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     *
     * JGUM format is a json format. In binary mode, the output use the BJData format.
     *
     * @param binary If true, the output will be in binary format, otherwise in text format.
     * @param indent The indentation level for the output in text format. If -1, the format is as
     * compact as possible.
     */
    GumBNWriter(bool binary = false, int indent = -1);

    /**
     * Destructor.
     */
    ~GumBNWriter() override;

    GumBNWriter(const GumBNWriter&)                = delete;
    GumBNWriter(GumBNWriter&&) noexcept            = delete;
    GumBNWriter& operator=(const GumBNWriter&)     = delete;
    GumBNWriter& operator=(GumBNWriter&&) noexcept = delete;

    /// @}

    protected:
    /**
     * Writes a Bayesian network in the output stream using the BN format.
     *
     * @param output The output stream.
     * @param bn The Bayesian network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes a Bayesian network in the referenced file using the BN format.
     * If the files doesn't exists, it is created.
     *
     * @param filePath The path to the file used to write the Bayesian network.
     * @param bn The Bayesian network writed in the file.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(std::string_view filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    bool _binary_;   ///< If true, the output will be in binary format, otherwise in text format.
    int  _indent_;   ///< The indentation level for the output.
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GumBNWriter< double >;
#endif
} /* namespace gum */

#include <agrum/BN/io/GUM/GumBNWriter_tpl.h>

#endif /* GUM_BN_WRITER_H */

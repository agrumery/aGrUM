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


/**
 * @file
 * @brief Definition of classe for BN file output manipulation
 *
 * This class servers to write the content of a Bayesian network in
 * the BN format.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_CNF_WRITER_H
#define GUM_CNF_WRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/io/BNWriter.h>

// Ajout sur branche DDE pour pouvoir compiler (à retirer très probablement)
// problème sur template <typename> class IApproximationPolicy = ExactPolicy
// sinon
// (reconnaissance de ExactPolicy?)
#include <agrum/base/core/approximations/exactPolicy.h>

namespace gum {

  /**
   * @class CNFWriter
   * @headerfile CNFWriter.h <agrum/BN/io/cnf/CNFWriter.h>
   * @ingroup bn_io
   * @brief Writes a IBayesNet in the BN format.
   *
   * This class servers to write the content of a Bayesian network in
   * the BN format. See
   * TODO
   * for information on this format.
   *
   */
  template < typename GUM_SCALAR, template < typename > class IApproximationPolicy = ExactPolicy >
  class CNFWriter: public BNWriter< GUM_SCALAR >, public IApproximationPolicy< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    CNFWriter();

    /**
     * Destructor.
     */
    ~CNFWriter() override;

    CNFWriter(const CNFWriter&)                = default;
    CNFWriter(CNFWriter&&) noexcept            = default;
    CNFWriter& operator=(const CNFWriter&)     = default;
    CNFWriter& operator=(CNFWriter&&) noexcept = default;

    /// @}

    protected:
    /**
     * Writes a Bayesian network in the output stream using the BN format.
     *
     * @param output The output stream.
     * @param bn The Bayesian network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) override = 0;

    /**
     * Writes a Bayesian network in the referenced file using the BN format.
     * If the files doesn't exists, it is created.
     *
     * @param filePath The path to the file used to write the Bayesian network.
     * @param bn The Bayesian network writed in the file.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) override = 0;

    inline GUM_SCALAR fromExact(const GUM_SCALAR& value) const override {
      return IApproximationPolicy< GUM_SCALAR >::fromExact(value);
    }
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class CNFWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/cnf/CNFWriter_tpl.h>
#endif   // GUM_NET_WRITER_H

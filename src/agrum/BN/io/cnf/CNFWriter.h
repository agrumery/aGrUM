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

#include <agrum/BN/io/BNWriter.h>
#include <agrum/agrum.h>

// Ajout sur branche DDE pour pouvoir compiler (à retirer très probablement)
// problème sur template <typename> class IApproximationPolicy = ExactPolicy
// sinon
// (reconnaissance de ExactPolicy?)
#include <agrum/tools/core/approximations/exactPolicy.h>

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

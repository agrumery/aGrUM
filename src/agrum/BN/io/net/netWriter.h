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

#ifndef GUM_NET_WRITER_H
#define GUM_NET_WRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>

#include <agrum/agrum.h>

namespace gum {

  /**
   * @class NetWriter
   * @headerfile netWriter.h <agrum/BN/io/net/netWriter.h>
   * @brief Writes a IBayesNet in the BN format.
   * @ingroup bn_io
   *
   * This class servers to write the content of a Bayesian network in
   * the BN format. See
   * TODO
   * for information on this format.
   *
   */
  template < typename GUM_SCALAR >
  class NetWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    NetWriter();

    /**
     * Destructor.
     */
    ~NetWriter() override;

    NetWriter(const NetWriter&)                = default;
    NetWriter(NetWriter&&) noexcept            = default;
    NetWriter& operator=(const NetWriter&)     = default;
    NetWriter& operator=(NetWriter&&) noexcept = default;

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
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    // Returns the header of the BN file.
    std::string _header_(const IBayesNet< GUM_SCALAR >& bn);

    // Returns a bloc defining a variable in the BN format.
    std::string _variableBloc_(const DiscreteVariable& var);

    // Returns a bloc defining a variable's CPT in the BN format.
    std::string _variableCPT_(const Potential< GUM_SCALAR >& cpt);

    // Returns the modalities labels of the variables in varsSeq
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class NetWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/net/netWriter_tpl.h>
#endif   // GUM_NET_WRITER_H

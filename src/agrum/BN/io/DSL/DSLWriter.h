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


#ifndef DSLWRITER_H
#define DSLWRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>
#include <agrum/agrum.h>

namespace gum {

  /**
   * @class DSLWriter
   * @headerfile DSLWriter.h <agrum/BN/io/DSL/DSLWriter.h>
   * @ingroup bn_io
   * @brief Writes a IBayesNet in the DSL format.
   *
   * This class servers to write the content of a Bayesian network in
   * the DSL format. See
   * http://www.cs.cmu.edu/~fgcozman/Research/InterchangeFormat/Old/xmlDSL02.html
   * for information on this format.
   *
   */
  template < typename GUM_SCALAR >
  class DSLWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    DSLWriter();

    /**
     * Destructor.
     */
    ~DSLWriter() override;

    DSLWriter(const DSLWriter&)                = default;
    DSLWriter(DSLWriter&&) noexcept            = default;
    DSLWriter& operator=(const DSLWriter&)     = default;
    DSLWriter& operator=(DSLWriter&&) noexcept = default;

    /// @}
    protected:
    /**
     * Writes a Bayesian network in the output stream using the DSL format.
     *
     * @param output The output stream.
     * @param bn The Bayesian network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes a Bayesian network in the referenced file using the DSL format.
     * If the files doesn't exists, it is created.
     *
     * @param filePath The path to the file used to write the Bayesian network.
     * @param bn The Bayesian network writed in the file.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Check whether the BN is syntactically correct for BIF format.
     *
     * @throw FatalError if found.
     */
    void _syntacticalCheck(const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    // Returns a bloc defining a variable in the DSL format.
    std::string _variableBloc_(const IBayesNet< GUM_SCALAR >& bn, const DiscreteVariable& var);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class gum::DSLWriter< double >;
#endif

} /* namespace gum */

#include "DSLWriter_tpl.h"

#endif   // DSLWRITER_H

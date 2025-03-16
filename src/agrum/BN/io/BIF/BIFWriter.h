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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/**
 * @file
 * @brief Definition of class for BIF file output manipulation
 *
 * This class servers to write the content of a Bayesian network in
 * the BIF format.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BIF_WRITER_H
#define GUM_BIF_WRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/io/BNWriter.h>

namespace gum {

  /**
   * @class BIFWriter
   * @headerfile BIFWriter.h <agrum/BN/io/BIF/BIFWriter.h>
   * @ingroup bn_io
   * @brief Writes a IBayesNet in the BIF format.
   *
   * This class servers to write the content of a Bayesian network in
   * the BIF format. See
   * http://www.cs.cmu.edu/~fgcozman/Research/InterchangeFormat/Old/xmlbif02.html
   * for information on this format.
   *
   */
  template < typename GUM_SCALAR >
  class BIFWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    BIFWriter();

    /**
     * Destructor.
     */
    ~BIFWriter() override;

    BIFWriter(const BIFWriter&)                = default;
    BIFWriter(BIFWriter&&) noexcept            = default;
    BIFWriter& operator=(const BIFWriter&)     = default;
    BIFWriter& operator=(BIFWriter&&) noexcept = default;

    /// @}

    protected:
    /**
     * Writes a Bayesian network in the output stream using the BIF format.
     *
     * @param output The output stream.
     * @param bn The Bayesian network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes a Bayesian network in the referenced file using the BIF format.
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
    // Returns the header of the BIF file.
    std::string _header_(const IBayesNet< GUM_SCALAR >& bn);

    // Returns a bloc defining a variable in the BIF format.
    std::string _variableBloc_(const DiscreteVariable& var);

    // Returns a bloc defining a variable's CPT in the BIF format.
    std::string _variableCPT_(const Tensor< GUM_SCALAR >& cpt);

    // Returns the modalities labels of the variables in varsSeq
    std::string _variablesLabels_(const Sequence< const DiscreteVariable* >& varsSeq,
                                  const Instantiation&                       inst);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BIFWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/BIF/BIFWriter_tpl.h>
#endif   // GUM_BIF_WRITER_H

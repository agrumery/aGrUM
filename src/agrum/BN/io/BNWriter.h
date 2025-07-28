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
 * @brief Definition of abstract classes for file output manipulation
 * of Bayesian networks.
 *
 * Every classe used to read or write a BN from a file, must inherit from
 * BNWriter or BNReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_BN_WRITER_H
#define GUM_BN_WRITER_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/IBayesNet.h>

namespace gum {

  /* =========================================================================*/
  /* ===                              WRITERS === */
  /* =========================================================================*/
  /**
   * @class BNWriter
   * @headerfile BNWriter.h <agrum/BN/io/BNWriter.h>
   * @brief virtual class for writing a BN to a file.
   * @ingroup bn_io
   *
   * Every class used to write the content of a Bayesian network in a stream, or
   * a file must be a subclass of BNWriter.
   */
  template < typename GUM_SCALAR >
  class BNWriter {
    public:
    /**
     * Default constructor.
     */
    explicit BNWriter();

    /**
     * Default destructor.
     */
    virtual ~BNWriter();

    BNWriter(const BNWriter&)                = default;
    BNWriter(BNWriter&&) noexcept            = default;
    BNWriter& operator=(const BNWriter&)     = default;
    BNWriter& operator=(BNWriter&&) noexcept = default;


    /**
     * Writes a Bayesian network in the output stream.
     *
     * - If the bn can not be written for syntactical reasons, throw an error or try to correct it
     * (conditioned by the value of _allowModification_
     *
     * @param output The output stream.
     * @param bn The Bayesian network written in output.
     * @throws IOError Raised if an I/O error occurs.
     * @throws SyntaxError Raised if syntactical errors in the BN
     */
    void write(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn);

    /**
     * Writes a Bayesian network in the file referenced by filePath.
     *
     * - If the file doesn't exists, it is created.
     * - If the file exists, it's content will be erased.
     * - If the bn can not be written for syntactical reasons, throw an error or try to correct it
     * (conditioned by the value of _allowModification_
     *
     *
     * @param filePath The path to the file used to write the Bayesian network.
     * @param bn The Bayesian network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void write(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn);

    GUM_NODISCARD bool isModificationAllowed() const;
    void               setAllowModification(bool am);

    protected:
    /**
     * Writes a Bayesian network in the output stream.
     *
     * @param output The output stream.
     * @param bn The Bayesian network written in output.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) = 0;

    /**
     * Writes a Bayesian network in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Bayesian network.
     * @param bn The Bayesian network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) = 0;

    ///@{

    /**
     * Check whether the BN is syntactically correct for BIF format.
     *
     * @throw FatalError if found.
     */
    virtual void _syntacticalCheck(const IBayesNet< GUM_SCALAR >& bn);

    void        _validCharInNamesCheck(const IBayesNet< GUM_SCALAR >& bn);
    std::string _onlyValidCharsInName(const std::string& name);
    std::string _buildNameWithOnlyValidChars(const std::string& name);

    ///}

    private:
    bool _allowModification_ = false;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BNWriter< double >;
#endif
} /* namespace gum */

#include <agrum/BN/io/BNWriter_tpl.h>

#endif   // GUM_BN_WRITER_H

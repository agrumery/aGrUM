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
 * @brief Definition file for BIF XML exportation class
 *
 * Writes an bayes net in XML files with BIF format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_BIF_XML_BN_WRITER_H
#define GUM_BIF_XML_BN_WRITER_H

#define TIXML_USE_TICPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/io/BNWriter.h>

namespace gum {

  /**
   * @class BIFXMLBNWriter BIFXMLBNWriter.h
   *<agrum/BN/io/BIFXML/BIFXMLBNWriter.h>
   * @ingroup bn_io
   * @brief Writes an bayes net in a XML file with BIF format
   *
   * This class export a bayes net into an XML files, using BIF format
   * See
   *http://www-2.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/
   * for information about this format.
   *
   */
  template < typename GUM_SCALAR >
  class BIFXMLBNWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    BIFXMLBNWriter();

    /**
     * Destructor.
     */
    ~BIFXMLBNWriter() override;

    BIFXMLBNWriter(const BIFXMLBNWriter&)                = default;
    BIFXMLBNWriter(BIFXMLBNWriter&&) noexcept            = default;
    BIFXMLBNWriter& operator=(const BIFXMLBNWriter&)     = default;
    BIFXMLBNWriter& operator=(BIFXMLBNWriter&&) noexcept = default;

    /// @}

    protected:
    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param infdiag The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param infdiag The bayes net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string _heading_(const IBayesNet< GUM_SCALAR >& bn);

    /**
     * Returns the end of the BIF file.
     */
    std::string _documentend_();

    /**
     * Returns a bloc defining a variable in the BIF format.
     */
    std::string _variableBloc_(const DiscreteVariable& var);

    /**
     * Returns a bloc defining a variable's table (if she has) in the BIF
     * format.
     */
    std::string _variableDefinition_(const NodeId& varNodeId, const IBayesNet< GUM_SCALAR >& bn);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BIFXMLBNWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/BIFXML/BIFXMLBNWriter_tpl.h>
#endif   // GUM_BIF_XML_BN_WRITER_H

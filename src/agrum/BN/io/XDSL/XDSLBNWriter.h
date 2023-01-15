/**
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Definition file for XDSL XML exportation class
 *
 * Writes an bayes net in XML files with XDSL format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_XDSL_BN_WRITER_H
#define GUM_XDSL_BN_WRITER_H

#define TIXML_USE_TICPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>
#include <agrum/agrum.h>

namespace gum {

  /**
   * @class XDSLBNWriter XDSLBNWriter.h
   *<agrum/BN/io/XDSLXML/XDSLBNWriter.h>
   * @ingroup bn_io
   * @brief Writes an bayes net in a XML file with XDSL format
   *
   * This class export a bayes net into an XML files, using XDSL format
   *
   */
  template < typename GUM_SCALAR >
  class XDSLBNWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    XDSLBNWriter();

    /**
     * Destructor.
     */
    ~XDSLBNWriter() override;

    XDSLBNWriter(const XDSLBNWriter&)                = default;
    XDSLBNWriter(XDSLBNWriter&&) noexcept            = default;
    XDSLBNWriter& operator=(const XDSLBNWriter&)     = default;
    XDSLBNWriter& operator=(XDSLBNWriter&&) noexcept = default;

    /// @}
    protected:
    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param bn The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param bn The bayes net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    /**
     * send to output the header of the XDSL file.
     */
    void _heading_(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn);

    /**
     * send to output the end of the XDSL file.
     */
    void _documentEnd_(std::ostream& output);

    void _headingExtension_(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn);
    void _endExtension_(std::ostream& output);

    /**
     * send to output a bloc defining a variable in the XDSL format.
     */
    void _variableBloc_(std::ostream&                  output,
                        const NodeId&                  varNodeId,
                        const IBayesNet< GUM_SCALAR >& bn);

    /**
     * send to output a bloc defining a variable's extensoion in the XDSL
     */
    void _variableExtension_(std::ostream&                  output,
                             const NodeId&                  varNodeId,
                             const IBayesNet< GUM_SCALAR >& bn);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class XDSLBNWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/XDSL/XDSLBNWriter_tpl.h>
#endif   // GUM_XDSL_BN_WRITER_H

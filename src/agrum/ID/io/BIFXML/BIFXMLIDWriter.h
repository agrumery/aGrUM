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
 * @brief Definition file for BIF XML exportation class
 *
 * Writes an influence diagram in XML files with BIF format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_BIF_XML_ID_WRITER_H
#define GUM_BIF_XML_ID_WRITER_H

#define TIXML_USE_TICPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/ID/io/IDWriter.h>
#include <agrum/agrum.h>

namespace gum {

  /**
   * @class BIFXMLIDWriter
   * @brief Writes an influence diagram in a XML files with BIF format
   * @ingroup id_group
   *
   * This class export an influence diagram into an XML files, using BIF format
   * See
   *http://www-2.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/
   * for information about this format.
   *
   */
  template < typename GUM_SCALAR >

  class BIFXMLIDWriter: public IDWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    BIFXMLIDWriter();

    /**
     * Destructor.
     */
    virtual ~BIFXMLIDWriter();

    /// @}

    /**
     * Writes an influence diagram in the given ouput stream.
     *
     * @param output The output stream.
     * @param infdiag The influence diagram writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write(std::ostream& output, const InfluenceDiagram< GUM_SCALAR >& infdiag);

    /**
     * Writes an Influence Diagram in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Influence Diagram.
     * @param infdiag The Influence Diagram writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(std::string filePath, const InfluenceDiagram< GUM_SCALAR >& infdiag);

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string _heading_();

    /**
     * Returns the end of the BIF file.
     */
    std::string _documentend_();

    /**
     * Returns a bloc defining a variable in the BIF format.
     */
    std::string _variableBloc_(const DiscreteVariable& var, int nodeType);

    /**
     * Returns a bloc defining a variable's table (if she has) in the BIF
     * format.
     */
    std::string _variableDefinition_(const NodeId&                         varNodeId,
                                     const InfluenceDiagram< GUM_SCALAR >& infdiag);
  };
} /* namespace gum */

#include <agrum/ID/io/BIFXML/BIFXMLIDWriter_tpl.h>
#endif   // GUM_BIF_XML_ID_WRITER_H

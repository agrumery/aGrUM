/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Definition file for BIF XML exportation class
 *
 * Writes an bayes net in XML files with BIF format
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BIF_XML_BN_WRITER_H
#define GUM_BIF_XML_BN_WRITER_H

#define TIXML_USE_TICPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/config.h>
#include <agrum/BN/io/BNWriter.h>

namespace gum {

  /**
   * @class BIFXMLBNWriter
   * @brief Writes an bayes net in a XML file with BIF format
   * @ingroup bn_group
   *
   * This class export a bayes net into an XML files, using BIF format
   * See http://www-2.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/
   * for information about this format.
   *
   */
  template <typename GUM_SCALAR> class BIFXMLBNWriter : public BNWriter<GUM_SCALAR> {
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
    virtual ~BIFXMLBNWriter();

    /// @}

    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param infdiag The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write(std::ostream &output, const IBayesNet<GUM_SCALAR> &infdiag);

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param infdiag The bayes net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(std::string filePath, const IBayesNet<GUM_SCALAR> &infdiag);

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string __heading(const IBayesNet<GUM_SCALAR> &bn);

    /**
     * Returns the end of the BIF file.
     */
    std::string __documentend();

    /**
     * Returns a bloc defining a variable in the BIF format.
     */
    std::string __variableBloc(const DiscreteVariable &var);

    /**
     * Returns a bloc defining a variable's table (if she has) in the BIF format.
     */
    std::string __variableDefinition(const NodeId &varNodeId,
                                     const IBayesNet<GUM_SCALAR> &infdiag);
  };

  extern template class BIFXMLBNWriter<float>;
  extern template class BIFXMLBNWriter<double>;
} /* namespace gum */

#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.tcc>
#endif // GUM_BIF_XML_BN_WRITER_H

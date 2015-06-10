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
 * @brief Definition of classe for BN file output manipulation
 *
 * This class servers to write the content of a Bayesian Network in
 * the BN format.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_NET_WRITER_H
#define GUM_NET_WRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>

#include <agrum/config.h>

namespace gum {

  /**
   * @class BNWriter
   * @brief Writes a IBayesNet in the BN format.
   * @ingroup bn_group
   *
   * This class servers to write the content of a Bayesian Network in
   * the BN format. See
   * TODO
   * for information on this format.
   *
   */
  template <typename GUM_SCALAR>

  class NetWriter : public BNWriter<GUM_SCALAR> {
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
    virtual ~NetWriter();

    /// @}

    /**
     * Writes a Bayesian Network in the output stream using the BN format.
     *
     * @param output The output stream.
     * @param bn The Bayesian Network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    virtual void write(std::ostream &output, const IBayesNet<GUM_SCALAR> &bn);

    /**
     * Writes a Bayesian Network in the referenced file using the BN format.
     * If the files doesn't exists, it is created.
     *
     * @param filePath The path to the file used to write the Bayesian Network.
     * @param bn The Bayesian Network writed in the file.
     * @throws IOError Raised if and I/O error occurs.
     */
    virtual void write(std::string filePath, const IBayesNet<GUM_SCALAR> &bn);

    private:
    // Returns the header of the BN file.
    std::string __header(const IBayesNet<GUM_SCALAR> &bn);

    // Returns a bloc defining a variable in the BN format.
    std::string __variableBloc(const DiscreteVariable &var);

    // Returns a bloc defining a variable's CPT in the BN format.
    std::string __variableCPT(const Potential<GUM_SCALAR> &cpt);

    // Returns the modalities labels of the variables in varsSeq
  };

  extern template class NetWriter<float>;
  extern template class NetWriter<double>;
} /* namespace gum */

#include <agrum/BN/io/net/netWriter.tcc>
#endif // GUM_NET_WRITER_H

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
#ifndef DSLWRITER_H
#define DSLWRITER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>
#include <agrum/config.h>

namespace gum {

  /**
     * @class DSLWriter
     * @brief Writes a IBayesNet in the DSL format.
     * @ingroup bn_group
     *
     * This class servers to write the content of a Bayesian Network in
     * the DSL format. See
     * http://www.cs.cmu.edu/~fgcozman/Research/InterchangeFormat/Old/xmlDSL02.html
     * for information on this format.
     *
  */
  template <typename GUM_SCALAR> class DSLWriter : public BNWriter<GUM_SCALAR> {

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
    virtual ~DSLWriter();

    /// @}

    /**
    * Writes a Bayesian Network in the output stream using the DSL format.
    *
    * @param output The output stream.
    * @param bn The Bayesian Network writen in output.
    * @throws IOError Raised if and I/O error occurs.
    */
    virtual void write(std::ostream &output, const IBayesNet<GUM_SCALAR> &bn);

    /**
    * Writes a Bayesian Network in the referenced file using the DSL format.
    * If the files doesn't exists, it is created.
    *
    * @param filePath The path to the file used to write the Bayesian Network.
    * @param bn The Bayesian Network writed in the file.
    * @throws IOError Raised if and I/O error occurs.
    */
    virtual void write(std::string filePath, const IBayesNet<GUM_SCALAR> &bn);

    private:
    // Returns a bloc defining a variable in the DSL format.
    std::string __variableBloc(const IBayesNet<GUM_SCALAR> &bn,
                               const DiscreteVariable &var);
  };

  extern template class gum::DSLWriter<float>;
  extern template class gum::DSLWriter<double>;
} /* namespace gum */

#include "DSLWriter.tcc"

#endif // DSLWRITER_H

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
 * @brief Definition of abstract classes for file input  manipulation
 * of Bayesian Networks.
 *
 * Every classe used to read a BN from a file, must inherit from
 * BNWriter or BNReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BN_READER_H
#define GUM_BN_READER_H

#include <iostream>
#include <string>
#include <agrum/config.h>
#include <agrum/BN/BayesNet.h>

namespace gum {
  /* ============================================================================ */
  /* ===                               READERS                                === */
  /* ============================================================================ */
  /**
   * @class BNReader
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian Network from a stream,
   * or a file must be a subclass of BNReader.
   */
  template <typename GUM_SCALAR> class BNReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the BN has to be built outside the reader. There is no delegation to
     * create/destroy
     * the BN from inside the reader.
     */
    BNReader(BayesNet<GUM_SCALAR> *bn, const std::string &filename);

    /**
     * Default destructor.
     */
    virtual ~BNReader();

    /**
     * Reads a Bayesian Network from the file referenced by filePath into`
     * parameter bayesNet.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual int proceed() = 0;
  };

  extern template class BNReader<float>;
  extern template class BNReader<double>;
} /* namespace gum */

#include <agrum/BN/io/BNReader.tcc>

#endif // GUM_BN_READER_H

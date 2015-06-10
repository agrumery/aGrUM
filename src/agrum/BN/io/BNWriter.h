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
 * @brief Definition of abstract classes for file output manipulation
 * of Bayesian Networks.
 *
 * Every classe used to read or write a BN from a file, must inherit from
 * BNWriter or BNReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BN_WRITER_H
#define GUM_BN_WRITER_H

#include <iostream>
#include <string>
#include <agrum/config.h>
#include <agrum/BN/IBayesNet.h>

namespace gum {

  /* ============================================================================ */
  /* ===                              WRITTERS                                === */
  /* ============================================================================ */
  /**
   * @class BNWriter
   * @brief Pure virtual class for writting a BN to a file.
   * @ingroup bn_group
   *
   * Every class used to write the content of a Bayesian Network in a stream, or
   * a file must be a subclass of BNWriter.
   */
  template <typename GUM_SCALAR>

  class BNWriter {
    public:
    /**
     * Default constructor.
     */
    BNWriter();

    /**
     * Default destructor.
     */
    virtual ~BNWriter();

    /**
     * Writes a Bayesian Network in the ouput stream.
     *
     * @param output The output stream.
     * @param bn The Bayesian Network writed in output.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write(std::ostream &output, const IBayesNet<GUM_SCALAR> &bn) = 0;

    /**
     * Writes a Bayesian Network in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Bayesian Network.
     * @param bn The Bayesian Network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(std::string filePath, const IBayesNet<GUM_SCALAR> &bn) = 0;
  };

  extern template class BNWriter<float>;
  extern template class BNWriter<double>;
} /* namespace gum */

#include <agrum/BN/io/BNWriter.tcc>

#endif // GUM_BN_WRITER_H

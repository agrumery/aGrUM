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
 * @brief Definition of abstract classes for file input / output manipulation
 * of Bayesian Networks.
 *
 * Every classe used to read or write a BN from a file, must inherit from
 * BNWriter or BNReader.
 *
 * @author Lionel Torti
 */
#ifndef GUM_BN_IO_H
#define GUM_BN_IO_H

#include <iostream>
#include <string>
#include <agrum/core/debug.h>
#include <agrum/BN/BayesNet.h>


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
  template<typename T_DATA>

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
     * @param ouput The output stream.
     * @param bn The Bayesian Network writed in output.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write( std::ostream &output, const BayesNet<T_DATA>& bn ) =0;

    /**
     * Writes a Bayesian Network in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Bayesian Network.
     * @param bn The Bayesian Network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write( std::string filePath, const BayesNet<T_DATA>& bn ) =0;
  };


  
  /* ============================================================================ */
  /* ===                               READERS                                === */
  /* ============================================================================ */
  /**
   * @class
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian Network from a stream,
   * or a file must be a subclass of BNReader.
   */
  class BNReader {
  public:
    /**
     * Default constructor.
     */
    BNReader();

    /**
     * Default destructor.
     */
    virtual ~BNReader();

    /**
     * Reads a Bayesian Network from the file referenced by filePath.
     * @return Returns the Bayesian Network represented in filePath.
     */
    virtual BayesNet<float>* read( std::string filePath ) =0;
  };


} /* namespace gum */


#include <agrum/BN/io/BNIO.tcc>

#endif // GUM_BN_IO_H


/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

#include <agrum/BN/IBayesNet.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

namespace gum {

  /* =========================================================================*/
  /* ===                              WRITTERS === */
  /* =========================================================================*/
  /**
   * @class BNWriter
   * @headerfile BNWriter.h <agrum/BN/io/BNWriter.h>
   * @brief Pure virtual class for writting a BN to a file.
   * @ingroup bn_io
   *
   * Every class used to write the content of a Bayesian Network in a stream, or
   * a file must be a subclass of BNWriter.
   */
  template < typename GUM_SCALAR >

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
    virtual void write(std::ostream&                  output,
                       const IBayesNet< GUM_SCALAR >& bn) = 0;

    /**
     * Writes a Bayesian Network in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Bayesian Network.
     * @param bn The Bayesian Network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(const std::string&             filePath,
                       const IBayesNet< GUM_SCALAR >& bn) = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BNWriter< double >;
#endif
} /* namespace gum */

#include <agrum/BN/io/BNWriter_tpl.h>

#endif   // GUM_BN_WRITER_H

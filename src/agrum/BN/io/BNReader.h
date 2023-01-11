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
 * @brief Definition of abstract classes for file input  manipulation
 * of Bayesian networks.
 *
 * Every classe used to read a BN from a file, must inherit from
 * BNWriter or BNReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_BN_READER_H
#define GUM_BN_READER_H

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS                            === */
  /* =========================================================================*/
  /**
   * @class BNReader
   * @headerfile BNReader.h <agrum/BN/io/BNReader.h>
   * @ingroup bn_io
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian network from a stream,
   * or a file must be a subclass of BNReader.
   */
  template < typename GUM_SCALAR >
  class BNReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the BN has to be built outside the reader. There is no
     * delegation to create/destroy the BN from inside the reader.
     */
    BNReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename);

    /**
     * Default destructor.
     */
    virtual ~BNReader();

    /**
     * Reads a Bayesian network from the file referenced by filePath into`
     * parameter bayesNet.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual Size proceed() = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BNReader< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/BNReader_tpl.h>

#endif   // GUM_BN_READER_H

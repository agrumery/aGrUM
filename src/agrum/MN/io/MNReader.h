
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * of Markov Networks.
 *
 * Every classe used to read a MN from a file, must inherit from
 * MNWriter or MNReader.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES
 */
#ifndef GUM_MN_READER_H
#define GUM_MN_READER_H

#include <agrum/MN/MarkovNet.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS                            === */
  /* =========================================================================*/
  /**
   * @class MNReader
   * @headerfile MNReader.h <agrum/MN/io/MNReader.h>
   * @ingroup bn_io
   * @brief Pure virtual class for reading a MN from a file.
   *
   * Every class used to read the content of a Markov Network from a stream,
   * or a file must be a subclass of MNReader.
   */
  template < typename GUM_SCALAR >
  class MNReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the MN has to be built outside the reader. There is no
     * delegation to create/destroy the MN from inside the reader.
     */
    MNReader(MarkovNet< GUM_SCALAR >* MN, const std::string& filename);

    /**
     * Default destructor.
     */
    virtual ~MNReader();

    /**
     * Reads a Markov Network from the file referenced by filePath into`
     * parameter MarkovNet.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual Size proceed() = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MNReader< double >;
#endif

} /* namespace gum */

#include <agrum/MN/io/MNReader_tpl.h>

#endif   // GUM_MN_READER_H

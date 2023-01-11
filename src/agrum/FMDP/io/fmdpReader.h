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
 * of Factored Markov Decision Process.
 *
 * Every classe used to read a FMDP from a file, must inherit from
 * FMDPReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
#ifndef GUM_FMDP_READER_H
#define GUM_FMDP_READER_H
//======================================================================
#include <iostream>
#include <string>
//======================================================================
#include <agrum/agrum.h>
//======================================================================
#include <agrum/FMDP/fmdp.h>
//======================================================================


namespace gum {
  /* ============================================================================
   */
  /* ===                               READERS === */
  /* ============================================================================
   */
  /**
   * @class FMDPReader
   * @brief Pure virtual class for reading a FMDP from a file.
   * @ingroup fmdp_group
   * Every class used to read the content of a Factored Markov Decision Process
   * from a stream,
   * or a file must be a subclass of FMDPReader.
   */
  template < typename GUM_SCALAR >
  class FMDPReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the FMDP has to be built outside the reader. There is no
     * delegation to create/destroy
     * the FMDP from inside the reader.
     */
    FMDPReader(FMDP< GUM_SCALAR >* fmdp, const std::string& filename);

    /**
     * Default destructor.
     */
    virtual ~FMDPReader();

    /**
     * Reads a Factored Markov Decision Process from the file referenced by
     * filePath into`
     * parameter fmdp.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual Size proceed() = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class FMDPReader< double >;
#endif


} /* namespace gum */


#include <agrum/FMDP/io/fmdpReader_tpl.h>

#endif   // GUM_FMDP_READER_H

/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Definition of abstract classes for file output manipulation
 * of Markov Networks.
 *
 * Every class used to read or write a MN from a file, must inherit from
 * MNWriter or MNReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MN_WRITER_H
#define GUM_MN_WRITER_H

#include <agrum/MN/IMarkovNet.h>
#include <agrum/agrum.h>
#include <iostream>
#include <string>

namespace gum {

  /* =========================================================================*/
  /* ===                              WRITERS === */
  /* =========================================================================*/
  /**
   * @class MNWriter
   * @headerfile MNWriter.h <agrum/MN/io/MNWriter.h>
   * @brief Pure virtual class for writting a MN to a file.
   * @ingroup mn_io
   *
   * Every class used to write the content of a Markov Network in a stream, or
   * a file must be a subclass of MNWriter.
   */
  template < typename GUM_SCALAR >

  class MNWriter {
    public:
    /**
     * Default constructor.
     */
    MNWriter();

    /**
     * Default destructor.
     */
    virtual ~MNWriter();

    /**
     * Writes a Markov Network in the ouput stream.
     *
     * @param output The output stream.
     * @param MN The Markov Network writed in output.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write(std::ostream& output, const IMarkovNet< GUM_SCALAR >& MN) = 0;

    /**
     * Writes a Markov Network in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Markov Network.
     * @param MN The Markov Network writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(const std::string& filePath, const IMarkovNet< GUM_SCALAR >& MN) = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MNWriter< double >;
#endif
} /* namespace gum */

#include <agrum/MN/io/MNWriter_tpl.h>

#endif   // GUM_%N_WRITER_H

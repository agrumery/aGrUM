
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
 * @brief Definition file for UAI exportation class
 *
 * Writes a markov net in UAI format
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES
 */


#ifndef UAI_MN_WRITER
#define UAI_MN_WRITER


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/MN/io/MNWriter.h>
#include <agrum/agrum.h>

namespace gum {
  /**
   * @class UAIMNWriter UAIMNWriter.h
   *<agrum/MN/io/UAI/UAIMNWriter.h>
   * @ingroup mn_io
   * @brief Writes an Markov net in a text file with UAI format
   *
   * This class export a Markov net into an text file, using UAI format
   *
   * cf. http://www.cs.huji.ac.il/project/PASCAL/fileFormat.php
   *
   */
  template < typename GUM_SCALAR >
  class UAIMNWriter: public MNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    UAIMNWriter();

    /**
     * Destructor.
     */
    ~UAIMNWriter() final;

    /// @}

    /**
     * Writes an Markov net in the given ouput stream.
     *
     * @param output The output stream.
     * @param MN The Markov net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void write(std::ostream& output, const IMarkovNet< GUM_SCALAR >& MN) final;

    /**
     * Writes an Markov net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Markov net.
     * @param MN The Markov net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void write(const std::string&              filePath,
               const IMarkovNet< GUM_SCALAR >& MN) final;

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string __preambule(const IMarkovNet< GUM_SCALAR >& MN);

    std::string __factorBloc(const IMarkovNet< GUM_SCALAR >& MN,
                             const Potential< GUM_SCALAR >&  clikpot);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class UAIMNWriter< double >;
#endif

} /* namespace gum */

#include <agrum/MN/io/UAI/UAIMNWriter_tpl.h>
#endif   // UAI_MN_WRITER

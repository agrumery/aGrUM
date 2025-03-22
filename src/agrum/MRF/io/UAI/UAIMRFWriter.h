/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Definition file for UAI exportation class
 *
 * Writes a markov net in UAI format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */


#ifndef UAI_MRF_WRITER
#define UAI_MRF_WRITER

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/MRF/io/MRFWriter.h>

namespace gum {
  /**
   * @class UAIMRFWriter UAIMRFWriter.h
   *<agrum/MRF/io/UAI/UAIMRFWriter.h>
   * @ingroup mn_io
   * @brief Writes an Markov net in a text file with UAI format
   *
   * This class export a Markov net into an text file, using UAI format
   *
   * cf. http://www.cs.huji.ac.il/project/PASCAL/fileFormat.php
   *
   */
  template < typename GUM_SCALAR >
  class UAIMRFWriter: public MRFWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    UAIMRFWriter();

    /**
     * Destructor.
     */
    ~UAIMRFWriter();

    /// @}

    /**
     * Writes an Markov net in the given ouput stream.
     *
     * @param output The output stream.
     * @param MN The Markov net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void write(std::ostream& output, const IMarkovRandomField< GUM_SCALAR >& MN) final;

    /**
     * Writes an Markov net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Markov net.
     * @param MN The Markov net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void write(const std::string& filePath, const IMarkovRandomField< GUM_SCALAR >& MN) final;

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string _preambule_(const IMarkovRandomField< GUM_SCALAR >& MN);

    std::string _factorBloc_(const IMarkovRandomField< GUM_SCALAR >& MN,
                             const Tensor< GUM_SCALAR >&             clikpot);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class UAIMRFWriter< double >;
#endif

} /* namespace gum */

#include <agrum/MRF/io/UAI/UAIMRFWriter_tpl.h>
#endif   // UAI_MRF_WRITER

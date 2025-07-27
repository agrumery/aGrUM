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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Definition of abstract classes for file input  manipulation
 * of Markov random fields.
 *
 * Every classe used to read a MRF from a file, must inherit from
 * MRFWriter or MRFReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MN_READER_H
#define GUM_MN_READER_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/MRF/MarkovRandomField.h>

namespace gum {
  /* =========================================================================*/
  /* ===                               READERS                            === */
  /* =========================================================================*/
  /**
   * @class MRFReader
   * @headerfile MRFReader.h <agrum/MRF/io/MRFReader.h>
   * @ingroup mn_io
   * @brief Pure virtual class for reading a MRF from a file.
   *
   * Every class used to read the content of a Markov random field from a stream,
   * or a file must be a subclass of MRFReader.
   */
  template < typename GUM_SCALAR >
  class MRFReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the MRF has to be built outside the reader. There is no
     * delegation to create/destroy the MRF from inside the reader.
     */
    MRFReader(MarkovRandomField< GUM_SCALAR >* MN, const std::string& filename);

    /**
     * Default destructor.
     */
    virtual ~MRFReader();

    /**
     * Reads a Markov random field from the file referenced by filePath into`
     * parameter MarkovRandomField.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual Size proceed() = 0;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MRFReader< double >;
#endif

} /* namespace gum */

#include <agrum/MRF/io/MRFReader_tpl.h>

#endif   // GUM_MN_READER_H

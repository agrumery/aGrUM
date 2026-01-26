/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

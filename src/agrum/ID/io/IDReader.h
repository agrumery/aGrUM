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
 * @brief Definition of abstract classes for import of Influence Diagram from a
 *file
 *
 * All classes used to import an influence diagram from a file must inherit from
 *IDReader
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 *GONZALES(_at_AMU)
 */
#ifndef GUM_ID_READER_H
#define GUM_ID_READER_H

#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/ID/influenceDiagram.h>

namespace gum {

  /**
   * @class IDReader
   * @brief Pure virtual class for importing an ID from a file.
   * @ingroup id_group
   *
   * All classes used to read an influence diagram from
   * a file must inherit from this class
   */
  template < typename GUM_SCALAR >
  class IDReader {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an ID as to be created before and given in parameter.
     */
    IDReader(InfluenceDiagram< GUM_SCALAR >* infdiag, const std::string& filePath);

    /**
     * Default destructor.
     */
    virtual ~IDReader();

    /**
     * Reads the influence diagram from the file referenced by filePath  given
     * at the
     * creation of class
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual void proceed() = 0;
  };

} /* namespace gum */

#include "IDReader_tpl.h"

#endif   // GUM_ID_READER_H

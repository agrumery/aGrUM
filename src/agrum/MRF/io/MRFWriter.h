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
 * @brief Definition of abstract classes for file output manipulation
 * of Markov random fields.
 *
 * Every class used to read or write a MRF from a file, must inherit from
 * MRFWriter or MRFReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MN_WRITER_H
#define GUM_MN_WRITER_H

#include <filesystem>
#include <iostream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/MRF/IMarkovRandomField.h>

#include <string_view>

namespace gum {

  /* =========================================================================*/
  /* ===                              WRITERS === */
  /* =========================================================================*/
  /**
   * @class MRFWriter
   * @headerfile MRFWriter.h <agrum/MRF/io/MRFWriter.h>
   * @brief Pure virtual class for writing a MRF to a file.
   * @ingroup mn_io
   *
   * Every class used to write the content of a Markov random field in a stream, or
   * a file must be a subclass of MRFWriter.
   */
  template < GUM_Numeric GUM_SCALAR >

  class MRFWriter {
    public:
    /**
     * Default constructor.
     */
    MRFWriter();

    /**
     * Default destructor.
     */
    virtual ~MRFWriter();

    /**
     * Writes a Markov random field to the output stream.
     *
     * @param output The output stream.
     * @param MN The Markov random field written to the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write(std::ostream& output, const IMarkovRandomField< GUM_SCALAR >& MN) = 0;

    /**
     * Writes a Markov random field in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the Markov random field.
     * @param MN The Markov random field written to the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(std::string_view filePath, const IMarkovRandomField< GUM_SCALAR >& MN) = 0;

    /**
     * Writes a Markov random field in the file referenced by filePath, updating
     * metadata (software, creation, lastModification) before writing.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * Default implementation delegates to the const overload.
     *
     * @param filePath The path to the file used to write the Markov random field.
     * @param MN The Markov random field written to the file (non-const to allow metadata update).
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write(std::string_view filePath, IMarkovRandomField< GUM_SCALAR >& MN) {
      write(filePath, static_cast< const IMarkovRandomField< GUM_SCALAR >& >(MN));
    }
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MRFWriter< double >;
#endif
} /* namespace gum */

#include <agrum/MRF/io/MRFWriter_tpl.h>

#endif   // GUM_MN_WRITER_H

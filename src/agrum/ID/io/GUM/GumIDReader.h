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
 * @brief Definition of class for reading an InfluenceDiagram from a GUM (json) file.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_ID_GUM_READER_H
#define GUM_ID_GUM_READER_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/errorsContainer.h>
#include <agrum/ID/io/IDReader.h>

namespace gum {
  /**
   * @class GumIDReader
   * @headerfile GumIDReader.h <agrum/ID/io/GUM/GumIDReader.h>
   * @ingroup id_io
   * @brief Reads an InfluenceDiagram from a GUM (json) file.
   *
   * Supports both text (.jgum) and binary msgpack (.bgum) formats.
   */
  template < GUM_Numeric GUM_SCALAR >
  class GumIDReader: public IDReader< GUM_SCALAR >, public ErrorsContainer {
    public:
    /**
     * Constructor.
     *
     * @param id    The InfluenceDiagram to populate.
     * @param filename Path to the file to read.
     * @param binary   If true, the file is in msgpack binary format.
     */
    GumIDReader(InfluenceDiagram< GUM_SCALAR >* id, std::string_view filename, bool binary = false);

    /// Constructor for string-based parsing only (no file).
    /// Use exclusively with proceedFromString(); calling proceed() will fail.
    explicit GumIDReader(InfluenceDiagram< GUM_SCALAR >* id);

    ~GumIDReader() override;

    /**
     * Parses the file and populates the InfluenceDiagram.
     * @return the number of detected errors (0 if successful).
     */
    Size proceed() override;

    /// Parse a jgum JSON string directly (no file I/O).
    /// @return the number of detected errors
    Size proceedFromString(std::string_view content);

    void showElegantErrorsAndWarnings(std::ostream& stream = std::cerr) const;
    void showErrorCounts(std::ostream& stream = std::cerr) const;

    private:
    template < typename JsonType >
    Size _proceedFromJson_(const JsonType& content);

    InfluenceDiagram< GUM_SCALAR >* _id_;
    std::string                     _streamName_;
    bool                            _parseDone_;
    bool                            _binary_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GumIDReader< double >;
#endif
} /* namespace gum */

#include <agrum/ID/io/GUM/GumIDReader_tpl.h>

#endif   // GUM_ID_GUM_READER_H

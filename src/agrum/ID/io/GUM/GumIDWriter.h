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
 * @brief Definition of class for GUM (json) file output for Influence Diagrams.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_ID_GUM_WRITER_H
#define GUM_ID_GUM_WRITER_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/ID/io/IDWriter.h>

namespace gum {
  /**
   * @class GumIDWriter
   * @headerfile GumIDWriter.h <agrum/ID/io/GUM/GumIDWriter.h>
   * @brief Writes an InfluenceDiagram in the GUM json format.
   * @ingroup id_io
   *
   * Supports both text (.jgum) and binary msgpack (.bgum) output.
   */
  template < GUM_Numeric GUM_SCALAR >
  class GumIDWriter: public IDWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     *
     * @param binary If true, output uses the msgpack binary format; otherwise JSON text.
     * @param indent Indentation level for text output. -1 produces compact output.
     */
    GumIDWriter(bool binary = false, int indent = -1);

    ~GumIDWriter() override;

    GumIDWriter(const GumIDWriter&)                = delete;
    GumIDWriter(GumIDWriter&&) noexcept            = delete;
    GumIDWriter& operator=(const GumIDWriter&)     = delete;
    GumIDWriter& operator=(GumIDWriter&&) noexcept = delete;

    /// @}

    /**
     * Serializes an InfluenceDiagram to a string in GUM json format.
     */
    std::string toString(const InfluenceDiagram< GUM_SCALAR >& id);

    /**
     * Writes an InfluenceDiagram to the output stream.
     *
     * @param output The output stream.
     * @param id The InfluenceDiagram to write.
     * @throws IOError if the stream is not writable.
     */
    void write(std::ostream& output, const InfluenceDiagram< GUM_SCALAR >& id) override;

    /**
     * Writes an InfluenceDiagram to the file at filePath, updating metadata first.
     *
     * @param filePath Path to the output file (created or overwritten).
     * @param id The InfluenceDiagram to write (non-const to allow metadata update).
     * @throws IOError if an I/O error occurs.
     */
    void write(std::string_view filePath, InfluenceDiagram< GUM_SCALAR >& id) override;

    /**
     * Writes an InfluenceDiagram to the file at filePath (const variant, no metadata update).
     *
     * @param filePath Path to the output file (created or overwritten).
     * @param id The InfluenceDiagram to write.
     * @throws IOError if an I/O error occurs.
     */
    void write(std::string_view filePath, const InfluenceDiagram< GUM_SCALAR >& id) override;

    private:
    static void _writeVector_(std::ostream& os, const std::vector< uint8_t >& vec);

    bool _binary_;
    int  _indent_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GumIDWriter< double >;
#endif
} /* namespace gum */

#include <agrum/ID/io/GUM/GumIDWriter_tpl.h>

#endif   // GUM_ID_GUM_WRITER_H

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
 * @brief The statements common to the graphs drawn with graphviz : preamble, attributes of nodes
 * and arcs.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DOT_ELEMENTS_H
#define GUM_DOT_ELEMENTS_H

#include <agrum/agrum.h>

#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/core/dot/dotStyle.h>

#include <string_view>

namespace gum::dot {

  /**
   * @brief Writes the beginning of a graph, as pyAgrum displays it.
   *
   * - `graph [bgcolor="transparent", size, rankdir, layout]` (the `size`, `rankdir` and `layout` of
   * the options if they are given, the ones of the style otherwise) and, if there is a label,
   * `label` and `fontcolor`;
   * - `node [style="filled", fillcolor, fontcolor, color]` : the default look of the nodes;
   * - `edge [color]` : the default look of the arcs.
   *
   * @param writer the writer, just created.
   * @param style the look of the graph, read from the configuration.
   * @param options the options given by the caller.
   * @param defaultLabel the label of the graph if the options give none (for instance the name of a
   * model).
   */
  GUM_SHARED_PUBLIC void writePreamble(DotWriter&        writer,
                                       const DotStyle&   style,
                                       const DotOptions& options,
                                       std::string_view  defaultLabel = "");

  /**
   * @brief The attributes of a node : its label and tooltip, and its colors if the options give
   * some.
   *
   * What the options do not give is not written, so the default look of the preamble applies. A
   * background color without font color gets an `adaptive` font color, black or white according to
   * the background. The attributes given with `setNodeAttribute` come last.
   *
   * @param style the look of the graph.
   * @param options the options of the node, or nullptr if there is none.
   * @param defaultLabel the label of the node if the options give none (none if empty).
   * @param defaultTooltip the tooltip of the node if the options give none (none if empty).
   */
  GUM_SHARED_PUBLIC DotAttributes nodeAttributes(const DotStyle&       style,
                                                 const DotNodeOptions* options,
                                                 std::string_view      defaultLabel   = "",
                                                 std::string_view      defaultTooltip = "");

  /**
   * @brief The attributes of an arc or an edge given by the options : `color`, `penwidth`, `label`,
   * `tooltip` and the ones given with `setArcAttribute`. Empty if there is no options.
   */
  GUM_SHARED_PUBLIC DotAttributes arcAttributes(const DotArcOptions* options);

  /**
   * @brief Adds an attribute at the beginning of the list, unless the list already gives a value
   * for this key : the value given by the caller of the `toDot()` wins over the one of the class.
   */
  GUM_SHARED_PUBLIC void
      addDefaultAttribute(DotAttributes& attributes, std::string_view key, std::string_view value);

}   // namespace gum::dot

#endif   // GUM_DOT_ELEMENTS_H

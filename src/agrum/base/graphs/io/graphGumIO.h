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
 * @brief Reading and writing the graphs in the native GUM format: jgum (JSON) and bgum (msgpack).
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_GUM_IO_H
#define GUM_GRAPH_GUM_IO_H

#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/graphs/MAG.h>
#include <agrum/base/graphs/MPDAG.h>
#include <agrum/base/io/GumBinaryIO.h>

namespace gum {

  namespace graph::detail {
    /// the content of a graph in the GUM format (shared by the readers and writers of all the
    /// graph classes)
    struct GUM_SHARED_PUBLIC GumGraphContent {
      /// the class of the graph: DiGraph, DAG, UndiGraph, MixedGraph, PDAG, ChainGraph,
      /// MarkedGraph, PAG, CPDAG, MPDAG, MAG or EssentialGraph
      std::string                                                     type;
      std::vector< NodeId >                                           nodes;
      std::vector< std::pair< NodeId, std::string > >                 names;
      std::vector< std::pair< NodeId, NodeId > >                      arcs;
      std::vector< std::pair< NodeId, NodeId > >                      edges;
      std::vector< std::tuple< NodeId, NodeId, EdgeMark, EdgeMark > > markedEdges;
    };

    /// the jgum text of a content (indent: -1 for the most compact output)
    GUM_SHARED_PUBLIC std::string gumGraphToString(const GumGraphContent& content, int indent);

    /// the content of a jgum text
    /** @throw IOError if the text is not a graph in the GUM format */
    GUM_SHARED_PUBLIC GumGraphContent gumGraphFromString(std::string_view text);

    /// writes a content in a jgum (text) or bgum (binary) file (format: see gum::isBinaryGUMFile)
    /** @throw IOError if the file can not be written
     * @throw InvalidArgument if binary contradicts the extension of path */
    GUM_SHARED_PUBLIC void gumGraphToFile(const GumGraphContent& content,
                                          std::string_view       path,
                                          std::optional< bool >  binary,
                                          int                    indent);

    /// the content of a jgum (text) or bgum (binary) file (format: see gum::isBinaryGUMFile)
    /** @throw IOError if the file can not be read or is not a graph in the GUM format
     * @throw InvalidArgument if binary contradicts the extension of path */
    GUM_SHARED_PUBLIC GumGraphContent gumGraphFromFile(std::string_view      path,
                                                       std::optional< bool > binary);

    /// the content of a PDAG, written as a graph of class type (for the classes of the other
    /// modules which store a PDAG, as EssentialGraph)
    GUM_SHARED_PUBLIC GumGraphContent gumGraphContentOf(const PDAG& g, const std::string& type);

    /// the PDAG of a content written as a graph of class type
    /** @throw IOError if the content is not a graph of class type */
    GUM_SHARED_PUBLIC PDAG gumGraphPDAGOf(const GumGraphContent& content, const std::string& type);

    /// rethrows the gum::Exception being handled as an IOError (unchanged if already an IOError)
    /** To be called in a catch block only: a content read as a graph of class type which does not
     * respect the properties of this class is a malformed GUM graph.
     * @throw IOError always */
    [[noreturn]] GUM_SHARED_PUBLIC void rethrowAsIOError(const std::string& type);
  }   // namespace graph::detail

  /**
   * @name Graphs in the GUM format
   *
   * The graphs (except CliqueGraph) are written in the native GUM format, as the graphical models:
   * a JSON text (jgum) or its msgpack encoding (bgum), with the class of the graph, its nodes
   * (isolated ones included), the names of the named nodes, its arcs, its edges and, for the
   * marked graphs, the marks of the edges. The writing functions are overloaded for each class;
   * the reading functions are templated by the class to read, which must be the one written (use
   * graphGUMType to know it). The immutable classes (CPDAG, MPDAG, MAG) are read by their checked
   * conversions.
   *
   * The format of a file is given by the optional argument binary, else by the extension of the
   * file: .jgum for the text format, .bgum or any other extension for the binary one (see
   * gum::isBinaryGUMFile). An explicit format which contradicts the extension .jgum or .bgum raises
   * InvalidArgument.
   *
   * Format (GumJsonVersion 1.0):
   * @code
   * {"type": "PAG", "GumJsonVersion": "1.0", "nodes": [0, 1, 2, 5], "names": {"0": "A"},
   *  "arcs": [], "edges": [], "markedEdges": [[1, 2, "circle", "arrowhead"]]}
   * @endcode
   * Only "type" and "nodes" are required on read: "GumJsonVersion" (whose major version must be 1
   * if given), "names", "arcs", "edges" and "markedEdges" are optional.
   *
   * The ids are kept. A graph stores its absent ids lower than its largest one (holes): on read,
   * their number must not exceed max(number of nodes, 65536), otherwise IOError (a graph with few
   * nodes and very large ids is written but not read back).
   * @{
   */

  /// the class of the graph written in a jgum text
  /** @throw IOError if the text is not a graph in the GUM format */
  GUM_SHARED_PUBLIC std::string graphGUMType(std::string_view content);

  /// the class of the graph written in a jgum or bgum file
  /** @throw IOError if the file can not be read or is not a graph in the GUM format */
  /** @throw InvalidArgument if binary contradicts the extension of path */
  GUM_SHARED_PUBLIC std::string graphGUMTypeOfFile(std::string_view      path,
                                                   std::optional< bool > binary = std::nullopt);

  /// the graph of class G written in a jgum text
  /** @throw IOError if the text is not a graph of class G in the GUM format, including a graph
   *  which does not respect the properties of G (e.g. a cycle in a DAG, a CPDAG which is not
   *  valid) */
  template < typename G >
  G graphFromGUMString(std::string_view content) = delete;

  /// the graph of class G written in a jgum (text) or bgum (binary) file
  /** @throw IOError if the file can not be read or is not a graph of class G in the GUM format,
   *  including a graph which does not respect the properties of G
   *  @throw InvalidArgument if binary contradicts the extension of path */
  template < typename G >
  G loadGraphGUM(std::string_view path, std::optional< bool > binary = std::nullopt) = delete;

#define GUM_DECLARE_GRAPH_GUM_IO(GRAPH)                                                           \
  GUM_SHARED_PUBLIC std::string graphToGUMString(const GRAPH& g,                                  \
                                                 int          indent = defaultJgumIndent());      \
  GUM_SHARED_PUBLIC void        saveGraphGUM(const GRAPH&          g,                             \
                                             std::string_view      path,                          \
                                             std::optional< bool > binary = std::nullopt,         \
                                             int                   indent = defaultJgumIndent()); \
  template <>                                                                                     \
  GUM_SHARED_PUBLIC GRAPH graphFromGUMString< GRAPH >(std::string_view content);                  \
  template <>                                                                                     \
  GUM_SHARED_PUBLIC GRAPH loadGraphGUM< GRAPH >(std::string_view      path,                       \
                                                std::optional< bool > binary);

  GUM_DECLARE_GRAPH_GUM_IO(DiGraph)
  GUM_DECLARE_GRAPH_GUM_IO(DAG)
  GUM_DECLARE_GRAPH_GUM_IO(UndiGraph)
  GUM_DECLARE_GRAPH_GUM_IO(MixedGraph)
  GUM_DECLARE_GRAPH_GUM_IO(PDAG)
  GUM_DECLARE_GRAPH_GUM_IO(ChainGraph)
  GUM_DECLARE_GRAPH_GUM_IO(MarkedGraph)
  GUM_DECLARE_GRAPH_GUM_IO(PAG)
  GUM_DECLARE_GRAPH_GUM_IO(CPDAG)
  GUM_DECLARE_GRAPH_GUM_IO(MPDAG)
  GUM_DECLARE_GRAPH_GUM_IO(MAG)

#undef GUM_DECLARE_GRAPH_GUM_IO

  /// @}

}   // namespace gum

#endif   // GUM_GRAPH_GUM_IO_H

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
 * @brief Reading and writing the graphs in the native GUM format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <algorithm>
#include <charconv>
#include <concepts>
#include <cstdint>
#include <fstream>
#include <limits>
#include <type_traits>

#include <agrum/base/graphs/io/graphGumIO.h>

#include <agrum/base/external/json/json.hpp>

using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace gum {

  namespace {
    std::string markName(EdgeMark mark) {
      switch (mark) {
        case EdgeMark::Circle : return "circle";
        case EdgeMark::Tail : return "tail";
        case EdgeMark::Arrowhead : return "arrowhead";
      }
      return "circle";
    }

    EdgeMark markFromName(const std::string& name) {
      if (name == "circle") return EdgeMark::Circle;
      if (name == "tail") return EdgeMark::Tail;
      if (name == "arrowhead") return EdgeMark::Arrowhead;
      GUM_ERROR(IOError, "GUM graph: unknown edge mark '" << name << "'")
    }

    ordered_json toJson(const graph::detail::GumGraphContent& content) {
      ordered_json j;
      j["type"]           = content.type;
      j["GumJsonVersion"] = "1.0";
      j["nodes"]          = content.nodes;
      j["names"]          = ordered_json::object();
      for (const auto& [node, name]: content.names)
        j["names"][std::to_string(node)] = name;
      j["arcs"] = ordered_json::array();
      for (const auto& [tail, head]: content.arcs)
        j["arcs"].push_back({tail, head});
      j["edges"] = ordered_json::array();
      for (const auto& [a, b]: content.edges)
        j["edges"].push_back({a, b});
      j["markedEdges"] = ordered_json::array();
      for (const auto& [x, y, markAtX, markAtY]: content.markedEdges)
        j["markedEdges"].push_back({x, y, markName(markAtX), markName(markAtY)});
      return j;
    }

    /// the node of a key of "names" (digits only: no sign, no space, no trailing text)
    NodeId nodeOfKey(const std::string& key) {
      NodeId     node      = 0;
      const auto end       = key.data() + key.size();
      const auto [ptr, ec] = std::from_chars(key.data(), end, node);
      // from_chars refuses an empty key, a space and a sign (NodeId is unsigned)
      if (ec != std::errc() || ptr != end)
        GUM_ERROR(IOError, "GUM graph: '" << key << "' is not a node id")
      return node;
    }

    /// a node id: an unsigned integer which fits in a NodeId (not a negative, real or boolean
    /// value)
    NodeId nodeOf(const json& value) {
      if (!value.is_number_unsigned()
          || value.get< std::uint64_t >() > std::numeric_limits< NodeId >::max())
        GUM_ERROR(IOError, "GUM graph: " << value.dump() << " is not a node id")
      return static_cast< NodeId >(value.get< std::uint64_t >());
    }

    /// the largest number of holes (absent ids lower than the largest id) allowed beyond the number
    /// of nodes in a GUM graph
    constexpr Size maxGumHoles = 65536;

    /// a graph stores its holes: their number is bounded by max(number of nodes, maxGumHoles), so
    /// that the memory of a read graph is linear in the size of its content ("nodes": [1000000000]
    /// would otherwise store 10^9 holes). Checked before any graph is built
    void checkHoles(const std::vector< NodeId >& nodes) {
      if (nodes.empty()) return;
      const NodeId maxId = *std::max_element(nodes.begin(), nodes.end());
      // maxId + 1 - nodes.size() > max(nodes.size(), maxGumHoles), without underflow when a node
      // is written twice (rejected later)
      if (maxId >= nodes.size() + std::max(nodes.size(), maxGumHoles))
        GUM_ERROR(IOError,
                  "GUM graph: the node id " << maxId << " is too large for " << nodes.size()
                                            << " nodes (at most max(" << nodes.size() << ", "
                                            << maxGumHoles
                                            << ") absent ids lower than the largest one)")
    }

    /// the array j[key] (an empty array if absent)
    const json& arrayAt(const json& j, const char* key) {
      static const json empty = json::array();
      if (!j.contains(key)) return empty;
      const auto& value = j.at(key);
      if (!value.is_array()) GUM_ERROR(IOError, "GUM graph: '" << key << "' is not an array")
      return value;
    }

    /// the tuple t, which must be an array of `size` elements
    const json& tupleOf(const json& t, std::size_t size, const char* key) {
      if (!t.is_array() || t.size() != size)
        GUM_ERROR(IOError,
                  "GUM graph: " << t.dump() << " in '" << key << "' is not an array of " << size
                                << " elements")
      return t;
    }

    graph::detail::GumGraphContent fromJson(const json& j) {
      try {
        graph::detail::GumGraphContent content;
        if (!j.is_object() || !j.contains("type") || !j.contains("nodes"))
          GUM_ERROR(IOError, "GUM graph: the content is not a graph in the GUM format")
        if (j.contains("GumJsonVersion")) {
          const auto version = j.at("GumJsonVersion").get< std::string >();
          if (version.substr(0, version.find('.')) != "1")
            GUM_ERROR(IOError, "GUM graph: unsupported GumJsonVersion '" << version << "'")
        }
        content.type = j.at("type").get< std::string >();
        for (const auto& node: arrayAt(j, "nodes"))
          content.nodes.push_back(nodeOf(node));
        checkHoles(content.nodes);
        if (j.contains("names")) {
          // an array would give its indices as keys, null no name at all
          if (!j.at("names").is_object())
            GUM_ERROR(IOError, "GUM graph: 'names' is not an object {node id: name}")
          for (const auto& [key, value]: j.at("names").items())
            content.names.emplace_back(nodeOfKey(key), value.get< std::string >());
        }
        for (const auto& arc: arrayAt(j, "arcs")) {
          const auto& t = tupleOf(arc, 2, "arcs");
          content.arcs.emplace_back(nodeOf(t[0]), nodeOf(t[1]));
        }
        for (const auto& edge: arrayAt(j, "edges")) {
          const auto& t = tupleOf(edge, 2, "edges");
          content.edges.emplace_back(nodeOf(t[0]), nodeOf(t[1]));
        }
        for (const auto& edge: arrayAt(j, "markedEdges")) {
          const auto& t = tupleOf(edge, 4, "markedEdges");
          content.markedEdges.emplace_back(nodeOf(t[0]),
                                           nodeOf(t[1]),
                                           markFromName(t[2].get< std::string >()),
                                           markFromName(t[3].get< std::string >()));
        }
        return content;
      } catch (const json::exception& e) {
        GUM_ERROR(IOError, "GUM graph: malformed content (" << e.what() << ")")
      }
    }

    /// the content of a graph g, written as a graph of class type
    template < typename G >
    graph::detail::GumGraphContent contentOf(const G& g, const std::string& type) {
      graph::detail::GumGraphContent content;
      content.type = type;
      for (const auto node: g.nodes()) {
        content.nodes.push_back(node);
        if (g.hasName(node)) content.names.emplace_back(node, g.nameFromId(node));
      }
      if constexpr (std::derived_from< G, MarkedGraph >) {
        for (const auto& edge: g.edges())
          content.markedEdges.emplace_back(edge.first(),
                                           edge.second(),
                                           g.markAt(edge.second(), edge.first()),
                                           g.markAt(edge.first(), edge.second()));
      } else {
        if constexpr (requires { g.arcs(); })
          for (const auto& arc: g.arcs())
            content.arcs.emplace_back(arc.tail(), arc.head());
        if constexpr (requires { g.edges(); })
          for (const auto& edge: g.edges())
            content.edges.emplace_back(edge.first(), edge.second());
      }
      return content;
    }

    /// the DAG of a digraph: checked once by a topological order, its arcs being added into a node
    /// before any arc out of it (the acyclicity checks of DAG::addArc then remain trivial)
    DAG dagOf(const DiGraph& digraph) {
      DAG dag;
      for (const auto node: digraph.nodes()) {
        dag.addNodeWithId(node);
        if (digraph.hasName(node)) dag.setName(node, digraph.nameFromId(node));
      }
      for (const auto node: digraph.topologicalOrder())
        for (const auto parent: digraph.parents(node))
          dag.addArc(parent, node);
      return dag;
    }

    /// the graph of class G of a content written as a graph of class type
    template < typename G >
    G graphOf(const graph::detail::GumGraphContent& content, const std::string& type) {
      // the classes forbidding some cycles are checked once, not at each arc or edge
      if constexpr (std::same_as< G, DAG > || std::same_as< G, PDAG >
                    || std::same_as< G, ChainGraph >) {
        using Unchecked      = std::conditional_t< std::same_as< G, DAG >, DiGraph, MixedGraph >;
        const auto unchecked = graphOf< Unchecked >(content, type);
        try {
          if constexpr (std::same_as< G, DAG >) return dagOf(unchecked);
          else return G(unchecked);
        } catch (const Exception&) { graph::detail::rethrowAsIOError(type); }
      } else {
        if (content.type != type)
          GUM_ERROR(IOError, "GUM graph: the content is a " << content.type << ", not a " << type)
        try {
          G g;
          for (const auto node: content.nodes)
            g.addNodeWithId(node);
          for (const auto& [node, name]: content.names)
            g.setName(node, name);
          if constexpr (std::derived_from< G, MarkedGraph >) {
            if (!content.arcs.empty() || !content.edges.empty())
              GUM_ERROR(IOError, "GUM graph: a " << type << " has only marked edges")
            for (const auto& [x, y, markAtX, markAtY]: content.markedEdges) {
              if (g.existsEdge(x, y))
                GUM_ERROR(IOError, "GUM graph: the edge " << x << "-" << y << " is written twice")
              g.addEdge(x, y, markAtX, markAtY);
            }
          } else {
            if (!content.markedEdges.empty())
              GUM_ERROR(IOError, "GUM graph: a " << type << " has no marked edge")
            if constexpr (requires { g.addArc(NodeId(0), NodeId(0)); }) {
              for (const auto& [tail, head]: content.arcs) {
                if (g.existsArc(tail, head))
                  GUM_ERROR(IOError,
                            "GUM graph: the arc " << tail << "->" << head << " is written twice")
                g.addArc(tail, head);
              }
            } else if (!content.arcs.empty()) {
              GUM_ERROR(IOError, "GUM graph: a " << type << " has no arc")
            }
            if constexpr (requires { g.edges(); }) {
              for (const auto& [a, b]: content.edges) {
                if (g.existsEdge(a, b))
                  GUM_ERROR(IOError, "GUM graph: the edge " << a << "-" << b << " is written twice")
                g.addEdge(a, b);
              }
            } else if (!content.edges.empty()) {
              GUM_ERROR(IOError, "GUM graph: a " << type << " has no edge")
            }
          }
          return g;
        } catch (const Exception&) { graph::detail::rethrowAsIOError(type); }
      }
    }
  }   // namespace

  namespace graph::detail {
    void rethrowAsIOError(const std::string& type) {
      try {
        throw;
      } catch (const IOError&) { throw; } catch (const Exception& e) {
        GUM_ERROR(IOError, "GUM graph: not a valid " << type << " (" << e.errorContent() << ")")
      }
    }

    std::string gumGraphToString(const GumGraphContent& content, int indent) {
      return toJson(content).dump(indent < -1 ? -1 : indent);
    }

    GumGraphContent gumGraphFromString(std::string_view text) {
      json j;
      try {
        j = json::parse(text);
      } catch (const json::exception& e) {
        GUM_ERROR(IOError, "GUM graph: not a JSON text (" << e.what() << ")")
      }
      return fromJson(j);
    }

    void gumGraphToFile(const GumGraphContent& content,
                        std::string_view       path,
                        std::optional< bool >  binary,
                        int                    indent) {
      const bool    isBinary = isBinaryGUMFile(path, binary);
      std::ofstream output(std::string(path), isBinary ? std::ios::binary : std::ios::out);
      if (!output.good()) GUM_ERROR(IOError, "GUM graph: can not write the file '" << path << "'")
      if (isBinary) gum::detail::writeVector(output, json::to_msgpack(toJson(content)));
      else output << gumGraphToString(content, indent);
      output.close();   // the final flush may fail too (disk full)
      if (!output.good()) GUM_ERROR(IOError, "GUM graph: can not write the file '" << path << "'")
    }

    GumGraphContent gumGraphFromFile(std::string_view path, std::optional< bool > binary) {
      const bool    isBinary = isBinaryGUMFile(path, binary);
      std::ifstream input(std::string(path), isBinary ? std::ios::binary : std::ios::in);
      if (!input.good()) GUM_ERROR(IOError, "GUM graph: can not read the file '" << path << "'")
      json j;
      try {
        if (isBinary) j = json::from_msgpack(gum::detail::readVector(input));
        else j = json::parse(input);
      } catch (const json::exception& e) {
        GUM_ERROR(IOError, "GUM graph: malformed file '" << path << "' (" << e.what() << ")")
      } catch (const IOError& e) {   // gum::detail::readVector
        GUM_ERROR(IOError,
                  "GUM graph: malformed file '" << path << "' (" << e.errorContent() << ")")
      }
      return fromJson(j);
    }
  }   // namespace graph::detail

  namespace graph::detail {
    GumGraphContent gumGraphContentOf(const PDAG& g, const std::string& type) {
      return contentOf(g, type);
    }

    PDAG gumGraphPDAGOf(const GumGraphContent& content, const std::string& type) {
      return graphOf< PDAG >(content, type);
    }
  }   // namespace graph::detail

  std::string graphGUMType(std::string_view content) {
    return graph::detail::gumGraphFromString(content).type;
  }

  std::string graphGUMTypeOfFile(std::string_view path, std::optional< bool > binary) {
    return graph::detail::gumGraphFromFile(path, binary).type;
  }

  // the mutable classes: written and read as such
#define GUM_DEFINE_GRAPH_GUM_IO(GRAPH)                                               \
  std::string graphToGUMString(const GRAPH& g, int indent) {                         \
    return graph::detail::gumGraphToString(contentOf(g, #GRAPH), indent);            \
  }                                                                                  \
  void saveGraphGUM(const GRAPH&          g,                                         \
                    std::string_view      path,                                      \
                    std::optional< bool > binary,                                    \
                    int                   indent) {                                  \
    graph::detail::gumGraphToFile(contentOf(g, #GRAPH), path, binary, indent);       \
  }                                                                                  \
  template <>                                                                        \
  GRAPH graphFromGUMString< GRAPH >(std::string_view content) {                      \
    return graphOf< GRAPH >(graph::detail::gumGraphFromString(content), #GRAPH);     \
  }                                                                                  \
  template <>                                                                        \
  GRAPH loadGraphGUM< GRAPH >(std::string_view path, std::optional< bool > binary) { \
    return graphOf< GRAPH >(graph::detail::gumGraphFromFile(path, binary), #GRAPH);  \
  }

  GUM_DEFINE_GRAPH_GUM_IO(DiGraph)
  GUM_DEFINE_GRAPH_GUM_IO(DAG)
  GUM_DEFINE_GRAPH_GUM_IO(UndiGraph)
  GUM_DEFINE_GRAPH_GUM_IO(MixedGraph)
  GUM_DEFINE_GRAPH_GUM_IO(PDAG)
  GUM_DEFINE_GRAPH_GUM_IO(ChainGraph)
  GUM_DEFINE_GRAPH_GUM_IO(MarkedGraph)
  GUM_DEFINE_GRAPH_GUM_IO(PAG)

#undef GUM_DEFINE_GRAPH_GUM_IO

  // the immutable classes: their graph is written, and read back by their checked conversion
#define GUM_DEFINE_IMMUTABLE_GRAPH_GUM_IO(GRAPH, STORAGE, CONVERSION)                   \
  std::string graphToGUMString(const GRAPH& g, int indent) {                            \
    return graph::detail::gumGraphToString(contentOf(g.graph(), #GRAPH), indent);       \
  }                                                                                     \
  void saveGraphGUM(const GRAPH&          g,                                            \
                    std::string_view      path,                                         \
                    std::optional< bool > binary,                                       \
                    int                   indent) {                                     \
    graph::detail::gumGraphToFile(contentOf(g.graph(), #GRAPH), path, binary, indent);  \
  }                                                                                     \
  template <>                                                                           \
  GRAPH graphFromGUMString< GRAPH >(std::string_view content) {                         \
    auto g = graphOf< STORAGE >(graph::detail::gumGraphFromString(content), #GRAPH);    \
    try {                                                                               \
      return GRAPH::CONVERSION(g);                                                      \
    } catch (const Exception&) { graph::detail::rethrowAsIOError(#GRAPH); }             \
  }                                                                                     \
  template <>                                                                           \
  GRAPH loadGraphGUM< GRAPH >(std::string_view path, std::optional< bool > binary) {    \
    auto g = graphOf< STORAGE >(graph::detail::gumGraphFromFile(path, binary), #GRAPH); \
    try {                                                                               \
      return GRAPH::CONVERSION(g);                                                      \
    } catch (const Exception&) { graph::detail::rethrowAsIOError(#GRAPH); }             \
  }

  GUM_DEFINE_IMMUTABLE_GRAPH_GUM_IO(CPDAG, PDAG, fromPDAG)
  GUM_DEFINE_IMMUTABLE_GRAPH_GUM_IO(MPDAG, PDAG, fromPDAG)
  GUM_DEFINE_IMMUTABLE_GRAPH_GUM_IO(MAG, MarkedGraph, fromMarkedGraph)

#undef GUM_DEFINE_IMMUTABLE_GRAPH_GUM_IO

}   // namespace gum

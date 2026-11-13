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


#pragma once

#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/io/graphGumIO.h>
#include <agrum/base/io/GumBinaryIO.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct GraphGumIOTestSuite {
    /// writes g as a string, as a jgum file and as a bgum file, and reads it back in the 3 ways
    template < typename G >
    static void checkRoundTrip(const G& g, const std::string& type) {
      const auto text = gum::graphToGUMString(g, -1);
      CHECK_EQ(gum::graphGUMType(text), type);
      CHECK(gum::graphFromGUMString< G >(text) == g);

      const TemporaryFile jgumFile(".jgum");
      const TemporaryFile bgumFile(".bgum");
      const auto&         jgum = jgumFile.path;
      const auto&         bgum = bgumFile.path;
      gum::saveGraphGUM(g, jgum);
      gum::saveGraphGUM(g, bgum, true);
      CHECK_EQ(gum::graphGUMTypeOfFile(bgum, true), type);
      CHECK(gum::loadGraphGUM< G >(jgum) == g);
      CHECK(gum::loadGraphGUM< G >(bgum, true) == g);
      // the format given by the extension
      gum::saveGraphGUM(g, bgum);
      CHECK_EQ(gum::graphGUMTypeOfFile(bgum), type);
      CHECK_EQ(gum::graphGUMTypeOfFile(jgum), type);
      CHECK(gum::loadGraphGUM< G >(bgum) == g);
    }

    /// the names of the nodes of two graphs are the same
    template < typename G1, typename G2 >
    static void checkNames(const G1& g1, const G2& g2) {
      for (const auto node: g1.nodes()) {
        CHECK_EQ(g1.hasName(node), g2.hasName(node));
        if (g1.hasName(node)) CHECK_EQ(g1.nameFromId(node), g2.nameFromId(node));
      }
    }
  };

  GUM_TEST(MutableGraphs) {
    // isolated nodes, named and unnamed nodes
    auto dag = gum::fastGraph< gum::DAG >("A->B->C;A->C;D");
    checkRoundTrip(dag, "DAG");
    checkNames(dag, gum::graphFromGUMString< gum::DAG >(gum::graphToGUMString(dag)));

    gum::DiGraph cyclic;
    cyclic.addNodes(4);
    cyclic.addArc(0, 1);
    cyclic.addArc(1, 0);
    cyclic.setName(2, "isolated");
    checkRoundTrip(cyclic, "DiGraph");

    checkRoundTrip(gum::fastGraph< gum::UndiGraph >("A-B-C;D"), "UndiGraph");
    checkRoundTrip(gum::fastGraph< gum::MixedGraph >("A->B-C;D"), "MixedGraph");
    checkRoundTrip(gum::fastGraph< gum::PDAG >("A->B-C;D"), "PDAG");
    checkRoundTrip(gum::fastGraph< gum::ChainGraph >("A->B-C;D"), "ChainGraph");

    const auto marked = gum::fastMarkedGraph("A o-> B;B<->C;C-D;D o-o E;F");
    checkRoundTrip(marked, "MarkedGraph");
    const auto read = gum::graphFromGUMString< gum::MarkedGraph >(gum::graphToGUMString(marked));
    checkNames(marked, read);
    checkRoundTrip(gum::fastMarkedGraph< gum::PAG >("A o-> B;B<->C;D"), "PAG");
  }

  GUM_TEST(ImmutableGraphs) {
    const auto dag   = gum::fastGraph< gum::DAG >("A->C<-B;C->D;E->F");
    const auto cpdag = gum::CPDAG::fromDAG(dag);
    checkRoundTrip(cpdag, "CPDAG");
    checkNames(cpdag.graph(),
               gum::graphFromGUMString< gum::CPDAG >(gum::graphToGUMString(cpdag)).graph());

    checkRoundTrip(gum::MPDAG::meekClosure(gum::PDAG(dag)), "MPDAG");
    checkRoundTrip(gum::MAG::fromDAG(dag, gum::NodeSet{dag.idFromName("C").value()}), "MAG");
  }

  GUM_TEST(Errors) {
    const auto dag = gum::fastGraph< gum::DAG >("A->B");
    // a DAG is not read as another class
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(gum::graphToGUMString(dag)),
                    gum::IOError&);
    // not a GUM graph
    CHECK_THROWS_AS(gum::graphGUMType("{\"nodes\": [0]}"), gum::IOError&);
    CHECK_THROWS_AS(gum::graphGUMType("not json"), gum::IOError&);
    CHECK_THROWS_AS(gum::loadGraphGUM< gum::DAG >("/nonexistent/dir/g.jgum"), gum::IOError&);
    // a content which does not respect the invariants of its class is a malformed GUM graph
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DAG >(
                        R"({"type": "DAG", "nodes": [0, 1], "arcs": [[0, 1], [1, 0]]})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::CPDAG >(
                        R"({"type": "CPDAG", "nodes": [0, 1, 2], "arcs": [[0, 1], [1, 2]]})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::MAG >(
                        R"({"type": "MAG", "nodes": [0, 1, 2],
                            "markedEdges": [[0, 1, "tail", "arrowhead"], [1, 2, "tail", "arrowhead"],
                                            [2, 0, "tail", "arrowhead"]]})"),
                    gum::IOError&);
    // a content which is not a graph
    CHECK_THROWS_AS(
        gum::graphFromGUMString< gum::DiGraph >(R"({"type": "DiGraph", "nodes": [0, 0]})"),
        gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "nodes": [0], "arcs": [[0, 1]]})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "nodes": [0], "names": {"1": "a"}})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "nodes": [0], "names": {"-1": "a"}})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "nodes": [0], "names": {"0x": "a"}})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::PAG >(
                        R"({"type": "PAG", "nodes": [0, 1], "markedEdges": [[0, 1, "o", ">"]]})"),
                    gum::IOError&);
  }

  GUM_TEST(MalformedNodeIdsAndTuples) {
    // a node id is an unsigned integer: not a negative, real or boolean value
    for (const char* content: {R"({"type": "DiGraph", "nodes": [-1]})",
                               R"({"type": "DiGraph", "nodes": [2.7]})",
                               R"({"type": "DiGraph", "nodes": [true]})",
                               R"({"type": "DiGraph", "nodes": [1e30]})",
                               R"({"type": "DiGraph", "nodes": [0, 1], "arcs": [[0, -1]]})",
                               R"({"type": "DiGraph", "nodes": [0, 1], "arcs": [[0.0, 1]]})"})
      CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(content), gum::IOError&);
    // the containers are arrays, the tuples have their exact size
    for (const char* content: {R"({"type": "DiGraph", "nodes": {"0": 0}})",
                               R"({"type": "DiGraph", "nodes": [0, 1], "arcs": {"a": [0, 1]}})",
                               R"({"type": "DiGraph", "nodes": [0, 1, 2], "arcs": [[0, 1, 2]]})",
                               R"({"type": "DiGraph", "nodes": [0, 1], "arcs": [[0]]})"})
      CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(content), gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::PAG >(
                        R"({"type": "PAG", "nodes": [0, 1], "markedEdges": [[0, 1, "tail"]]})"),
                    gum::IOError&);
    // nothing is written twice
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "nodes": [0, 1], "arcs": [[0, 1], [0, 1]]})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::UndiGraph >(
                        R"({"type": "UndiGraph", "nodes": [0, 1], "edges": [[0, 1], [1, 0]]})"),
                    gum::IOError&);
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::PAG >(
                        R"({"type": "PAG", "nodes": [0, 1],
                            "markedEdges": [[0, 1, "tail", "arrowhead"], [1, 0, "tail", "arrowhead"]]})"),
                    gum::IOError&);
    // the version: 1.x only
    CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(
                        R"({"type": "DiGraph", "GumJsonVersion": "2.0", "nodes": [0]})"),
                    gum::IOError&);
    CHECK_NOTHROW(gum::graphFromGUMString< gum::DiGraph >(
        R"({"type": "DiGraph", "GumJsonVersion": "1.3", "nodes": [0]})"));
    CHECK_NOTHROW(gum::graphFromGUMString< gum::DiGraph >(R"({"type": "DiGraph", "nodes": [0]})"));
  }

  GUM_TEST(LargeNodeIds) {
    // a graph stores its absent ids lower than the largest one (holes): at most max(#nodes, 65536)
    for (const char* content: {R"({"type": "DiGraph", "nodes": [1000000000]})",
                               R"({"type": "DiGraph", "nodes": [0, 18446744073709551615]})",
                               R"({"type": "DiGraph", "nodes": [0, 1, 65539]})"})
      CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(content), gum::IOError&);
    const auto sparse = gum::graphFromGUMString< gum::DiGraph >(
        R"({"type": "DiGraph", "nodes": [0, 1, 65538]})");   // 65536 holes
    CHECK_EQ(sparse.size(), gum::Size(3));
    CHECK(sparse.existsNode(65538));
    checkRoundTrip(sparse, "DiGraph");
  }

  GUM_TEST(NamesMustBeAnObject) {
    for (const char* content: {R"({"type": "DiGraph", "nodes": [0, 1], "names": ["A", "B"]})",
                               R"({"type": "DiGraph", "nodes": [0, 1], "names": null})"})
      CHECK_THROWS_AS(gum::graphFromGUMString< gum::DiGraph >(content), gum::IOError&);
  }

  GUM_TEST(BinaryLengthIsLittleEndianWhateverTheHost) {
    const TemporaryFile file(".bgum");
    const auto&         path = file.path;
    gum::saveGraphGUM(gum::fastGraph< gum::DAG >("A->B->C"), path);
    std::ifstream     input(path, std::ios::binary);
    const std::string bytes{std::istreambuf_iterator< char >(input),
                            std::istreambuf_iterator< char >()};
    REQUIRE_GT(bytes.size(), std::size_t(8));
    uint64_t length = 0;
    for (std::size_t i = 0; i < 8; ++i)
      length |= static_cast< uint64_t >(static_cast< unsigned char >(bytes[i])) << (8 * i);
    CHECK_EQ(length, bytes.size() - 8);
  }

  GUM_TEST(BinaryFileRefusesDataAfterThePayload) {
    const TemporaryFile file(".bgum");
    const auto&         path = file.path;
    gum::saveGraphGUM(gum::fastGraph< gum::DAG >("A->B->C"), path);
    {
      std::ofstream output(path, std::ios::binary | std::ios::app);
      output << 'x';
    }
    CHECK_THROWS_AS(gum::loadGraphGUM< gum::DAG >(path), gum::IOError&);
  }

  GUM_TEST(BinaryFileRefusesAWrongLength) {
    const TemporaryFile file(".bgum");
    const auto&         path = file.path;
    // a truncated length field (4 bytes instead of 8), a length far above the size of the file,
    // a length above the size of the payload (truncated data)
    const std::string contents[] = {std::string(4, '\0'),
                                    std::string(8, '\xFF'),
                                    std::string("\x64\0\0\0\0\0\0\0", 8) + std::string(10, 'x')};
    for (const auto& content: contents) {
      {
        std::ofstream output(path, std::ios::binary);
        output << content;
      }
      CHECK_THROWS_AS(gum::loadGraphGUM< gum::DAG >(path), gum::IOError&);
    }
  }

  GUM_TEST(BinaryFileAcceptsNodeIdsStoredAsSignedIntegers) {
    // msgpack encoders other than aGrUM's may store a non-negative integer as int8...int64
    const TemporaryFile file(".bgum");
    const auto&         path  = file.path;
    const auto          write = [&path](const std::string& payload) {
      std::ofstream output(path, std::ios::binary);
      for (std::size_t i = 0; i < 8; ++i)   // little-endian length of the payload
        output.put(static_cast< char >((payload.size() >> (8 * i)) & 0xFF));
      output << payload;
    };
    // {"type": "DiGraph", "nodes": [int8 0, int16 1], "arcs": [[int32 0, int64 1]]}
    const std::string head = std::string("\x83\xA4type\xA7"
                                         "DiGraph\xA5nodes\x92\xD0\x00",
                                         23);
    const std::string tail = std::string("\xA4"
                                         "arcs\x91\x92\xD2\x00\x00\x00\x00"
                                         "\xD3\x00\x00\x00\x00\x00\x00\x00\x01",
                                         21);
    write(head + std::string("\xD1\x00\x01", 3) + tail);
    const auto g = gum::loadGraphGUM< gum::DiGraph >(path);
    CHECK_EQ(g.size(), gum::Size(2));
    CHECK(g.existsArc(0, 1));
    // a negative value stays refused: int16 -1 as the second node
    write(head + std::string("\xD1\xFF\xFF", 3) + tail);
    CHECK_THROWS_AS(gum::loadGraphGUM< gum::DiGraph >(path), gum::IOError&);
  }

  GUM_TEST(FormatOfTheFile) {
    CHECK_EQ(gum::binaryOfGUMExtension("g.bgum"), std::optional< bool >(true));
    CHECK_EQ(gum::binaryOfGUMExtension("g.JGUM"), std::optional< bool >(false));
    CHECK(!gum::binaryOfGUMExtension("g.gum").has_value());
    CHECK(gum::isBinaryGUMFile("g.bgum"));
    CHECK(!gum::isBinaryGUMFile("g.jgum"));
    CHECK(gum::isBinaryGUMFile("g.graph"));   // binary for another extension
    CHECK(!gum::isBinaryGUMFile("g.graph", false));
    CHECK_THROWS_AS(gum::isBinaryGUMFile("g.jgum", true), gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::isBinaryGUMFile("g.bgum", false), gum::InvalidArgument&);

    const auto          g = gum::fastGraph< gum::DAG >("A->B->C");
    const TemporaryFile otherFile(".graph");
    const auto&         other = otherFile.path;
    gum::saveGraphGUM(g, other);   // binary
    std::ifstream input(other, std::ios::binary);
    CHECK_NE(input.get(), int('{'));
    CHECK(gum::loadGraphGUM< gum::DAG >(other) == g);
    CHECK(gum::loadGraphGUM< gum::DAG >(other, true) == g);
    const TemporaryFile jgum(".jgum");
    CHECK_THROWS_AS(gum::saveGraphGUM(g, jgum.path, true), gum::InvalidArgument&);
  }

}   // namespace gum_tests

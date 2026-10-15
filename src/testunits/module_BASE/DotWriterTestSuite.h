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


#include <string>

#include <agrum/base/core/dot/dotWriter.h>
#include <agrum/base/core/exceptions.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DotWriterTestSuite {};

  GUM_TEST(EmptyGraphs) {
    CHECK_EQ(gum::dot::DotWriter().str(), "digraph {\n}\n");
    CHECK_EQ(gum::dot::DotWriter(false).str(), "graph {\n}\n");
    CHECK_EQ(gum::dot::DotWriter(true, "bn").str(), "digraph \"bn\" {\n}\n");
    CHECK_EQ(gum::dot::DotWriter(false, "a \"b\"").str(), "graph \"a \\\"b\\\"\" {\n}\n");
  }

  GUM_TEST(QuoteEscapes) {
    using gum::dot::DotWriter;
    CHECK_EQ(DotWriter::quote(""), "\"\"");
    CHECK_EQ(DotWriter::quote("A"), "\"A\"");
    CHECK_EQ(DotWriter::quote("(0) A b"), "\"(0) A b\"");
    CHECK_EQ(DotWriter::quote("say \"hi\""), "\"say \\\"hi\\\"\"");
    CHECK_EQ(DotWriter::quote("a\\b"), "\"a\\\\b\"");
    CHECK_EQ(DotWriter::quote("a\nb\rc"), "\"a\\nb\\rc\"");
    CHECK_EQ(DotWriter::quote("é, ü, 日本"), "\"é, ü, 日本\"");
    CHECK_EQ(DotWriter::quote("{a|b} <c> #d;"), "\"{a|b} <c> #d;\"");
  }

  GUM_TEST(Numbers) {
    using gum::dot::DotWriter;
    CHECK_EQ(DotWriter::number(0.5), "0.5");
    CHECK_EQ(DotWriter::number(1.0), "1");
    CHECK_EQ(DotWriter::number(0.1), "0.1");
    CHECK_EQ(DotWriter::number(-2.25), "-2.25");
    CHECK_EQ(DotWriter::number(0.0), "0");
    CHECK_EQ(DotWriter::number(12345.678), "12345.678");
    CHECK_EQ(DotWriter::number(1e-5), "1e-05");
    // the text reads back as the same number
    for (const double x:
         {0.3, 1.0 / 3.0, 0.1 + 0.2, 1e22, 123456789.123456789, 2.2250738585072014e-308})
      CHECK_EQ(std::stod(DotWriter::number(x)), x);
  }

  GUM_TEST(NodesAndEdges) {
    gum::dot::DotWriter w(true, "g");
    w.node("A");
    w.node("B C", {{"label", "(1) B C"}, {"fillcolor", "#404040"}});
    w.edge("A", "B C");
    w.edge("B C", "A", {{"penwidth", "1.5"}, {"color", "red"}});
    CHECK_EQ(w.str(),
             "digraph \"g\" {\n"
             "  \"A\";\n"
             "  \"B C\" [label=\"(1) B C\", fillcolor=\"#404040\"];\n"
             "  \"A\" -> \"B C\";\n"
             "  \"B C\" -> \"A\" [penwidth=\"1.5\", color=\"red\"];\n"
             "}\n");
  }

  GUM_TEST(NodesDesignatedByNumbers) {
    gum::dot::DotWriter w;
    w.node(0, {{"label", "(0) alpha"}});
    w.node(1);
    w.edge(0, 1);
    w.edge(1, 12, {{"color", "red"}});
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  0 [label=\"(0) alpha\"];\n"
             "  1;\n"
             "  0 -> 1;\n"
             "  1 -> 12 [color=\"red\"];\n"
             "}\n");

    gum::dot::DotWriter u(false);
    u.edge(2, 3);
    CHECK_EQ(u.str(), "graph {\n  2 -- 3;\n}\n");
  }

  GUM_TEST(UndirectedEdges) {
    gum::dot::DotWriter w(false);
    w.edge("A", "B", {{"style", "dashed"}});
    CHECK_EQ(w.str(), "graph {\n  \"A\" -- \"B\" [style=\"dashed\"];\n}\n");
  }

  GUM_TEST(DefaultsAndGraphAttributes) {
    gum::dot::DotWriter w;
    w.graphAttributes({{"bgcolor", "transparent"}, {"size", "5"}});
    w.nodeDefaults({{"style", "filled"}, {"fontname", "Helvetica"}});
    w.edgeDefaults({{"color", "#4A4A4A"}});
    w.graphAttribute({"rankdir", "LR"});
    // nothing to say : nothing written
    w.graphAttributes({});
    w.nodeDefaults({});
    w.edgeDefaults({});
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  graph [bgcolor=\"transparent\", size=\"5\"];\n"
             "  node [style=\"filled\", fontname=\"Helvetica\"];\n"
             "  edge [color=\"#4A4A4A\"];\n"
             "  rankdir=\"LR\";\n"
             "}\n");
  }

  GUM_TEST(ValuesAreEscaped) {
    gum::dot::DotWriter w;
    w.node("a\"b", {{"tooltip", "line1\nline2 \"q\" \\"}});
    w.edge("a\"b", "c\\d", {{"label", "x\ry"}});
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  \"a\\\"b\" [tooltip=\"line1\\nline2 \\\"q\\\" \\\\\"];\n"
             "  \"a\\\"b\" -> \"c\\\\d\" [label=\"x\\ry\"];\n"
             "}\n");
  }

  GUM_TEST(VerbatimValues) {
    gum::dot::DotWriter w;
    w.node("A",
           {{"label", "<<b>A</b>>", true},
            {"xlabel", "\"left\\lright\\l\"", true},
            {"width", "1", false}});
    CHECK_EQ(
        w.str(),
        "digraph {\n  \"A\" [label=<<b>A</b>>, xlabel=\"left\\lright\\l\", width=\"1\"];\n}\n");
  }

  GUM_TEST(Subgraphs) {
    gum::dot::DotWriter w;
    w.beginSubgraph("cluster_0", {{"label", "slice 0"}, {"bgcolor", "#DDDDDD"}});
    w.node("A0");
    w.beginSubgraph("", {{"rank", "same"}});
    w.node("B0");
    w.endSubgraph();
    w.endSubgraph();
    w.edge("A0", "B0");
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  subgraph \"cluster_0\" {\n"
             "    label=\"slice 0\";\n"
             "    bgcolor=\"#DDDDDD\";\n"
             "    \"A0\";\n"
             "    subgraph {\n"
             "      rank=\"same\";\n"
             "      \"B0\";\n"
             "    }\n"
             "  }\n"
             "  \"A0\" -> \"B0\";\n"
             "}\n");
  }

  GUM_TEST(StrCanBeCalledAnyTimeWithoutClosingTheWriter) {
    gum::dot::DotWriter w;
    w.node("A");
    const std::string first = w.str();
    CHECK_EQ(first, "digraph {\n  \"A\";\n}\n");
    CHECK_EQ(w.str(), first);
    w.node("B");
    CHECK_EQ(w.str(), "digraph {\n  \"A\";\n  \"B\";\n}\n");
  }

  GUM_TEST(Errors) {
    gum::dot::DotWriter w;
    // keys which are not identifiers
    for (const std::string& key: {"", "a b", "a\"b", "1a", "a-b", "a=b", "a;", "label]"}) {
      INFO("key '" << key << "'");
      CHECK_THROWS_AS(w.node("A", {{key, "v"}}), gum::InvalidArgument);
      CHECK_THROWS_AS(w.graphAttribute({key, "v"}), gum::InvalidArgument);
      CHECK_THROWS_AS(w.nodeDefaults({{key, "v"}}), gum::InvalidArgument);
      CHECK_THROWS_AS(w.beginSubgraph("s", {{key, "v"}}), gum::InvalidArgument);
    }
    // ... and nothing has been written
    CHECK_EQ(w.str(), "digraph {\n}\n");
    for (const std::string& key: {"a", "A", "_a", "a1", "font_name", "_"})
      CHECK_NOTHROW(w.node("A", {{key, "v"}}));

    // a subgraph which is closed too often, or not closed
    gum::dot::DotWriter v;
    CHECK_THROWS_AS(v.endSubgraph(), gum::OperationNotAllowed);
    v.beginSubgraph("cluster_0");
    CHECK_THROWS_AS(v.str(), gum::OperationNotAllowed);
    v.endSubgraph();
    CHECK_NOTHROW(v.str());
    CHECK_THROWS_AS(v.endSubgraph(), gum::OperationNotAllowed);
  }

}   // namespace gum_tests

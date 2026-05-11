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


#include <iostream>
#include <sstream>
#include <string>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/algorithms/generic/bayesBall.h>
#include <agrum/BN/algorithms/BayesBall.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  BayesBall
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct BayesBallTestSuite {
    public:
    // --- Tests for graph::requisiteNodes / graph::dConnected ---
    // These exercise the optimised _bayesBall_ (getWithDefault marks, CTAD pairs).

    // Chain A→B→C — no evidence: all nodes reachable from query C upward.
    static void testGenericChainNoEvidence() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(b, c);

      gum::NodeSet result = gum::graph::requisiteNodes(dag, {c});
      CHECK(result.exists(a));
      CHECK(result.exists(b));
      CHECK(result.exists(c));
      CHECK(result.size() == 3);
    }

    // Chain A→B→C — B hard evidence: blocks propagation to A.
    static void testGenericChainHardEvidence() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(b, c);

      // hard evidence on B: B is in result (visited from child C) but A is not reached
      gum::NodeSet result = gum::graph::requisiteNodes(dag, {c}, {b});
      CHECK(result.exists(b));
      CHECK(result.exists(c));
      CHECK(!result.exists(a));
    }

    // V-structure A→B←C — B not observed: A and C d-separated (collider blocked).
    static void testGenericColliderBlocked() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(c, b);

      // query = {a}: upward from a → no parents; downward to b (from_child=false).
      // b has no evidence → downward path blocked at collider.
      gum::NodeSet result = gum::graph::dConnected(dag, {a});
      CHECK(result.exists(a));
      CHECK(result.exists(b));   // b visited (from parent), added by CollectAll
      CHECK(!result.exists(c));  // c never activated: collider with no evidence
    }

    // V-structure A→B←C — B soft evidence: collider activated, C reachable.
    static void testGenericColliderSoftEvidence() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(c, b);

      gum::NodeSet result = gum::graph::dConnected(dag, {a}, {}, {b});
      CHECK(result.exists(a));
      CHECK(result.exists(b));
      CHECK(result.exists(c));   // c reached via collider activation
    }

    // V-structure A→B←C — B hard evidence: collider activated, C reachable.
    static void testGenericColliderHardEvidence() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(c, b);

      // hard evidence on b: from_child=false path, is_hard=true → upward propagation to parents
      gum::NodeSet result = gum::graph::dConnected(dag, {a}, {b});
      CHECK(result.exists(a));
      CHECK(result.exists(b));
      CHECK(result.exists(c));
    }

    // Fork A←B→C — B hard evidence: A and C d-separated.
    static void testGenericForkHardEvidenceBlocks() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(b, a);
      dag.addArc(b, c);

      // query={a}: upward → b (hard evidence, continue path), so c unreachable
      gum::NodeSet result = gum::graph::dConnected(dag, {a}, {b});
      CHECK(result.exists(a));
      CHECK(result.exists(b));   // b visited from child, added by CollectAll before continue
      CHECK(!result.exists(c));
    }

    // Fork A←B→C — no evidence: all three reachable.
    static void testGenericForkNoEvidence() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(b, a);
      dag.addArc(b, c);

      gum::NodeSet result = gum::graph::dConnected(dag, {a});
      CHECK(result.exists(a));
      CHECK(result.exists(b));
      CHECK(result.exists(c));
    }

    // dConnected vs requisiteNodes: different CollectAll semantics.
    // Chain A→B→C, query={a}, no evidence.
    // dConnected collects all visited; requisiteNodes only upward visits + activated colliders.
    static void testGenericDConnectedVsRequisite() {
      gum::DAG dag;
      auto     a = dag.addNode();
      auto     b = dag.addNode();
      auto     c = dag.addNode();
      dag.addArc(a, b);
      dag.addArc(b, c);

      auto dc  = gum::graph::dConnected(dag, {a});
      auto req = gum::graph::requisiteNodes(dag, {a});

      // Both reachable from a; dConnected includes b and c (downward visits counted)
      CHECK(dc.exists(a));
      CHECK(dc.exists(b));
      CHECK(dc.exists(c));

      // requisiteNodes: a is requisite (queried); b and c are visited from parent
      // (from_child=false), not from child, so not added unless evidence activates collider.
      // In a chain with no evidence, b and c are NOT upward-visited from a's perspective.
      CHECK(req.exists(a));
      CHECK(!req.exists(b));
      CHECK(!req.exists(c));
    }

    // Empty query: result must be empty.
    static void testGenericEmptyQuery() {
      gum::DAG dag;
      auto     a = dag.addNode();
      dag.addNode();
      dag.addArc(a, dag.addNode());

      CHECK(gum::graph::requisiteNodes(dag, {}).empty());
      CHECK(gum::graph::dConnected(dag, {}).empty());
    }

    // --- Original tests ---

    static void testRequisiteNodes() {
      gum::SimpleBayesNetGenerator< double > gen(50, 200, 2);
      gum::BayesNet< double >                bn;
      gen.generateBN(bn);
      gum::Set< gum::NodeId > requisite;

      gum::Set< gum::NodeId >      query, hardEvidence, softEvidence;
      gum::Sequence< gum::NodeId > nodes_seq;
      for (const auto node: bn.nodes())
        nodes_seq.insert(node);

      for (gum::Idx i = 0; i < 5; ++i)
        hardEvidence.insert(nodes_seq.atPos(i));

      for (gum::Idx j = 24; j > 19; --j)
        query.insert(nodes_seq.atPos(j));

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));

      CHECK(requisite.size() >= 5);
    }   // namespace gum_tests

    static void testRelevantTensors() {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      CHECK(nbrErr == gum::Size(0));

      gum::Sequence< gum::NodeId >             nodes_seq;
      gum::Set< const gum::Tensor< double >* > tensors;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        tensors.insert(&bn.cpt(node));
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      CHECK_NOTHROW(
          gum::BayesBall::relevantTensors(bn, query, hardEvidence, softEvidence, tensors));

      CHECK(tensors.size() == 5);
    }

    static void testRelevantScheduleMultiDims() {
      std::string file = GET_RESSOURCES_PATH("bif/asia.bif");

      gum::BayesNet< double >  bn;
      gum::BIFReader< double > reader(&bn, file);
      gum::Size                nbrErr = reader.proceed();
      CHECK(nbrErr == gum::Size(0));

      gum::Sequence< gum::NodeId >                                      nodes_seq;
      gum::Set< const gum::ScheduleMultiDim< gum::Tensor< double > >* > tensors;

      for (const auto node: bn.nodes()) {
        nodes_seq.insert(node);
        auto sched = new gum::ScheduleMultiDim< gum::Tensor< double > >(bn.cpt(node), false, node);
        tensors.insert(sched);
        // std::cout << node << " : " << bn.variable(node).name() << std::endl;
      }
      gum::Set< const gum::ScheduleMultiDim< gum::Tensor< double > >* > pots = tensors;

      gum::Set< gum::NodeId > requisite;
      gum::Set< gum::NodeId > query, hardEvidence, softEvidence;

      query.insert(gum::NodeId(1));          // tuberculosis
      hardEvidence.insert(gum::NodeId(5));   // smoking

      CHECK_NOTHROW(
          gum::BayesBall::requisiteNodes(bn.dag(), query, hardEvidence, softEvidence, requisite));
      CHECK_NOTHROW(
          gum::BayesBall::relevantTensors(bn, query, hardEvidence, softEvidence, tensors));

      CHECK(tensors.size() == 5);

      for (const auto pot: pots)
        delete pot;
    }
  };

  GUM_TEST_ACTIF(GenericChainNoEvidence)
  GUM_TEST_ACTIF(GenericChainHardEvidence)
  GUM_TEST_ACTIF(GenericColliderBlocked)
  GUM_TEST_ACTIF(GenericColliderSoftEvidence)
  GUM_TEST_ACTIF(GenericColliderHardEvidence)
  GUM_TEST_ACTIF(GenericForkHardEvidenceBlocks)
  GUM_TEST_ACTIF(GenericForkNoEvidence)
  GUM_TEST_ACTIF(GenericDConnectedVsRequisite)
  GUM_TEST_ACTIF(GenericEmptyQuery)
  GUM_TEST_ACTIF(RequisiteNodes)
  GUM_TEST_ACTIF(RelevantTensors)
  GUM_TEST_ACTIF(RelevantScheduleMultiDims)
}   // namespace gum_tests

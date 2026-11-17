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
#include <iostream>
#include <string>

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/threads/threadExecutor.h>
#include <agrum/base/core/threads/threads.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CN/credalNet.h>
#include <agrum/CN/inference/CNLoopyPropagation.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {

  ////////////////////////////////////////////////////////////////////
  class L2UListener: public gum::ApproximationSchemeListener {
    protected:
    int         nbr_;
    std::string msg_;

    public:
    explicit L2UListener(gum::ApproximationScheme& aS) :
        gum::ApproximationSchemeListener(aS), nbr_(0), msg_("") {}

    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) override {
      nbr_++;
    }

    // the last period is notified by whenStop, not by whenProgress
    void whenStop(const void* buffer, std::string_view s) override {
      nbr_++;
      msg_ = s;
    }

    int nbr() const { return nbr_; }

    std::string msg() const { return msg_; }
  };   // end of : class l2uListener

  ////////////////////////////////////////////////////////////////
  struct CNLoopyPropagationTestSuite {
    public:
    gum::credal::CredalNet< double >* cn;

    // not dynamic (2U network - fast)
    void initCNet() {
#ifdef _OPENMP
      gum::threadsOMP::setNumberOfThreads(1);
#endif
      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_RESSOURCES_PATH("cn/2Umin.bif"));
      REQUIRE_EQ(readera.proceed(), gum::Size(0));

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_RESSOURCES_PATH("cn/2Umax.bif"));
      REQUIRE_EQ(readerb.proceed(), gum::Size(0));

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeBinaryCPTMinMax();
    }

    // dynamic (dynaCheese network - slow)
    void initDCNet() {
#ifdef _OPENMP
      gum::threadsOMP::setNumberOfThreads(1);
#endif
      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(&monBNa, GET_RESSOURCES_PATH("cn/dbn_bin_min.bif"));
      REQUIRE_EQ(readera.proceed(), gum::Size(0));

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(&monBNb, GET_RESSOURCES_PATH("cn/dbn_bin_max.bif"));
      REQUIRE_EQ(readerb.proceed(), gum::Size(0));

      cn = new gum::credal::CredalNet< double >(monBNa, monBNb);

      cn->intervalToCredal();
      cn->computeBinaryCPTMinMax();
    }

    void clearCNet() { delete cn; }

    // not dynamic (2U network) - with evidence
    // end of : testL2UInference (2U network)

    // dynamic (dynaCheese) - strong indep
    // end of : testL2UInferenceD

    // with dynamic network
    // end of : testL2UListener

    // not dynamic (2U network) - with evidence


  };   // end of : class L2UInferenceTestSuite

  GUM_TEST(L2UInference) {
    initCNet();

    gum::credal::CNLoopyPropagation< double > lp = gum::credal::CNLoopyPropagation< double >(*cn);

    // evidence from file
    try {
      lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/L2U.evi"));
    } catch (gum::Exception&) { CHECK(false); }

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    // evidence from map
    std::map< std::string, std::vector< double > > eviMap;
    std::vector< double >                          evi0(2, 0);
    evi0[0] = 1;
    std::vector< double > evi1(2, 0);
    evi1[1]     = 1;
    eviMap["L"] = evi1;
    eviMap["G"] = evi0;

    try {
      lp.insertEvidence(eviMap);
    } catch (gum::Exception&) { CHECK(false); }

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    std::map< std::string, std::vector< double > > modals;
    std::vector< double >                          binaryModal(2, 0);
    binaryModal[1] = 1;

    // modalities from map
    // from file with dynamic network, not 2U
    try {
      for (const auto node: cn->current_bn().nodes())
        modals[cn->current_bn().variable(node).name()] = binaryModal;
    } catch (gum::Exception&) { CHECK(false); }

    /*try {
      lp.insertModals( modals );
    } catch ( gum::Exception & ) {
      CHECK( false );
    }*/

    try {
      lp.makeInference();
    } catch (gum::Exception&) { CHECK(false); }

    try {
      for (const auto node: cn->current_bn().nodes()) {
        auto tmin = lp.marginalMin(node);
        auto tmax = lp.marginalMax(node);
        for (gum::Instantiation i(tmin); !i.end(); ++i) {
          CHECK(tmin.get(i) >= 0.0);
          CHECK(tmin.get(i) <= 1.0);
          CHECK(tmax.get(i) >= 0.0);
          CHECK(tmax.get(i) <= 1.0);
          CHECK(tmin.get(i) <= tmax.get(i));
        }
      }
    } catch (gum::Exception&) { CHECK(false); }

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    clearCNet();
  }

  GUM_TEST(L2UInferenceD) {
    initDCNet();

    gum::credal::CNLoopyPropagation< double > lp = gum::credal::CNLoopyPropagation< double >(*cn);

    //////////////////////////////////////////////////////
    // strong independence
    //////////////////////////////////////////////////////

    // evidence from file
    try {
      lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/dbn_bin_evi.evi"));
    } catch (gum::Exception&) { CHECK(false); }

    // modalities from file
    /*try {
      lp.insertModalsFile( GET_RESSOURCES_PATH ( modalities.modal ) );
    } catch ( gum::Exception & ) {
      CHECK( false );
    }*/

    try {
      lp.makeInference();
    } catch (gum::Exception&) { CHECK(false); }

    try {
      for (const auto node: cn->current_bn().nodes()) {
        auto tmin(lp.marginalMin(node));
        auto tmax(lp.marginalMax(node));
        for (gum::Instantiation i(tmin); !i.end(); ++i) {
          CHECK(tmin.get(i) >= 0.0);
          CHECK(tmin.get(i) <= 1.0);
          CHECK(tmax.get(i) >= 0.0);
          CHECK(tmax.get(i) <= 1.0);
          CHECK(tmin.get(i) <= tmax.get(i));
        }
      }
    } catch (gum::Exception&) { CHECK(false); }

    /*
    try {
      lp.dynamicExpectations();
    } catch ( gum::Exception & ) {
      CHECK( false );
    }*/
    /*
            try {
              //std::vector< double >  ekm_inf ( mcs.dynamicExpMin ( "km" ) );
              //std::vector< double >  ekm_sup ( mcs.dynamicExpMax ( "km" ) );
              //std::vector< double >  elo_inf ( mcs.dynamicExpMin ( "lo" ) );
              //std::vector< double >  elo_sup ( mcs.dynamicExpMax ( "lo" ) );
              //std::vector< double >  etemp_inf ( lp.dynamicExpMin ( "temp" )
       );
              //std::vector< double >  etemp_sup ( lp.dynamicExpMax ( "temp" )
       );
            } catch ( gum::Exception & ) {
              CHECK( false );
            }*/

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    clearCNet();
  }

  GUM_TEST(L2UListener) {
    initDCNet();
    gum::credal::CNLoopyPropagation< double > lp = gum::credal::CNLoopyPropagation< double >(*cn);

    // evidence from file
    try {
      lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/dbn_bin_evi.evi"));
    } catch (gum::Exception&) { CHECK(false); }

    // lp.inferenceType(gum::CNLoopyPropagation<double>::InferenceType::randomOrder);
    L2UListener mcl(lp);

    try {
      lp.makeInference();
    } catch (gum::Exception&) { CHECK(false); }

    CHECK_EQ(mcl.nbr() * lp.periodSize(), lp.nbrIterations());
    CHECK_NE(mcl.msg(), std::string(""));

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    clearCNet();
  }

  GUM_TEST(L2UInferenceFromBug) {
    initCNet();

    gum::credal::CNLoopyPropagation< double > lp = gum::credal::CNLoopyPropagation< double >(*cn);

    // evidence from file
    lp.eraseAllEvidence();
    try {
      lp.insertEvidenceFile(GET_RESSOURCES_PATH("cn/L2U.evi"));
    } catch (gum::Exception&) { CHECK(false); }

    try {
      lp.makeInference();
    } catch (gum::Exception&) { CHECK(false); }

    try {
      for (const auto node: cn->current_bn().nodes()) {
        auto tmin(lp.marginalMin(node));
        auto tmax(lp.marginalMax(node));
        for (gum::Instantiation i(tmin); !i.end(); ++i) {
          CHECK(tmin.get(i) >= 0.0);
          CHECK(tmin.get(i) <= 1.0);
          CHECK(tmax.get(i) >= 0.0);
          CHECK(tmax.get(i) <= 1.0);
          CHECK(tmin.get(i) <= tmax.get(i));
        }
      }
    } catch (gum::Exception&) { CHECK(false); }

    try {
      lp.eraseAllEvidence();
    } catch (gum::Exception&) { CHECK(false); }

    clearCNet();
  }

  GUM_TEST(SaveVerticesFormat) {
    // Bug5: saveVertices wrote [v0v1] instead of [v0,v1] because first=false was
    // only set inside the if(!first) block and never triggered.
    initCNet();

    gum::credal::CNLoopyPropagation< double > lp(*cn);
    lp.storeVertices(true);

    // insertModals required so computeExpectations_() fills marginalSets_
    std::map< std::string, std::vector< double > > modals;
    std::vector< double >                          binaryModal(2, 0.0);
    binaryModal[1] = 1.0;
    for (const auto node: cn->current_bn().nodes())
      modals[cn->current_bn().variable(node).name()] = binaryModal;
    lp.insertModals(modals);

    lp.makeInference();

    std::string path = gum_tests::getTempFilePath("_gum_saveVertices.txt");
    lp.saveVertices(path);

    std::ifstream f(path);
    CHECK(f.good());

    std::string line;
    bool        found_vertex_line = false;
    while (std::getline(f, line)) {
      if (!line.empty() && line.front() == '[') {
        found_vertex_line = true;
        CHECK(line.find(',') != std::string::npos);
      }
    }
    f.close();
    CHECK(found_vertex_line);
    std::remove(path.c_str());

    clearCNet();
  }

  GUM_TEST(L2UDynamicExpectationsWithoutVertices) {
    // computeExpectations_() used to fill marginalSets_ even when the vertices are not stored
    // (NotFound) and never computed the dynamic expectations (OperationNotAllowed)
    initCNet();

    gum::credal::CNLoopyPropagation< double > lp(*cn);

    std::map< std::string, std::vector< double > > modals;
    for (const auto node: cn->current_bn().nodes())
      modals[cn->current_bn().variable(node).name()] = {0.0, 10.0};
    lp.insertModals(modals);

    GUM_CHECK_ASSERT_THROWS_NOTHING(lp.makeInference());

    for (const auto node: cn->current_bn().nodes()) {
      const auto& name = cn->current_bn().variable(node).name();
      const auto  emin = lp.dynamicExpMin(name);
      const auto  emax = lp.dynamicExpMax(name);
      CHECK((emin.size()) == (static_cast< std::size_t >(1)));
      CHECK((emax.size()) == (static_cast< std::size_t >(1)));
      CHECK((emin[0]) == doctest::Approx(lp.expectationMin(name)));
      CHECK((emax[0]) == doctest::Approx(lp.expectationMax(name)));
      CHECK(emin[0] <= emax[0]);
      CHECK(emin[0] >= 0.0);
      CHECK(emax[0] <= 10.0);
    }

    clearCNet();
  }

  GUM_TEST(EpsilonIsTheLargestChangeOfTheMarginals) {
    // epsilon is the largest change of a marginal bound between two iterations: it lies in [0,1],
    // and with the default settings the scheme runs until it falls below the threshold
    initCNet();
    gum::credal::CNLoopyPropagation< double > lp(*cn);
    lp.setVerbosity(true);   // needed to read the history
    lp.insertEvidence(std::map< std::string, std::vector< double > >{{"L", {0.0, 1.0}}});
    lp.makeInference();

    const auto& history = lp.history();
    REQUIRE_FALSE(history.empty());
    for (const auto eps: history) {
      CHECK_GE(eps, 0.0);
      CHECK_LE(eps, 1.0);
    }
    CHECK_LE(history.back(), lp.epsilon());

    clearCNet();
  }

  GUM_TEST(L2UIsExactOnABinaryPolytreeWithEvidence) {
    // on a binary polytree, L2U gives the exact bounds; the evidence below A and B makes them
    // depend on the likelihood intervals sent by the children
    auto bnmin = gum::BayesNet< double >::fastPrototype("A->B->C");
    auto bnmax = gum::BayesNet< double >(bnmin);
    bnmin.cpt("A").fillWith({0.6, 0.3});
    bnmax.cpt("A").fillWith({0.7, 0.4});
    bnmin.cpt("B").fillWith({0.7, 0.2, 0.2, 0.6});
    bnmax.cpt("B").fillWith({0.8, 0.3, 0.4, 0.8});
    bnmin.cpt("C").fillWith({0.8, 0.1, 0.3, 0.5});
    bnmax.cpt("C").fillWith({0.9, 0.2, 0.5, 0.7});

    gum::credal::CredalNet< double > cnet(bnmin, bnmax);
    cnet.intervalToCredal();
    cnet.computeBinaryCPTMinMax();

    gum::credal::CNLoopyPropagation< double > lp(cnet);
    lp.insertEvidence(std::map< std::string, std::vector< double > >{{"C", {0.0, 1.0}}});
    lp.makeInference();

    // probability of the k-th value in a tensor over one variable
    const auto proba = [](const gum::Tensor< double >& t, gum::Idx k) {
      gum::Instantiation inst(t);
      inst.chgVal(0, k);
      return t.get(inst);
    };
    // exact bounds, computed by enumerating the vertices of the credal sets
    const auto& bn = cnet.current_bn();
    CHECK_EQ(proba(lp.marginalMin(bn.idFromName("A")), 1),
             doctest::Approx(0.3596214511).epsilon(1e-6));
    CHECK_EQ(proba(lp.marginalMax(bn.idFromName("A")), 1),
             doctest::Approx(0.6373626374).epsilon(1e-6));
    CHECK_EQ(proba(lp.marginalMin(bn.idFromName("B")), 1),
             doctest::Approx(0.5405405405).epsilon(1e-6));
    CHECK_EQ(proba(lp.marginalMax(bn.idFromName("B")), 1), doctest::Approx(0.875).epsilon(1e-6));
  }

  GUM_TEST(L2URaisesOnImpossibleEvidence) {
    // B (resp. C) never takes the value 1: no message can be computed from an observation B=1
    // (resp. C=1), the inference must fail instead of returning arbitrary bounds
    for (const auto& observed: {"B", "C"}) {
      auto bnmin = gum::BayesNet< double >::fastPrototype("A->B->C");
      bnmin.cpt("A").fillWith({0.6, 0.4});
      bnmin.cpt("B").fillWith({0.7, 0.3, 0.2, 0.8});
      bnmin.cpt("C").fillWith({0.8, 0.2, 0.3, 0.7});
      bnmin.cpt(observed).fillWith({1.0, 0.0, 1.0, 0.0});
      auto bnmax = gum::BayesNet< double >(bnmin);

      gum::credal::CredalNet< double > cnet(bnmin, bnmax);
      cnet.intervalToCredal();
      cnet.computeBinaryCPTMinMax();

      gum::credal::CNLoopyPropagation< double > lp(cnet);
      lp.insertEvidence(std::map< std::string, std::vector< double > >{{observed, {0.0, 1.0}}});
      CHECK_THROWS_AS(lp.makeInference(), gum::IncompatibleEvidence&);
    }
  }

  GUM_TEST(ChangedParameterInvalidatesTheInference) {
    initCNet();
    gum::credal::CNLoopyPropagation< double > lp(*cn);
    // only maxIter stops the scheme (the default inference type also stops when no message
    // changes): the number of iterations shows which parameters were used
    lp.inferenceType(gum::credal::CNLoopyPropagation< double >::InferenceType::ordered);
    lp.disableEpsilon();
    lp.disableMinEpsilonRate();
    lp.disableMaxTime();
    lp.setPeriodSize(1);
    lp.setMaxIter(3);
    lp.makeInference();
    CHECK_EQ(lp.nbrIterations(), static_cast< gum::Size >(3));

    lp.setMaxIter(5);
    lp.makeInference();   // a new computation
    CHECK_EQ(lp.nbrIterations(), static_cast< gum::Size >(5));

    clearCNet();
  }

  GUM_TEST(ANewComputationForgetsTheFormerExpectations) {
    // the expectations, dynamic expectations and vertices of a new computation (after a change of
    // the approximation scheme) are those of a fresh inference: they were accumulated
    initCNet();
    std::map< std::string, std::vector< double > > modals;
    gum::NodeId                                    last = 0;
    for (const auto node: cn->current_bn().nodes()) {
      modals[cn->current_bn().variable(node).name()] = {0.0, 10.0};
      last                                           = node;
    }
    // with evidence, one iteration does not give the result of ten
    const std::map< std::string, std::vector< double > > evidence{
        {cn->current_bn().variable(last).name(), {1.0, 0.0}}};
    const auto configure
        = [&modals, &evidence](gum::credal::CNLoopyPropagation< double >& lp, gum::Size iter) {
            lp.disableEpsilon();
            lp.disableMinEpsilonRate();
            lp.disableMaxTime();
            lp.setPeriodSize(1);
            lp.storeVertices(true);
            lp.insertModals(modals);
            lp.insertEvidence(evidence);
            lp.setMaxIter(iter);
          };

    gum::credal::CNLoopyPropagation< double > again(*cn);
    configure(again, 1);
    again.makeInference();
    gum::NodeProperty< double > firstMin, firstMax;
    for (const auto node: cn->current_bn().nodes()) {
      firstMin.insert(node, again.expectationMin(node));
      firstMax.insert(node, again.expectationMax(node));
    }
    again.setMaxIter(10);
    again.makeInference();

    gum::credal::CNLoopyPropagation< double > fresh(*cn);
    configure(fresh, 10);
    fresh.makeInference();
    // the two computations differ (otherwise the test would not show the accumulation)
    bool differ = false;
    for (const auto node: cn->current_bn().nodes())
      differ = differ || firstMin[node] != doctest::Approx(fresh.expectationMin(node))
            || firstMax[node] != doctest::Approx(fresh.expectationMax(node));
    REQUIRE(differ);

    for (const auto node: cn->current_bn().nodes()) {
      const auto& name = cn->current_bn().variable(node).name();
      CHECK(again.expectationMin(node) == doctest::Approx(fresh.expectationMin(node)));
      CHECK(again.expectationMax(node) == doctest::Approx(fresh.expectationMax(node)));
      CHECK(again.dynamicExpMin(name) == fresh.dynamicExpMin(name));
      CHECK(again.dynamicExpMax(name) == fresh.dynamicExpMax(name));
      CHECK(again.vertices(node) == fresh.vertices(node));
    }
    clearCNet();
  }

  GUM_TEST(L2UConvergesWithTheDefaultCriteria) {
    // on a binary network with loops and evidence, the first error is close to 1 (0.995 here: the
    // evidence changes the bounds of the observed variable): the scheme must not stop there, and
    // the default criteria must give the bounds obtained with very strict criteria. The CPTs are
    // fixed: with node-to-neighbours messages, the error may decrease non monotonically and the
    // default epsilon be reached on a quiet iteration before the convergence (rare, about 1e-5 off)
    auto bn = gum::BayesNet< double >::fastPrototype("A->B->D;A->C->D;D->E;C->E;B->F;E->F");
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.8, 0.2, 0.25, 0.75});
    bn.cpt("C").fillWith({0.6, 0.4, 0.1, 0.9});
    bn.cpt("D").fillWith({0.9, 0.1, 0.4, 0.6, 0.3, 0.7, 0.05, 0.95});
    bn.cpt("E").fillWith({0.7, 0.3, 0.2, 0.8, 0.35, 0.65, 0.15, 0.85});
    bn.cpt("F").fillWith({0.95, 0.05, 0.5, 0.5, 0.6, 0.4, 0.1, 0.9});
    gum::credal::CredalNet< double > cnet(bn, bn);
    cnet.bnToCredal(0.8, false);
    cnet.computeBinaryCPTMinMax();

    const auto evidence = std::map< std::string, std::vector< double > >{{"F", {0.0, 1.0}}};

    gum::credal::CNLoopyPropagation< double > byDefault(cnet);
    byDefault.setVerbosity(true);   // needed to read the history
    byDefault.insertEvidence(evidence);
    byDefault.makeInference();

    gum::credal::CNLoopyPropagation< double > strict(cnet);
    strict.setEpsilon(1e-12);
    strict.setMinEpsilonRate(1e-14);
    strict.setMaxIter(10000);
    strict.insertEvidence(evidence);
    strict.makeInference();

    // one entry of the history per computed iteration (the period size is 1)
    CHECK_EQ(byDefault.history().size(), byDefault.nbrIterations());
    CHECK_GT(byDefault.nbrIterations(), static_cast< gum::Size >(1));
    for (const auto node: cnet.current_bn().nodes()) {
      const auto         defaultMin = byDefault.marginalMin(node);
      const auto         defaultMax = byDefault.marginalMax(node);
      const auto         strictMin  = strict.marginalMin(node);
      const auto         strictMax  = strict.marginalMax(node);
      gum::Instantiation inst(defaultMin);
      for (inst.setFirst(); !inst.end(); ++inst) {
        CHECK_EQ(defaultMin[inst], doctest::Approx(strictMin[inst]).epsilon(1e-6));
        CHECK_EQ(defaultMax[inst], doctest::Approx(strictMax[inst]).epsilon(1e-6));
      }
    }
  }

  GUM_TEST(L2UDefaultsComeFromTheConfiguration) {
    initCNet();
    {
      gum::credal::CNLoopyPropagation< double > lp(*cn);
      CHECK_EQ(lp.epsilon(), doctest::Approx(1e-8));
      CHECK_EQ(lp.minEpsilonRate(), doctest::Approx(1e-10));
      CHECK_EQ(lp.maxIter(), static_cast< gum::Size >(100));
      CHECK_EQ(lp.periodSize(), static_cast< gum::Size >(1));
      CHECK(!lp.verbosity());
      CHECK(!lp.isEnabledMaxTime());   // no timeout
    }
    {
      auto&              conf = gum::Configuration::instance();
      ConfigurationGuard guard;
      conf.set("CNLoopy", "maxIter", "7");
      conf.set("CNLoopy", "epsilon", "0.001");
      conf.set("CNLoopy", "minEpsilonRate", "0.0001");
      conf.set("CNLoopy", "periodSize", "2");
      conf.set("CNLoopy", "verbosity", "True");
      gum::credal::CNLoopyPropagation< double > lp(*cn);
      CHECK_EQ(lp.epsilon(), doctest::Approx(0.001));
      CHECK_EQ(lp.minEpsilonRate(), doctest::Approx(0.0001));
      CHECK_EQ(lp.maxIter(), static_cast< gum::Size >(7));
      CHECK_EQ(lp.periodSize(), static_cast< gum::Size >(2));
      CHECK(lp.verbosity());
    }
    clearCNet();
  }
}   // namespace gum_tests

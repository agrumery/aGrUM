
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <cxxtest/testsuite_utils.h>

#include <memory>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/variables/labelizedVariable.h>


// The graph used for the tests, the ids and the domain sizes:
/*
    A             0             2
     \             \             \
      B   G         1   6         3   4
       \ / \         \ / \         \ / \
        C   H         2   7         2   3
       / \           / \           / \
      D   E         3   4         3   2
     /             /             /
    F             5             3
*/

namespace gum_tests {


  class LazyPropagationIncrementalTestSuite: public CxxTest::TestSuite {
    typedef std::unique_ptr< gum::Potential< double > > __Potential;
    typedef gum::Set< const gum::Potential< double >* > __PotentialSet;

    public:
    // the function used to combine two tables
    static gum::Potential< double >*
       LPIncrmultiPotential(const gum::Potential< double >& t1,
                            const gum::Potential< double >& t2) {
      return new gum::Potential< double >(t1 * t2);
    }

    // the function used to combine two tables
    static gum::Potential< double >* LPIncrprojPotential(
       const gum::Potential< double >&                 t1,
       const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return new gum::Potential< double >(t1.margSumOut(del_vars));
    }


    gum::BayesNet< double >*                    bn;
    std::vector< gum::NodeId >                  BN_node_index;
    std::vector< const gum::DiscreteVariable* > BN_variable;
    gum::Set< const gum::DiscreteVariable* >*   BN_variable_set;

    double __epsilon{1e-6};

    gum::Potential< double >* joint;

    gum::MultiDimCombinationDefault< double, gum::Potential >* combination;
    gum::MultiDimProjection< double, gum::Potential >*         proj;


    LazyPropagationIncrementalTestSuite() {
      combination = new gum::MultiDimCombinationDefault< double, gum::Potential >(
         LPIncrmultiPotential);

      proj = new gum::MultiDimProjection< double, gum::Potential >(
         LPIncrprojPotential);

      BN_variable_set = new gum::Set< const gum::DiscreteVariable* >;


      // create the BN
      std::vector< gum::LabelizedVariable > variables{
         gum::LabelizedVariable("A", "", 2),
         gum::LabelizedVariable("B", "", 3),
         gum::LabelizedVariable("C", "", 2),
         gum::LabelizedVariable("D", "", 3),
         gum::LabelizedVariable("E", "", 2),
         gum::LabelizedVariable("F", "", 3),
         gum::LabelizedVariable("G", "", 4),
         gum::LabelizedVariable("H", "", 3)};

      bn = new gum::BayesNet< double >;

      for (unsigned int i = 0; i < variables.size(); ++i) {
        BN_node_index.push_back(bn->add(variables[i]));
        BN_variable.push_back(&(bn->variable(BN_node_index[i])));
        BN_variable_set->insert(BN_variable[i]);
      }

      bn->addArc(BN_node_index[0], BN_node_index[1]);
      bn->addArc(BN_node_index[1], BN_node_index[2]);
      bn->addArc(BN_node_index[2], BN_node_index[3]);
      bn->addArc(BN_node_index[2], BN_node_index[4]);
      bn->addArc(BN_node_index[3], BN_node_index[5]);
      bn->addArc(BN_node_index[6], BN_node_index[2]);
      bn->addArc(BN_node_index[6], BN_node_index[7]);

      bn->cpt(BN_node_index[0]).fillWith({0.2, 0.8});
      bn->cpt(BN_node_index[1]).fillWith({0.3, 0.4, 0.3, 0.1, 0.3, 0.6});
      bn->cpt(BN_node_index[2]).fillWith({0.1, 0.9,     // 1
                                          0.2, 0.8,     // 2
                                          0.3, 0.7,     // 3
                                          0.4, 0.6,     // 4
                                          0.5, 0.5,     // 5
                                          0.6, 0.4,     // 6
                                          0.7, 0.3,     // 7
                                          0.8, 0.2,     // 8
                                          0.9, 0.1,     // 9
                                          0.8, 0.2,     // 10
                                          0.7, 0.3,     // 11
                                          0.6, 0.4});   // 12
      bn->cpt(BN_node_index[3]).fillWith({0.3, 0.2, 0.5, 0.7, 0.1, 0.2});
      bn->cpt(BN_node_index[4]).fillWith({0.4, 0.6, 0.9, 0.1});
      bn->cpt(BN_node_index[5])
         .fillWith({0.2, 0.4, 0.4, 0.4, 0.1, 0.5, 0.7, 0.2, 0.1});
      bn->cpt(BN_node_index[6]).fillWith({0.1, 0.4, 0.2, 0.3});
      bn->cpt(BN_node_index[7])
         .fillWith({0.4, 0.3, 0.3, 0.1, 0.8, 0.1, 0.3, 0.4, 0.3, 0.7, 0.1, 0.2});

      // create the joint probability
      gum::Set< const gum::Potential< double >* > potset;
      for (unsigned int i = 0; i < BN_node_index.size(); ++i) {
        potset.insert(&(bn->cpt(i)));
      }

      joint = combination->combine(potset);
    }

    void theEnd() {
      delete combination;
      delete proj;
      delete joint;
      delete bn;
      delete BN_variable_set;
      BN_node_index.clear();
      BN_variable.clear();
    }

    void setUp() {}
    void tearDown() {}


    // ============================================================================
    gum::Potential< double >* create_evidence(const gum::NodeId            node_id,
                                              const std::vector< double >& ev) {
      gum::Potential< double >* proba = new gum::Potential< double >;
      proba->add(*(BN_variable[node_id]));
      proba->fillWith(ev);

      return proba;
    }


    // ============================================================================
    gum::Potential< double >*
       posterior_joint(const gum::Potential< double >*             joint,
                       gum::Set< const gum::Potential< double >* > evidence) {
      evidence.insert(joint);
      gum::Potential< double >* joint_pot = combination->combine(evidence);

      return joint_pot;
    }


    // ============================================================================
    gum::Potential< double >* get_marginal(const gum::Potential< double >* joint,
                                           const gum::NodeId target_id) {
      // get the set of variables to erase
      gum::Set< const gum::DiscreteVariable* > myset = *BN_variable_set;
      myset.erase(BN_variable[target_id]);
      gum::Potential< double >* res = LPIncrprojPotential(*joint, myset);
      res->normalize();
      return res;
    }


    // ============================================================================
    gum::Potential< double >* get_joint(const gum::Potential< double >* joint,
                                        const gum::NodeSet& target_ids) {
      // get the set of variables to erase
      gum::Set< const gum::DiscreteVariable* > myset = *BN_variable_set;
      for (auto target_id: target_ids)
        myset.erase(BN_variable[target_id]);
      return proj->project(*joint, myset);
    }


    // ============================================================================
    bool equalPotentials(const gum::Potential< double >& p1,
                         const gum::Potential< double >& p2) {
      gum::Instantiation i1(p1);
      gum::Instantiation i2(p2);

      for (i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc()) {
        if ((p1[i1] == 0) && (std::fabs(p2[i2]) > __epsilon)) return false;
        if (p1[i1] > p2[i2]) {
          if (std::fabs((p1[i1] - p2[i2]) / p1[i1]) > __epsilon) return false;
        } else {
          if (std::fabs((p1[i1] - p2[i2]) / p1[i2]) > __epsilon) return false;
        }
      }

      return true;
    }


    // ============================================================================
    // ============================================================================
    void test_prior() {
      gum::LazyPropagation< double > inf(bn);
      TS_ASSERT_THROWS_NOTHING(inf.makeInference());

      // get the marginals of A, C, D, H
      std::unique_ptr< gum::Potential< double > > pa(
         get_marginal(joint, BN_node_index[0])),
         pc(get_marginal(joint, BN_node_index[2])),
         pd(get_marginal(joint, BN_node_index[3])),
         ph(get_marginal(joint, BN_node_index[7]));

      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[2]), *pc));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[7]), *ph));
    }

    // ============================================================================
    // ============================================================================
    void test_prior_with_targets() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(2);   // C

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());

      // get the marginals of A, C, D
      __Potential pa(get_marginal(joint, BN_node_index[0]));
      __Potential pc(get_marginal(joint, BN_node_index[2]));

      TS_ASSERT_THROWS(inf.posterior(BN_node_index[3]), gum::UndefinedElement);
      TS_ASSERT_THROWS(inf.posterior(BN_node_index[1]), gum::UndefinedElement);

      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[2]), *pc));
    }


    // ============================================================================
    // ============================================================================
    void test_prior_with_targets_evidence() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(5);   // F

      __Potential ev1(create_evidence(1, {0, 0, 1}));
      inf.addEvidence(1, 2);
      __Potential ev3(create_evidence(3, {0.2, 0.6, 0.6}));
      inf.addEvidence(*ev3);

      __PotentialSet evset;
      evset.insert(ev1.get());
      evset.insert(ev3.get());
      __Potential posterior(posterior_joint(joint, evset));

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());

      // get the marginals of A, F
      std::unique_ptr< gum::Potential< double > > pa(
         get_marginal(posterior.get(), BN_node_index[0])),
         pf(get_marginal(posterior.get(), BN_node_index[5]));

      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[5]), *pf));
    }


    // ============================================================================
    // ============================================================================
    void test_prior_with_targets_outside_evidence() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(3);   // D

      __Potential ev0(create_evidence(0, {0.3, 0.7}));
      inf.addEvidence(*ev0);
      __Potential ev1(create_evidence(1, {0.3, 0.1, 0.8}));
      inf.addEvidence(*ev1);
      __Potential ev7(create_evidence(7, {0.4, 0.2, 0.3}));
      inf.addEvidence(*ev7);

      __PotentialSet evset;
      evset.insert(ev0.get());
      evset.insert(ev1.get());
      evset.insert(ev7.get());
      __Potential posterior(posterior_joint(joint, evset));

      TS_ASSERT_THROWS_NOTHING(inf.makeInference());

      // get the marginals of A, D
      std::unique_ptr< gum::Potential< double > > pa(
         get_marginal(posterior.get(), BN_node_index[0])),
         pd(get_marginal(posterior.get(), BN_node_index[3]));

      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
      TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
    }


    // ============================================================================
    // ============================================================================
    void test_prior_with_targets_evidence_values_changed() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(3);   // D

      __Potential ev0(create_evidence(0, {0.3, 0.7}));
      inf.addEvidence(*ev0);
      __Potential ev1(create_evidence(1, {0.3, 0.1, 0.8}));
      inf.addEvidence(*ev1);
      __Potential ev7(create_evidence(7, {0.4, 0.2, 0.3}));
      inf.addEvidence(*ev7);


      __PotentialSet evset;
      evset.insert(ev0.get());
      evset.insert(ev1.get());
      evset.insert(ev7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      __Potential evp0(create_evidence(0, {1, 0}));
      inf.chgEvidence(0, 0);
      __Potential evp1(create_evidence(1, {0.8, 0.4, 0.1}));
      inf.chgEvidence(1, std::vector< double >{0.8, 0.4, 0.1});
      __Potential evp7(create_evidence(7, {0.2, 0.3, 0.6}));
      inf.chgEvidence(*evp7);

      evset.clear();
      evset.insert(evp0.get());
      evset.insert(evp1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      __Potential evpp7(create_evidence(7, {0.9, 0.1, 0.3}));
      inf.chgEvidence(*evpp7);

      evset.clear();
      evset.insert(evp0.get());
      evset.insert(evp1.get());
      evset.insert(evpp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      inf.chgEvidence(*ev0);
      inf.chgEvidence(*evp7);
      evset.clear();
      evset.insert(ev0.get());
      evset.insert(evp1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }

      inf.eraseEvidence(0);
      evset.clear();
      evset.insert(evp1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      inf.addEvidence(*evp0);
      TS_ASSERT_THROWS_NOTHING(inf.makeInference());
      inf.eraseEvidence(0);

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }
    }


    // ============================================================================
    // ============================================================================
    void test_prior_with_targets_hard_evidence_values_changed() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(3);   // D

      __Potential ev0(create_evidence(0, {0.3, 0.7}));
      inf.addEvidence(*ev0);
      __Potential ev1(create_evidence(1, {0, 1, 0}));
      inf.addEvidence(*ev1);
      __Potential ev7(create_evidence(7, {0.4, 0.2, 0.3}));
      inf.addEvidence(*ev7);


      __PotentialSet evset;
      evset.insert(ev0.get());
      evset.insert(ev1.get());
      evset.insert(ev7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      __Potential evp0(create_evidence(0, {1, 0}));
      inf.chgEvidence(0, 0);
      __Potential evp7(create_evidence(7, {0.2, 0.3, 0.6}));
      inf.chgEvidence(*evp7);

      evset.clear();
      evset.insert(evp0.get());
      evset.insert(ev1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      __Potential evpp0(create_evidence(0, {0, 1}));
      inf.chgEvidence(*evpp0);

      evset.clear();
      evset.insert(evpp0.get());
      evset.insert(ev1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      inf.chgEvidence(*ev0);
      inf.chgEvidence(*evp7);
      evset.clear();
      evset.insert(ev0.get());
      evset.insert(ev1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }

      inf.eraseEvidence(0);
      evset.clear();
      evset.insert(ev1.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      inf.addEvidence(*evp0);
      TS_ASSERT_THROWS_NOTHING(inf.makeInference());
      inf.eraseEvidence(0);

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }
    }


    // ============================================================================
    // ============================================================================
    void test_prior_with_targets_evidence_changed() {
      gum::LazyPropagation< double > inf(bn);
      inf.eraseAllTargets();
      inf.addTarget(0);   // A
      inf.addTarget(3);   // D

      __Potential ev0(create_evidence(0, {0.3, 1.7}));
      inf.addEvidence(*ev0);
      __Potential ev1(create_evidence(1, {0.3, 0.1, 0.8}));
      inf.addEvidence(*ev1);
      __Potential ev7(create_evidence(7, {0.4, 0.2, 0.3}));
      inf.addEvidence(*ev7);

      __PotentialSet evset;
      evset.insert(ev0.get());
      evset.insert(ev1.get());
      evset.insert(ev7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }


      __Potential ev4(create_evidence(4, {1, 0}));
      __Potential evp7(create_evidence(7, {0.2, 0.3, 0.6}));
      inf.eraseEvidence(0);
      inf.addEvidence(*ev4);
      inf.chgEvidence(*evp7);
      evset.clear();
      evset.insert(ev1.get());
      evset.insert(ev4.get());
      evset.insert(evp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }

      __Potential evp0(create_evidence(0, {1, 0}));
      __Potential evpp4(create_evidence(4, {0.7, 0.7}));
      __Potential evpp7(create_evidence(7, {0, 1, 0}));
      inf.addEvidence(0, 0);
      inf.chgEvidence(*evpp4);
      inf.chgEvidence(*evpp7);
      evset.clear();
      evset.insert(evp0.get());
      evset.insert(ev1.get());
      evset.insert(evpp4.get());
      evset.insert(evpp7.get());

      {
        __Potential posterior(posterior_joint(joint, evset));

        TS_ASSERT_THROWS_NOTHING(inf.makeInference());

        // get the marginals of A, D
        std::unique_ptr< gum::Potential< double > > pa(
           get_marginal(posterior.get(), BN_node_index[0])),
           pd(get_marginal(posterior.get(), BN_node_index[3]));

        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[0]), *pa));
        TS_ASSERT(equalPotentials(inf.posterior(BN_node_index[3]), *pd));
      }
    }


    void testEnd() { theEnd(); }
  };
}   // namespace gum_tests

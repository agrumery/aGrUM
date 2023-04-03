/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <iostream>
#include <vector>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#define PRINT(x) std::cout << x << std::endl;

using Potential = std::vector< double >;
using NodeSet   = std::vector< int >;

class LazyPropagation {
  public:
  void      addJointTarget(const NodeSet& target) { PRINT("    in Lazy:addJoint"); };
  Potential jointPosterior(const NodeSet& target) {
    PRINT("    in Lazy:JointPost");
    Potential p;
    return p;
  };
  void makeInference() { PRINT("    -> in Lazy:makeInference"); };
};

class ShaferShenoyMRF {
  public:
  void      addJointTarget(const NodeSet& target) { PRINT("    in SSMRF:addJoint"); };
  Potential jointPosterior(const NodeSet& target) {
    PRINT("    in SSMRF:JointPost");
    Potential p;
    return p;
  };
  void makeInference() { PRINT("    -> in SSMRF:makeInference"); };
};

class VariableElimination {
  public:
  void      addMonoTarget(int target) { PRINT("    in Variable Elimination:add"); };
  Potential jointPosterior(const int target) {
    Potential p;
    return p;
  };
  void makeInference() { PRINT("    -> in VariableElimination:makeInference"); };
};


//////////////////////////////////////////////////////////////////////////////////
template < typename T >
concept JointTargettable = requires(T t, const NodeSet& target) {
  { t.addJointTarget(target) } -> std::same_as< void >;
  { t.jointPosterior(target) } -> std::same_as< Potential >;
};


template < typename INFERENCE_ENGINE >
  requires JointTargettable< INFERENCE_ENGINE >
class InformationTheory {
  private:
  INFERENCE_ENGINE& engine_;

  public:
  InformationTheory(INFERENCE_ENGINE& engine, const NodeSet& target, const NodeSet& conditioning) :
      engine_(engine) {
    PRINT("  - in InforamtionTheory");
    engine.addJointTarget(target);   // X=target / Y=conditioning
    engine.makeInference();
    Potential p = engine.jointPosterior(target);
    // computes H(X) / H(X|Y ) / I(X;Y) etc.
  };
};


namespace gum_tests {

  class [[maybe_unused]] InformationTheoryTestSuite: public CxxTest::TestSuite {
    public:
    GUM_TEST(Constructor2) {
      NodeSet         X, Y;
      LazyPropagation ie;
      ShaferShenoyMRF ssie;

      PRINT("GOOD OLD ONES")
      InformationTheory< LazyPropagation > it1(ie, X, Y);
      InformationTheory< ShaferShenoyMRF > it2(ssie, X, Y);

      PRINT("IMPLICIT TEMPLATE ARGUMENT")
      InformationTheory it3(ie, X, Y);
      InformationTheory it4(ssie, X, Y);

      // ca plante (avec un message assez clair pour le coup)
      // VariableElimination ve;
      // InformationTheory it5(ve,X,Y);
    };
  };
} // namespace gum_tests
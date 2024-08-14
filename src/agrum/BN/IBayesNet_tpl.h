/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


/**
 * @file
 * @brief Template implementation of bns/bayesNet.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 */

#include <cmath>
#include <limits>

#include <agrum/tools/multidim/aggregators/and.h>
#include <agrum/tools/multidim/aggregators/or.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/tools/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/tools/multidim/potential.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/IBayesNet.h>

namespace gum {

  //                                  IBayesNet

  template < typename GUM_SCALAR >
  INLINE IBayesNet< GUM_SCALAR >::IBayesNet() : DAGmodel() {
    GUM_CONSTRUCTOR(IBayesNet)
  }

  template < typename GUM_SCALAR >
  INLINE IBayesNet< GUM_SCALAR >::IBayesNet(std::string name) : DAGmodel() {
    GUM_CONSTRUCTOR(IBayesNet)
    this->setProperty("name", name);
  }

  template < typename GUM_SCALAR >
  IBayesNet< GUM_SCALAR >::IBayesNet(const IBayesNet< GUM_SCALAR >& source) : DAGmodel(source) {
    GUM_CONS_CPY(IBayesNet)
  }

  template < typename GUM_SCALAR >
  IBayesNet< GUM_SCALAR >&
      IBayesNet< GUM_SCALAR >::operator=(const IBayesNet< GUM_SCALAR >& source) {
    if (this != &source) { DAGmodel::operator=(source); }

    return *this;
  }

  template < typename GUM_SCALAR >
  IBayesNet< GUM_SCALAR >::~IBayesNet() {
    GUM_DESTRUCTOR(IBayesNet)
  }

  template < typename GUM_SCALAR >
  Size IBayesNet< GUM_SCALAR >::dim() const {
    Size dim = 0;

    for (auto node: nodes()) {
      Size q = 1;

      for (auto parent: parents(node))
        q *= variable(parent).domainSize();

      dim += (variable(node).domainSize() - 1) * q;
    }

    return dim;
  }

  template < typename GUM_SCALAR >
  Size IBayesNet< GUM_SCALAR >::maxVarDomainSize() const {
    Size res = 0;
    for (auto node: nodes()) {
      auto v = variable(node).domainSize();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::minParam() const {
    GUM_SCALAR res = 1.0;
    for (auto node: nodes()) {
      auto v = cpt(node).min();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::maxParam() const {
    GUM_SCALAR res = 1.0;
    for (auto node: nodes()) {
      auto v = cpt(node).max();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::minNonZeroParam() const {
    GUM_SCALAR res = 1.0;
    for (auto node: nodes()) {
      auto v = cpt(node).minNonZero();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::maxNonOneParam() const {
    GUM_SCALAR res = 0.0;
    for (auto node: nodes()) {
      auto v = cpt(node).maxNonOne();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE Size IBayesNet< GUM_SCALAR >::memoryFootprint() const {
    Size usedMem = 0;

    for (auto node: nodes())
      usedMem += cpt(node).memoryFootprint();
    return usedMem;
  }

  template < typename GUM_SCALAR >
  INLINE std::string IBayesNet< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", ";
    spaceCplxToStream(s, log10DomainSize(), (int)dim(), memoryFootprint());
    s << "}";
    return s.str();
  }

  template < typename GUM_SCALAR >
  std::string IBayesNet< GUM_SCALAR >::toDot() const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    try {
      bn_name = this->property("name");
    } catch (NotFound const&) { bn_name = "no_name"; }

    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];" << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl << std::endl;

    for (auto node: nodes())
      output << "\"" << variable(node).name() << "\" [comment=\"" << node << ":"
             << variable(node).toStringWithDescription() << "\"];" << std::endl;

    output << std::endl;

    std::string tab = "  ";

    for (auto node: nodes()) {
      if (children(node).size() > 0) {
        for (auto child: children(node)) {
          output << tab << "\"" << variable(node).name() << "\" -> " << "\""
                 << variable(child).name() << "\";" << std::endl;
        }
      } else if (parents(node).size() == 0) {
        output << tab << "\"" << variable(node).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }

  /// Compute a parameter of the joint probability for the BN (given an
  /// instantiation
  /// of the vars)
  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::jointProbability(const Instantiation& i) const {
    auto value = (GUM_SCALAR)1.0;

    GUM_SCALAR tmp;

    for (auto node: nodes()) {
      if ((tmp = cpt(node)[i]) == (GUM_SCALAR)0) { return (GUM_SCALAR)0; }

      value *= tmp;
    }

    return value;
  }

  /// Compute a parameter of the joint probability for the BN (given an
  /// instantiation
  /// of the vars)
  template < typename GUM_SCALAR >
  GUM_SCALAR IBayesNet< GUM_SCALAR >::log2JointProbability(const Instantiation& i) const {
    auto value = (GUM_SCALAR)0.0;

    GUM_SCALAR tmp;

    for (auto node: nodes()) {
      if ((tmp = cpt(node)[i]) == (GUM_SCALAR)0) {
        return (GUM_SCALAR)(-std::numeric_limits< double >::infinity());
      }

      value += std::log2(cpt(node)[i]);
    }

    return value;
  }

  template < typename GUM_SCALAR >
  bool IBayesNet< GUM_SCALAR >::operator==(const IBayesNet& from) const {
    if (size() != from.size()) { return false; }

    if (sizeArcs() != from.sizeArcs()) { return false; }

    // alignment of variables between the 2 BNs
    Bijection< const DiscreteVariable*, const DiscreteVariable* > alignment;

    for (auto node: nodes()) {
      try {
        const auto& v1 = variable(node);
        const auto& v2 = from.variableFromName(variable(node).name());
        if (v1 != v2) { return false; }

        alignment.insert(&variable(node), &from.variableFromName(variable(node).name()));
      } catch (NotFound const&) {
        // a name is not found in from
        return false;
      }
    }

    for (auto node: nodes()) {
      NodeId fromnode = from.idFromName(variable(node).name());

      if (cpt(node).nbrDim() != from.cpt(fromnode).nbrDim()) { return false; }

      if (cpt(node).domainSize() != from.cpt(fromnode).domainSize()) { return false; }

      Instantiation i(cpt(node));
      Instantiation j(from.cpt(fromnode));

      for (i.setFirst(); !i.end(); i.inc()) {
        for (Idx indice = 0; indice < cpt(node).nbrDim(); ++indice) {
          const DiscreteVariable* p = &(i.variable(indice));
          j.chgVal(*(alignment.second(p)), i.val(*p));
        }

        if (std::pow(cpt(node).get(i) - from.cpt(fromnode).get(j), (GUM_SCALAR)2)
            > (GUM_SCALAR)1e-6) {
          return false;
        }
      }
    }

    return true;
  }

  template < typename GUM_SCALAR >
  bool IBayesNet< GUM_SCALAR >::operator!=(const IBayesNet& from) const {
    return !this->operator==(from);
  }

  // visit the nodes and add some of node from soids in minimal
  template < typename GUM_SCALAR >
  void IBayesNet< GUM_SCALAR >::_minimalCondSetVisitUp_(NodeId         node,
                                                        const NodeSet& soids,
                                                        NodeSet&       minimal,
                                                        NodeSet&       alreadyVisitedUp,
                                                        NodeSet&       alreadyVisitedDn) const {
    if (alreadyVisitedUp.contains(node)) return;
    alreadyVisitedUp << node;

    if (soids.contains(node)) {
      minimal << node;
    } else {
      for (auto fath: dag_.parents(node))
        _minimalCondSetVisitUp_(fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
      for (auto chil: dag_.children(node))
        _minimalCondSetVisitDn_(chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    }
  }

  // visit the nodes and add some of node from soids in minimal
  template < typename GUM_SCALAR >
  void IBayesNet< GUM_SCALAR >::_minimalCondSetVisitDn_(NodeId         node,
                                                        const NodeSet& soids,
                                                        NodeSet&       minimal,
                                                        NodeSet&       alreadyVisitedUp,
                                                        NodeSet&       alreadyVisitedDn) const {
    if (alreadyVisitedDn.contains(node)) return;
    alreadyVisitedDn << node;

    if (soids.contains(node)) {
      minimal << node;
      for (auto fath: dag_.parents(node))
        _minimalCondSetVisitUp_(fath, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    } else {
      for (auto chil: dag_.children(node))
        _minimalCondSetVisitDn_(chil, soids, minimal, alreadyVisitedUp, alreadyVisitedDn);
    }
  }

  template < typename GUM_SCALAR >
  NodeSet IBayesNet< GUM_SCALAR >::minimalCondSet(NodeId target, const NodeSet& soids) const {
    if (soids.contains(target)) return NodeSet({target});

    NodeSet res;
    NodeSet alreadyVisitedUp;
    NodeSet alreadyVisitedDn;
    alreadyVisitedDn << target;
    alreadyVisitedUp << target;

    for (auto fath: dag_.parents(target))
      _minimalCondSetVisitUp_(fath, soids, res, alreadyVisitedUp, alreadyVisitedDn);
    for (auto chil: dag_.children(target))
      _minimalCondSetVisitDn_(chil, soids, res, alreadyVisitedUp, alreadyVisitedDn);
    return res;
  }

  template < typename GUM_SCALAR >
  NodeSet IBayesNet< GUM_SCALAR >::minimalCondSet(const NodeSet& targets,
                                                  const NodeSet& soids) const {
    NodeSet res;
    for (auto node: targets) {
      res += minimalCondSet(node, soids);
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) {
    output << bn.toString();
    return output;
  }

  template < typename GUM_SCALAR >
  std::vector< std::string > IBayesNet< GUM_SCALAR >::check() const {
    std::vector< std::string > comments;

    const double epsilon       = 1e-8;
    const double error_epsilon = 1e-1;

    // CHECKING domain
    for (const auto i: nodes())
      if (variable(i).domainSize() < 2) {
        std::stringstream s;
        s << "Variable " << variable(i).name() << ": not consistent (domainSize=1).";
        comments.push_back(s.str());
      }

    // CHECKING parameters are probabilities
    // >0
    for (const auto i: nodes()) {
      const auto [amin, minval] = cpt(i).argmin();
      if (minval < (GUM_SCALAR)0.0) {
        std::stringstream s;
        s << "Variable " << variable(i).name() << " : P(" << *(amin.begin()) << ") < 0.0";
        comments.push_back(s.str());
      }
    }
    // <1
    for (const auto i: nodes()) {
      const auto [amax, maxval] = cpt(i).argmax();
      if (maxval > (GUM_SCALAR)1.0) {
        std::stringstream s;
        s << "Variable " << variable(i).name() << " : P(" << *(amax.begin()) << ") > 1.0";
        comments.push_back(s.str());
      }
    }

    // CHECKING distributions sum to 1
    for (const auto i: nodes()) {
      const auto p              = cpt(i).sumOut({&variable(i)});
      const auto [amin, minval] = p.argmin();
      if (minval < (GUM_SCALAR)(1.0 - epsilon)) {
        std::stringstream s;
        s << "Variable " << variable(i).name() << " : ";
        if (!parents(i).empty()) s << "with (at least) parents " << *(amin.begin()) << ", ";
        s << "the CPT sum to less than 1";
        if (minval > (GUM_SCALAR)(1.0 - error_epsilon)) s << " (normalization problem ?)";
        s << ".";
        comments.push_back(s.str());
        continue;
      }
      const auto [amax, maxval] = p.argmax();
      if (maxval > (GUM_SCALAR)(1.0 + epsilon)) {
        std::stringstream s;
        s << "Variable " << variable(i).name() << " : ";
        if (!parents(i).empty()) s << "with (at least) parents " << *(amax.begin()) << ", ";
        s << "the CPT sum to more than 1";
        if (maxval < (GUM_SCALAR)(1.0 + error_epsilon)) s << " (normalization problem ?)";
        s << ".";
        comments.push_back(s.str());
      }
    }

    return comments;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > IBayesNet< GUM_SCALAR >::evEq(const std::string& name,
                                                        double             value) const {
    return Potential< GUM_SCALAR >::evEq(variableFromName(name), value);
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
      IBayesNet< GUM_SCALAR >::evIn(const std::string& name, double val1, double val2) const {
    return Potential< GUM_SCALAR >::evIn(variableFromName(name), val1, val2);
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > IBayesNet< GUM_SCALAR >::evGt(const std::string& name, double val) const {
    return Potential< GUM_SCALAR >::evGt(variableFromName(name), val);
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > IBayesNet< GUM_SCALAR >::evLt(const std::string& name, double val) const {
    return Potential< GUM_SCALAR >::evLt(variableFromName(name), val);
  }

} /* namespace gum */

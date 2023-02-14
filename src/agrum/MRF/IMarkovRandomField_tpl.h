/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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

#include <limits>

#include <agrum/MRF/IMarkovRandomField.h>
#include <agrum/tools/multidim/potential.h>

#define EF get
namespace gum {

  // IMarkovRandomField

  template < typename GUM_SCALAR >
  INLINE IMarkovRandomField< GUM_SCALAR >::IMarkovRandomField() : UGmodel() {
    GUM_CONSTRUCTOR(IMarkovRandomField);
  }

  template < typename GUM_SCALAR >
  INLINE IMarkovRandomField< GUM_SCALAR >::IMarkovRandomField(std::string name) : UGmodel() {
    GUM_CONSTRUCTOR(IMarkovRandomField);
    this->setProperty("name", name);
  }

  template < typename GUM_SCALAR >
  IMarkovRandomField< GUM_SCALAR >::IMarkovRandomField(
     const IMarkovRandomField< GUM_SCALAR >& source) :
      UGmodel(source) {
    GUM_CONS_CPY(IMarkovRandomField);
  }

  template < typename GUM_SCALAR >
  IMarkovRandomField< GUM_SCALAR >&
     IMarkovRandomField< GUM_SCALAR >::operator=(const IMarkovRandomField< GUM_SCALAR >& source) {
    if (this != &source) { UGmodel::operator=(source); }

    return *this;
  }

  template < typename GUM_SCALAR >
  IMarkovRandomField< GUM_SCALAR >::~IMarkovRandomField() {
    GUM_DESTRUCTOR(IMarkovRandomField);
  }

  template < typename GUM_SCALAR >
  INLINE Size IMarkovRandomField< GUM_SCALAR >::dim() const {
    Size res = 0;
    for (auto f: factors()) {
      res += f.second->domainSize();
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE Size IMarkovRandomField< GUM_SCALAR >::maxVarDomainSize() const {
    Size res = 0;
    for (auto node: nodes()) {
      auto v = variable(node).domainSize();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR IMarkovRandomField< GUM_SCALAR >::minParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->min();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR IMarkovRandomField< GUM_SCALAR >::maxParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->max();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR IMarkovRandomField< GUM_SCALAR >::minNonZeroParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->minNonZero();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR IMarkovRandomField< GUM_SCALAR >::maxNonOneParam() const {
    GUM_SCALAR res = 0.0;
    for (auto elt: factors()) {
      auto v = elt.second->maxNonOne();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE std::string IMarkovRandomField< GUM_SCALAR >::toString() const {
    Size   param = 0;
    double dSize = log10DomainSize();

    for (auto factor: factors())
      param += factor.second->content()->realSize();

    std::stringstream s;
    s << "MRF{nodes: " << size() << ", edges: " << graph().sizeEdges() << ", ";

    if (dSize > 6) s << "domainSize: 10^" << dSize;
    else s << "domainSize: " << std::round(std::pow(10.0, dSize));

    s << ", dim: " << param << "}";

    return s.str();
  }

  template < typename GUM_SCALAR >
  std::string IMarkovRandomField< GUM_SCALAR >::toDot() const {
    std::stringstream output;
    output << "graph \"";

    std::string mn_name;

    try {
      mn_name = this->property("name");
    } catch (NotFound const&) { mn_name = "no_name"; }

    output << mn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << mn_name << "\"];" << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl << std::endl;

    for (auto node: nodes())
      output << "  \"" << variable(node).name() << "\" [comment=\"" << node << ":"
             << variable(node).toStringWithDescription() << "\"];" << std::endl;

    output << std::endl;

    std::string tab = "  ";

    for (auto node: nodes()) {
      if (neighbours(node).size() > 0) {
        for (auto nei: neighbours(node)) {
          if (variable(node).name() < variable(nei).name()) {
            output << tab << "\"" << variable(node).name() << "\" -- "
                   << "\"" << variable(nei).name() << "\";" << std::endl;
          }
        }
      } else {
        output << tab << "\"" << variable(node).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }


  template < typename GUM_SCALAR >
  std::string IMarkovRandomField< GUM_SCALAR >::toDotAsFactorGraph() const {
    std::stringstream output;
    std::string       mn_name;
    try {
      mn_name = this->property("name");
    } catch (NotFound const&) { mn_name = "no_name"; }

    output << "graph FG_" << mn_name << " {" << std::endl;
    output << "  layout=neato;" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"factor graph for " << mn_name << "\"];"
           << std::endl;

    // the variables
    output << "  node [shape=rectangle,margin=0.04,width=0,height=0, "
              "style=filled,color=\"coral\"];"
           << std::endl;
    for (auto nod: nodes()) {
      output << "\"" << variable(nod).name() << "\";" << std::endl;
    }
    output << std::endl;

    // the factor
    output << "node[shape = point,width = 0.1,height = 0.1,style = filled,color = "
              "\"burlywood\"];"
           << std::endl;
    for (const auto& kv: factors()) {
      output << "  \"f";
      for (NodeId nod: kv.first) {
        output << "#" << variable(nod).name();
      }
      output << "\";" << std::endl;
    }

    // the link variable--factors
    output << "  edge[len = 0.7];" << std::endl;
    for (const auto& kv: factors()) {
      std::string clicname = "\"f";
      for (NodeId nod: kv.first) {
        clicname += "#";
        clicname += variable(nod).name();
      }
      clicname += "\"";

      for (NodeId nod: kv.first)
        output << "  " << clicname << " -- \"" << variable(nod).name() << "\";" << std::endl;
    }
    output << "}" << std::endl;

    return output.str();
  }

  template < typename GUM_SCALAR >
  bool IMarkovRandomField< GUM_SCALAR >::operator==(const IMarkovRandomField& from) const {
    if (size() != from.size()) { return false; }

    if (sizeEdges() != from.sizeEdges()) { return false; }

    // alignment of variables between the 2 BNs
    Bijection< const DiscreteVariable*, const DiscreteVariable* > alignment;

    for (auto node: nodes()) {
      try {
        alignment.insert(&variable(node), &from.variableFromName(variable(node).name()));
      } catch (NotFound const&) {
        // a name is not found in from
        return false;
      }
    }

    for (const auto& elt: factors()) {
      const auto& key    = elt.first;
      const auto& factor = *elt.second;

      NodeSet fromkey;
      for (const auto n: key)
        fromkey.insert(from.idFromName(variable(n).name()));

      if (!from.factors().exists(fromkey)) { return false; }

      const auto& fromfactor = from.factor(fromkey);

      Instantiation i(factor);
      Instantiation j(fromfactor);
      for (i.setFirst(); !i.end(); i.inc()) {
        for (Idx indice = 0; indice < factor.nbrDim(); ++indice) {
          const DiscreteVariable* p = &(i.variable(indice));
          j.chgVal(*(alignment.second(p)), i.val(*p));
        }

        if (std::pow(factor.get(i) - fromfactor.get(j), (GUM_SCALAR)2) > (GUM_SCALAR)1e-6) {
          return false;
        }
      }
    }
    return true;
  }

  template < typename GUM_SCALAR >
  INLINE bool IMarkovRandomField< GUM_SCALAR >::operator!=(const IMarkovRandomField& from) const {
    return !this->operator==(from);
  }

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream&                           output,
                                  const IMarkovRandomField< GUM_SCALAR >& bn) {
    output << bn.toString();
    return output;
  }

  template < typename GUM_SCALAR >
  INLINE const NodeSet&
     IMarkovRandomField< GUM_SCALAR >::smallestFactorFromNode(const std::string& name) const {
    try {
      return smallestFactorFromNode(idFromName(name));
    } catch (NotFound const&) {
      GUM_ERROR(NotFound, "No factor containing the variable <" << name << ">")
    }
  }

  // visit the nodes and add some of node from soids in minimal
  template < typename GUM_SCALAR >
  void IMarkovRandomField< GUM_SCALAR >::_minimalCondSetVisit_(NodeId         node,
                                                               const NodeSet& soids,
                                                               NodeSet&       minimal,
                                                               NodeSet& alreadyVisited) const {
    if (alreadyVisited.contains(node)) return;
    alreadyVisited << node;

    if (soids.contains(node)) {
      minimal << node;
    } else {
      for (auto neig: graph_.neighbours(node))
        _minimalCondSetVisit_(neig, soids, minimal, alreadyVisited);
    }
  }


  template < typename GUM_SCALAR >
  NodeSet IMarkovRandomField< GUM_SCALAR >::minimalCondSet(NodeId         target,
                                                           const NodeSet& soids) const {
    if (soids.contains(target)) return NodeSet({target});

    NodeSet res;
    NodeSet alreadyVisited;
    alreadyVisited << target;

    for (auto neig: graph_.neighbours(target))
      _minimalCondSetVisit_(neig, soids, res, alreadyVisited);
    return res;
  }

  template < typename GUM_SCALAR >
  NodeSet IMarkovRandomField< GUM_SCALAR >::minimalCondSet(const NodeSet& targets,
                                                           const NodeSet& soids) const {
    NodeSet res;
    for (auto node: targets) {
      res += minimalCondSet(node, soids);
    }
    return res;
  }
} /* namespace gum */

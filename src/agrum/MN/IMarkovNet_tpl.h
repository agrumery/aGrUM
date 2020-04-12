
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Lionel TORTI
 */

#include <limits>

#include <agrum/MN/IMarkovNet.h>
#include <agrum/tools/multidim/potential.h>

#define EF get
namespace gum {

  // IMarkovNet

  template < typename GUM_SCALAR >
  INLINE IMarkovNet< GUM_SCALAR >::IMarkovNet() : UGmodel() {
    GUM_CONSTRUCTOR(IMarkovNet);
  }

  template < typename GUM_SCALAR >
  INLINE IMarkovNet< GUM_SCALAR >::IMarkovNet(std::string name) : UGmodel() {
    GUM_CONSTRUCTOR(IMarkovNet);
    this->setProperty("name", name);
  }

  template < typename GUM_SCALAR >
  IMarkovNet< GUM_SCALAR >::IMarkovNet(const IMarkovNet< GUM_SCALAR >& source) :
      UGmodel(source) {
    GUM_CONS_CPY(IMarkovNet);
  }

  template < typename GUM_SCALAR >
  IMarkovNet< GUM_SCALAR >&
     IMarkovNet< GUM_SCALAR >::operator=(const IMarkovNet< GUM_SCALAR >& source) {
    if (this != &source) { UGmodel::operator=(source); }

    return *this;
  }

  template < typename GUM_SCALAR >
  IMarkovNet< GUM_SCALAR >::~IMarkovNet() {
    GUM_DESTRUCTOR(IMarkovNet);
  }

  template < typename GUM_SCALAR >
  Size IMarkovNet< GUM_SCALAR >::dim() const {
    Size res = 0;
    for (auto f: factors()) {
      res += f.second->domainSize();
    }
    return res;
  }

  template < typename GUM_SCALAR >
  Size IMarkovNet< GUM_SCALAR >::maxVarDomainSize() const {
    Size res = 0;
    for (auto node: nodes()) {
      auto v = variable(node).domainSize();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IMarkovNet< GUM_SCALAR >::minParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->min();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IMarkovNet< GUM_SCALAR >::maxParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->max();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IMarkovNet< GUM_SCALAR >::minNonZeroParam() const {
    GUM_SCALAR res = 1.0;
    for (auto elt: factors()) {
      auto v = elt.second->minNonZero();
      if (v < res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR IMarkovNet< GUM_SCALAR >::maxNonOneParam() const {
    GUM_SCALAR res = 0.0;
    for (auto elt: factors()) {
      auto v = elt.second->maxNonOne();
      if (v > res) { res = v; }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE std::string IMarkovNet< GUM_SCALAR >::toString() const {
    Size   param = 0;
    double dSize = log10DomainSize();

    for (auto factor: factors())
      param += factor.second->content()->realSize();

    std::stringstream s;
    s << "MN{nodes: " << size() << ", edges: " << graph().sizeEdges() << ", ";

    if (dSize > 6)
      s << "domainSize: 10^" << dSize;
    else
      s << "domainSize: " << std::round(std::pow(10.0, dSize));

    s << ", dim: " << param << "}";

    return s.str();
  }

  template < typename GUM_SCALAR >
  std::string IMarkovNet< GUM_SCALAR >::toDot() const {
    std::stringstream output;
    output << "graph \"";

    std::string mn_name;

    try {
      mn_name = this->property("name");
    } catch (NotFound&) { mn_name = "no_name"; }

    output << mn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << mn_name << "\"];"
           << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl
           << std::endl;

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
  std::string IMarkovNet< GUM_SCALAR >::toDotAsFactorGraph() const {
    std::stringstream output;
    std::string       mn_name;
    try {
      mn_name = this->property("name");
    } catch (NotFound&) { mn_name = "no_name"; }

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
        output<<"  "<<clicname<<" -- \""<<variable(nod).name()<<"\";"<<std::endl;
    }
    output<<"}"<<std::endl;

    return output.str();
  }

  template < typename GUM_SCALAR >
  bool IMarkovNet< GUM_SCALAR >::operator==(const IMarkovNet& from) const {
    if (size() != from.size()) { return false; }

    if (sizeEdges() != from.sizeEdges()) { return false; }

    // alignment of variables between the 2 BNs
    Bijection< const DiscreteVariable*, const DiscreteVariable* > alignment;

    for (auto node: nodes()) {
      try {
        alignment.insert(&variable(node),
                         &from.variableFromName(variable(node).name()));
      } catch (NotFound&) {
        // a name is not found in from
        return false;
      }
    }

    for (const auto& elt: factors()) {
      const auto& key = elt.first;
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

        if (std::pow(factor.get(i) - fromfactor.get(j), (GUM_SCALAR)2)
            > (GUM_SCALAR)1e-6) {
          return false;
        }
      }
    }

    return true;
  }

  template < typename GUM_SCALAR >
  bool IMarkovNet< GUM_SCALAR >::operator!=(const IMarkovNet& from) const {
    return !this->operator==(from);
  }

  template < typename GUM_SCALAR >
  INLINE std::ostream& operator<<(std::ostream&                   output,
                                  const IMarkovNet< GUM_SCALAR >& bn) {
    output << bn.toString();
    return output;
  }

} /* namespace gum */

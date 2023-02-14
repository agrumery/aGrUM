/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/PRM/o3prm/O3prmBNWriter.h>

#  define O3PRM_INDENT "  "

namespace gum {
  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE O3prmBNWriter< GUM_SCALAR >::O3prmBNWriter() {
    GUM_CONSTRUCTOR(O3prmBNWriter)
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE O3prmBNWriter< GUM_SCALAR >::~O3prmBNWriter() {
    GUM_DESTRUCTOR(O3prmBNWriter)
  }

  /*
   * Writes a bayes net in the given output stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void O3prmBNWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                    const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }
    std::string bnName = bn.propertyWithDefault("name", "");
    if (bnName.empty()) bnName = "bayesnet";

    output << "class " << bnName << " {" << std::endl;

    for (auto node: bn.nodes()) {
      output << _extractAttribute_(bn, node) << std::endl;
    }

    output << "}" << std::endl;

    output << std::endl;

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         O3prmBNWriter< GUM_SCALAR >::_extractAttribute_(const IBayesNet< GUM_SCALAR >& bn,
                                                     NodeId                         node) {
    std::stringstream str;
    str << O3PRM_INDENT;
    str << _extractType_(bn, node) << " ";
    str << _extractName_(bn, node) << " ";
    if (bn.parents(node).size() > 0) { str << "dependson " << _extractParents_(bn, node) << " "; }
    str << " {" << _extractCPT_(bn, node) << "};" << std::endl;
    return str.str();
  }

  template < typename GUM_SCALAR >
  INLINE std::string
     O3prmBNWriter< GUM_SCALAR >::_extractParents_(const IBayesNet< GUM_SCALAR >& bn, NodeId node) {
    std::stringstream str;
    auto              var = &(bn.variable(node));
    for (auto parent: bn.cpt(node).variablesSequence()) {
      if (var != parent) { str << parent->name() << ", "; }
    }
    return str.str().substr(0, str.str().size() - 2);
  }

  template < typename GUM_SCALAR >
  INLINE std::string O3prmBNWriter< GUM_SCALAR >::_extractCPT_(const IBayesNet< GUM_SCALAR >& bn,
                                                               NodeId node) {
    std::stringstream str;
    bool              first = true;
    Instantiation     inst(bn.cpt(node));

    str << "[";
    if (inst.nbrDim() == 1) {
      // 1D potential
      for (inst.setFirst(); !inst.end(); inst.inc()) {
        if (!first) {
          str << ", ";
        } else {
          first = false;
        }
        str << bn.cpt(node)[inst];
      }
    } else {
      // (>1)D potential (with parents)
      Instantiation jnst;
      for (auto var = inst.variablesSequence().rbegin(); var != inst.variablesSequence().rend();
           --var) {
        jnst.add(**var);
      }
      inst.setFirst();
      auto currentval = inst.val(0) + 1;
      for (jnst.setFirst(); !jnst.end(); jnst.inc()) {
        inst.setVals(jnst);
        if (!first) {
          str << ", ";
        } else {
          first = false;
        }
        if (currentval != inst.val(0)) {   // begins line
          str << std::endl << O3PRM_INDENT << O3PRM_INDENT;
          currentval = inst.val(0);
        }
        str << bn.cpt(node)[inst];
      }
      str << std::endl << O3PRM_INDENT;
    }

    str << "]";
    return str.str();
  }

  template < typename GUM_SCALAR >
  INLINE std::string O3prmBNWriter< GUM_SCALAR >::_extractType_(const IBayesNet< GUM_SCALAR >& bn,
                                                                NodeId node) {
    switch (bn.variable(node).varType()) {
      case gum::VarType::Discretized: {
        auto double_var
           = dynamic_cast< const DiscretizedVariable< double >* >(&(bn.variable(node)));
        if (double_var != nullptr) {
          return _extractDiscretizedType_< DiscretizedVariable< double > >(double_var);
        } else {
          auto float_var
             = dynamic_cast< const DiscretizedVariable< float >* >(&(bn.variable(node)));
          if (float_var != nullptr) {
            return _extractDiscretizedType_< DiscretizedVariable< float > >(float_var);
          }
        }
        GUM_ERROR(InvalidArgument, "DiscretizedVariable ticks are neither doubles or floats")
      }
      case gum::VarType::Range: {
        return _extractRangeType_(bn, node);
      }
      default: {
        return _extractLabelizedType_(bn, node);
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         O3prmBNWriter< GUM_SCALAR >::_extractRangeType_(const IBayesNet< GUM_SCALAR >& bn,
                                                     NodeId                         node) {
    const auto&       var = static_cast< const RangeVariable& >(bn.variable(node));
    std::stringstream str;
    str << "int (" << var.minVal() << ", " << var.maxVal() << ")";
    return str.str();
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         O3prmBNWriter< GUM_SCALAR >::_extractLabelizedType_(const IBayesNet< GUM_SCALAR >& bn,
                                                         NodeId                         node) {
    std::stringstream str;
    str << "labels(";
    for (auto l: bn.variable(node).labels()) {
      str << l << ", ";
    }
    return str.str().substr(0, str.str().size() - 2) + ")";
  }

  template < typename GUM_SCALAR >
  template < typename VARTYPE >
  INLINE std::string O3prmBNWriter< GUM_SCALAR >::_extractDiscretizedType_(const VARTYPE* var) {
    std::stringstream str;
    if (var->ticks().size() >= 3) {
      str << "real(" << var->ticks()[0];
      for (size_t i = 1; i < var->ticks().size(); ++i) {
        str << ", " << var->ticks()[i];
      }
      str << ")";
      return str.str();
    }
    GUM_ERROR(InvalidArgument, "discretized variable does not have enough ticks")
  }

  template < typename GUM_SCALAR >
  INLINE std::string O3prmBNWriter< GUM_SCALAR >::_extractName_(const IBayesNet< GUM_SCALAR >& bn,
                                                                NodeId node) {
    if (!bn.variable(node).name().empty()) {
      return bn.variable(node).name();
    } else {
      std::stringstream str;
      str << node;
      return str.str();
    }
  }

  /*
   * Writes a bayes net in the file referenced by filePath.
   * If the file doesn't exist, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the bayes net.
   * @param bn The bayes net writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void O3prmBNWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                                    const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

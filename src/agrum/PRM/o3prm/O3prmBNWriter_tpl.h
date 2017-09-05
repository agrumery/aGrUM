/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/PRM/o3prm/O3prmBNWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template <typename GUM_SCALAR>
  INLINE O3prmBNWriter<GUM_SCALAR>::O3prmBNWriter() {
    GUM_CONSTRUCTOR( O3prmBNWriter );
  }

  /*
   * Destructor.
   */
  template <typename GUM_SCALAR>
  INLINE O3prmBNWriter<GUM_SCALAR>::~O3prmBNWriter() {
    GUM_DESTRUCTOR( O3prmBNWriter );
  }

  /*
   * Writes a bayes net in the given ouput stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  INLINE void O3prmBNWriter<GUM_SCALAR>::write( std::ostream& output,
                                                const IBayesNet<GUM_SCALAR>& bn ) {
    if ( !output.good() ) {
      GUM_ERROR( IOError, "Stream states flags are not all unset." );
    }

    output << "class BayesNet {" << std::endl;

    for ( auto node : bn.nodes() ) {
      output << __extractAttribute( bn, node ) << std::endl;
    }

    output << "}" << std::endl;

    output << std::endl;

    output.flush();

    if ( output.fail() ) {
      GUM_ERROR( IOError, "Writing in the ostream failed." );
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractAttribute( const IBayesNet<GUM_SCALAR>& bn, NodeId node ) {
    std::stringstream str;
    str << __extractType(bn, node) << " ";
    str << __extractName(bn, node) << " ";
    if (bn.dag().parents(node).size() > 0) {
      str << "dependson " << __extractParents(bn, node) << " ";
    }
    str << " {" << __extractCPT(bn, node) << "};" << std::endl;
    return str.str();
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractParents(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {
    std::stringstream str;
    auto var =&( bn.variable(node));
    for (auto parent: bn.cpt(node).variablesSequence()) {
      if (var != parent) {
        str << parent->name() << ", ";
      }
    }
    return str.str().substr(0, str.str().size() - 2);
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractCPT(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {

    Instantiation inst( bn.cpt(node) );
    Instantiation jnst;
    for ( auto var = inst.variablesSequence().rbegin();
        var != inst.variablesSequence().rend();
        --var ) {
      jnst.add( **var );
    }

    std::stringstream str;
    str << "[" << std::endl;
    for (jnst.begin(); !jnst.end(); jnst.inc()) {
      inst.setVals( jnst );
      str << bn.cpt(node)[inst] << ", ";
    }
    str << std::endl;
    return str.str().substr(0, str.str().size()-3) + "]";

    //try {
    //auto cpt = std::vector<GUM_SCALAR>();
    //auto inst = Instantiation(bn.cpt(node));
    //for (inst.setFirst(); ! inst.end(); inst.inc()) {
    //  cpt.push_back(bn.cpt(node)[inst]);
    //}
    //std::stringstream str;
    //str << "[" << std::endl;
    //for (size_t mod = 0; mod < bn.variable(node).domainSize(); ++mod) {
    //  for (size_t i = mod; i < cpt.size();i += bn.variable(node).domainSize()) {
    //    str << cpt[i] << ", ";
    //  }
    //  str << std::endl;
    //}
    //return str.str().substr(0, str.str().size()-3) + "]";
    //} catch (gum::Exception& e) {
    //  GUM_SHOWERROR(e);
    //  throw e;
    //}

    // const auto& cpt = bn.cpt(node);
    // auto inst = Instantiation(cpt);
    // auto var = Instantiation();
    // var.add(bn.variable(node));
    // std::stringstream str;
    // str << "[" << std::endl;
    // //for (inst.setFirst(); ! inst.end(); inst.inc()) {
    // //  str << cpt[inst] << ", ";
    // //}
    // //str << std::endl;
    // for (var.setFirst(); ! var.end(); var.inc()) {
    //   inst.setFirst();
    //   inst.setVals(var);
    //   for (;! inst.end(); inst.incOut(var)) {
    //     str << cpt[inst] << ", ";
    //   }
    //   str << std::endl;
    // }
    // return str.str().substr(0, str.str().size()-3) + "]";
  }
 
  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractType(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {
    switch (bn.variable(node).varType()) {
      case gum::DiscreteVariable::VarType::Discretized:
        {
          auto double_var = dynamic_cast<const DiscretizedVariable<double>*>(&(bn.variable(node)));
          if ( double_var != nullptr ) {
            return __extractDiscretizedType<DiscretizedVariable<double>>(double_var);
          } else {
            auto float_var = dynamic_cast<const DiscretizedVariable<float>*>(&(bn.variable(node)));
            if ( float_var != nullptr ) {
              return __extractDiscretizedType<DiscretizedVariable<float>>(float_var);
            }
          }
          GUM_ERROR(InvalidArgument, "DiscretizedVariable ticks are neither doubles or floats");
        }
      case gum::DiscreteVariable::VarType::Range:
        {
          return __extractRangeType(bn, node);
        }
      default:
        {
          return __extractLabelizedType(bn, node);
        }
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractRangeType(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {
    const auto& var = static_cast<const RangeVariable&>(bn.variable(node));
    std::stringstream str;
    str << "int (" << var.minVal() << ", " << var.maxVal() << ")";
    return str.str();
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractLabelizedType(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {
    std::stringstream str;
    str << "labels(";
    for (auto l: bn.variable(node).labels()) {
      str << l << ", ";
    }
    return str.str().substr(0, str.str().size() - 2) + ")";
  }

  template <typename GUM_SCALAR>
  template <typename VARTYPE>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractDiscretizedType(const VARTYPE* var) {
    std::stringstream str;
    if ( var->ticks().size() > 3 ) {
      str << "real(" << var->ticks()[0];
      for (size_t i = 1; i < var->ticks().size() - 1; ++i) {
        str << ", " << var->ticks()[i];
      }
      str << ")";
      return str.str();
    }
    GUM_ERROR( InvalidArgument, "discretized variable does not have enough ticks");
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  O3prmBNWriter<GUM_SCALAR>::__extractName(const IBayesNet<GUM_SCALAR>& bn, NodeId node) {
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
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the bayes net.
   * @param bn The bayes net writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  INLINE void O3prmBNWriter<GUM_SCALAR>::write( std::string filePath,
                                                const IBayesNet<GUM_SCALAR>& bn ) {
    std::ofstream output( filePath.c_str(), std::ios_base::trunc );

    write( output, bn );

    output.close();

    if ( output.fail() ) {
      GUM_ERROR( IOError, "Writing in the ostream failed." );
    }
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS

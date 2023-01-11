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

#  include <agrum/agrum.h>

// to ease parsing in IDE
#  include <agrum/BN/io/BIF/BIFWriter.h>

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BIF_WRITER                         === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE BIFWriter< GUM_SCALAR >::BIFWriter() {
    GUM_CONSTRUCTOR(BIFWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR >
  INLINE BIFWriter< GUM_SCALAR >::~BIFWriter() {
    GUM_DESTRUCTOR(BIFWriter);
  }

  //
  // Writes a Bayesian network in the output stream using the BIF format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR >
  INLINE void BIFWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    output << _header_(bn) << std::endl;

    for (const auto node: bn.nodes()) {
      output << _variableBloc_(bn.variable(node)) << std::endl;
    }

    for (const auto node: bn.nodes()) {
      const Potential< GUM_SCALAR >& proba = bn.cpt(node);
      output << _variableCPT_(proba);
    }

    output << std::endl;

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  // Writes a Bayesian network in the referenced file using the BIF format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian network.
  // @param bn The Bayesian network writed in the file.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR >
  INLINE void BIFWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                                const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  // Returns a bloc defining a variable's CPT in the BIF format.
  template < typename GUM_SCALAR >
  INLINE std::string BIFWriter< GUM_SCALAR >::_variableCPT_(const Potential< GUM_SCALAR >& cpt) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation

    if (cpt.nbrDim() == 1) {
      Instantiation inst(cpt);
      str << "probability (" << this->_onlyValidCharsInName(cpt.variable(0).name()) << ") {"
          << std::endl;
      str << tab << "default";

      for (inst.setFirst(); !inst.end(); ++inst) {
        str << " " << cpt[inst];
      }

      str << ";" << std::endl << "}" << std::endl;
    } else if (cpt.domainSize() > 1) {
      Instantiation inst(cpt);
      Instantiation condVars;   // Instantiation on the conditioning variables
      const Sequence< const DiscreteVariable* >& varsSeq = cpt.variablesSequence();
      str << "probability (" << this->_onlyValidCharsInName((varsSeq[(Idx)0])->name()) << " | ";

      for (Idx i = 1; i < varsSeq.size(); i++) {
        if (i > 1) str << ", ";
        str << this->_onlyValidCharsInName(varsSeq[i]->name());
        condVars << *(varsSeq[i]);
      }
      str << ") {" << std::endl;

      for (inst.setFirstIn(condVars); !inst.end(); inst.incIn(condVars)) {
        str << tab << "(" << _variablesLabels_(varsSeq, inst) << ")";
        // Writing the probabilities of the variable

        for (inst.setFirstOut(condVars); !inst.end(); inst.incOut(condVars)) {
          str << " " << cpt[inst];
        }

        str << ";" << std::endl;

        inst.unsetOverflow();
      }

      str << "}" << std::endl;
    }

    return str.str();
  }

  // Returns the header of the BIF file.
  template < typename GUM_SCALAR >
  INLINE std::string BIFWriter< GUM_SCALAR >::_header_(const IBayesNet< GUM_SCALAR >& bn) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation
    str << "network \"" << bn.propertyWithDefault("name", "unnamedBN") << "\" {" << std::endl;
    str << "// written by aGrUM " << GUM_VERSION << std::endl;
    str << "}" << std::endl;
    return str.str();
  }

  // Returns a bloc defining a variable in the BIF format.
  template < typename GUM_SCALAR >
  INLINE std::string BIFWriter< GUM_SCALAR >::_variableBloc_(const DiscreteVariable& var) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation
    str << "variable " << this->_onlyValidCharsInName(var.name()) << " {" << std::endl;
    str << tab << "type discrete[" << var.domainSize() << "] {";

    for (Idx i = 0; i < var.domainSize(); i++) {
      if (i > 0) str << ", ";
      str << this->_onlyValidCharsInName(var.label(i));
    }

    str << "};" << std::endl;

    str << "}" << std::endl;
    return str.str();
  }

  // Returns the modalities labels of the variables in varsSeq
  template < typename GUM_SCALAR >
  INLINE std::string
     BIFWriter< GUM_SCALAR >::_variablesLabels_(const Sequence< const DiscreteVariable* >& varsSeq,
                                                const Instantiation&                       inst) {
    std::stringstream       str;
    const DiscreteVariable* varPtr = nullptr;

    for (Idx i = 1; i < varsSeq.size(); i++) {
      if (i > 1) str << ", ";
      varPtr = varsSeq[i];
      str << this->_onlyValidCharsInName(varPtr->label(inst.val(*varPtr)));
    }
    return str.str();
  }

  template < typename GUM_SCALAR >
  void BIFWriter< GUM_SCALAR >::_syntacticalCheck(const IBayesNet< GUM_SCALAR >& bn) {
    this->_validCharInNamesCheck(bn);
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

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

// to ease parsing in IDE
#include <agrum/BN/io/BIF/BIFWriter.h>

namespace gum {

  /* ============================================================================ */
  /* ===                           GUM_BIF_WRITER                             === */
  /* ============================================================================ */
  // Default constructor.
  template <typename GUM_SCALAR> INLINE BIFWriter<GUM_SCALAR>::BIFWriter() {
    GUM_CONSTRUCTOR(BIFWriter);
  }

  // Default destructor.
  template <typename GUM_SCALAR> INLINE BIFWriter<GUM_SCALAR>::~BIFWriter() {
    GUM_DESTRUCTOR(BIFWriter);
  }

  //
  // Writes a Bayesian Network in the output stream using the BIF format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian Network writen in output.
  // @throws Raised if an I/O error occurs.
  template <typename GUM_SCALAR>
  INLINE void BIFWriter<GUM_SCALAR>::write(std::ostream &output,
                                           const IBayesNet<GUM_SCALAR> &bn) {
    if (!output.good()) {
      GUM_ERROR(IOError, "Stream states flags are not all unset.");
    }

    output << __header(bn) << std::endl;

    for (auto node : bn.nodes()) {
      output << __variableBloc(bn.variable(node)) << std::endl;
    }

    for (auto node : bn.nodes()) {
      const Potential<GUM_SCALAR> &proba = bn.cpt(node);
      output << __variableCPT(proba);
    }

    output << std::endl;

    output.flush();

    if (output.fail()) {
      GUM_ERROR(IOError, "Writting in the ostream failed.");
    }
  }

  // Writes a Bayesian Network in the referenced file using the BIF format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian Network.
  // @param bn The Bayesian Network writed in the file.
  // @throws Raised if an I/O error occurs.
  template <typename GUM_SCALAR>
  INLINE void BIFWriter<GUM_SCALAR>::write(std::string filePath,
                                           const IBayesNet<GUM_SCALAR> &bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    if (!output.good()) {
      GUM_ERROR(IOError, "Stream states flags are not all unset.");
    }

    output << __header(bn) << std::endl;

    for (auto node : bn.nodes()) {
      output << __variableBloc(bn.variable(node)) << std::endl;
    }

    for (auto node : bn.nodes()) {
      const Potential<GUM_SCALAR> &proba = bn.cpt(node);
      output << __variableCPT(proba);
    }

    output << std::endl;

    output.flush();
    output.close();

    if (output.fail()) {
      GUM_ERROR(IOError, "Writting in the ostream failed.");
    }
  }

  // Returns a bloc defining a variable's CPT in the BIF format.
  template <typename GUM_SCALAR>
  INLINE std::string
  BIFWriter<GUM_SCALAR>::__variableCPT(const Potential<GUM_SCALAR> &cpt) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation

    if (cpt.nbrDim() == 1) {
      Instantiation inst(cpt);
      str << "probability (" << cpt.variable(0).name() << ") {" << std::endl;
      str << tab << "default";

      for (inst.setFirst(); !inst.end(); ++inst) {
        str << " " << cpt[inst];
      }

      str << ";" << std::endl << "}" << std::endl;
    } else if (cpt.domainSize() > 1) {
      Instantiation inst(cpt);
      Instantiation condVars; // Instantiation on the conditioning variables
      const Sequence<const DiscreteVariable *> &varsSeq = cpt.variablesSequence();
      str << "probability (" << (varsSeq[(Idx)0])->name() << " | ";

      for (Idx i = 1; i < varsSeq.size() - 1; i++) {
        str << varsSeq[i]->name() << ", ";
        condVars << *(varsSeq[i]);
      }

      str << varsSeq[varsSeq.size() - 1]->name() << ") {" << std::endl;

      condVars << *(varsSeq[varsSeq.size() - 1]);

      for (inst.setFirstIn(condVars); !inst.end(); inst.incIn(condVars)) {
        str << tab << "(" << __variablesLabels(varsSeq, inst) << ")";
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
  template <typename GUM_SCALAR>
  INLINE std::string
  BIFWriter<GUM_SCALAR>::__header(const IBayesNet<GUM_SCALAR> &bn) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation
    str << std::endl << "network " << bn.propertyWithDefault("name", "unnamedBN")
        << "{" << std::endl;
    str << tab << "property"
        << " software aGrUM" << GUM_VERSION << ";" << std::endl;
    str << "}" << std::endl;
    return str.str();
  }

  // Returns a bloc defining a variable in the BIF format.
  template <typename GUM_SCALAR>
  INLINE std::string
  BIFWriter<GUM_SCALAR>::__variableBloc(const DiscreteVariable &var) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation
    str << "variable " << var.name() << " {" << std::endl;
    str << tab << "type discrete[" << var.domainSize() << "] {";

    for (Idx i = 0; i < var.domainSize() - 1; i++) {
      str << var.label(i) << ", ";
    }

    str << var.label(var.domainSize() - 1) << "};" << std::endl;

    str << "}" << std::endl;
    return str.str();
  }

  // Returns the modalities labels of the variables in varsSeq
  template <typename GUM_SCALAR>
  INLINE std::string BIFWriter<GUM_SCALAR>::__variablesLabels(
      const Sequence<const DiscreteVariable *> &varsSeq, const Instantiation &inst) {
    std::stringstream str;
    const DiscreteVariable *varPtr = nullptr;

    for (Idx i = 1; i < varsSeq.size() - 1; i++) {
      varPtr = varsSeq[i];
      str << varPtr->label(inst.val(*varPtr)) << ", ";
    }

    varPtr = varsSeq[varsSeq.size() - 1];

    str << varPtr->label(inst.val(*varPtr));
    return str.str();
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;

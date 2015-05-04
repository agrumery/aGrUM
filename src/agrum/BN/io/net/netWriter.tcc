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
#include <agrum/BN/io/net/netWriter.h>

namespace gum {

  /* ============================================================================ */
  /* ===                           GUM_BN_WRITER                              === */
  /* ============================================================================ */
  // Default constructor.
  template <typename GUM_SCALAR> INLINE NetWriter<GUM_SCALAR>::NetWriter() {
    GUM_CONSTRUCTOR(NetWriter);
  }

  // Default destructor.
  template <typename GUM_SCALAR> INLINE NetWriter<GUM_SCALAR>::~NetWriter() {
    GUM_DESTRUCTOR(NetWriter);
  }

  //
  // Writes a Bayesian Network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian Network writen in output.
  // @throws Raised if an I/O error occurs.
  template <typename GUM_SCALAR>
  INLINE void NetWriter<GUM_SCALAR>::write(std::ostream &output,
                                           const IBayesNet<GUM_SCALAR> &bn) {
    if (!output.good())
      GUM_ERROR(IOError, "Stream states flags are not all unset.");

    output << __header(bn) << std::endl;

    for (auto node : bn.nodes())
      output << __variableBloc(bn.variable(node)) << std::endl;

    for (auto node : bn.nodes())
      output << __variableCPT(bn.cpt(node));

    output << std::endl;

    output.flush();

    if (output.fail()) {
      GUM_ERROR(IOError, "Writting in the ostream failed.");
    }
  }

  // Writes a Bayesian Network in the referenced file using the BN format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian Network.
  // @param bn The Bayesian Network writed in the file.
  // @throws Raised if an I/O error occurs.
  template <typename GUM_SCALAR>
  INLINE void NetWriter<GUM_SCALAR>::write(std::string filePath,
                                           const IBayesNet<GUM_SCALAR> &bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    if (!output.good()) {
      GUM_ERROR(IOError, "Stream states flags are not all unset.");
    }

    output << __header(bn) << std::endl;

    for (auto node : bn.nodes())
      output << __variableBloc(bn.variable(node)) << std::endl;

    for (auto node : bn.nodes())
      output << __variableCPT(bn.cpt(node));

    output << std::endl;

    output.flush();
    output.close();

    if (output.fail()) {
      GUM_ERROR(IOError, "Writting in the ostream failed.");
    }
  }

  // Returns a bloc defining a variable's CPT in the BN format.
  template <typename GUM_SCALAR>
  INLINE std::string
  NetWriter<GUM_SCALAR>::__variableCPT(const Potential<GUM_SCALAR> &cpt) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation

    if (cpt.nbrDim() == 1) {
      Instantiation inst(cpt);
      str << "potential (" << cpt.variable(0).name() << ") {" << std::endl << tab
          << "data = ( ";

      for (inst.setFirst(); !inst.end(); ++inst) {
        str << " " << cpt[inst];
      }

      str << ");" << std::endl << "}" << std::endl;
    } else if (cpt.domainSize() > 1) {
      // Instantiation inst( cpt );
      Instantiation condVars; // Instantiation on the conditioning variables
      const Sequence<const DiscreteVariable *> &varsSeq = cpt.variablesSequence();
      str << "potential ( " << (varsSeq[(Idx)0])->name() << " | ";

      for (Idx i = 0; i < varsSeq.size(); i++) {
        if (i != 0)
          str << varsSeq[i]->name() << "   ";

        condVars << *(varsSeq[i]);
      }

      // condVars << *( varsSeq[(Idx)0] );
      str << ") {" << std::endl << tab << "data = ";

      for (condVars.setFirst(); !condVars.end(); ++condVars) {

        // Writing the probabilities of the variable
        if (condVars.valFromPtr(varsSeq[(Idx)0]) !=
                (varsSeq[(Idx)0])->domainSize() - 1 &&
            condVars.valFromPtr(varsSeq[(Idx)0]) != 0) {
          str << tab << cpt[condVars];
        } else {
          bool notend = false;

          for (Idx i = 0; i < condVars.nbrDim(); i++) {
            if (condVars.val(condVars.variable(i)) == 0) {
              notend = true;
              str << (i == 0 ? "\n   " : "") << "(";
            } else
              break;
          }

          str << tab << cpt[condVars];

          if (!notend) {
            for (Idx i = 0; i < condVars.nbrDim(); i++) {

              if (condVars.val(condVars.variable(i)) ==
                  condVars.variable(i).domainSize() - 1)
                str << ")" << (i == condVars.nbrDim() - 1 ? ";" : "");
              else
                break;
            }
          }
        }
      }

      //       inst.unsetOverflow();
      str << "\n}" << std::endl;
    }

    return str.str();
  }

  // Returns the header of the BN file.
  template <typename GUM_SCALAR>
  INLINE std::string
  NetWriter<GUM_SCALAR>::__header(const IBayesNet<GUM_SCALAR> &bn) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation
    str << std::endl << "net {" << std::endl;
    str << "  name = " << bn.propertyWithDefault("name", "unnamedBN") << ";"
        << std::endl;
    str << "  software = \"aGrUM " << GUM_VERSION << "\";" << std::endl;
    str << "  node_size = (50 50);" << std::endl;
    str << "}" << std::endl;
    return str.str();
  }

  // Returns a bloc defining a variable in the BN format.
  template <typename GUM_SCALAR>
  INLINE std::string
  NetWriter<GUM_SCALAR>::__variableBloc(const DiscreteVariable &var) {
    std::stringstream str;
    std::string tab = "   "; // poor tabulation
    str << "node " << var.name() << " {" << std::endl;
    str << tab << "states = (";

    for (Idx i = 0; i < var.domainSize(); i++) {
      str << var.label(i) << " ";
    }

    str << ");" << std::endl;
    str << tab << "label = \"" << var.name() << "\";" << std::endl;
    str << tab << "ID = \"" << var.name() << "\";" << std::endl;

    str << "}" << std::endl;

    return str.str();
  }
} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS

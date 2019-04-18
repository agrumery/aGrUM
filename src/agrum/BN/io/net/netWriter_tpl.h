
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

// to ease parsing in IDE
#  include <agrum/BN/io/net/netWriter.h>

namespace gum {
  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE NetWriter< GUM_SCALAR >::NetWriter() {
    GUM_CONSTRUCTOR(NetWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR >
  INLINE NetWriter< GUM_SCALAR >::~NetWriter() {
    GUM_DESTRUCTOR(NetWriter);
  }

  //
  // Writes a Bayesian Network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian Network writen in output.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR >
  INLINE void NetWriter< GUM_SCALAR >::write(std::ostream&                  output,
                                             const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good())
      GUM_ERROR(IOError, "Stream states flags are not all unset.");

    output << __header(bn) << std::endl;

    for (auto node : bn.nodes())
      output << __variableBloc(bn.variable(node)) << std::endl;

    for (auto node : bn.nodes())
      output << __variableCPT(bn.cpt(node));

    output << std::endl;

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writting in the ostream failed."); }
  }

  // Writes a Bayesian Network in the referenced file using the BN format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian Network.
  // @param bn The Bayesian Network writed in the file.
  // @throws Raised if an I/O error occurs.
  template < typename GUM_SCALAR >
  INLINE void NetWriter< GUM_SCALAR >::write(const std::string& filePath,
                                             const IBayesNet< GUM_SCALAR >& bn) {
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

    if (output.fail()) { GUM_ERROR(IOError, "Writting in the ostream failed."); }
  }

  // Returns a bloc defining a variable's CPT in the BN format.
  template < typename GUM_SCALAR >
  INLINE std::string
         NetWriter< GUM_SCALAR >::__variableCPT(const Potential< GUM_SCALAR >& cpt) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation

    Instantiation inst(cpt);
    if (cpt.nbrDim() == 1) {
      str << "potential (" << cpt.variable(0).name() << ") {" << std::endl
          << tab << "data = ( ";

      for (inst.setFirst(); !inst.end(); ++inst) {
        str << " " << cpt[inst];
      }

      str << ");";
    } else {   // cpt.domainSize() > 1
      const Sequence< const DiscreteVariable* >& varsSeq = cpt.variablesSequence();

      Instantiation conds;
      for (Idx i = 1; i < varsSeq.size(); i++)
        conds.add(*varsSeq[i]);

      str << "potential ( " << (varsSeq[(Idx)0])->name() << " | ";
      for (Idx i = 1; i < varsSeq.size(); i++)
        str << varsSeq[i]->name() << "   ";
      str << ") {" << std::endl << tab << "data = \n";

      std::string comment;
      conds.setFirst();
      while (true) {
        str << tab << "(";
        for (Idx i = 0; i < conds.nbrDim(); i++) {
          if (conds.val(i) != 0) break;
          str << "(";
        }

        inst.setVals(conds);
        for (inst.setFirstVar(*varsSeq[0]); !inst.end(); inst.incVar(*varsSeq[0]))
          str << tab << cpt[inst];

        comment = tab + "% ";
        for (Idx i = 0; i < conds.nbrDim(); i++) {
          comment += conds.variable(i).name() + "="
                     + conds.variable(i).label(conds.val(i)) + tab;
        }

        ++conds;
        if (conds.end()) {
          for (Idx i = 0; i < inst.nbrDim(); i++) {
            str << ")";
          }
          str << ";" << comment;
          break;
        } else {
          for (Idx i = 0; i < conds.nbrDim(); i++) {
            str << ")";
            if (conds.val(i) != 0) break;
          }
          str << comment << "\n";
        }
      }
    }
    str << "\n}\n" << std::endl;
    return str.str();
  }

  // Returns the header of the BN file.
  template < typename GUM_SCALAR >
  INLINE std::string
         NetWriter< GUM_SCALAR >::__header(const IBayesNet< GUM_SCALAR >& bn) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation
    str << std::endl << "net {" << std::endl;
    str << "  name = " << bn.propertyWithDefault("name", "unnamedBN") << ";"
        << std::endl;
    str << "  software = \"aGrUM " << GUM_VERSION << "\";" << std::endl;
    str << "  node_size = (50 50);" << std::endl;
    str << "}" << std::endl;
    return str.str();
  }

  // Returns a bloc defining a variable in the BN format.
  template < typename GUM_SCALAR >
  INLINE std::string
         NetWriter< GUM_SCALAR >::__variableBloc(const DiscreteVariable& var) {
    std::stringstream str;
    std::string       tab = "   ";   // poor tabulation
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

#endif   // DOXYGEN_SHOULD_SKIP_THIS

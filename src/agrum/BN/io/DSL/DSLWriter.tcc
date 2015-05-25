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

/** @file
 * @brief Templates implementation of bns/io/gumBNWriter.h classes.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease automatic parser
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/io/DSL/DSLWriter.h>

namespace gum {

  /* ============================================================================ */
  /* ===                           GUM_DSL_WRITTER                             === */
  /* ============================================================================ */
  // Default constructor.
  template <typename GUM_SCALAR> INLINE DSLWriter<GUM_SCALAR>::DSLWriter() {
    GUM_CONSTRUCTOR(DSLWriter);
  }

  // Default destructor.
  template <typename GUM_SCALAR> INLINE DSLWriter<GUM_SCALAR>::~DSLWriter() {
    GUM_DESTRUCTOR(DSLWriter);
  }

  /** Writes a Bayesian Network in the output stream using the DSL format.
   * @param ouput The output stream.
   * @param bn The Bayesian Network writen in output.
   * @throws Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  void DSLWriter<GUM_SCALAR>::write(std::ostream &output,
                                    const IBayesNet<GUM_SCALAR> &bn) {
    if (!output.good()) {
      GUM_ERROR(IOError, "Stream states flags are not all unset.");
    }

    output << "net " << bn.propertyWithDefault("name", "unnamedBN") << std::endl
           << "{" << std::endl;

    output << "// property softwar aGrUM " << GUM_VERSION << std::endl << std::endl;

    for (auto node : bn.topologicalOrder()) {
      output << __variableBloc(bn, bn.variable(node));
    }

    output << "};";

    output.flush();

    if (output.fail()) {
      GUM_ERROR(IOError, "Writting in the ostream failed.");
    }
  }

  /** Writes a Bayesian Network in the referenced file using the DSL format.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   * @param filePath The path to the file used to write the Bayesian Network.
   * @param bn The Bayesian Network writed in the file.
   * @throws Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  void DSLWriter<GUM_SCALAR>::write(std::string filePath,
                                    const IBayesNet<GUM_SCALAR> &bn) {
    std::filebuf fb;
    fb.open(filePath.c_str(), std::ios::out);
    std::ostream output(&fb);

    write(output, bn);

    fb.close();
  }

  /**
   * Returns a bloc defining a variable in the DSL format.
   */
  template <typename GUM_SCALAR>
  std::string DSLWriter<GUM_SCALAR>::__variableBloc(const IBayesNet<GUM_SCALAR> &bn,
                                                    const DiscreteVariable &var) {
    NodeId id;
    gum::Size i = 0;
    std::ostringstream oss;

    std::string val("");

    id = bn.idFromName(var.name());

    oss << "\tnode " << var.name() << "\n\t{\n";

    oss << "\t\tTYPE = CPT;\n";

    oss << "\t\tHEADER =\n\t\t{\n";
    oss << "\t\t\tID = " << var.name() << ";\n";
    oss << "\t\t\tNAME = \"" << var.name() << "\";\n";
    oss << "\t\t};\n";

    oss << "\t\tPARENTS = (";
    const Sequence<const DiscreteVariable *> &tmp_vars =
        bn.cpt(id).variablesSequence();

    for (Idx i = tmp_vars.size() - 1; i > 0; i--) {
      if (i < tmp_vars.size() - 1)
        oss << ", ";

      oss << tmp_vars[i]->name();
    }

    oss << ");\n";

    oss << "\t\tDEFINITION =\n\t\t{\n";

    ////////////////////////////
    oss << "\t\t\tNAMESTATES = (";

    for (i = 0; i < var.domainSize(); i++) {
      if (i != 0)
        oss << ", ";

      oss << var.label(i);
    }

    oss << ");\n";

    ////////////////////////////

    ////////////////////////////
    oss << "\t\t\tPROBABILITIES = (";
    i = 0;

    for (Instantiation iter = bn.cpt(id).getMasterRef(); i < bn.cpt(id).domainSize();
         ++iter) {
      if (i != 0)
        oss << ", ";

      oss << bn.cpt(id)[iter];

      i++;
    }

    oss << ");\n";

    ///////////////////////////

    oss << "\t\t};\n";

    oss << "\t};\n\n";

    return oss.str();
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;

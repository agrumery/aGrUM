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


/** @file
 * @brief Templates implementation of bns/io/gumBNWriter.h classes.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease automatic parser
#  include <agrum/BN/IBayesNet.h>
#  include <agrum/BN/io/DSL/DSLWriter.h>

namespace gum {
  /* =========================================================================*/
  /* ===                           GUM_DSL_WRITER === */
  /* =========================================================================*/
  // Default constructor.
  template < typename GUM_SCALAR >
  INLINE DSLWriter< GUM_SCALAR >::DSLWriter() {
    GUM_CONSTRUCTOR(DSLWriter);
  }

  // Default destructor.
  template < typename GUM_SCALAR >
  INLINE DSLWriter< GUM_SCALAR >::~DSLWriter() {
    GUM_DESTRUCTOR(DSLWriter);
  }

  /** Writes a Bayesian network in the output stream using the DSL format.
   * @param ouput The output stream.
   * @param bn The Bayesian network writen in output.
   * @throws Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  void DSLWriter< GUM_SCALAR >::_doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    output << "net " << bn.propertyWithDefault("name", "unnamedBN") << std::endl
           << "{" << std::endl;

    output << "// property softwar aGrUM " << GUM_VERSION << std::endl << std::endl;

    for (auto node: bn.topologicalOrder()) {
      output << _variableBloc_(bn, bn.variable(node));
    }

    output << "};";

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /** Writes a Bayesian network in the referenced file using the DSL format.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   * @param filePath The path to the file used to write the Bayesian network.
   * @param bn The Bayesian network writed in the file.
   * @throws Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  void DSLWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                         const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /**
   * Returns a bloc defining a variable in the DSL format.
   */
  template < typename GUM_SCALAR >
  std::string DSLWriter< GUM_SCALAR >::_variableBloc_(const IBayesNet< GUM_SCALAR >& bn,
                                                      const DiscreteVariable&        var) {
    NodeId             id;
    std::ostringstream oss;

    id = bn.idFromName(var.name());

    oss << "\tnode " << this->_onlyValidCharsInName(var.name()) << "\n\t{\n";

    oss << "\t\tTYPE = CPT;\n";

    oss << "\t\tHEADER =\n\t\t{\n";
    oss << "\t\t\tID = " << this->_onlyValidCharsInName(var.name()) << ";\n";
    oss << "\t\t\tNAME = \"" << this->_onlyValidCharsInName(var.name()) << "\";\n";
    oss << "\t\t};\n";

    oss << "\t\tPARENTS = (";
    const Sequence< const DiscreteVariable* >& tmp_vars = bn.cpt(id).variablesSequence();

    for (Idx i = tmp_vars.size() - 1; i > 0; i--) {
      if (i < tmp_vars.size() - 1) oss << ", ";

      oss << this->_onlyValidCharsInName(tmp_vars[i]->name());
    }

    oss << ");\n";

    oss << "\t\tDEFINITION =\n\t\t{\n";

    ////////////////////////////
    oss << "\t\t\tNAMESTATES = (";

    for (Idx i = 0; i < var.domainSize(); i++) {
      if (i != 0) oss << ", ";

      oss << this->_onlyValidCharsInName(var.label(i));
    }

    oss << ");\n";

    ////////////////////////////

    ////////////////////////////
    oss << "\t\t\tPROBABILITIES = (";
    Idx i = 0;

    Instantiation iter(*bn.cpt(id).content());
    for (iter.setFirst(); i < bn.cpt(id).domainSize(); ++iter, ++i) {
      if (i != 0) oss << ", ";
      oss << bn.cpt(id)[iter];
    }

    oss << ");\n";

    ///////////////////////////

    oss << "\t\t};\n";

    oss << "\t};\n\n";

    return oss.str();
  }

  template < typename GUM_SCALAR >
  void DSLWriter< GUM_SCALAR >::_syntacticalCheck(const IBayesNet< GUM_SCALAR >& bn) {
    this->_validCharInNamesCheck(bn);
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

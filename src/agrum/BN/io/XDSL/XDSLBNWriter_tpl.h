/**
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#  include <agrum/BN/io/XDSL/XDSLBNWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE XDSLBNWriter< GUM_SCALAR >::XDSLBNWriter() {
    GUM_CONSTRUCTOR(XDSLBNWriter);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE XDSLBNWriter< GUM_SCALAR >::~XDSLBNWriter() {
    GUM_DESTRUCTOR(XDSLBNWriter);
  }

  /*
   * Writes a bayes net in the given ouput stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                   const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    _heading_(output, bn);
    output << std::endl;

    // heading variables
    output << "  <nodes>" << std::endl;
    for (auto node: bn.topologicalOrder())
      _variableBloc_(output, node, bn);
    output << "  </nodes>" << std::endl;
    output << std::endl;

    _headingExtension_(output, bn);
    for (auto node: bn.topologicalOrder())
      _variableExtension_(output, node, bn);
    _endExtension_(output);

    output << std::endl;

    _documentEnd_(output);

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
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
  INLINE void XDSLBNWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                                   const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /*
   * Send to output the header of the XDSL file.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_heading_(std::ostream&                  output,
                                                    const IBayesNet< GUM_SCALAR >& bn) {
    // Header for every xml
    output << "<?xml version=\"1.0\" ?>" << std::endl;
    output << "<!-- This network was created by pyAgrum " << GUM_VERSION
           << " for any purpose you want -->" << std::endl;
    // Network declaration
    output << "<smile version=\"1.0\" id=\"" << bn.propertyWithDefault("name", "unnamedBN") << "\">"
           << std::endl;
  }

  /*
   * Send to output the header of the extension section
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_headingExtension_(std::ostream&                  output,
                                                             const IBayesNet< GUM_SCALAR >& bn) {
    // Header for every xml
    output << "  <extensions>" << std::endl;
    output << "    <genie version=\"1.0\" app=\"aGrUM " << GUM_VERSION << "\" name=\""
           << bn.propertyWithDefault("name", "unnamedBN") << "\">" << std::endl;
  }
  /*
   * Send to output a bloc defining a variable in the XDSL format.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_variableBloc_(std::ostream&                  output,
                                                         const NodeId&                  varNodeId,
                                                         const IBayesNet< GUM_SCALAR >& bn) {
    //<cpt id="Org_arrangements">
    //   <state id="LOW" />
    //   <state id="HIGH" />
    //   <parents>Business_planning</parents>
    //   <probabilities>0.6909999999999999 0.3090000000000001 0.426
    //   0.5740000000000001</probabilities>
    //</cpt>
    const auto& var = bn.variable(varNodeId);

    output << "    <cpt id=\"" << this->_buildNameWithOnlyValidChars(var.name()) << "\">"
           << std::endl;

    // labels
    for (const auto& lab: var.labels()) {
      output << "      <state id=\"" << lab << "\" />" << std::endl;
    }

    bool first;

    // parents
    const auto& cpt = bn.cpt(varNodeId);
    const auto  nd  = cpt.nbrDim();
    if (nd > 1) {
      output << "      <parents>";
      first = true;
      for (Idx i = 1; i < nd; i++) {
        if (first) first = false;
        else output << " ";

        output << this->_buildNameWithOnlyValidChars(cpt.variable(i).name());
      }
      output << "</parents>" << std::endl;
    }

    output << "      <probabilities>";
    gum::Instantiation I(cpt);
    I.setFirst();
    first = true;
    while (!I.end()) {
      if (first) first = false;
      else output << " ";
      output << cpt[I];
      I.inc();
    }
    output << "</probabilities>" << std::endl;

    //     //Closing tag
    output << "    </cpt>" << std::endl;
  }

  /*
   * Send to output a bloc defining a variable's CPT in the XDSL format.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_variableExtension_(std::ostream& output,
                                                              const NodeId& varNodeId,
                                                              const IBayesNet< GUM_SCALAR >& bn) {
    //<node id="Vendor_support">
    //   <name>Vendor support</name>
    //</node>
    const std::string& name = bn.variable(varNodeId).name();
    output << "      <node id=\"" << this->_buildNameWithOnlyValidChars(name) << "\">" << std::endl;
    output << "        <name>" << name << "</name>" << std::endl;
    output << "      </node>" << std::endl;
  }

  /*
   * Send to output the end of the XDSL file.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_endExtension_(std::ostream& output) {
    output << "    </genie>" << std::endl;
    output << "  </extensions>" << std::endl;
  }
  /*
   * Send to output the end of the XDSL file.
   */
  template < typename GUM_SCALAR >
  INLINE void XDSLBNWriter< GUM_SCALAR >::_documentEnd_(std::ostream& output) {
    output << "</smile>" << std::endl;
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

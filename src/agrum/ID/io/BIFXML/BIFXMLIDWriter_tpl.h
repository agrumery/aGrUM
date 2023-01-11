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

#  include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLIDWriter< GUM_SCALAR >::BIFXMLIDWriter() {
    GUM_CONSTRUCTOR(BIFXMLIDWriter);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLIDWriter< GUM_SCALAR >::~BIFXMLIDWriter() {
    GUM_DESTRUCTOR(BIFXMLIDWriter);
  }

  /*
   * Writes an influence diagram in the given ouput stream.
   *
   * @param output The output stream.
   * @param infdiag The influence diagram writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void BIFXMLIDWriter< GUM_SCALAR >::write(std::ostream&                         output,
                                                  const InfluenceDiagram< GUM_SCALAR >& infdiag) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    output << _heading_() << std::endl;
    output << "<!-- Variables -->" << std::endl;

    for (const auto node: infdiag.nodes()) {
      int nodeType = 1;

      if (infdiag.isChanceNode(node)) nodeType = 2;
      else if (infdiag.isUtilityNode(node)) nodeType = 3;

      output << _variableBloc_(infdiag.variable(node), nodeType) << std::endl;
    }

    output << "<!-- Probability distributions -->" << std::endl;

    for (const auto node: infdiag.nodes())
      output << _variableDefinition_(node, infdiag);

    output << std::endl;
    output << _documentend_();
    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /*
   * Writes an Influence Diagram in the file referenced by filePath.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the Influence Diagram.
   * @param infdiag The Influence Diagram writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void BIFXMLIDWriter< GUM_SCALAR >::write(std::string                           filePath,
                                                  const InfluenceDiagram< GUM_SCALAR >& infdiag) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    write(output, infdiag);

    output.close();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /*
   * Returns the header of the BIF file.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLIDWriter< GUM_SCALAR >::_heading_() {
    std::stringstream str;

    // Header for every xml
    str << "<?xml version=\"1.0\" ?>" << std::endl;

    // Document type definition of BIF 0.3
    /*str << "<!-- DTD for the XMLBIF 0.3 format -->" << std::endl;
    str << "<!DOCTYPE BIF [" << std::endl;
    str << "\t<!ELEMENT BIF ( NETWORK )*>" << std::endl;
    str << "\t\t<!ATTLIST BIF VERSION CDATA #REQUIRED>" << std::endl;
    str << "\t<!ELEMENT NETWORK ( NAME, ( PROPERTY | VARIABLE | DEFINITION )*
    )>" <<
    std::endl;
    str << "\t<!ELEMENT NAME (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT VARIABLE ( NAME, ( OUTCOME |  PROPERTY )* ) >" <<
    std::endl;
    str << "\t\t<!ATTLIST VARIABLE TYPE (nature|decision|utility) \"nature\">"
    <<
    std::endl;
    str << "\t<!ELEMENT OUTCOME (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT DEFINITION ( FOR | GIVEN | TABLE | PROPERTY )* >" <<
    std::endl;
    str << "\t<!ELEMENT FOR (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT GIVEN (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT TABLE (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT PROPERTY (#PCDATA)>" << std::endl;
    str << "]>" << std::endl;*/

    // BIF version Tag
    str << std::endl << "<BIF VERSION=\"0.3\">" << std::endl;

    // Network declaration
    str << "<NETWORK>" << std::endl;

    return str.str();
  }

  /*
   * Returns a bloc defining a variable in the BIF format.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLIDWriter< GUM_SCALAR >::_variableBloc_(const DiscreteVariable& var,
                                                                  int                     varType) {
    //<VARIABLE TYPE="nature|decision|utility">
    //<NAME>name</NAME>
    //<OUTCOME>outcome1</OUTCOME>
    //<OUTCOME>outcome2</OUTCOME>
    //<PROPERTY>property</PROPERTY>
    //</VARIABLE>

    std::stringstream str;

    // Declaration of variable and his type
    str << "<VARIABLE TYPE=\"";

    switch (varType) {
      case 1: str << "decision"; break;

      case 2: str << "nature"; break;

      case 3: str << "utility"; break;

      default: break;
    }

    str << "\">" << std::endl;

    // Name and description
    str << "\t<NAME>" << var.name() << "</NAME>" << std::endl;
    str << "\t<PROPERTY>" << var.description() << "</PROPERTY>" << std::endl;

    // Outcomes

    for (Idx i = 0; i < var.domainSize(); i++)
      str << "\t<OUTCOME>" << var.label(i) << "</OUTCOME>" << std::endl;

    //     //Closing tag
    str << "</VARIABLE>" << std::endl;

    return str.str();
  }

  /*
   * Returns a bloc defining a variable's CPT in the BIF format.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLIDWriter< GUM_SCALAR >::_variableDefinition_(
     const NodeId&                         varNodeId,
     const InfluenceDiagram< GUM_SCALAR >& infdiag) {
    //<DEFINITION>
    //<FOR>var</FOR>
    //<GIVEN>conditional var</GIVEN>
    //<TABLE>conditional probabilities</TABLE>
    //</DEFINITION>
    std::stringstream str;

    if (!((infdiag.isDecisionNode(varNodeId)) && (infdiag.parents(varNodeId).empty()))) {
      // Declaration
      str << "<DEFINITION>" << std::endl;

      // Variable
      str << "\t<FOR>" << infdiag.variable(varNodeId).name() << "</FOR>" << std::endl;

      // Conditional Parents
      List< std::string > parentList;

      for (const auto par: infdiag.parents(varNodeId))
        parentList.pushBack(infdiag.variable(par).name());

      for (List< std::string >::iterator parentListIte = parentList.rbegin();
           parentListIte != parentList.rend();
           --parentListIte)
        str << "\t<GIVEN>" << (*parentListIte) << "</GIVEN>" << std::endl;

      if (infdiag.isChanceNode(varNodeId)) {
        Instantiation inst(infdiag.cpt(varNodeId));
        str << "\t<TABLE>";

        for (inst.setFirst(); !inst.end(); inst.inc())
          str << infdiag.cpt(varNodeId)[inst] << " ";

        str << "</TABLE>" << std::endl;
      } else if (infdiag.isUtilityNode(varNodeId)) {
        // Values
        Instantiation inst(infdiag.utility(varNodeId));
        str << "\t<TABLE>";

        for (inst.setFirst(); !inst.end(); inst.inc())
          str << infdiag.utility(varNodeId)[inst] << " ";

        str << "</TABLE>" << std::endl;
      }

      // Closing tag
      str << "</DEFINITION>" << std::endl;
    }

    return str.str();
  }

  /*
   * Returns the end of the BIF file.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLIDWriter< GUM_SCALAR >::_documentend_() {
    std::stringstream str;

    str << "</NETWORK>" << std::endl;
    str << "</BIF>" << std::endl;

    return str.str();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

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

#  include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLBNWriter< GUM_SCALAR >::BIFXMLBNWriter() {
    GUM_CONSTRUCTOR(BIFXMLBNWriter);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLBNWriter< GUM_SCALAR >::~BIFXMLBNWriter() {
    GUM_DESTRUCTOR(BIFXMLBNWriter);
  }

  /*
   * Writes a bayes net in the given ouput stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void BIFXMLBNWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                     const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    output << _heading_(bn) << std::endl;

    output << "<!-- Variables -->" << std::endl;

    for (auto node: bn.nodes())
      output << _variableBloc_(bn.variable(node)) << std::endl;

    output << "<!-- Probability distributions -->" << std::endl;

    for (auto node: bn.nodes())
      output << _variableDefinition_(node, bn);

    output << std::endl;

    output << _documentend_();

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
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
  template < typename GUM_SCALAR >
  INLINE void BIFXMLBNWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                                     const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /*
   * Returns the header of the BIF file.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLBNWriter< GUM_SCALAR >::_heading_(const IBayesNet< GUM_SCALAR >& bn) {
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
    str << "<NAME>" << bn.propertyWithDefault("name", "unnamedBN") << "</NAME>" << std::endl;
    str << "<PROPERTY>software aGrUM</PROPERTY>" << std::endl;

    return str.str();
  }

  /*
   * Returns a bloc defining a variable in the BIF format.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLBNWriter< GUM_SCALAR >::_variableBloc_(const DiscreteVariable& var) {
    //<VARIABLE TYPE="nature|decision|utility">
    //<NAME>name</NAME>
    //<OUTCOME>outcome1</OUTCOME>
    //<OUTCOME>outcome2</OUTCOME>
    //<PROPERTY>property</PROPERTY>
    //</VARIABLE>

    std::stringstream str;

    // Declaration of variable and his type
    str << "<VARIABLE TYPE=\"nature\">" << std::endl;

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
  INLINE std::string
         BIFXMLBNWriter< GUM_SCALAR >::_variableDefinition_(const NodeId&                  varNodeId,
                                                        const IBayesNet< GUM_SCALAR >& bn) {
    //<DEFINITION>
    //<FOR>var</FOR>
    //<GIVEN>conditional var</GIVEN>
    //<TABLE>conditional probabilities</TABLE>
    //</DEFINITION>
    std::stringstream str;

    // Declaration
    str << "<DEFINITION>" << std::endl;

    // Variable
    str << "\t<FOR>" << bn.variable(varNodeId).name() << "</FOR>" << std::endl;

    // Table
    // For historical reason, the code is not the same betwen bIXML for BN and
    // for ID
    // ...
    const Potential< GUM_SCALAR >& cpt = bn.cpt(varNodeId);

    // Conditional Parents
    for (Idx i = 1; i < cpt.nbrDim(); i++)
      str << "\t<GIVEN>" << cpt.variable(i).name() << "</GIVEN>" << std::endl;

    Instantiation inst;
    inst << cpt.variable(0);

    for (Idx i = cpt.nbrDim() - 1; i > 0; i--)
      inst << cpt.variable(i);

    str << "\t<TABLE>";

    for (inst.setFirst(); !inst.end(); inst.inc()) {
      if (inst.val(0) == 0) str << std::endl << "\t\t";
      else str << " ";

      str << cpt[inst];   //"<!-- "<<inst<<" -->"<<std::endl;
    }

    str << std::endl << "\t</TABLE>" << std::endl;

    // Closing tag
    str << "</DEFINITION>" << std::endl;

    return str.str();
  }

  /*
   * Returns the end of the BIF file.
   */
  template < typename GUM_SCALAR >
  INLINE std::string BIFXMLBNWriter< GUM_SCALAR >::_documentend_() {
    std::stringstream str;

    str << "</NETWORK>" << std::endl;
    str << "</BIF>" << std::endl;

    return str.str();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

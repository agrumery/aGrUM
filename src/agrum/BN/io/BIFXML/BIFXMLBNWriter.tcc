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

#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>



namespace gum {

  /*
   * Default constructor.
   */
  template<typename T_DATA> INLINE
  BIFXMLBNWriter<T_DATA>::BIFXMLBNWriter()
  {
    GUM_CONSTRUCTOR( BIFXMLBNWriter );
  }

  /*
   * Destructor.
   */
  template<typename T_DATA> INLINE
  BIFXMLBNWriter<T_DATA>::~BIFXMLBNWriter() 
  {
    GUM_DESTRUCTOR( BIFXMLBNWriter );
  }

  /*
   * Writes a bayes net in the given ouput stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template<typename T_DATA> INLINE
  void
  BIFXMLBNWriter<T_DATA>::write( std::ostream &output, const BayesNet<T_DATA>& bn )
  {
    if ( ! output.good() )
      GUM_ERROR( IOError, "Stream states flags are not all unset." );

    output << __heading() << std::endl;

    output << "<!-- Variables -->" << std::endl;
    for ( DAG::NodeIterator iter = bn.beginNodes(); iter != bn.endNodes(); ++iter )
	output << __variableBloc( bn.variable( *iter ) ) << std::endl;

    output << "<!-- Probability distributions -->" << std::endl;
    for ( DAG::NodeIterator iter = bn.beginNodes(); iter != bn.endNodes(); ++iter )
      output << __variableDefinition( *iter, bn );

    output << std::endl;
    
    output << __documentend();

    output.flush();

    if ( output.fail() )
      GUM_ERROR( IOError, "Writting in the ostream failed." );
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
  template<typename T_DATA> INLINE
  void
  BIFXMLBNWriter<T_DATA>::write( std::string filePath, const BayesNet<T_DATA>& bn ) 
  {
    std::ofstream output( filePath.c_str(), std::ios_base::trunc );

    write(output, bn);
    
    output.close();

    if ( output.fail() )
      GUM_ERROR( IOError, "Writting in the ostream failed." );
  }

  

  /*
   * Returns the header of the BIF file.
   */
  template<typename T_DATA> INLINE
  std::string
  BIFXMLBNWriter<T_DATA>::__heading()
  {
    std::stringstream str;
    
    // Header for every xml
    str << "<?xml version=\"1.0\" ?>" << std::endl;
    
    // Document type definition of BIF 0.3
    /*str << "<!-- DTD for the XMLBIF 0.3 format -->" << std::endl;
    str << "<!DOCTYPE BIF [" << std::endl;
    str << "\t<!ELEMENT BIF ( NETWORK )*>" << std::endl;
    str << "\t\t<!ATTLIST BIF VERSION CDATA #REQUIRED>" << std::endl;
    str << "\t<!ELEMENT NETWORK ( NAME, ( PROPERTY | VARIABLE | DEFINITION )* )>" << std::endl;
    str << "\t<!ELEMENT NAME (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT VARIABLE ( NAME, ( OUTCOME |  PROPERTY )* ) >" << std::endl;
    str << "\t\t<!ATTLIST VARIABLE TYPE (nature|decision|utility) \"nature\">" << std::endl;
    str << "\t<!ELEMENT OUTCOME (#PCDATA)>" << std::endl;
    str << "\t<!ELEMENT DEFINITION ( FOR | GIVEN | TABLE | PROPERTY )* >" << std::endl;
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
  template<typename T_DATA> INLINE
  std::string
  BIFXMLBNWriter<T_DATA>::__variableBloc( const DiscreteVariable& var )
  {
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
    
    //Outcomes
    for ( Idx i = 0; i < var.domainSize(); i++ )
      str << "\t<OUTCOMES>" << var.label( i ) << "</OUTCOMES>" << std::endl;

//     //Closing tag
    str << "</VARIABLE>" << std::endl;
    
    return str.str();
  }

  /*
   * Returns a bloc defining a variable's CPT in the BIF format.
   */
  template<typename T_DATA> INLINE
  std::string
  BIFXMLBNWriter<T_DATA>::__variableDefinition( const NodeId& varNodeId, const BayesNet<T_DATA>& bn  )
  {
    //<DEFINITION>
	    //<FOR>var</FOR>
	    //<GIVEN>conditional var</GIVEN>
	    //<TABLE>conditional probabilities</TABLE>
    //</DEFINITION>
    std::stringstream str;
    
    //Declaration
    str << "<DEFINITION>" << std::endl;
    
    // Variable
    str << "\t<FOR>" << bn.variable(varNodeId).name() << "</FOR>" << std::endl;
    
    // Conditional Parents
    List< std::string > parentList;
		const ArcSet& parentArcs=bn.dag().parents( varNodeId );
    for(ArcSetIterator parentIter = parentArcs.begin(); parentArcs.end(); ++ parentIter)
      parentList.push_back( bn.variable( (*parentIter).tail() ).name() );
    for( List< std::string >::iterator parentListIte = parentList.rbegin(); parentListIte != parentList.rend(); --parentListIte )
      str << "\t<GIVEN>" << ( *parentListIte ) << "</GIVEN>" << std::endl;
    
    // Table
    Instantiation inst( bn.cpt(varNodeId) );
    str << "\t<TABLE>";
    for(inst.setFirst(); !inst.end(); inst.inc())
	str << bn.cpt(varNodeId)[inst] << " ";
    str << "</TABLE>" << std::endl;
      
    // Closing tag
    str << "</DEFINITION>" << std::endl;
    
    return str.str();
  }
  
  
  /*
   * Returns the end of the BIF file.
   */
  template<typename T_DATA> INLINE
  std::string
  BIFXMLBNWriter<T_DATA>::__documentend()
  {
    std::stringstream str;
    
    str << "</NETWORK>" << std::endl;
    str << "</BIF>" << std::endl;
    
    return str.str();
  }
  
  
} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS

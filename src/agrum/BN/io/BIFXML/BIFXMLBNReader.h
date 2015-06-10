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
/**
 * @file
 * @brief classe for import of bayes net from a XML file written with BIF Format
 *
 * Read an bayes net from a XML file with BIF format
 *
 * how to use it :
 * @code
// OPTIONAL LISTENER CLASS
class aSimpleListener : public gum::Listener {
public:
  void whenProceeding(const void *buffer,int percent, std::string status) {
  // percent goes from 0 to 100 (whenLoading is called at most once for each integer
between 0 and 100
    // percent=200 recieved when End Of File.
 }
};
// END OF OPTIONAL LISTENER

 gum::BayesNet<double> bn;

 try {
  gum::BIFXMLBNReader<double> reader(&bn,std::string(args[1]));

  // OPTIONAL SECTION
  aSimpleListener asl;
  GUM_CONNECT( reader, onProceed, asl, aSimpleListener::whenProceing );
  // END OF OPTIONNAL SECTION

  // Not implemented yet section
  //if (reader.proceed()==0) {
   //std::cerr<<"Well done !"<<std::endl;
  //} else {
   //reader.showElegantErrorsAndWarnings();
   //reader.showErrorCounts();
  //}
  // End not implemented section

 } catch (gum::IOError& e) {GUM_SHOWERROR(e);}

  return 0;

 * @endcode
 *
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_BIF_XML_BN_READER_H
#define GUM_BIF_XML_BN_READER_H

#define TIXML_USE_TICPP

#include <sstream>
#include <string>
#include <list>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/signal/signaler2.h>
#include <agrum/core/tinyxml/ticpp/ticpp.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/io/BNReader.h>

namespace gum {

  /**
   * @class BIFXMLBNReader
   * @brief Read an bayes net from an XML file with BIF format.
   * @ingroup bn_group
   *
   * This class import an bayes net from an XML files using BIF format
   * See http://www-2.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/
   * for information about this format.
   *
   */
  template <typename GUM_SCALAR> class BIFXMLBNReader : BNReader<GUM_SCALAR> {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an BN as to be created before and given in parameter.
     */
    BIFXMLBNReader(BayesNet<GUM_SCALAR> *bn, const std::string &filePath);

    /**
     * Default destructor.
     */
    ~BIFXMLBNReader();

    /**
     * Reads the bayes net from the file referenced by filePath  given at the
     * creation of class
     * @return Returns 0 if no error, 1 if any
     * @warning XMLBNReader can not give the number of errors.
     */
    virtual int proceed();

    /**
     * Signaler used to indicates how many percent of the Xml files have been parsed
     * yet
     */
    typename gum::Signaler2<int, std::string> onProceed;

    private:
    /**
     * Parsing xml element containing data on variables
     */
    void __parsingVariables(ticpp::Element *parentNetwork);

    /**
     * fill the diagram
     */
    void __fillingBN(ticpp::Element *parentNetwork);

    /**
     * An handle to the bayes net in which will be load the content of the xml
     * filePath
     */
    BayesNet<GUM_SCALAR> *__bn;

    /**
     * the path to the xml filePath
     */
    std::string __filePath;
  };

  extern template class BIFXMLBNReader<float>;
  extern template class BIFXMLBNReader<double>;
} /* namespace gum */

#include <agrum/BN/io/BIFXML/BIFXMLBNReader.tcc>

#endif // GUM_BIF_XML_BN_READER_H

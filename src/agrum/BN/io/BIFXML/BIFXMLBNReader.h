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
  // percent goes from 0 to 100 (whenLoading is called at most once for each
integer
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
  // END OF OPTIONAL SECTION

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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
GONZALES(_at_AMU)
 */
#ifndef GUM_BIF_XML_BN_READER_H
#define GUM_BIF_XML_BN_READER_H

#define TIXML_USE_TICPP

#include <list>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNReader.h>
#include <agrum/agrum.h>
#include <agrum/tools/external/tinyxml/ticpp/ticpp.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum {

  /**
   * @class BIFXMLBNReader BIFXMLBNReader.h
   *<agrum/BN/io/BIFXML/BIFXMLBNReader.h>
   * @ingroup bn_io
   * @brief Read an bayes net from an XML file with BIF format.
   *
   * This class import an bayes net from an XML files using BIF format
   * See
   *http://www-2.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/
   * for information about this format.
   *
   */
  template < typename GUM_SCALAR >
  class BIFXMLBNReader: BNReader< GUM_SCALAR > {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an BN as to be created before and given in parameter.
     */
    BIFXMLBNReader(BayesNet< GUM_SCALAR >* bn, const std::string& filePath);

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
    Size proceed() final;

    /**
     * Signaler used to indicates how many percent of the Xml files have been
     * parsed
     * yet
     */
    typename gum::Signaler2< int, std::string > onProceed;

    private:
    /**
     * Parsing xml element containing data on variables
     */
    void _parsingVariables_(ticpp::Element* parentNetwork);

    /**
     * fill the diagram
     */
    void _fillingBN_(ticpp::Element* parentNetwork);

    /**
     * An handle to the bayes net in which will be load the content of the xml
     * filePath
     */
    BayesNet< GUM_SCALAR >* _bn_;

    /**
     * the path to the xml filePath
     */
    std::string _filePath_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BIFXMLBNReader< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/BIFXML/BIFXMLBNReader_tpl.h>

#endif   // GUM_BIF_XML_BN_READER_H

/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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
 gum::XDSLBNReader<double> reader(&bn,std::string(args[1]));

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
#ifndef GUM_XDSL_BN_READER_H
#define GUM_XDSL_BN_READER_H

#define TIXML_USE_TICPP

#include <list>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/base/external/tinyxml/ticpp/ticpp.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/io/BNReader.h>

namespace gum {

  /**
   * @class XDSLBNReader XDSLBNReader.h
   *<agrum/BN/io/XDSL/XDSLBNReader.h>
   * @ingroup bn_io
   * @brief Read an bayes net from an XML file with BIF format.
   *
   * This class import an bayes net from an XML files using DSL format
   * for information about this format.
   *
   */
  template < typename GUM_SCALAR >
  class XDSLBNReader: BNReader< GUM_SCALAR > {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an BN as to be created before and given in parameter.
     */
    XDSLBNReader(BayesNet< GUM_SCALAR >* bn, const std::string& filePath);

    /**
     * Default destructor.
     */
    ~XDSLBNReader();

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
     *
     * @return the number of variables found in the file
     */
    Size _parsingCpts_(ticpp::Element* cptsNetwork);

    /**
     * Parsing xml element containing extentions on variables
     */
    void _parsingExtension_(ticpp::Element* nodesNetwork);

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
  extern template class XDSLBNReader< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/XDSL/XDSLBNReader_tpl.h>

#endif   // GUM_XDSL_BN_READER_H

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
#  include <agrum/BN/io/XDSL/XDSLBNReader.h>
#  include <agrum/tools/core/utils_string.h>

namespace gum {
  /*
   * Constructor
   * A reader is created to reading a defined file.
   * Note that an BN has to be created before and given in parameter.
   */
  template < typename GUM_SCALAR >
  INLINE XDSLBNReader< GUM_SCALAR >::XDSLBNReader(BayesNet< GUM_SCALAR >* bn,
                                                  const std::string&      filePath) :
      BNReader< GUM_SCALAR >(bn, filePath) {
    GUM_CONSTRUCTOR(XDSLBNReader);
    _bn_       = bn;
    _filePath_ = filePath;
  }

  /*
   * Default destructor.
   */
  template < typename GUM_SCALAR >
  INLINE XDSLBNReader< GUM_SCALAR >::~XDSLBNReader() {
    GUM_DESTRUCTOR(XDSLBNReader);
  }

  /*
   * Reads the bayes net from the file referenced by filePath  given at the
   * creation
   * of class
   * @return Returns the number of error during the parsing (0 if none).
   */
  template < typename GUM_SCALAR >
  Size XDSLBNReader< GUM_SCALAR >::proceed() {
    try {
      // Loading file
      std::string status = "Loading File ...";
      GUM_EMIT2(onProceed, 0, status);

      ticpp::Document xmlDoc(_filePath_);
      xmlDoc.LoadFile();

      if (xmlDoc.NoChildren()) {
        GUM_ERROR(IOError, ": Loading fail, please check the file for any syntax error.")
      }

      // Finding BIF element
      status = "File loaded. Now looking for DSL element ...";
      GUM_EMIT2(onProceed, 4, status);

      ticpp::Element* bifElement = xmlDoc.FirstChildElement("smile");
      std::string     netName    = "unnamedBN";
      bifElement->GetAttribute("id", &netName, false);
      _bn_->setProperty("name", netName);

      // Finding network element
      status = "smile Element reached. Now searching network ...";
      GUM_EMIT2(onProceed, 7, status);

      ticpp::Element* nodesElement = bifElement->FirstChildElement("nodes");

      // Finding id variables
      status = "Network found. Now proceeding variables instantiation...";
      GUM_EMIT2(onProceed, 10, status);

      _parsingCpts_(nodesElement);

      // Filling diagram
      status = "All variables have been instantiated. Now filling up diagram...";
      GUM_EMIT2(onProceed, 55, status);

      ticpp::Element* extensionsElement
         = bifElement->FirstChildElement("extensions")->FirstChildElement("genie");

      // Filling diagram
      _parsingExtension_(extensionsElement);
      status = "All variables have been renamed. Now filling up diagram...";
      GUM_EMIT2(onProceed, 85, status);


      status = "Instantiation of network completed";
      GUM_EMIT2(onProceed, 100, status);

      return 0;
    } catch (ticpp::Exception& tinyexception) { GUM_ERROR(IOError, tinyexception.what()) }
  }

  template < typename GUM_SCALAR >
  Size XDSLBNReader< GUM_SCALAR >::_parsingCpts_(ticpp::Element* cptsNetwork) {
    // Counting the number of variable for the signal
    Size                              nbVar = Size(0);
    ticpp::Iterator< ticpp::Element > varIte("cpt");

    for (varIte = varIte.begin(cptsNetwork); varIte != varIte.end(); ++varIte)
      nbVar++;
    nbVar              = 3 * nbVar;   // 3 loops on vars
    std::string status = "Network found. Now proceeding variables instantiation...";

    // Iterating on variable element
    int nbIte = 0;

    // definition of the variables
    for (varIte = varIte.begin(cptsNetwork); varIte != varIte.end(); ++varIte) {
      ticpp::Element* currentVar = varIte.Get();

      // Getting variable name
      std::string varName        = currentVar->GetAttribute("id");
      std::string varDescription = varName;

      // Instanciation de la variable
      auto newVar = new LabelizedVariable(varName, varDescription, 0);

      // Getting variable outcomes
      ticpp::Iterator< ticpp::Element > varOutComesIte("state");

      for (varOutComesIte = varOutComesIte.begin(currentVar);
           varOutComesIte != varOutComesIte.end();
           ++varOutComesIte)
        newVar->addLabel(varOutComesIte->GetAttribute("id"));

      // Add the variable to the bn and then delete newVar (add makes a copy)
      _bn_->add(*newVar);
      delete (newVar);

      // Emitting progress.
      int progress = (int)((float)nbIte / (float)nbVar * 45) + 10;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }

    // ADDING ARCS and then CPTS
    for (varIte = varIte.begin(cptsNetwork); varIte != varIte.end(); ++varIte) {
      ticpp::Element* currentVar = varIte.Get();
      std::string     varName    = currentVar->GetAttribute("id");

      auto elt = currentVar->FirstChildElement("parents", false);
      if (elt != nullptr) {
        // iteration in the list of parents in reverse order
        const auto& strvec = split(elt->GetTextOrDefault(""), " ");
        for (auto rit = strvec.begin(); rit != strvec.end(); ++rit)
          _bn_->addArc(*rit, varName);
      }

      std::istringstream issTableString(
         currentVar->FirstChildElement("probabilities")->GetTextOrDefault(""));
      std::vector< GUM_SCALAR > tablevector;
      GUM_SCALAR                value;

      while (!issTableString.eof()) {
        issTableString >> value;
        tablevector.push_back(value);
      }

      // Filling tables
      _bn_->cpt(varName).fillWith(tablevector);
    }

    //
    return nbIte;
  }

  template < typename GUM_SCALAR >
  void XDSLBNReader< GUM_SCALAR >::_parsingExtension_(ticpp::Element* nodesNetwork) {
    ticpp::Iterator< ticpp::Element > varIte("node");
    for (varIte = varIte.begin(nodesNetwork); varIte != varIte.end(); ++varIte) {
      ticpp::Element* currentVar = varIte.Get();
      std::string     varName    = currentVar->GetAttribute("id");
      std::string     descName   = currentVar->FirstChildElement("name")->GetTextOrDefault("");
      if (descName != varName) _bn_->changeVariableName(varName, descName);
    }
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

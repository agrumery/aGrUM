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
#  include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>

namespace gum {
  /*
   * Constructor
   * A reader is created to reading a defined file.
   * Note that an BN as to be created before and given in parameter.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLBNReader< GUM_SCALAR >::BIFXMLBNReader(BayesNet< GUM_SCALAR >* bn,
                                                      const std::string&      filePath) :
      BNReader< GUM_SCALAR >(bn, filePath) {
    GUM_CONSTRUCTOR(BIFXMLBNReader);
    _bn_       = bn;
    _filePath_ = filePath;
  }

  /*
   * Default destructor.
   */
  template < typename GUM_SCALAR >
  INLINE BIFXMLBNReader< GUM_SCALAR >::~BIFXMLBNReader() {
    GUM_DESTRUCTOR(BIFXMLBNReader);
  }

  /*
   * Reads the bayes net from the file referenced by filePath  given at the
   * creation
   * of class
   * @return Returns the number of errors during the parsing (0 if none).
   */
  template < typename GUM_SCALAR >
  Size BIFXMLBNReader< GUM_SCALAR >::proceed() {
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
      status = "File loaded. Now looking for BIF element ...";
      GUM_EMIT2(onProceed, 4, status);

      ticpp::Element* bifElement = xmlDoc.FirstChildElement("BIF");

      // Finding network element
      status = "BIF Element reached. Now searching network ...";
      GUM_EMIT2(onProceed, 7, status);

      ticpp::Element* networkElement = bifElement->FirstChildElement("NETWORK");

      // Finding id variables
      status = "Network found. Now proceeding variables instantiation...";
      GUM_EMIT2(onProceed, 10, status);

      _parsingVariables_(networkElement);

      // Filling diagram
      status = "All variables have been instantiated. Now filling up diagram...";
      GUM_EMIT2(onProceed, 55, status);

      _fillingBN_(networkElement);

      status = "Instantiation of network completed";
      GUM_EMIT2(onProceed, 100, status);

      return 0;
    } catch (ticpp::Exception& tinyexception) { GUM_ERROR(IOError, tinyexception.what()) }
  }

  template < typename GUM_SCALAR >
  void BIFXMLBNReader< GUM_SCALAR >::_parsingVariables_(ticpp::Element* parentNetwork) {
    // Counting the number of variable for the signal
    int                               nbVar = 0;
    ticpp::Iterator< ticpp::Element > varIte("VARIABLE");

    for (varIte = varIte.begin(parentNetwork); varIte != varIte.end(); ++varIte)
      nbVar++;

    // Iterating on variable element
    int nbIte = 0;

    for (varIte = varIte.begin(parentNetwork); varIte != varIte.end(); ++varIte) {
      ticpp::Element* currentVar = varIte.Get();

      // Getting variable name
      ticpp::Element* varNameElement = currentVar->FirstChildElement("NAME");
      std::string     varName        = varNameElement->GetTextOrDefault("");

      // Getting variable description
      ticpp::Element* varDescrElement = currentVar->FirstChildElement("PROPERTY");
      std::string     varDescription  = varDescrElement->GetTextOrDefault("");

      // Instanciation de la variable
      auto newVar = new LabelizedVariable(varName, varDescription, 0);

      // Getting variable outcomes
      ticpp::Iterator< ticpp::Element > varOutComesIte("OUTCOME");

      for (varOutComesIte = varOutComesIte.begin(currentVar);
           varOutComesIte != varOutComesIte.end();
           ++varOutComesIte)
        newVar->addLabel(varOutComesIte->GetTextOrDefault(""));

      // Add the variable to the bn and then delete newVar (add makes a copy)
      _bn_->add(*newVar);
      delete (newVar);

      // Emitting progress.
      std::string status   = "Network found. Now proceedind variables instanciation...";
      int         progress = (int)((float)nbIte / (float)nbVar * 45) + 10;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }
  }

  template < typename GUM_SCALAR >
  void BIFXMLBNReader< GUM_SCALAR >::_fillingBN_(ticpp::Element* parentNetwork) {
    // Counting the number of variable for the signal
    int                               nbDef = 0;
    ticpp::Iterator< ticpp::Element > definitionIte("DEFINITION");

    for (definitionIte = definitionIte.begin(parentNetwork); definitionIte != definitionIte.end();
         ++definitionIte)
      nbDef++;

    // Iterating on definition nodes
    int nbIte = 0;

    for (definitionIte = definitionIte.begin(parentNetwork); definitionIte != definitionIte.end();
         ++definitionIte) {
      ticpp::Element* currentVar = definitionIte.Get();

      // Considered Node
      std::string currentVarName = currentVar->FirstChildElement("FOR")->GetTextOrDefault("");
      NodeId      currentVarId   = _bn_->idFromName(currentVarName);

      // Get Node's parents
      ticpp::Iterator< ticpp::Element > givenIte("GIVEN");
      List< NodeId >                    parentList;

      for (givenIte = givenIte.begin(currentVar); givenIte != givenIte.end(); ++givenIte) {
        std::string parentNode = givenIte->GetTextOrDefault("");
        NodeId      parentId   = _bn_->idFromName(parentNode);
        parentList.pushBack(parentId);
      }

      for (List< NodeId >::iterator_safe parentListIte = parentList.rbeginSafe();
           parentListIte != parentList.rendSafe();
           --parentListIte)
        _bn_->addArc(*parentListIte, currentVarId);

      // Recuperating tables values
      ticpp::Element*         tableElement = currentVar->FirstChildElement("TABLE");
      std::istringstream      issTableString(tableElement->GetTextOrDefault(""));
      std::list< GUM_SCALAR > tablelist;
      GUM_SCALAR              value;

      while (!issTableString.eof()) {
        issTableString >> value;
        tablelist.push_back(value);
      }

      std::vector< GUM_SCALAR > tablevector(tablelist.begin(), tablelist.end());

      // Filling tables
      _bn_->cpt(currentVarId).fillWith(tablevector);

      // Emitting progress.
      std::string status   = "All variables have been instancied. Now filling up diagram...";
      int         progress = (int)((float)nbIte / (float)nbDef * 45) + 55;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS

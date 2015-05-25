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
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>

namespace gum {
  /*
   * Constructor
   * A reader is created to reading a defined file.
   * Note that an BN as to be created before and given in parameter.
   */
  template <typename GUM_SCALAR>
  INLINE BIFXMLBNReader<GUM_SCALAR>::BIFXMLBNReader(BayesNet<GUM_SCALAR> *bn,
                                                    const std::string &filePath)
      : BNReader<GUM_SCALAR>(bn, filePath) {
    GUM_CONSTRUCTOR(BIFXMLBNReader);
    __bn = bn;
    __filePath = filePath;
  }

  /*
   * Default destructor.
   */
  template <typename GUM_SCALAR>
  INLINE BIFXMLBNReader<GUM_SCALAR>::~BIFXMLBNReader() {
    GUM_DESTRUCTOR(BIFXMLBNReader);
  }

  /*
   * Reads the bayes net from the file referenced by filePath  given at the creation
   * of class
   * @return Returns the number of error during the parsing (0 if none).
   */
  template <typename GUM_SCALAR> int BIFXMLBNReader<GUM_SCALAR>::proceed() {
    try {
      // Loading file
      std::string status = "Loading File ...";
      GUM_EMIT2(onProceed, 0, status);

      ticpp::Document xmlDoc(__filePath);
      xmlDoc.LoadFile();

      if (xmlDoc.NoChildren()) {
        GUM_ERROR(IOError,
                  ": Loading fail, please check the file for any syntax error.");
      }

      // Finding BIF element
      status = "File loaded. Now looking for BIF element ...";
      GUM_EMIT2(onProceed, 4, status);

      ticpp::Element *bifElement = xmlDoc.FirstChildElement("BIF");

      // Finding network element
      status = "BIF Element reached. Now searching network ...";
      GUM_EMIT2(onProceed, 7, status);

      ticpp::Element *networkElement = bifElement->FirstChildElement("NETWORK");

      // Finding id variables
      status = "Network found. Now proceedind variables instanciation...";
      GUM_EMIT2(onProceed, 10, status);

      __parsingVariables(networkElement);

      // Filling diagram
      status = "All variables have been instancied. Now filling up diagram...";
      GUM_EMIT2(onProceed, 55, status);

      __fillingBN(networkElement);

      status = "Instanciation of network completed";
      GUM_EMIT2(onProceed, 100, status);

      return 0;
    } catch (ticpp::Exception tinyexception) {
      GUM_ERROR(IOError, tinyexception.what());
      return 1;
    }
  }

  template <typename GUM_SCALAR>
  void
  BIFXMLBNReader<GUM_SCALAR>::__parsingVariables(ticpp::Element *parentNetwork) {
    // Counting the number of variable for the signal
    int nbVar = 0;
    ticpp::Iterator<ticpp::Element> varIte("VARIABLE");

    for (varIte = varIte.begin(parentNetwork); varIte != varIte.end(); ++varIte)
      nbVar++;

    // Iterating on variable element
    int nbIte = 0;

    for (varIte = varIte.begin(parentNetwork); varIte != varIte.end(); ++varIte) {
      ticpp::Element *currentVar = varIte.Get();

      // Getting variable name
      ticpp::Element *varNameElement = currentVar->FirstChildElement("NAME");
      std::string varName = varNameElement->GetTextOrDefault("");

      // Getting variable description
      ticpp::Element *varDescrElement = currentVar->FirstChildElement("PROPERTY");
      std::string varDescription = varDescrElement->GetTextOrDefault("");

      // Instanciation de la variable
      LabelizedVariable *newVar = new LabelizedVariable(varName, varDescription, 0);

      // Getting variable outcomes
      ticpp::Iterator<ticpp::Element> varOutComesIte("OUTCOME");

      for (varOutComesIte = varOutComesIte.begin(currentVar);
           varOutComesIte != varOutComesIte.end(); ++varOutComesIte)
        newVar->addLabel(varOutComesIte->GetTextOrDefault(""));

      // Getting variable type
      std::string nodeType = currentVar->GetAttribute<std::string>("TYPE");

      // Add the variable to the bn and then delete newVar (add makes a copy)
      __bn->add(*newVar);
      delete (newVar);

      // Emitting progress.
      std::string status =
          "Network found. Now proceedind variables instanciation...";
      int progress = (int)((float)nbIte / (float)nbVar * 45) + 10;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }
  }

  template <typename GUM_SCALAR>
  void BIFXMLBNReader<GUM_SCALAR>::__fillingBN(ticpp::Element *parentNetwork) {
    // Counting the number of variable for the signal
    int nbDef = 0;
    ticpp::Iterator<ticpp::Element> definitionIte("DEFINITION");

    for (definitionIte = definitionIte.begin(parentNetwork);
         definitionIte != definitionIte.end(); ++definitionIte)
      nbDef++;

    // Iterating on definition nodes
    int nbIte = 0;

    for (definitionIte = definitionIte.begin(parentNetwork);
         definitionIte != definitionIte.end(); ++definitionIte) {
      ticpp::Element *currentVar = definitionIte.Get();

      // Considered Node
      std::string currentVarName =
          currentVar->FirstChildElement("FOR")->GetTextOrDefault("");
      NodeId currentVarId = __bn->idFromName(currentVarName);

      // Get Node's parents
      ticpp::Iterator<ticpp::Element> givenIte("GIVEN");
      List<NodeId> parentList;

      for (givenIte = givenIte.begin(currentVar); givenIte != givenIte.end();
           ++givenIte) {
        std::string parentNode = givenIte->GetTextOrDefault("");
        NodeId parentId = __bn->idFromName(parentNode);
        parentList.pushBack(parentId);
      }

      for (List<NodeId>::iterator_safe parentListIte = parentList.rbeginSafe();
           parentListIte != parentList.rendSafe(); --parentListIte)
        __bn->addArc(*parentListIte, currentVarId);

      // Recuperating tables values
      ticpp::Element *tableElement = currentVar->FirstChildElement("TABLE");
      std::istringstream issTableString(tableElement->GetTextOrDefault(""));
      std::list<GUM_SCALAR> tablelist;
      GUM_SCALAR value;

      while (!issTableString.eof()) {
        issTableString >> value;
        tablelist.push_back(value);
      }

      std::vector<GUM_SCALAR> tablevector(tablelist.begin(), tablelist.end());

      // Filling tables
      const Potential<GUM_SCALAR> *table = &__bn->cpt(currentVarId);
      table->fillWith(tablevector);

      // Emitting progress.
      std::string status =
          "All variables have been instancied. Now filling up diagram...";
      int progress = (int)((float)nbIte / (float)nbDef * 45) + 55;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS

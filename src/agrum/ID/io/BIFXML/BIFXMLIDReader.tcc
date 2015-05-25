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

#include <agrum/ID/io/BIFXML/BIFXMLIDReader.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace gum {
  /*
   * Constructor
   * A reader is created to reading a defined file.
   * Note that an ID as to be created before and given in parameter.
   */
  template <typename GUM_SCALAR>
  INLINE
  BIFXMLIDReader<GUM_SCALAR>::BIFXMLIDReader(InfluenceDiagram<GUM_SCALAR> *infdiag,
                                             const std::string &filePath)
      : IDReader<GUM_SCALAR>(infdiag, filePath) {
    GUM_CONSTRUCTOR(BIFXMLIDReader);
    __infdiag = infdiag;
    __filePath = filePath;
  }

  /*
   * Default destructor.
   */
  template <typename GUM_SCALAR>
  INLINE BIFXMLIDReader<GUM_SCALAR>::~BIFXMLIDReader() {
    GUM_DESTRUCTOR(BIFXMLIDReader);
  }

  /*
   * Reads the influence diagram from the file referenced by filePath  given at the
   * creation of class
   * @return Returns the number of error during the parsing (0 if none).
   */
  template <typename GUM_SCALAR> void BIFXMLIDReader<GUM_SCALAR>::proceed() {
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
      status = "Network found. Now proceeding variables instanciation...";
      GUM_EMIT2(onProceed, 10, status);

      __parsingVariables(networkElement);

      // Filling diagram
      status = "All variables have been instancied. Now filling up diagram...";
      GUM_EMIT2(onProceed, 55, status);

      __fillingDiagram(networkElement);

      status = "Instanciation of network completed";
      GUM_EMIT2(onProceed, 100, status);

    } catch (ticpp::Exception tinyexception) {
      GUM_ERROR(IOError, tinyexception.what());
    }
  }

  template <typename GUM_SCALAR>
  void
  BIFXMLIDReader<GUM_SCALAR>::__parsingVariables(ticpp::Element *parentNetwork) {
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
      LabelizedVariable newVar(varName, varDescription, 0);

      // Getting variable outcomes
      ticpp::Iterator<ticpp::Element> varOutComesIte("OUTCOME");

      for (varOutComesIte = varOutComesIte.begin(currentVar);
           varOutComesIte != varOutComesIte.end(); ++varOutComesIte)
        newVar.addLabel(varOutComesIte->GetTextOrDefault(""));

      // Getting variable type
      std::string nodeType = currentVar->GetAttribute<std::string>("TYPE");

      // Add the variable to the id
      if (nodeType.compare("decision") == 0)
        __infdiag->addDecisionNode(newVar);
      else if (nodeType.compare("utility") == 0)
        __infdiag->addUtilityNode(newVar);
      else
        __infdiag->addChanceNode(newVar);

      // Emitting progress.
      std::string status =
          "Network found. Now proceedind variables instanciation...";
      int progress = (int)((float)nbIte / (float)nbVar * 45) + 10;
      GUM_EMIT2(onProceed, progress, status);
      nbIte++;
    }
  }

  template <typename GUM_SCALAR>
  void BIFXMLIDReader<GUM_SCALAR>::__fillingDiagram(ticpp::Element *parentNetwork) {
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
      NodeId currentVarId = __infdiag->idFromName(currentVarName);

      // Get Node's parents
      ticpp::Iterator<ticpp::Element> givenIte("GIVEN");
      List<NodeId> parentList;

      for (givenIte = givenIte.begin(currentVar); givenIte != givenIte.end();
           ++givenIte) {
        std::string parentNode = givenIte->GetTextOrDefault("");
        NodeId parentId = __infdiag->idFromName(parentNode);
        parentList.pushBack(parentId);
      }

      for (List<NodeId>::iterator_safe parentListIte = parentList.rbeginSafe();
           parentListIte != parentList.rendSafe(); --parentListIte)
        __infdiag->addArc(*parentListIte, currentVarId);

      // Recuperating tables values
      if (!__infdiag->isDecisionNode(currentVarId)) {
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
        if (__infdiag->isChanceNode(currentVarId)) {
          const Potential<GUM_SCALAR> *table = &__infdiag->cpt(currentVarId);
          table->fillWith(tablevector);
        } else if (__infdiag->isUtilityNode(currentVarId)) {
          const UtilityTable<GUM_SCALAR> *table = &__infdiag->utility(currentVarId);
          table->fillWith(tablevector);
        }
      }

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

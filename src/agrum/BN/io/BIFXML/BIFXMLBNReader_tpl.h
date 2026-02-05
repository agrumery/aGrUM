/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>

#  include <agrum/base/core/utils_string.h>

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

      std::string description = "";
      std::string fast        = "";
      // Getting variable description and/or fast syntax
      ticpp::Iterator< ticpp::Element > varPropertiesIte("PROPERTY");
      for (varPropertiesIte = varPropertiesIte.begin(currentVar);
           varPropertiesIte != varPropertiesIte.end();
           ++varPropertiesIte) {
        const auto pair = gum::split(varPropertiesIte->GetTextOrDefault(""), "=");
        if (pair.size() == 2) {
          const auto property = gum::toLower(gum::trim_copy(pair[0]));
          const auto value    = gum::trim_copy(pair[1]);
          // check for descritpion and fast
          if (property == "description") {
            description = value;
          } else if (property == "fast") {
            fast = value;
          }
        }
      }

      if (fast == "") {
        // if no fast syntax, we create a variable with the default}
        // Instanciation de la variable
        auto newVar = new LabelizedVariable(varName, description, 0);

        // Getting variable outcomes
        ticpp::Iterator< ticpp::Element > varOutComesIte("OUTCOME");

        for (varOutComesIte = varOutComesIte.begin(currentVar);
             varOutComesIte != varOutComesIte.end();
             ++varOutComesIte)
          newVar->addLabel(varOutComesIte->GetTextOrDefault(""));

        // Add the variable to the bn and then delete newVar (add makes a copy)
        _bn_->add(*newVar);
        delete newVar;
      } else {
        auto newVar = gum::fastVariable(fast, 2);
        newVar->setDescription(description);
        // we could check if varName is OK
        if (newVar->name() != varName) {
          GUM_ERROR(IOError,
                    "Variable name (" << varName << ") and fast syntax (" << fast
                                      << ") are not compatible. Please check the syntax.")
        }
        // Add the variable to the bn and then delete newVar (add makes a copy)
        _bn_->add(*newVar);
      }

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

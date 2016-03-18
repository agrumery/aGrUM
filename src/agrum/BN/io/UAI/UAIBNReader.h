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
 * @brief classe for import of bayes net from a file with UAI Format
 *
 * Read an bayes net from a file with UAI format
 * cf. http://www.cs.huji.ac.il/project/PASCAL/fileFormat.php
 *
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#ifndef GUM_UAIREADER_H
#define GUM_UAIREADER_H

#include <sstream>
#include <string>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/signal/signaler2.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/io/BNReader.h>

namespace gum {
  /**
   * @class UAIBNReader UAIBNReader.h
   *<agrum/BN/io/UAI/UAIBNReader.h>
   * @ingroup bn_io
   * @brief Read an bayes net from an file with UAI format
   *
   * This class import an bayes net from an text files using UAI format
   * cf. http://www.cs.huji.ac.il/project/PASCAL/fileFormat.php
   * for information about this format.
   *
   */
  template <typename GUM_SCALAR>
  class UAIBNReader : BNReader<GUM_SCALAR> {
    public:
    /**
     * Constructor
     * A reader is created to reading a defined file.
     * Note that an BN as to be created before and given in parameter.
     */
    UAIBNReader( BayesNet<GUM_SCALAR>* bn, const std::string& filePath );

    /**
     * Default destructor.
     */
    ~UAIBNReader();

    /**
     * Reads the bayes net from the file referenced by filePath  given at the
     * creation of class
     * @return Returns 0 if no error, 1 if any
     * @warning XMLBNReader can not give the number of errors.
     */
    virtual int proceed();
    /**
     * Signaler used to indicates how many percent of the Xml files have been
     * parsed
     * yet
     */
    typename gum::Signaler2<int, std::string> onProceed;
  };
}


#endif  // GUM_UAIBNREADER_H

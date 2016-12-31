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
 * @brief Definition file for BIF XML exportation class
 *
 * Writes an bayes net in XML files with BIF format
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_UAIWRITER_H
#define GUM_UAIWRITER_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>
#include <agrum/config.h>

namespace gum {
  /**
   * @class UAIWriter UAIWriter.h
   *<agrum/BN/io/UAI/UAIWriter.h>
   * @ingroup bn_io
   * @brief Writes an bayes net in a text file with UAI format
   *
   * This class export a bayes net into an text file, using UAI format
   *
   * cf. http://www.cs.huji.ac.il/project/PASCAL/fileFormat.php
   *
   */
  template <typename GUM_SCALAR>
  class UAIWriter : public BNWriter<GUM_SCALAR> {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    UAIWriter();

    /**
     * Destructor.
     */
    virtual ~UAIWriter();

    /// @}

    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param bn The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    virtual void write( std::ostream& output, const IBayesNet<GUM_SCALAR>& bn );

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param bn The bayes net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    virtual void write( std::string filePath, const IBayesNet<GUM_SCALAR>& bn );

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string __preambule( const IBayesNet<GUM_SCALAR>& bn );

    std::string __cptBloc( const IBayesNet<GUM_SCALAR>& bn, NodeId node );
  };


  extern template class UAIWriter<float>;
  extern template class UAIWriter<double>;

} /* namespace gum */

#include <agrum/BN/io/UAI/UAIWriter_tpl.h>
#endif  // GUM_UAIWRITER_H


/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#include <agrum/agrum.h>

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
  template < typename GUM_SCALAR >
  class UAIWriter : public BNWriter< GUM_SCALAR > {
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
    ~UAIWriter() final;

    /// @}

    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param bn The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void write(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param bn The bayes net writen in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void write(const std::string&             filePath,
               const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    /**
     * Returns the header of the BIF file.
     */
    std::string __preambule(const IBayesNet< GUM_SCALAR >& bn);

    std::string __cptBloc(const IBayesNet< GUM_SCALAR >& bn, NodeId node);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class UAIWriter< double >;
#endif

} /* namespace gum */

#include <agrum/BN/io/UAI/UAIWriter_tpl.h>
#endif   // GUM_UAIWRITER_H

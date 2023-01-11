/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Definition file for BIF XML exportation class
 *
 * Writes an bayes net in XML files with BIF format
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_O3PRMBNWRITER_H
#define GUM_O3PRMBNWRITER_H


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <agrum/BN/io/BNWriter.h>
#include <agrum/agrum.h>

#include <agrum/tools/variables/allDiscreteVariables.h>

namespace gum {
  /**
   * @class O3prmBNWriter O3prmBNWriter.h
   *<agrum/PRM/o3prm/O3prmBNWriter.h>
   * @ingroup bn_io
   * @brief Writes an bayes net in a text file with O3PRM format
   *
   * This class export a bayes net into an text file, using O3PRM format
   *
   */
  template < typename GUM_SCALAR >
  class O3prmBNWriter: public BNWriter< GUM_SCALAR > {
    public:
    // ==========================================================================
    /// @name Constructor & destructor
    // ==========================================================================
    /// @{

    /**
     * Default constructor.
     */
    O3prmBNWriter();

    /**
     * Destructor.
     */
    ~O3prmBNWriter() override;

    /// @}
    protected:
    /**
     * Writes an bayes net in the given ouput stream.
     *
     * @param output The output stream.
     * @param bn The bayes net writen in the stream.
     * @throws IOError Raised if an I/O error occurs.
     */
    void _doWrite(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) final;

    /**
     * Writes an bayes net in the file referenced by filePath.
     * If the file doesn't exists, it is created.
     * If the file exists, it's content will be erased.
     *
     * @param filePath The path to the file used to write the bayes net.
     * @param bn The bayes net written in the file.
     * @throw IOError Raised if an I/O error occurs.
     */
    void _doWrite(const std::string& filePath, const IBayesNet< GUM_SCALAR >& bn) final;

    private:
    std::string _extractAttribute_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    std::string _extractType_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    template < typename VARTYPE >
    std::string _extractDiscretizedType_(const VARTYPE* var);

    std::string _extractName_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    std::string _extractParents_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    std::string _extractCPT_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    std::string _extractRangeType_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);

    std::string _extractLabelizedType_(const IBayesNet< GUM_SCALAR >& bn, NodeId node);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class O3prmBNWriter< double >;
#endif

} /* namespace gum */

#include <agrum/PRM/o3prm/O3prmBNWriter_tpl.h>

#endif   // GUM_O3PRMBNWRITER_H

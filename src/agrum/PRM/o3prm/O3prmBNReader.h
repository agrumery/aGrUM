/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of O3prBNmReader : reader for BN using o3prm
 * syntaxt
 *
 * this class generates the groundedBN from a class or an instance name.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Lionel TORTI
 */
#ifndef GUM_PRM_O3PRM_BNREADER_H
#define GUM_PRM_O3PRM_BNREADER_H

#include <algorithm>
#include <string>

#include <agrum/BN/io/BNReader.h>
#include <agrum/tools/core/errorsContainer.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {
  /**
   * @class O3prmBNReader
   * @headerfile O3prmBNReader.h <agrum/PRM/o3prm/O3prmBNReader.h>
   * @ingroup o3prm_group
   *
   * @brief Read an O3PRM and transform the gum::prm::PRMSystem into
   * gum::BayesNet.
   *
   * @tparam GUM_SCALAR The scalar type used both for the gum::prm::PRM and the
   * gum::BayesNet.
   */
  template < typename GUM_SCALAR >
  class O3prmBNReader: public BNReader< GUM_SCALAR > {
    public:
    O3prmBNReader(BayesNet< GUM_SCALAR >* bn,
                  const std::string&      filename,
                  const std::string&      entityName = "",
                  const std::string&      classPath  = "");

    ~O3prmBNReader();

    /// parse the file
    /// @return the number of detected errors and warnings
    /// @throws IOError if file not exists
    Size proceed();

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors() { return errors__.error_count; }
    /// # of errors
    Size warnings() { return errors__.warning_count; }

    Idx errLine(Idx i) { return errors__.error(i).line; }
    /// col of ith error or warning
    Idx errCol(Idx i) { return errors__.error(i).column; }
    /// type of ith error or warning
    bool errIsError(Idx i) { return errors__.error(i).is_error; }
    /// message of ith error or warning
    std::string errMsg(Idx i) { return errors__.error(i).msg; }

    /// send on std::cerr the list of errors
    void showElegantErrors(std::ostream& o = std::cerr) {
      errors__.elegantErrors(o);
    }

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings(std::ostream& o = std::cerr) {
      errors__.elegantErrorsAndWarnings(o);
    }

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream& o = std::cerr) {
      errors__.syntheticResults(o);
    }
    /// @}

    private:
    std::string filename__;

    std::string classpath__;
    std::string entityName__;

    BayesNet< GUM_SCALAR >* bn__;
    ErrorsContainer         errors__;

    void               generateBN__(prm::PRMSystem< GUM_SCALAR >& system);
    static std::string getVariableName__(const std::string& path,
                                         const std::string& type,
                                         const std::string& name,
                                         const std::string& toRemove = "");
    static std::string getEntityName__(const std::string& filename);
    static std::string getInstanceName__(const std::string& classname);
  };

}   // namespace gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3prmBNReader_tpl.h>


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::O3prmBNReader< double >;
#endif


#endif   // GUM_PRM_O3PRM_BNREADER_H

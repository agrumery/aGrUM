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
 * @brief Inline implementation of O3prBNmReader : reader for BN using o3prm
 * syntaxt
 *
 * this class generates the groundedBN from a class or an instance name.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 */
#ifndef GUM_PRM_O3PRM_BNREADER_H
#define GUM_PRM_O3PRM_BNREADER_H

#include <algorithm>
#include <string>

#include <agrum/BN/io/BNReader.h>

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
    Size errors() { return _errors_.error_count; }
    /// # of errors
    Size warnings() { return _errors_.warning_count; }

    Idx errLine(Idx i) { return _errors_.error(i).line; }
    /// col of ith error or warning
    Idx errCol(Idx i) { return _errors_.error(i).column; }
    /// type of ith error or warning
    bool errIsError(Idx i) { return _errors_.error(i).is_error; }
    /// message of ith error or warning
    std::string errMsg(Idx i) { return _errors_.error(i).msg; }

    /// send on std::cerr the list of errors
    void showElegantErrors(std::ostream& o = std::cerr) { _errors_.elegantErrors(o); }

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings(std::ostream& o = std::cerr) {
      _errors_.elegantErrorsAndWarnings(o);
    }

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream& o = std::cerr) { _errors_.syntheticResults(o); }
    /// @}

    private:
    std::string _filename_;

    std::string _classpath_;
    std::string _entityName_;

    BayesNet< GUM_SCALAR >* _bn_;
    ErrorsContainer         _errors_;

    void               _generateBN_(prm::PRMSystem< GUM_SCALAR >& system);
    static std::string _getVariableName_(const std::string& path,
                                         const std::string& type,
                                         const std::string& name,
                                         const std::string& toRemove = "");
    static std::string _getEntityName_(const std::string& filename);
    static std::string _getInstanceName_(const std::string& classname);
  };

}   // namespace gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3prmBNReader_tpl.h>


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::O3prmBNReader< double >;
#endif


#endif   // GUM_PRM_O3PRM_BNREADER_H

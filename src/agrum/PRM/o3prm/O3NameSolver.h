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
 * @brief Headers for the O3NameSolver class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <algorithm>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/errors.h>
#include <agrum/tools/core/utils_string.h>

#ifndef GUM_PRM_O3PRM_O3NAMESOLVER_H
#  define GUM_PRM_O3PRM_O3NAMESOLVER_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3NameSolver
       * @headerfile O3NameSolver.h <agrum/PRM/o3prm/O3NameSolver.h>
       * @ingroup o3prm_group
       *
       * @brief Resolves names for the different O3PRM factories.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template < typename GUM_SCALAR >
      class O3NameSolver {
        public:
        O3NameSolver(PRM< GUM_SCALAR >& prm, O3PRM& o3_prm, ErrorsContainer& errors);

        O3NameSolver(const O3NameSolver< GUM_SCALAR >& src);

        O3NameSolver(O3NameSolver< GUM_SCALAR >&& src);

        ~O3NameSolver();

        O3NameSolver< GUM_SCALAR >& operator=(const O3NameSolver< GUM_SCALAR >& src);

        O3NameSolver< GUM_SCALAR >& operator=(O3NameSolver< GUM_SCALAR >&& src);

        bool resolveClassElement(O3Label& name);

        bool resolveType(O3Label& name);

        bool resolveInterface(O3Label& name);

        bool resolveClass(O3Label& name);

        bool resolveSlotType(O3Label& name);

        private:
        PRM< GUM_SCALAR >* _prm_;
        O3PRM*             _o3_prm_;
        ErrorsContainer*   _errors_;

        using NameMap = HashTable< std::string, std::string >;

        NameMap _typeName_;
        NameMap _eltName_;
        NameMap _refName_;
        NameMap _interfaceName_;
        NameMap _className_;
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3NameSolver< double >;
#  endif


// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3NameSolver_tpl.h>

#endif   // GUM_PRM_O3PRM_O3NAMESOLVER_H

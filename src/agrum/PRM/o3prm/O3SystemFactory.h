/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers for the O3SystemFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/PRMFactory.h>

#ifndef GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H
#  define GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3SystemFactory
       * @headerfile O3SystemFactory.h <agrum/PRM/o3prm/O3SystemFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Builds gum::prm::PRMSystem from gum::prm::o3prm::O3System.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template < typename GUM_SCALAR >
      class O3SystemFactory {
        public:
        O3SystemFactory(PRM< GUM_SCALAR >&          prm,
                        O3PRM&                      o3_prm,
                        O3NameSolver< GUM_SCALAR >& solver,
                        ErrorsContainer&            errors);
        O3SystemFactory(const O3SystemFactory< GUM_SCALAR >& src);
        O3SystemFactory(O3SystemFactory< GUM_SCALAR >&& src);
        ~O3SystemFactory();
        O3SystemFactory< GUM_SCALAR >& operator=(const O3SystemFactory< GUM_SCALAR >& src);
        O3SystemFactory< GUM_SCALAR >& operator=(O3SystemFactory< GUM_SCALAR >&& src);

        void build();

        private:
        PRM< GUM_SCALAR >*          _prm_;
        O3PRM*                      _o3_prm_;
        O3NameSolver< GUM_SCALAR >* _solver_;
        ErrorsContainer*            _errors_;

        HashTable< std::string, O3Instance* > _nameMap_;

        void _addInstances_(PRMFactory< GUM_SCALAR >& factory, O3System& sys);

        void _addAssignments_(PRMFactory< GUM_SCALAR >& factory, O3System& sys);

        void _addIncrements_(PRMFactory< GUM_SCALAR >& factory, O3System& sys);

        bool _checkSystem_(O3System& sys);

        bool _checkIncrements_(O3System& sys);

        bool _checkParameters_(const PRMClass< GUM_SCALAR >& type, const O3Instance& inst);

        bool _checkInstance_(O3System& sys);

        bool _checkAssignments_(O3System& sys);
      };

    }   // namespace o3prm
  }   // namespace prm
}   // namespace gum

// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3SystemFactory_tpl.h>


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3SystemFactory< double >;
#  endif


#endif   // GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H

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
 * @brief Headers for the O3TypeFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/PRMFactory.h>

#ifndef GUM_PRM_O3PRM_O3TYPE_FACTORY_H
#  define GUM_PRM_O3PRM_O3TYPE_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3TypeFactory
       * @headerfile O3TypeFactory.h <agrum/PRM/o3prm/O3TypeFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Builds gum::prm::PRMType from gum::prm::o3prm::O3Type,
       * gum::prm::o3prm::O3IntType and gum::prm::o3prm::O3RealType.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template < typename GUM_SCALAR >
      class O3TypeFactory {
        public:
        O3TypeFactory(PRM< GUM_SCALAR >&          prm,
                      O3PRM&                      o3_prm,
                      O3NameSolver< GUM_SCALAR >& solver,
                      ErrorsContainer&            errors);
        O3TypeFactory(const O3TypeFactory< GUM_SCALAR >& src);
        O3TypeFactory(O3TypeFactory< GUM_SCALAR >&& src);
        ~O3TypeFactory();
        O3TypeFactory& operator=(const O3TypeFactory< GUM_SCALAR >& src);
        O3TypeFactory& operator=(O3TypeFactory< GUM_SCALAR >&& src);

        void build();

        private:
        PRM< GUM_SCALAR >*          _prm_;
        O3PRM*                      _o3_prm_;
        O3NameSolver< GUM_SCALAR >* _solver_;
        ErrorsContainer*            _errors_;

        bool _build_;

        HashTable< std::string, O3Type* >     _superMap_;
        HashTable< std::string, gum::NodeId > _nameMap_;
        HashTable< std::string, O3Type* >     _typeMap_;
        HashTable< NodeId, O3Type* >          _nodeMap_;
        DAG                                   _dag_;
        std::vector< O3Type* >                _o3Types_;
        std::vector< O3IntType* >             _o3IntTypes_;
        std::vector< O3RealType* >            _o3RealTypes_;

        void _buildTypes_();
        void _buildIntTypes_();
        void _buildRealTypes_();

        bool _isPrimitiveType_(O3Type& type);

        bool _checkO3Types_();
        void _checkDepreactedO3Types_();
        bool _addTypes2Dag_();
        bool _addArcs2Dag_();
        bool _checkLabels_(O3Type& type);

        void _setO3TypeCreationOrder_();

        bool _checkO3IntTypes_();
        bool _checkO3RealTypes_();
      };

    }   // namespace o3prm
  }   // namespace prm
}   // namespace gum

// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3TypeFactory_tpl.h>


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3TypeFactory< double >;
#  endif


#endif   // GUM_PRM_O3PRM_O3TYPE_FACTORY_H

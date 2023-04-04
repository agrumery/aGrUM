/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <concepts>

#include <agrum/agrum.h>

/**
 * @file
 * @brief Class encapsulating computations of notions from Information Theory
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_INFORMATION_THEORY
#  define GUM_INFORMATION_THEORY

#  include <agrum/tools/multidim/potential.h>

namespace gum {
  template < typename T >
  concept JointTargettable = requires(T t, const NodeSet& target) {
    { t.addJointTarget(target) } -> std::same_as< void >;
  };

  template < template < typename > class INFERENCE_ENGINE, typename GUM_SCALAR >
    requires JointTargettable< INFERENCE_ENGINE< GUM_SCALAR > >
  class InformationTheory {
    public:
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >& engine,
                      const gum::NodeSet&             X,
                      const gum::NodeSet&             Y);
    InformationTheory(INFERENCE_ENGINE< GUM_SCALAR >&   engine,
                      const std::vector< std::string >& Xnames,
                      const std::vector< std::string >& Ynames);

    protected:
    INFERENCE_ENGINE< GUM_SCALAR >& engine_;
    const gum::NodeSet&             X_;
    const gum::NodeSet&             Y_;

    void makeInference_();
  };
}   // namespace gum

#  include <agrum/tools/graphicalModels/algorithms/informationTheory_tpl.h>
#endif   // GUM_INFORMATION_THEORY

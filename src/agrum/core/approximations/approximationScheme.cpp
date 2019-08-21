
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
 * @brief Outlined implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <agrum/core/approximations/approximationScheme.h>

namespace gum {

  ApproximationScheme::ApproximationScheme(bool verbosity) :
      _current_state(ApproximationSchemeSTATE::Undefined), _eps(5e-2),
      _enabled_eps(true), _min_rate_eps(1e-2), _enabled_min_rate_eps(true),
      _max_time(1.), _enabled_max_time(false), _max_iter((Size)10000),
      _enabled_max_iter(true), _burn_in((Size)0), _period_size((Size)1),
      _verbosity(verbosity) {
    GUM_CONSTRUCTOR(ApproximationScheme);
  }

  ApproximationScheme::~ApproximationScheme() {
    GUM_DESTRUCTOR(ApproximationScheme);
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/core/approximations/approximationScheme_inl.h>
#endif


/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Outlined implementatioh of gum::ApproximationSettings.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/approximations/approximationScheme.h>

namespace gum {

  ApproximationScheme::ApproximationScheme(bool verbosity) :
      current_state_(ApproximationSchemeSTATE::Undefined), eps_(5e-2),
      enabled_eps_(true), min_rate_eps_(1e-2), enabled_min_rate_eps_(true),
      max_time_(1.), enabled_max_time_(false), max_iter_((Size)10000),
      enabled_max_iter_(true), burn_in_((Size)0), period_size_((Size)1),
      verbosity_(verbosity) {
    GUM_CONSTRUCTOR(ApproximationScheme);
  }

  ApproximationScheme::~ApproximationScheme() {
    GUM_DESTRUCTOR(ApproximationScheme);
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/approximations/approximationScheme_inl.h>
#endif

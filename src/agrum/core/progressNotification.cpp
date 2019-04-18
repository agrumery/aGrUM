
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
 * @brief gum::ProgressListener header file.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <agrum/core/progressNotification.h>

namespace gum {

  ProgressListener::ProgressListener(ProgressNotifier& notif) : __notif(notif) {
    GUM_CONSTRUCTOR(ProgressListener);

    GUM_CONNECT((__notif), onProgress, (*this), ProgressListener::whenProgress);
    GUM_CONNECT((__notif), onStop, (*this), ProgressListener::whenStop);
  }

  ProgressListener::ProgressListener(const ProgressListener& other) :
      __notif(other.__notif) {
    GUM_CONS_CPY(ProgressListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for ProgressListener");
  }

  ProgressListener::~ProgressListener() { GUM_DESTRUCTOR(ProgressListener); }

  ProgressListener& ProgressListener::operator=(const ProgressListener& other) {
    GUM_CONS_CPY(ProgressListener);
    GUM_ERROR(OperationNotAllowed, "No copy constructor for ProgressListener");
  }

}   // namespace gum

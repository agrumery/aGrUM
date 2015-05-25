/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Pierre-Henri WUILLEMIN <pierre-henri.wuillemin@lip6.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
s
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

namespace gum {
  ApproximationSchemeListener::ApproximationSchemeListener(
      IApproximationSchemeConfiguration &sch)
      : __sch(sch) {
    GUM_CONSTRUCTOR(ApproximationSchemeListener);

    GUM_CONNECT((__sch), onProgress, (*this),
                ApproximationSchemeListener::whenProgress);
    GUM_CONNECT((__sch), onStop, (*this), ApproximationSchemeListener::whenStop);
  }

  ApproximationSchemeListener::ApproximationSchemeListener(
      const ApproximationSchemeListener &other)
      : __sch(other.__sch) {
    GUM_CONS_CPY(ApproximationSchemeListener);
    GUM_ERROR(OperationNotAllowed,
              "No copy constructor for ApproximationSchemeListener");
  }

  ApproximationSchemeListener::~ApproximationSchemeListener() {
    GUM_DESTRUCTOR(ApproximationSchemeListener);
  }

  ApproximationSchemeListener &ApproximationSchemeListener::
  operator=(const ApproximationSchemeListener &other) {
    GUM_CONS_CPY(ApproximationSchemeListener);
    GUM_ERROR(OperationNotAllowed,
              "No copy constructor for ApproximationSchemeListener");
  }
}

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

/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (c) 2012  Pierre-Henri WUILLEMIN(_at_LIP6)
   <pierre-henri.wuillemin@lip6.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file
 * @brief A listener that allows BNLearner to be used as a proxy for its inner
 * algorithms
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef BNLEARNERLISTENER_H
#define BNLEARNERLISTENER_H

#include <agrum/agrum.h>
#include <agrum/tools/core/approximations/approximationScheme.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

namespace gum {
  namespace learning {
    class IBNLearner;

    /** @class BNLearnerListener
     * @brief A class that redirects gum_signal from algorithms to the listeners
     *of
     *BNLearn
     *
     * @ingroup learning_group
     */
    class BNLearnerListener: public ApproximationSchemeListener {
      public:
      BNLearnerListener(IBNLearner* bnl, ApproximationScheme& sch);
      virtual ~BNLearnerListener();

      virtual void whenProgress(const void*  src,
                                const Size   pourcent,
                                const double error,
                                const double time) final;
      virtual void whenStop(const void* src, const std::string& message) override;

      private:
      BNLearnerListener(const BNLearnerListener& other);
      BNLearnerListener& operator=(const BNLearnerListener& other);

      IBNLearner* _bnlearner_;
    };
  }   // namespace learning
}   // namespace gum

#endif   // BNLEARNERLISTENER_H

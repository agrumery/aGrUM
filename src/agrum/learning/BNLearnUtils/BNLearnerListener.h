/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Pierre-Henri WUILLEMIN <pierre-henri.wuillemin@lip6.fr>

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

/** @file
 * @brief A listener that allows BNLearner to be used as a proxy for its inner
 *algorithms
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#ifndef BNLEARNERLISTENER_H
#define BNLEARNERLISTENER_H

#include <agrum/config.h>
#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>

namespace gum {
  namespace learning {
    class genericBNLearner;

    /** @class BNLearnerListener
     * @brief A class that redirects gum_signal from algorithms to the listeners of
     *BNLearn
     *
     * @ingroup learning_group
     */
    class BNLearnerListener : public ApproximationSchemeListener {
      public:
      BNLearnerListener(genericBNLearner *bnl, ApproximationScheme &sch);
      virtual ~BNLearnerListener();

      virtual void whenProgress(const void *src, Size pourcent, double error,
                                double time) final;
      virtual void whenStop(const void *src, std::string message) final;

      private:
      BNLearnerListener(const BNLearnerListener &other);
      BNLearnerListener &operator=(const BNLearnerListener &other);

      genericBNLearner *__bnlearner;
    };
  } // namespace learning
} // namespace gum

#endif // BNLEARNERLISTENER_H

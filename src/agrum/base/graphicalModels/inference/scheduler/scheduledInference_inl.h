/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <agrum/base/graphicalModels/inference/scheduler/scheduledInference.h>

namespace gum {

  // returns the current scheduler
  INLINE Scheduler& ScheduledInference::scheduler() const {
    if (ThreadNumberManager::getNumberOfThreads() != 1)
      return const_cast< SchedulerParallel& >(_scheduler_parallel_);
    else return const_cast< SchedulerSequential& >(_scheduler_sequential_);
  }

  // sets the number max of threads that can be used
  INLINE void ScheduledInference::setNumberOfThreads(Size nb) {
    ThreadNumberManager::setNumberOfThreads(nb);
    _scheduler_parallel_.setNumberOfThreads(nb);
  }

  // sets an upper bound on the memory consumption admissible
  INLINE void ScheduledInference::setMaxMemory(double megabytes) {
    if (megabytes < 0.0) megabytes = 0.0;
    _scheduler_parallel_.setMaxMemory(megabytes);
    _scheduler_sequential_.setMaxMemory(megabytes);
  }

  // returns the Max memory (in megabytes) available for executing schedules
  INLINE double ScheduledInference::maxMemory() const { return _scheduler_parallel_.maxMemory(); }

} /* namespace gum */

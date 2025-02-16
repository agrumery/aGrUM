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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/core/threads/threadNumberManager.h>

namespace gum {

  // default constructor
  INLINE ThreadNumberManager::ThreadNumberManager(Size nb_threads) : _nb_threads_(nb_threads) {
    GUM_CONSTRUCTOR(ThreadNumberManager);
  }

  // copy constructor
  INLINE ThreadNumberManager::ThreadNumberManager(const ThreadNumberManager& from) :
      _nb_threads_(from._nb_threads_) {
    GUM_CONS_CPY(ThreadNumberManager);
  }

  // move constructor
  INLINE ThreadNumberManager::ThreadNumberManager(ThreadNumberManager&& from) :
      _nb_threads_(from._nb_threads_) {
    GUM_CONS_MOV(ThreadNumberManager);
  }

  // destructor
  INLINE ThreadNumberManager::~ThreadNumberManager() { GUM_DESTRUCTOR(ThreadNumberManager); }

  // copy operator
  INLINE ThreadNumberManager& ThreadNumberManager::operator=(const ThreadNumberManager& from) {
    _nb_threads_ = from._nb_threads_;
    return *this;
  }

  // move operator
  INLINE ThreadNumberManager& ThreadNumberManager::operator=(ThreadNumberManager&& from) {
    _nb_threads_ = from._nb_threads_;
    return *this;
  }

  // sets the number max of threads that can be used
  INLINE void ThreadNumberManager::setNumberOfThreads(Size nb) { _nb_threads_ = nb; }

  // returns the current max number of threads of the scheduler
  INLINE Size ThreadNumberManager::getNumberOfThreads() const {
    if (_nb_threads_ == 0) return gum::getNumberOfThreads();
    else return _nb_threads_;
  }

  // indicates whether the user set herself the number of threads
  INLINE bool ThreadNumberManager::isGumNumberOfThreadsOverriden() const {
    return _nb_threads_ != 0;
  }

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS

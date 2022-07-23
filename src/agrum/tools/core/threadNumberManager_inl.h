/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/core/threadNumberManager.h>

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

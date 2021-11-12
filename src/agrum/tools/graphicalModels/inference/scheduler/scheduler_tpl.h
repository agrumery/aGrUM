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


/** @file
 * @brief the common interface of all the schedulers
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

namespace gum {

  /// returns the allocator used by the scheduler
  template < template < typename > class ALLOC >
  INLINE typename Scheduler< ALLOC >::allocator_type
     Scheduler< ALLOC >::get_allocator() const {
    return *this;
  }


  /// default constructor
  template < template < typename > class ALLOC >
  Scheduler< ALLOC >::Scheduler(Size                  max_nb_threads,
                                double                max_memory,
                                const allocator_type& alloc):
      ALLOC< Idx >(alloc),
      _max_memory(max_memory * 1048576.0) { // 1048576 = 1024 * 1024 = 1Mb
    // set the number of threads
    if (max_nb_threads != Size(0)) _max_nb_threads = max_nb_threads;
    if (_max_memory <= 0.0) _max_memory = 0.0;

    // for debugging purposes
    GUM_CONSTRUCTOR(Scheduler);
  }


  /// copy constructor with a given allocator
  template < template < typename > class ALLOC >
  Scheduler< ALLOC >::Scheduler(
     const Scheduler< ALLOC >&                          from,
     const typename Scheduler< ALLOC >::allocator_type& alloc):
      ALLOC< Idx > (alloc),
      _max_nb_threads(from._max_nb_threads),
      _max_memory(from._max_memory) {
    // for debugging purposes
    GUM_CONS_CPY(Scheduler);
  }


  /// copy constructor
  template < template < typename > class ALLOC >
  INLINE Scheduler< ALLOC >::Scheduler(const Scheduler< ALLOC >& from) :
      Scheduler< ALLOC >(from, from.get_allocator()) {}


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  Scheduler< ALLOC >::Scheduler(
     Scheduler< ALLOC >&&                               from,
     const typename Scheduler< ALLOC >::allocator_type& alloc) :
      ALLOC< Idx > (alloc),
      _max_nb_threads(from._max_nb_threads),
      _max_memory(from._max_memory) {
    // for debugging purposes
    GUM_CONS_MOV(Scheduler);
  }


  /// move constructor with a given allocator
  template < template < typename > class ALLOC >
  INLINE Scheduler< ALLOC >::Scheduler(Scheduler< ALLOC >&& from) :
      Scheduler< ALLOC >(std::move(from), from.get_allocator()) {}


  /// destructor
  template < template < typename > class ALLOC >
  Scheduler< ALLOC >::~Scheduler() {
    // for debugging purposes
    GUM_DESTRUCTOR(Scheduler);
  }


  /// copy operator
  template < template < typename > class ALLOC >
  INLINE Scheduler< ALLOC >&
     Scheduler< ALLOC >::operator=(const Scheduler< ALLOC >& from) {
    if (this != & from) {
      _max_nb_threads = from._max_nb_threads;
      _max_memory     = from._max_memory;
    }

    return *this;
  }


  /// move operator
  template < template < typename > class ALLOC >
  INLINE Scheduler< ALLOC >&
     Scheduler< ALLOC >::operator=(Scheduler< ALLOC >&& from){
    if (this != & from) {
      _max_nb_threads = from._max_nb_threads;
      _max_memory     = from._max_memory;
    }

    return *this;
  }


  /// sets the number max of threads that can be used
  template < template < typename > class ALLOC >
  INLINE void Scheduler< ALLOC >::setMaxNbThreads(Size nb) {
    if (nb != Size(0))
      _max_nb_threads = nb;
    else
      _max_nb_threads = thread::getMaxNumberOfThreads();
  }


  /// returns the current max number of threads of the scheduler
  template < template < typename > class ALLOC >
  INLINE Size Scheduler< ALLOC >::maxNbThreads() const {
    return _max_nb_threads;
  }


  /// sets an upper bound on the memory consumption admissible
  template < template < typename > class ALLOC >
  INLINE void Scheduler< ALLOC >::setMaxMemory(double megabytes) {
    _max_memory = megabytes * 1048576.0; // 1048576 = 1024 * 1024 = 1Mb
    if (_max_memory < 0.0) _max_memory = 0.0;
  }


  /// returns the Max memory (in megabytes) available for executing schedules
  template < template < typename > class ALLOC >
  INLINE double Scheduler< ALLOC >::maxMemory() const {
    return _max_memory / 1048576.0; // 1048576 = 1024 * 1024 = 1Mb
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

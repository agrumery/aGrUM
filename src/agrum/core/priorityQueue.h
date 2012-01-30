/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief priority queues
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides class PriorityQueue that is essentially a heap in which
 * elements are sorted according to a dynamically modifiable priority. As in
 * heaps, elements are sorted according to a weak order which is < by default,
 * i.e., the top element of the queue is the smallest element and the more to the
 * bottom the greater the element. In addition, PriorityQueue has a special feature
 * that enables it to prevent multiple identical elements to be stored into it.
 * @par Usage example:
 * @code
 * // create a priority queue of strings, the priorities of which are integers
 * // the element at the top of the queue has the smallest priority
 * PriorityQueue<std::string> queue1;
 *
 * // insert elements into the queue
 * queue1.insert (8,  "AAA");
 * queue1.insert (10, "BBB");
 * queue1.insert (2,  "CCC");
 * queue1.insert (23, "DDD");
 * queue1.insert (24, "EEE");
 * queue1.insert (10, "AAA");
 *
 * // copy the queue
 * PriorityQueue<std::string> queue2 = queue1;
 *
 * // create a priority queue of strings, the priorities of which are pairs of int
 * PriorityQueue< std::string, std::pair<int,int> > queue3;
 *
 * // get the top element, then remove it
 * std::cerr << queue2.top() << std::endl;
 * queue2.eraseTop();
 *
 * // get the top element, then remove it
 * std::cerr << queue2.pop() << std::endl;
 *
 * // output the content of the queue
 * std::cerr << queue1 << std::endl;
 *
 * // change the priority of the element at position 3
 * queue1.setPriority (3,100);
 *
 * // change the priority of all instances of element "AAA"
 * queue1.setPriorityByVal ("AAA",100);
 * @endcode
 */

#ifndef GUM_PRIORITY_QUEUE_H
#define GUM_PRIORITY_QUEUE_H

#include <functional>
#include <vector>
#include <utility>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>
#include <agrum/core/utils.h>
#include <agrum/core/hashTable.h>


namespace gum {


  #define GUM_PRIORITY_QUEUE_DEFAULT_CAPACITY 10

  
  // ==============================================================================
  // templates provided by this file
  // ==============================================================================
  template <typename Val, typename Priority, typename Cmp> class PriorityQueue;
  template <typename Val, typename Priority, typename Cmp>
  std::ostream& operator<<
  ( std::ostream&, const PriorityQueue<Val,Priority,Cmp>& );



  /* =========================================================================== */
  /* ===                           PRIORITY QUEUES                           === */
  /* =========================================================================== */
  /** @class PriorityQueue
   * @brief A priorityQueue is a heap in which each element has a mutable priority
   * @ingroup basicstruct_group
   *
   * A priority queue is quite similar to a heap except that a priority (a score)
   * is assigned to each element in the structure. The elements are sorted
   * according to a weak order on the scores. The priority of any element can be
   * changed at any moment by the user. The priority queue then restores a heap
   * property accordingly.
   * @par Usage example:
   * @code
   * // create a priority queue of strings, the priorities of which are integers
   * // the element at the top of the queue has the smallest priority
   * PriorityQueue<std::string> queue1;
   *
   * // insert elements into the queue
   * queue1.insert (8,  "AAA");
   * queue1.insert (10, "BBB");
   * queue1.insert (2,  "CCC");
   * queue1.insert (23, "DDD");
   * queue1.insert (24, "EEE");
   * queue1.insert (10, "AAA");
   *
   * // copy the queue
   * PriorityQueue<std::string> queue2 = queue1;
   *
   * // create a priority queue of strings, the priorities of which are
   * // pairs of ints
   * PriorityQueue< std::string, std::pair<int,int> > queue3;
   *
   * // get the top element, then remove it
   * std::cerr << queue2.top() << std::endl;
   * queue2.eraseTop();
   *
   * // get the top element, then remove it
   * std::cerr << queue2.pop() << std::endl;
   *
   * // output the content of the queue
   * std::cerr << queue1 << std::endl;
   *
   * // change the priority of the element at position 3
   * queue1.setPriority (3,100);
   *
   * // change the priority of all instances of element "AAA"
   * queue1.setPriorityByVal ("AAA",100);
   * @endcode
   */
  /* =========================================================================== */
  template <typename Val, typename Priority=int, typename Cmp=std::less<Priority> >
  class PriorityQueue {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// basic constructor. Creates an empty priority queue
    /** @param compare a function taking two elements in argument, say e1 and e2,
     * and returning a Boolean indicating wether e1 < e2, i.e., whether e1 should
     * be nearer than e2 to the top of the heap.
     * @param uniqueness turn on/off the uniqueness policy, i.e., the possibility
     * that a given element appears several times in the queue
     * (default is nonuniqueness)
     * @param capacity the size of the internal data structures containing the
     * elements (could be for instance vectors or hashtables) */
    // ============================================================================
    explicit PriorityQueue( Cmp compare = Cmp(),
                            bool uniqueness = false,
                            Size capacity = GUM_PRIORITY_QUEUE_DEFAULT_CAPACITY );

    // ============================================================================
    /// copy constructor
    // ============================================================================
    PriorityQueue( const PriorityQueue<Val,Priority,Cmp>& );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~PriorityQueue();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// returns the number of elements in the priority queue
    // ============================================================================
    Size size() const ;

    // ============================================================================
    /// indicates whether the priority queue is empty
    // ============================================================================
    bool empty() const ;

    // ============================================================================
    /// indicates whether the priority queue contains a given value
    // ============================================================================
    bool contains( const Val& ) const ;

    // ============================================================================
    /// returns the element at the top of the priority queue
    /** @throw NotFound exception is thrown if the queue is empty */
    // ============================================================================
    const Val& top() const;

    // ============================================================================
    /// returns the priority of the top element
    /** @throw NotFound exception is thrown if the queue is empty */
    // ============================================================================
    const Priority& topPriority() const;

    // ============================================================================
    /// removes the top element from the priority queue and return it
    /** @throw NotFound exception is thrown if the queue is empty */
    // ============================================================================
    Val pop();

    // ============================================================================
    /// inserts a new (a copy) element in the priority queue
    /** @return the index of the element inserted into the priority queue (see
     * method erase for more details about the index)
     * @throw DuplicateElement exception is thrown if the uniqueness property is
     * set to true and the element already exists */
    // ============================================================================
    Size insert( const Priority& priority, const Val& val );

    // ============================================================================
    /// removes the top of the priority queue (but does not return it)
    /** If the heap is empty, it does nothing (in particular, it does not throw
     * any exception). */
    // ============================================================================
    void eraseTop();

    // ============================================================================
    /// removes the element at position "index" from the priority queue
    /** If the element cannot be found, the function returns without throwing any
     * exception.
     * @param index represents the position of the element to be removed. This is
     * computed as follows: suppose that the queue is a complete binary tree, that
     * is, a binary tree where all levels are completely filled except, maybe, the
     * last one and, in this case, the elements of this level are all to the left
     * of the tree. Then parsing the tree from top to bottom and, for each level,
     * from left to right, and assigning index 0 to the root of the tree and,
     * incrementing the index by 1 each time we jump to another node, we get a
     * unique index for each element. This is precisely what the index passed in
     * argument of the function represents. */
    // ============================================================================
    void erase( Size index );

    // ============================================================================
    /// removes a given element from the priority queue (but does not return it)
    /** If the element cannot be found, the function returns without throwing any
     * exception.
     * @param val the element we wish to remove. If the queue contains several
     * times this element, then the one with the smallest index is removed. */
    // ============================================================================
    void eraseByVal( const Val& val );

    // ============================================================================
    /// modifies the priority of the element at position "index" of the queue
    /** If the element cannot be found, the function returns without throwing any
     * exception. */
    // ============================================================================
    void setPriority( Size index, const Priority& new_priority );

    // ============================================================================
    /// modifies the priority of each instance of a given element
    // ============================================================================
    void setPriorityByVal( const Val& elt, const Priority& new_priority );

    // ============================================================================
    /// returns the priority of an instance of the value passed in argument
    /** Of course, this method is really meaningful only when there is only one
     * instance of the given element within the PriorityQueue. */
    // ============================================================================
    const Priority& priorityByVal( const Val& elt ) const ;

    // ============================================================================
    /// removes all the elements from the queue
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns a hashtable the keys of which are the values stored in the queue
    /** The keys of the hashtable correspond to the values stored in the
     * priority queue and, for each key, the corresponding value is the list of
     * indices in the queue where we can find the key. */
    // ============================================================================
    const HashTable< Val,std::vector<Size> >& allValues() const ;

    // ============================================================================
    /// displays the content of the queue
    // ============================================================================
    std::string toString() const;

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// copy operator
    /** When a problem occurs during the copy (for instance when not enough memory
     * is available), the operator guarantees that the heap stays in a coherent
     * state. Actually, the priority queue becomes empty. An exception is then
     * thrown. */
    // ============================================================================
    PriorityQueue<Val,Priority,Cmp>&
    operator= ( const PriorityQueue<Val,Priority,Cmp>& );

    // ============================================================================
    /// returns the element at index "index_elt" from the priority queue
    /**
     * @throw NotFound
     */
    // ============================================================================
    const Val& operator[]( Size index_elt ) const;

    /// @}


    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// return the size of the internal structure storing the priority queue
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// changes the size of the internal structure storing the priority queue
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// returns the current uniqueness policy
    // ============================================================================
    bool uniquenessPolicy() const ;

    // ============================================================================
    /** @brief enables the user to change dynamically the policy for checking
     * whether there can exist several identical elements in the queue
     *
     * By default, queues can store several times the same element. However, for
     * some applications, we should ensure that all elements in the queue are
     * distinct.
     * @warning When setting the policy to "uniqueness", the function does not
     * check whether the queue already contains identical elements. It thus only
     * ensures that elements inserted from now on do not already belong to the
     * queue. */
    // ============================================================================
    void setUniquenessPolicy( const bool new_policy ) ;

    /// @}



  private:
    /// an array storing all the elements of the heap as well as their score
    std::vector<std::pair<Priority, Val*>*> __heap;

    /// a hashtable for quickly finding the elements by their value
    HashTable< Val,std::vector<Size> > __indices;

    /// the number of elements in the heap
    Size __nb_elements;

    /// comparison function
    Cmp __cmp;
  };


} /* namespace gum */


/// always include the implementation of the templates
#include <agrum/core/priorityQueue.tcc>


#endif /* GUM_PRIORITY_QUEUE_H */

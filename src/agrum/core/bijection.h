/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
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
 * @brief Set of pairs of elements with fast search for both elements
 *
 * Bijections are some kind of sets of pairs (T1,T2) with the additional feature
 * as compared to Sets: we can search very quickly T2's elements when knowing T1
 * and T1's elements when knowing T2.
 *
 * @author Jean-Philippe DUBUS / Christophe Gonzales
 */

#ifndef GUM_BIJECTION_H
#define GUM_BIJECTION_H


#include <iostream>
#include <string>
#include <sstream>
#include <agrum/core/hashTable.h>


namespace gum {

  
  template <typename T1, typename T2> class BijectionIterator;

  
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  
  // a class used to create the static iterator used by Bijections. The aim of
  // using this class rather than just creating __BijectionIterEnd as a global
  // variable is to prevent other classes to access and modify __BijectionIterEnd
  class BijectionIteratorStaticEnd {
  private:
    // the iterator used by everyone
    static const BijectionIterator<int,int>* __BijectionIterEnd;

    // creates (if needed) and returns the iterator __BijectionIterEnd
    static const BijectionIterator<int,int>* end4Statics ();

    // friends that have access to the iterator
    template<typename T1, typename T2> friend class Bijection;
  };
  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  
  
  // ==============================================================================
  // ==============================================================================
  /// Iterators for bijection
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class BijectionIterator {
  public:

    /// the possible positions for the iterators 
    enum Position {
      GUM_BIJECTION_BEGIN,
      GUM_BIJECTION_END
    };

    
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// Default constructor
    // ============================================================================
    BijectionIterator ();

    // ============================================================================
    /// Constructor
    /** By default, the iterator points to the starting point of the bijection */
    // ============================================================================
    BijectionIterator ( const Bijection<T1,T2>& bijection,
                        Position pos = GUM_BIJECTION_BEGIN);

    // ============================================================================
    /// Copy constructor
    // ============================================================================
    BijectionIterator (const BijectionIterator<T1,T2>& toCopy);

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~BijectionIterator();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// Copy operator
    // ============================================================================
    BijectionIterator<T1,T2>&
    operator=(const BijectionIterator<T1,T2>& toCopy);

    // ============================================================================
    /// Go to the next association (if it exists)
    // ============================================================================
    BijectionIterator<T1,T2>& operator++();

    // ============================================================================
    /// Comparison of iterators
    // ============================================================================
    bool operator!=(const BijectionIterator<T1,T2>& toCompare) const;
    bool operator==(const BijectionIterator<T1,T2>& toCompare) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{
   
    // ============================================================================
    /// returns the first element of the current association
    // ============================================================================
    const T1& first() const;

    // ============================================================================
    /// returns the second element of the current association
    // ============================================================================
    const T2& second() const;

    /// @}

    
  private:
    /// the hashTable iterator that actually does all the job
    typename HashTable<T1,T2*>::const_iterator __iter;

  };



  

  // ==============================================================================
  // ==============================================================================
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1 and type T2
   * - for x in T1, there exists only one y in T2 associated to x
   * - for y in T2, there exists only one x in T1 associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class Bijection {
  public:
    typedef BijectionIterator<T1,T2> iterator;
    typedef BijectionIterator<T1,T2> const_iterator;

    
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// Default constructor: creates a bijection without any association
    // ============================================================================
    Bijection(Size size = GUM_HASHTABLE_DEFAULT_SIZE,
	      bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY);

    // ============================================================================
    /// Copy constructor
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection(const Bijection<T1,T2>& toCopy);

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~Bijection();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection<T1,T2>& operator=(const Bijection<T1,T2>& toCopy);

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the iterator at the beginning of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    iterator begin() const;

    // ============================================================================
    /// returns the iterator to the end of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    const iterator& end() const;

    // ============================================================================
    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    // ============================================================================
    static const iterator& end4Statics ();
    
    /// @}


    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    const T1& first(const T2& second) const;

    // ============================================================================
    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    const T1& firstWithDefault(const T2& second, const T1& default_val) const;
 
    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    const T2& second(const T1& first) const;

    // ============================================================================
    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    const T2& secondWithDefault(const T1& first, const T2& default_val) const;

    // ============================================================================
    /// Test whether the bijection contains the "first" value
    // ============================================================================
    bool existsFirst(const T1& first) const;
    
    // ============================================================================
    /// Test whether the bijection contains the "second" value
    // ============================================================================
    bool existsSecond(const T2& second) const;

    // ============================================================================
    /// inserts a new association in the bijection
    /** Note that what is actually inserted into the bijection is a copy of
     * the pair (first,second)
     * @throws DuplicateElement exception is thrown if the association
     * already exists */
    // ============================================================================
    void insert(const T1& first, const T2& second);

    // ============================================================================
    /// removes all the associations from the bijection
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns true if the bijection doesn't contain any association
    // ============================================================================
    bool empty() const;

    // ============================================================================
    /// returns the number of associations stored within the bijection
    // ============================================================================
    Size size() const;

    // ============================================================================
    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseFirst(const T1& first);

    // ============================================================================
    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseSecond(const T2& second);

    // ============================================================================
    /// friendly displays the content of the CliqueGraph
    // ============================================================================
    virtual const std::string toString() const;
    
    /// @}
    

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    // ============================================================================
    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// similar to the hashtable's resize
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    // ============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    // ============================================================================
    bool resizePolicy() const ;

    /// @}

    
  private:
    /// a friend to speed-up accesses
    friend class BijectionIterator<T1,T2>;
  
    // below, we create the two hashtables used by the bijection. Note that
    // the values of these hashtables are actually pointers. This enables to
    // create only once objects (T1,T2). When using bijections with large
    // size objects, this feature is of particular interest
    
    /// hashtable associating T2 objects to T1 objects
    HashTable<T1,T2*> __firstToSecond;

    /// hashtable associating T1 objects to T2 objects
    HashTable<T2,T1*> __secondToFirst;
    
    
    // ============================================================================
    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    // ============================================================================
    void __copy (const HashTable<T1,T2*>& f2s); 

    // ============================================================================
    /// inserts a new association in the bijection
    // ============================================================================
    HashTableBucket<T1,T2*>* __insert(const T1& first, const T2& second);
    
  };



  
  

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  
  // ##############################################################################
  // ##############################################################################
  //
  // BELOW, CLASSES ARE SPECIALIZATIONS DESIGNED TO OPTIMIZE BIJECTIONS OF POINTERS
  //
  // ##############################################################################
  // ##############################################################################



  // ==============================================================================
  // ==============================================================================
  /// Iterators for bijection
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class BijectionIterator<T1*,T2*> {
  public:

    /// the possible positions for the iterators 
    enum Position {
      GUM_BIJECTION_BEGIN,
      GUM_BIJECTION_END
    };

    
    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{
    // ============================================================================
    /// Default constructor
    // ============================================================================
    BijectionIterator ();

    // ============================================================================
    /// Constructor
    /** By default, the iterator points to the starting point of the bijection */
    // ============================================================================
    BijectionIterator ( const Bijection<T1*,T2*>& bijection,
                        Position pos = GUM_BIJECTION_BEGIN);

    // ============================================================================
    /// Copy constructor
    // ============================================================================
    BijectionIterator (const BijectionIterator<T1*,T2*>& toCopy);

    // ============================================================================
    /// Destructor
    // ============================================================================
    virtual ~BijectionIterator();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{
    // ============================================================================
    /// Copy operator
    // ============================================================================
    BijectionIterator<T1*,T2*>&
    operator=(const BijectionIterator<T1*,T2*>& toCopy);

    // ============================================================================
    /// Go to the next association (if it exists)
    // ============================================================================
    BijectionIterator<T1*,T2*>& operator++();

    // ============================================================================
    /// Comparison of iterators
    // ============================================================================
    bool operator!=(const BijectionIterator<T1*,T2*>& toCompare) const;
    bool operator==(const BijectionIterator<T1*,T2*>& toCompare) const;

    /// @}


    // ############################################################################
    /// @name Accessors/Modifiers
    // ############################################################################
    /// @{
   
    // ============================================================================
    /// returns the first element of the current association
    // ============================================================================
    T1* const first() const;

    // ============================================================================
    /// returns the second element of the current association
    // ============================================================================
    T2* const second() const;

    /// @}

    
  private:
    /// the hashTable iterator that actually does all the job
    typename HashTable<T1*,T2*>::const_iterator __iter;

  };


  

  
  // a class used to create the static iterator used by Bijections. The aim of
  // using this class rather than just creating __BijectionStarIterEnd as
  // a global variable is to prevent other classes to access and modify
  // __BijectionStarIterEnd
  class BijectionStarIteratorStaticEnd {
  private:
    // the iterator used by everyone
    static const BijectionIterator<int*,int*>* __BijectionStarIterEnd;

    // creates (if needed) and returns the iterator __BijectionStarIterEnd
    static const BijectionIterator<int*,int*>* end4Statics ();

    // friends that have access to the iterator
    template<typename T1, typename T2> friend class Bijection;
  };
  


  

  // ==============================================================================
  // ==============================================================================
  /**
   * This class is designed for modeling a bijection between two sets, the idea is
   * following :
   * - we want to create a bijection relation between type T1* and type T2*
   * - for x in T1*, there exists only one y in T2* associated to x
   * - for y in T2*, there exists only one x in T1* associated to y
   * - the user inserts all the (x, y) associations and can search efficiently the
   * values thus associated
   */
  // ==============================================================================
  // ==============================================================================
  template <typename T1, typename T2>
  class Bijection<T1*,T2*> {
  public:
    typedef BijectionIterator<T1*,T2*> iterator;
    typedef BijectionIterator<T1*,T2*> const_iterator;

    // ############################################################################
    /// @name Constructors/destructors
    // ############################################################################
    /// @{

    // ============================================================================
    /// Default constructor: creates a bijection without association
    // ============================================================================
    Bijection(Size size = GUM_HASHTABLE_DEFAULT_SIZE,
	      bool resize_policy = GUM_HASHTABLE_DEFAULT_RESIZE_POLICY);

    // ============================================================================
    /// Copy constructor
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection(const Bijection<T1*,T2*>& toCopy);

    // ============================================================================
    /// destructor
    // ============================================================================
    virtual ~Bijection();

    /// @}


    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    // ============================================================================
    /// Copy operator
    /** @param toCopy Bijection to copy */
    // ============================================================================
    Bijection<T1*,T2*>& operator=(const Bijection<T1*,T2*>& toCopy);

    /// @}


    // ############################################################################
    /// @name Iterators
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the iterator at the beginning of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    iterator begin() const;

    // ============================================================================
    /// returns the iterator to the end of the bijection
    /** Note that the notion of a beginning/end of a bijection is rather fuzzy.
     * What is important here is that
     * for(iterator iter = begin(); iter != end; ++iter) loops will parse all the
     * associations */
    // ============================================================================
    const iterator& end() const;

    // ============================================================================
    /** @brief returns the end iterator for other classes' statics (read the
     * detailed description of this method)
     *
     * To reduce the Bijections memory consumption (which are heavily used in
     * aGrUM) while allowing fast for(iter=begin(); iter!=end();++iter) loops, end
     * iterators are created just once as a static member of a non-template
     * Bijection. While this scheme is efficient and it works quite effectively
     * when manipulating bijections, it has a drawback: other classes with static
     * members using the Bijection's end() iterator may fail to work due to the
     * well known "static initialization order fiasco" (see Marshall Cline's C++
     * FAQ for more details about this C++ feature). OK, so what is the problem?
     * Consider a class, say X, containing a Bijection that stores all its elements
     * in a convenient way. To reduce memory consumption, X::end iterator is a
     * static member that is initialized with a Bijection::end iterator. If the
     * compiler decides to initialize X::end before initializing Bijection::end,
     * then X::end will be in an incoherent state. Unfortunately, we cannot know
     * for sure in which order static members will be initialized (the order is a
     * compiler's decision). Hence, we shall enfore the fact that Bijection::end
     * is initialized before X::end. Using method Bijection::end4Statics will
     * ensure this fact: it uses the C++ "construct on first use" idiom (see the
     * C++ FAQ) that ensures that the order fiasco is avoided. More precisely,
     * end4Statics uses a global variable that is the very end iterator used by
     * all Bijections. Now, this induces a small overhead. So, we also provide a
     * Bijection::end() method that returns the Bijection::end iterator without
     * this small overhead, but assuming that function end4Statics has already
     * been called once (which is always the case) when a Bijection has been
     * created.
     *
     * So, to summarize: when initializing static members, use end4Statics() rather
     * than end(). In all the other cases, use simply the usual method end(). */
    // ============================================================================
    static const iterator& end4Statics ();

    /// @}


    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    T1* const first(T2* const second) const;

    // ============================================================================
    /** @brief Same method as first, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    T1* const firstWithDefault(T2* const second, T1* const default_val) const;
 
    // ============================================================================
    /// returns the value associated to the element passed in argument
    /** @throws NotFound exception is thrown if the element cannot be found. */
    // ============================================================================
    T2* const second(T1* const first) const;

    // ============================================================================
    /** @brief Same method as second, but if the value is not found, a default
     * value is inserted into the bijection */
    // ============================================================================
    T2* const secondWithDefault(T1* const first, T2* const default_val) const;

    // ============================================================================
    /// Test whether the bijection contains the "first" value
    // ============================================================================
    bool existsFirst(T1* const first) const;
    
    // ============================================================================
    /// Test whether the bijection contains the "second" value
    // ============================================================================
    bool existsSecond(T2* const second) const;

    // ============================================================================
    /// inserts a new association in the bijection
    /**@throws DuplicateElement exception is thrown if the association
     * already exists */
    // ============================================================================
    void insert(T1* const first, T2* const second);

    // ============================================================================
    /// removes all the associations from the bijection
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns true if the bijection doesn't contain any association
    // ============================================================================
    bool empty() const;

    // ============================================================================
    /// returns the number of associations stored within the bijection
    // ============================================================================
    Size size() const;

    // ============================================================================
    /// erases an association containing the given first element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseFirst(T1* const first);

    // ============================================================================
    /// erase an association containing the given second element
    /** If the element cannot be found, nothing is done. In particular, no
     * exception is raised. */
    // ============================================================================
    void eraseSecond(T2* const second);

    // ============================================================================
    /// friendly displays the content of the CliqueGraph
    // ============================================================================
    virtual const std::string toString() const;
    
    /// @}
    

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    // ============================================================================
    /// returns the number of hashtables' slots used (@sa hashTable's capacity)
    // ============================================================================
    Size capacity() const ;

    // ============================================================================
    /// similar to the hashtable's resize
    // ============================================================================
    void resize( Size new_size );

    // ============================================================================
    /// enables the user to change dynamically the resizing policy
    /** @sa HashTable's setResizePolicy */
    // ============================================================================
    void setResizePolicy( const bool new_policy ) ;

    // ============================================================================
    /// returns the current resizing policy
    /** @sa HashTable's resizePolicy */
    // ============================================================================
    bool resizePolicy() const ;

    /// @}

    
  private:
    /// a friend to speed-up accesses
    friend class BijectionIterator<T1*,T2*>;
  
    /// hashtable associating T2* objects to T1* objects
    HashTable<T1*,T2*> __firstToSecond;

    /// hashtable associating T1* objects to T2* objects
    HashTable<T2*,T1*> __secondToFirst;

    
    
    // ============================================================================
    /// a function that performs a complete copy of another bijection
    /** @warning this function assumes that "this" is an empty bijection. If this
     * is not the case, use function clear() before calling __copy. */
    // ============================================================================
    void __copy (const HashTable<T1*,T2*>& f2s); 

    // ============================================================================
    /// inserts a new association in the bijection
    // ============================================================================
    void __insert(T1* const first, T2* const second);
    
  };

 
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  // ============================================================================
  /// for friendly displaying the content of bijections
  // ============================================================================
  template <typename T1, typename T2>
  std::ostream& operator<< ( std::ostream&, const Bijection<T1,T2>& );
  
    
} /* namespace gum */


#include <agrum/core/bijection.tcc>


#endif /* GUM_BIJECTION_H */


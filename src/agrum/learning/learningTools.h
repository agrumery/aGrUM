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
 * @brief Common tools used by learning algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides common tools used by various learning algorithms.
 * */

#ifndef GUM_LEARNING_TOOLS_H
#define GUM_LEARNING_TOOLS_H

#include <vector>
#include <utility>
#include <agrum/core/utils.h>
#include <agrum/core/exceptions.h>
#include <agrum/learning/database.h>


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===   STRUCTURE USED FOR VARIABLES OCCURRENCE COUNTING FROM A DATABASE   === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class LearnCounting
   * @brief The class for counting occurrences of variables from a database
   * @ingroup learning_group
   *
   * Usually, when learning a Bayes net from a database, some independence tests are
   * performed (either implicitly through a score function like in the K2 algorithm
   * or explicitly like in PC algorithm). Various different tests can be used but
   * they all share a common feature: to compute them efficiently, you need creating
   * a tree. Each level of the tree corresponds to a given variable. Each node of the
   * tree corresponds to the number of times each value of the variable at the level
   * has been observed in the database conditionally to the values of the variables
   * higher in the tree. Class LearnCounting provides a structure to compute
   * efficiently these numbers of occurrence. */
  /* ============================================================================ */

  class LearnCounting {
  public:
    // ============================================================================
    // ============================================================================
    /// CountingBox represents the nodes of the tree
    // ============================================================================
    // ============================================================================

    class CountingBox {
    public:
      // ==========================================================================
      /// returns the vector of occurrence counters of the box
      // ==========================================================================
      const std::vector<int>& getCounters() const ;

      // ==========================================================================
      /// returns the number of occurrences of modality i found in the database
      // ==========================================================================
      int getCounter( unsigned int i ) const;

      // ==========================================================================
      /// returns the vector of the children of the box
      // ==========================================================================
      const std::vector<CountingBox*>& getChildren() const ;

      // ==========================================================================
      /// returns the next box on the same level of the tree
      // ==========================================================================
      CountingBox* getNextBox() const ;


    private:
      /// the node on the right of the current node (same level in the tree)
      CountingBox *next;

      /// the number of cases observed from the database
      std::vector<int> nb_cases;

      /// pointers to the children of the current node
      std::vector<CountingBox*> children;

      /// LearnCounting can create and access its own CountingBox
      friend class LearnCounting;
      
      // ==========================================================================
      /// create a new box
      // ==========================================================================
      explicit CountingBox( unsigned int nb_modalities );

      // ==========================================================================
      /// destructor
      // ==========================================================================
      ~CountingBox();
    };


    
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// basic constructor
    // ============================================================================
    LearnCounting( const Database& );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~LearnCounting();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    // ============================================================================
    /// reset the counting structure to an empty tree
    // ============================================================================
    void clear();

    // ============================================================================
    /// a function that parses the database and store the results into the tree
    /** @param min_level the level below which we update the counters of the tree
     * (these include min_level). The top of the tree is level 0
     * @param max_level the level above which we update the counters of the tree
     * (these include max_level). The top of the tree is level 0 */
    // ============================================================================
    void parseDatabase( unsigned int min_level, unsigned int max_level ) ;

    // ============================================================================
    /// a function that parses the database and stores the results into the tree
    // ============================================================================
    void parseDatabase() ;

    // ============================================================================
    /// remove the last level of the tree, if any
    /** If the tree is empty, the function does not perform anything.
     * @warning for speed reasons, the function does not alter the children field of
     * the nodes at the level above the last one.*/
    // ============================================================================
    void deleteLastLevel();

    // ============================================================================
    /// deletes a given level of the tree as well as the levels below
    /** @warning for efficiency reasons, the function does not alter the children
     * field of the nodes at the level above the ones deleted. */
    // ============================================================================
    void deleteLevel( unsigned int level );

    // ============================================================================
    /// remove a given level that does not belong to the tree
    /** @warning for speed reasons, the function does not alter the children field of
     * the nodes at the level above the last one. Nor does it remove its own
     * children */
    // ============================================================================
    void deleteLevel( CountingBox* level_beg, CountingBox* level_end );

    // ============================================================================
    /// extracts and remove a level from the tree
    /** if the level does not exist, the function returns (0,0).
     * @warning For efficiency reasons, the children field of the level above the
     * deleted one is not updated, nor are the levels below deleted. */
    // ============================================================================
    std::pair<CountingBox*,CountingBox*> removeNGetLevel( unsigned int level );

    // ============================================================================
    /// extracts and remove the last level from the tree
    /** if the level does not exist, the function returns (0,0).
     * @warning For efficiency reasons, the children field of the level above the
     * last one is not updated. However, the levels below are removed
     * from the tree.*/
    // ============================================================================
    std::pair<CountingBox*,CountingBox*> removeNGetLastLevel();

    // ============================================================================
    /// extracts a level from the tree but keep it in the tree
    /** if the level does not exist, the function returns 0 */
    // ============================================================================
    CountingBox* getLevel( unsigned int level ) const ;

    // ============================================================================
    /// extracts the last level from the tree but keep it in the tree
    /** if the level does not exist, the function returns 0 */
    // ============================================================================
    CountingBox* getLastLevel() const ;

    // ============================================================================
    /// returns the number of levels in the tree
    // ============================================================================
    unsigned int getNbrLevels() const ;

    // ============================================================================
    /// adds an already constructed level to the bottom of the tree
    // ============================================================================
    void insertLevel( CountingBox* new_level_beg,
                      CountingBox* new_level_end,
                      unsigned int    new_level_row );

    // ============================================================================
    /// adds a new level corresponding to node "row" at the bottom of the tree
    // ============================================================================
    void createLevel( unsigned int row );

    // ============================================================================
    /// returns the row corresponding to a given level
    /**
     * @throw NotFound
     */
    // ============================================================================
    unsigned int getRow( unsigned int level ) const;

    // ============================================================================
    /// collapses the last level of the tree with the one above it
    /** For instance, if the last three levels of the tree correspond to x, y and z
     * respectively, then collapse will remove the y level and update the z level
     * accordingly. Hence, after collapse, the tree will contain levels x and z. */
    // ============================================================================
    void collapseLastLevel();

    /// @}


  protected:
    /// the database we take the data from
    const Database& database;

    /// vectors indicating the beginning and the end of each level of the tree
    std::vector<CountingBox*> level_beg;
    std::vector<CountingBox*> level_end;

    /** @brief for each level, the index of the row corresponding to the node
     * in the database */
    std::vector<unsigned int> level_row;

    /// indicates to which level we arrived
    unsigned int tree_level;

    /// a garbage used to avoid newing/deleting CountingBoxes too often
    CountingBox *garbage;

    // ============================================================================
    /// forbid copy constructor
    // ============================================================================
    LearnCounting( const LearnCounting& from );

    // ============================================================================
    /// forbid copy operator
    // ============================================================================
    LearnCounting& operator= ( const LearnCounting& from );

    // ============================================================================
    /// returns a new box for the tree (either newly created or taken from garbage)
    // ============================================================================
    CountingBox* newCountingBox( unsigned int modal );
  };


  
  #define GUM_LEARNING_CONFIDENCE_PROBA 0.05


  /* ============================================================================ */
  /* ============================================================================ */
  /* === STRUCTURE USED FOR COMPUTING CHI2 INDEPENDENCE TESTS FROM A DATABASE === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class LearnChi2Test
   * @brief The class for computing chi2 conditional independence tests
   */
  /* ============================================================================ */
  class LearnChi2Test {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// basic constructor
    // ============================================================================
    LearnChi2Test( const Database& data,
                   float confidence_proba = GUM_LEARNING_CONFIDENCE_PROBA );

    // ============================================================================
    /// destructor
    // ============================================================================
    ~LearnChi2Test();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    
    // ============================================================================
    /// computes the Chi2 value of the current tree
    // ============================================================================
    float getChi2Value();

    // ============================================================================
    /// computes the thresholdaccording to the number of degrees of freedom
    // ============================================================================
    float getChi2Threshold();

    // ============================================================================
    /// returns the number of degrees of freedom
    // ============================================================================
    unsigned long getChi2DegreesOfFreedom();

    // ============================================================================
    /// modifies the confidence proba
    // ============================================================================
    void setConfidenceProba( float new_proba );

    // ============================================================================
    /// deletes a variable from the structure containing all the variables
    /**
     * @throw NullElement
     * @throw NotFound
     */
    // ============================================================================
    void deleteVariable( unsigned int row ) ;

    // ============================================================================
    /// inserts a new variable at the end of the tree
    // ============================================================================
    void insertVariable( unsigned int row );

    // ============================================================================
    /// removes all the variables from the current structure
    // ============================================================================
    void clear();

    // ============================================================================
    /// returns the level in the tree of a given variable
    /**
     * @throw NotFound
     */
    // ============================================================================
     unsigned int getLevel( unsigned int row ) const;

    // ============================================================================
    /// returns the variable (its row in the database) of a given tree level
    // ============================================================================
    unsigned int variable( unsigned int level ) const;

    // ============================================================================
    /// returns the list of variables in the parsing tree
    /** The vextor contains the rows corresponding to the variables in
     * the database */
    // ============================================================================
    const std::vector<unsigned int> variables() const ;

    /// @}


  private:
    /// the database from which we compute the numbers of occurence
    const Database& database;

    /// the counting tree used to store the numbers of occurence
    LearnCounting tree;

    /// the set of variables in the tree (from top to bottom)
    std::vector<unsigned int> rows;

    /// the current chi2 value
    float chi2_value;

    /// indicates whether the chi2_value has been computed
    bool has_chi2_value;

    /// the current chi2 threshold
    float chi2_threshold;

    /// indicates whether the chi2_threshold has been computed
    bool has_chi2_threshold;

    /// the current number of degrees of freedom
    unsigned long degrees_of_freedom;

    /// indicates whether the number of degrees of freedom has been computed
    bool has_degrees_of_freedom;

    /// the probability for the confidence intervals
    float confidence_proba;

    /// a vector containing the thresholds for given degrees of freedom
    std::vector<float> chi2_thresholds;

    /// a vector indicating wether a given threshold has been computed
    std::vector<float> has_chi2_thresholds;

    /// the level of the tree from which we need update the parsing
    unsigned int min_parsing_level;

    /// indicates whether the tree requires a new database parsing
    bool require_database_parsing;

    // ============================================================================
    /// computes the threshold of a chi2 for a given proba and degrees of freedom
    // ============================================================================
    float computeChi2Threshold();

    // ============================================================================
    /// computes the critical value of a given chi2 test (used by the cache)
    /** This code has been written by Gary Perlman */
    // ============================================================================
    static double getCriticalValue( double proba, unsigned long df );

    // ============================================================================
    /// computes the probability of chi2 value (used by the cache)
    /** This code has been written by Gary Perlman.
     * ALGORITHM Compute probability of chi square value.
     * Adapted from:
     * Hill, I. D. and Pike, M. C.  Algorithm 299
     * Collected Algorithms for the CACM 1967 p. 243
     * Updated for rounding errors based on remark in
     * ACM TOMS June 1985, page 185 */
    // ============================================================================
    static double getProbaChi2( double x, unsigned long df );

    // ============================================================================
    /// computes the probability of normal z value (used by the cache)
    /** This code has been written by Grary Perlman.
     * ALGORITHM Adapted from a polynomial approximation in:
     Ibbetson D, Algorithm 209
     Collected Algorithms of the CACM 1963 p. 616
     Note:
     This routine has six digit accuracy, so it is only useful for absolute
     z values < 6.  For z values >= to 6.0, poz() returns 0.0. */
    // ============================================================================
    static double getProbaZValue( double z );
  };

  
  // ##### TODO: a placer a l'interieur de IndepCache
  // ============================================================================
  /// the tree structure of the cache
  // ============================================================================
  template <typename Score> struct CacheNode {
    // ==========================================================================
    /// default constructor
    // ==========================================================================
    CacheNode();
    
    // ==========================================================================
    /// copy constructor
    // ==========================================================================
    CacheNode( const CacheNode<Score>& from );

    // ==========================================================================
    /// copy operator
    // ==========================================================================
    CacheNode<Score>& operator= ( const CacheNode<Score>& from );

    // ==========================================================================
    /// destructor
    // ==========================================================================
    ~CacheNode();

    /// the independence test value of the node
    Score score;

    /// indicate whether the node has a score or not
    bool has_score;

    /// the list of children nodes of the current node
    HashTable<Id, CacheNode<Score> > children;
  };

  


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===   STRUCTURE USED FOR CACHING CONDITIONAL INDEPENDENCE TEST VALUES    === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class IndepCache
   * @brief The class for storing the values of conditional independence tests
   */
  /* ============================================================================ */
  template <typename Score> class IndepCache {
  public:
    // ============================================================================
    /// default constructor: creates an empty cache
    // ============================================================================
    IndepCache();
    
    // ============================================================================
    /// destructor
    // ============================================================================
    ~IndepCache();

    // ============================================================================
    /// adds a new value into the cache
    /** @throw DuplicateElement exception is thrown if the value of a similar test
     * has already been entered into the cache. If, in this case, you would like the
     * value to be updated, use function setTestVal instead. */
    // ============================================================================
    void insertTestVal( const Id& node1, const Id& node2,
                        const std::vector<Id>& conditional_set, const Score& val,
                        bool safe_mode = false );

    // ============================================================================
    /// updates a value in the cache (or adds it if it does not already exist)
    // ============================================================================
    void setTestVal( const Id& node1, const Id& node2,
                     const std::vector<Id>& conditional_set, const Score& val,
                     bool safe_mode = false );

    // ============================================================================
    /// returns the value of a given test from the cache
    /** @throw NotFound exception is thrown if the so-called value
     * cannot be found in the cache.
     */
    // ============================================================================
    Score& getTestVal( const Id& node1, const Id& node2,
                       const std::vector<Id>& conditional_set,
                       bool safe_mode = false ) const;

    // ============================================================================
    /// returns a pointer to the value of a given independence test
    /** If the value we look for cannot be found in the cache, the function returns
     * a 0 pointer. */
    // ============================================================================
    Score* getPtrTestVal( const Id& node1, const Id& node2,
                          const std::vector<Id>& conditional_set,
                          bool safe_mode = false ) const ;


  private:
    /// the cache itself
    HashTable< std::pair<Id,Id>, CacheNode<Score> > cache;

    // ============================================================================
    /// creates the tree necessary for the insertion/update of a new test value
    // ============================================================================
    CacheNode<Score>& _prepareInsertVal( const Id& node1, const Id& node2,
                                         const std::vector<Id>& conditional_set,
                                         const Score& val, bool safe_mode = false );
  };

  
  /// necessary for the hashtable operator <<
  template <typename Score>
  std::ostream& operator<< ( std::ostream&, const CacheNode<Score>& );


} /* namespace gum */




/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/learningTools.inl>
#endif /* GUM_NO_INLINE */


/// always include the template implementations
#include <agrum/learning/learningTools.tcc>


#endif /* GUM_LEARNING_TOOLS_H */

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

#include <agrum/config.h>

#include <agrum/learning/database.h>


namespace gum {




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

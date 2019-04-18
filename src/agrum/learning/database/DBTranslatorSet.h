
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief A class for storing several translators
 *
 * When learning Bayesian networks, the records of the train dataset are
 * used to construct contingency tables that are either exploited in
 * statistical conditional independence tests or in scores. In both cases,
 * the values observed in the records must be translated into indices in
 * the finite domain of the corresponding random variables. DBTranslator
 * instances are used for this purpose. To make the parsing of all the columns
 * of the dataset easier, all the DBTranslator instances used are gathered
 * into a single DBRowTranslatorSet.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_SET_H
#define GUM_LEARNING_DB_TRANSLATOR_SET_H

#include <vector>
#include <limits>
#include <type_traits>

#include <agrum/agrum.h>
#include <agrum/learning/database/DBTranslator.h>
#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslator4DiscretizedVariable.h>
#include <agrum/learning/database/DBTranslator4RangeVariable.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>


namespace gum {

  namespace learning {

    /** @class DBTranslatorSet
     * @headerfile DBTranslatorSet.h <agrum/learning/database/DBTranslatorSet.h>
     * @ingroup learning_database
     * @brief the class for packing together the translators used to preprocess
     * the datasets
     *
     * When learning Bayesian networks, the records of the train dataset are
     * used to construct contingency tables that are either exploited in
     * statistical conditional independence tests or in scores. In both cases,
     * the values observed in the records must be translated into indices in
     * the finite domain of the corresponding random variables. The
     * DBTranslator classes are used for this purpose. To make the parsing
     * of all the columns of the dataset easier, all the DBTranslator
     * instances used are gathered into a DBTranslatorSet.
     *
     * @par Here is an example of how to use this class:
     * @code
     * // create an empty translator set
     * gum::learning::DBTranslatorSet<> set;
     *
     * std::vector<std::string> missing { "?", "N/A", "???" };
     *
     * // create the translators and add them to the translator set. First,
     * // create translator1 that will perform its translations on Column 1
     * // of the dataset (columns start from index 0)
     * gum::learning::DBTranslator4LabelizedVariable<> translator1 ( missing, 3 );
     * std::size_t pos1 = set.insertTranslator ( translator1, 1 );
     * // currently, pos1 is equal to 0, that is, translator1 is the first
     * // translator in the translator set
     *
     * // create a translator handling Column 0 of the dataset
     * gum::learning::DBTranslator4ContinuousVariable<> translator0;
     * std::size_t pos0 = set.insertTranslator ( translator0, 0 );
     * // translator0 has been inserted into the translator set at position pos0.
     * // pos0 = 0 because translators are sorted by increasing column order in
     * // the translator set. So, now, in the set, the first translator is
     * // translator0 and the the second one is translator1.
     *
     * gum::learning::DBTranslator4LabelizedVariable<> translator2;
     * std::size_t pos2 = set.insertTranslator ( translator2, 2 );
     * // the set contains { translator0, translator1, translator2 }, in this order
     *
     * // parsing the rows of the dataset
     * std::vector<std::string> row1 { ".33", "toto", "titi" };
     * float val0       = set.translate ( row1, 0 ).cont_val;  // val0 = 0.33f
     * std::size_t val1 = set.translate ( row1, 1 ).discr_val; // val1 = 0 (toto)
     * std::size_t val2 = set.translate ( row1, 2 ).discr_val; // val2 = 0 (titi)
     * std::vector<std::string> row2 { "4.22x", "???", "??" };
     * val0 = set.translate ( row2, 0 ).cont_val;  // raises gum::TypeError
     * val1 = set.translate ( row2, 1 ).discr_val;
     *                               // = std::numeric_limits<std::size_t>::max ()
     * val2 = set.translate ( row2, 2 ).discr_val; // = 1 (??)
     *
     * // with method translateSafe, an exception is raised whenever we try to
     * // translate a column that is not taken into account by the translators
     * set.translateSafe ( row2, 3 ); // raises gum::UndefinedElement
     * @endcode
     */
    template < template < typename > class ALLOC = std::allocator >
    class DBTranslatorSet {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = typename DBTranslator< ALLOC >::allocator_type;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTranslatorSet(const allocator_type& alloc = allocator_type());

      /// copy constructor
      DBTranslatorSet(const DBTranslatorSet< ALLOC >& from);

      /// copy constructor with a given allocator
      DBTranslatorSet(const DBTranslatorSet< ALLOC >& from,
                      const allocator_type&           alloc);

      /// move constructor
      DBTranslatorSet(DBTranslatorSet< ALLOC >&& from);

      /// move constructor with a given allocator
      DBTranslatorSet(DBTranslatorSet< ALLOC >&& from,
                      const allocator_type&      alloc);

      /// virtual copy constructor
      virtual DBTranslatorSet< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual DBTranslatorSet< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~DBTranslatorSet();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslatorSet< ALLOC >& operator=(const DBTranslatorSet< ALLOC >& from);

      /// move operator
      DBTranslatorSet< ALLOC >& operator=(DBTranslatorSet< ALLOC >&& from);

      /// returns the kth translator
      /** @warning this operator assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translatorSafe that performs this check. */
      DBTranslator< ALLOC >& operator[](const std::size_t k);

      /// returns the kth translator
      /** @warning this operator assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translatorSafe that performs this check. */
      const DBTranslator< ALLOC >& operator[](const std::size_t k) const;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// inserts a new translator at the end of the translator set
      /** @param translator a translator that will be copied into the
       * translator set
       * @param column the index of the column that this new translator should
       * read in the database.
       * @param unique_column indicates whether the column can be read by
       * several translators.
       * @return the position of the translator within the translator set.
       * @throw DuplicateElement is raised if there already exists a translator
       * reading the column passed in argument and the unique_column
       * argument is set to true. */
      template < template < template < typename > class > class Translator >
      std::size_t insertTranslator(const Translator< ALLOC >& translator,
                                   const std::size_t          column,
                                   const bool unique_column = true);

      /** @brief inserts a new translator for a given variable at the end of
       * the translator set
       *
       * @param var the variable that will be contained into the translator
       * @param column the index of the column that this new translator should
       * read in the database.
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param unique_column indicates whether the column can be read by
       * several translators.
       * @throw DuplicateElement is raised if there already exists a translator
       * reading the column passed in argument and the unique_column
       * argument is set to true.
       */
      template < template < typename > class XALLOC >
      std::size_t insertTranslator(
         const Variable&                                          var,
         const std::size_t                                        column,
         const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
         const bool unique_column = true);

      /** @brief inserts a new translator for a given variable at the end of
       * the translator set
       *
       * @param var the variable that will be contained into the translator
       * @param column the index of the column that this new translator should
       * read in the database.
       * @param unique_column indicates whether the column can be read by
       * several translators.
       * @throw DuplicateElement is raised if there already exists a translator
       * reading the column passed in argumentt and the unique_column
       * argument is set to true.
       */
      std::size_t insertTranslator(const Variable&   var,
                                   const std::size_t column,
                                   const bool        unique_column = true);

      /** @brief erases either the kth translator or those parsing the kth
       * column of the input database
       *
       * DBTranslatorSets do not necessarily read all the columns of their
       * input database. For instance, a CSV may contain 10 columns, but the
       * DBTranslatorSet may only contain two translators reading columns 3
       * and 5 respectively. When k_is_input_col is set to false, Parameter k
       * passed in argument corresponds to either 0 or 1, i.e., to the index of
       * one of the two translators stored into the DBTranslatorSet. When
       * k_is_input_col is set to true, the translators to be erased are the ones
       * that parse the kth column of the input database (when several
       * translators parse the column k, all of them are removed).
       * @warning if the translator does not exists, nothing is done. In
       * particular, no exception is raised. */
      void eraseTranslator(const std::size_t k, const bool k_is_input_col = false);

      /// returns the kth translator
      /** @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translatorSafe that performs this check.*/
      DBTranslator< ALLOC >& translator(const std::size_t k);

      /// returns the kth translator
      /** @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translatorSafe that performs this check.*/
      const DBTranslator< ALLOC >& translator(const std::size_t k) const;

      /// returns the kth translator
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      DBTranslator< ALLOC >& translatorSafe(const std::size_t k);

      /// returns the kth translator
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      const DBTranslator< ALLOC >& translatorSafe(const std::size_t k) const;

      /// ask the kth translator to translate a string in a row of the database
      /** @param row a row of the original database
       * @param k the index of the translator that will perform the translation
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translateSafe that performs this check.
       * @warning as there is not necessarily an identity mapping between the
       * set of columns of the database and the set of translators used, k may
       * not necessarily corresponds to the index of a column in the database:
       * this is the index of a translator within the set */
      template < template < typename > class OTHER_ALLOC >
      DBTranslatedValue translate(
         const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
         const std::size_t                                             k) const;

      /** @brief similar to method translate, except that it checks that the kth
       * translator exists
       *
       * @param row a row of the original database
       * @param k the index of the translator that will perform the translation
       * @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set.
       * @warning as there is not necessarily an identity mapping between the
       * set of columns of the database and the set of translators used, k may
       * not necessarily corresponds to the index of a column in the database:
       * this is the index of a translator within the set */
      template < template < typename > class OTHER_ALLOC >
      DBTranslatedValue translateSafe(
         const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
         const std::size_t                                             k) const;

      /// returns the original string that was translated into translated_val
      /** @param translated_val the value from which we look for the
       * original string
       * @param k the index of the translator that performed the translation
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method translateBackSafe that performs this check.
       * @warning as there is not necessarily an identity mapping between the
       * set of columns of the database and the set of translators used, k may
       * not necessarily corresponds to the index of a column in the database:
       * this is the index of a translator within the set */
      std::string translateBack(const DBTranslatedValue translated_val,
                                const std::size_t       k) const;

      /** @brief similar to method translateBack, except that it checks that
       * the kth translator exists
       *
       * @param translated_val the value from which we look for the
       * original string
       * @param k the index of the translator that performed the translation
       * @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set.
       * @warning as there is not necessarily an identity mapping between the
       * set of columns of the database and the set of translators used, k may
       * not necessarily corresponds to the index of a column in the database:
       * this is the index of a translator within the set */
      std::string translateBackSafe(const DBTranslatedValue translated_val,
                                    const std::size_t       k) const;

      /** @brief indicates whether the kth translator considers a translated_val
       * as a missing value
       *
       * @param translated_val the value that we compare to the translation of
       * a missing value
       * @param k the index of the translator that performed the translation
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method isMissingValueSafe that performs this check.
       */
      bool isMissingValue(const DBTranslatedValue translated_val,
                          const std::size_t       k) const;

      /** @brief similar to method isMissingValue, except that it checks that
       * the kth translator exists
       *
       * @param translated_val the value that we compare to the translation of
       * a missing value
       * @param k the index of the translator that performed the translation
       * @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set.
       */
      bool isMissingValueSafe(const DBTranslatedValue translated_val,
                              const std::size_t       k) const;

      /// returns the domain size of the variable stored into the kth translator
      /** @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method domainSizeSafe that performs this check. */
      std::size_t domainSize(const std::size_t k) const;

      /// returns the domain size of the variable stored into the kth translator
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      std::size_t domainSizeSafe(const std::size_t k) const;

      /// returns the variable stored into the kth translator
      /** @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method variableSafe that performs this check. */
      const Variable& variable(const std::size_t k) const;

      /// returns the variable stored into the kth translator
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      const Variable& variableSafe(const std::size_t k) const;

      /** @brief indicates whether a reordering is needed to make the kth
       * translator sorted
       *
       * For a given translator, if the strings represented by the translations
       * are only numbers, the translations are considered to be sorted if and
       * only if they are sorted by increasing number. If the strings do not
       * only represent numbers, then translations are considered to be sorted
       * if and only if they are sorted lexicographically.
       *
       * When constructing dynamically its dictionary, the translator may
       * assign wrong DBTranslatedValue values to strings. For instance, a
       * translator reading sequentially integer strings 4, 1, 3, may map
       * 4 into DBTranslatedValue{std::size_t(0)},
       * 1 into DBTranslatedValue{std::size_t(1)} and
       * 3 into DBTranslatedValue{std::size_t(2)}, resulting in random variables
       * having domain {4,1,3}. The user may prefer having domain {1,3,4}, i.e.,
       * a domain specified with increasing values. This requires a
       * reordering. Method needsReodering() returns a Boolean indicating
       * whether such a reordering should be performed or whether the current
       * order is OK.
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method needsReorderingSafe that performs this check. */
      bool needsReordering(const std::size_t k) const;

      /// same as method needsReordering but checks that the kth translator exists
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      bool needsReorderingSafe(const std::size_t k) const;

      /** @brief performs a reordering of the dictionary and returns a mapping
       * from the old translated values to the new ones.
       *
       * When a reordering is needed, i.e., string values must be translated
       * differently, Method reorder() computes how the translations should be
       * changed. It updates accordingly the dictionary and returns the mapping
       * that enables changing the old dictionary values into the new ones.
       * Note that the hash table returned is expressed in terms of std::size_t
       * because only the translations for discrete random variables need be
       * reordered, those for continuous random variables are identity mappings.
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method reorderSafe that performs this check. */
      HashTable< std::size_t,
                 std::size_t,
                 ALLOC< std::pair< std::size_t, std::size_t > > >
         reorder(const std::size_t k);

      /// same as method reorder but checks that the kth translator exists
      /** @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      HashTable< std::size_t,
                 std::size_t,
                 ALLOC< std::pair< std::size_t, std::size_t > > >
         reorderSafe(const std::size_t k);

      /** @brief returns the column of the input database that will be read
       * by the kth translator
       *
       * @warning this method assumes that there are at least k translators.
       * So, it won't check that the kth translator actually exists. If unsure,
       * use method inputColumnSafe that performs this check. */
      std::size_t inputColumn(const std::size_t k) const;

      /** @brief returns the column of the input database that will be read
       * by the kth translator
       * @throw UndefinedElement is raised if there are fewer than k
       * translators in the translator set. */
      std::size_t inputColumnSafe(const std::size_t k) const;

      /// returns the largest input database column index read by the translators
      std::size_t highestInputColumn() const;

      /// remove all the translators
      void clear();

      /// returns the number of translators stored into the set
      std::size_t nbTranslators() const;

      /// returns the number of translators stored into the set
      std::size_t size() const;

      /// returns the allocator used by the translator set
      allocator_type getAllocator() const;

      /// returns the set of translators
      const std::vector< DBTranslator< ALLOC >*, ALLOC< DBTranslator< ALLOC >* > >&
         translators() const;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the set of all the translators
      std::vector< DBTranslator< ALLOC >*, ALLOC< DBTranslator< ALLOC >* > >
         __translators;

      // a vector indicating which column of the original database each
      // translator should translate
      std::vector< std::size_t, ALLOC< std::size_t > > __columns;

      // the highest column index read by the translators
      std::size_t __highest_column{std::size_t(0)};

      /// copy the content of another translator set that uses another allocator
      void __copy(const DBTranslatorSet< ALLOC >& from,
                  const allocator_type&           alloc);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// always include templated implementation
#include <agrum/learning/database/DBTranslatorSet_tpl.h>

#endif /* GUM_LEARNING_DB_TRANSLATOR_SET_H */

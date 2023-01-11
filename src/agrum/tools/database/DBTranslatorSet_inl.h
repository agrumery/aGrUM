/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#  include <agrum/tools/database/DBTranslatorSet.h>

namespace gum {

  namespace learning {
    /// default constructor
    INLINE DBTranslatorSet::DBTranslatorSet() { GUM_CONSTRUCTOR(DBTranslatorSet); }


    /// copy constructor
    INLINE DBTranslatorSet::DBTranslatorSet(const DBTranslatorSet& from) {
      _copy_(from);

      GUM_CONS_CPY(DBTranslatorSet);
    }


    /// move constructor
    INLINE DBTranslatorSet::DBTranslatorSet(DBTranslatorSet&& from) :
        _translators_(std::move(from._translators_)), _columns_(std::move(from._columns_)),
        _highest_column_(from._highest_column_) {
      GUM_CONS_MOV(DBTranslatorSet);
    }


    /// virtual copy constructor
    INLINE DBTranslatorSet* DBTranslatorSet::clone() const { return new DBTranslatorSet(*this); }


    /// destructor
    INLINE DBTranslatorSet::~DBTranslatorSet() {
      clear();
      GUM_DESTRUCTOR(DBTranslatorSet);
    }


    /// move operator
    INLINE DBTranslatorSet& DBTranslatorSet::operator=(DBTranslatorSet&& from) {
      if (this != &from) {
        clear();
        _translators_    = std::move(from._translators_);
        _columns_        = std::move(from._columns_);
        _highest_column_ = from._highest_column_;
      }

      return *this;
    }


    /// returns the ith translator
    INLINE DBTranslator& DBTranslatorSet::operator[](const std::size_t k) {
      return *(_translators_[k]);
    }


    /// returns the ith translator
    INLINE const DBTranslator& DBTranslatorSet::operator[](const std::size_t k) const {
      return *(_translators_[k]);
    }


    /// inserts a new translator for a given variable in the translator set
    INLINE std::size_t DBTranslatorSet::insertTranslator(const Variable&   var,
                                                         const std::size_t column,
                                                         const bool        unique_column) {
      const std::vector< std::string > missing;
      return this->insertTranslator(var, column, missing, unique_column);
    }


    /// ask the kth translator to translate a string in a row of the database
    INLINE DBTranslatedValue DBTranslatorSet::translate(const std::vector< std::string >& row,
                                                        const std::size_t                 k) const {
      return _translators_[k]->translate(row[_columns_[k]]);
    }


    /// ask the kth translator to translate a string in a row of the database
    INLINE DBTranslatedValue DBTranslatorSet::translateSafe(const std::vector< std::string >& row,
                                                            const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << " could not be found")

      return _translators_[k]->translate(row[_columns_[k]]);
    }


    /// returns the original string that was translated into translated_val
    INLINE std::string DBTranslatorSet::translateBack(const DBTranslatedValue translated_val,
                                                      const std::size_t       k) const {
      return _translators_[k]->translateBack(translated_val);
    }


    /// returns the original string that was translated into translated_val
    INLINE std::string DBTranslatorSet::translateBackSafe(const DBTranslatedValue translated_val,
                                                          const std::size_t       k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return _translators_[k]->translateBack(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    INLINE bool DBTranslatorSet::isMissingValue(const DBTranslatedValue translated_val,
                                                const std::size_t       k) const {
      return _translators_[k]->isMissingValue(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    INLINE bool DBTranslatorSet::isMissingValueSafe(const DBTranslatedValue translated_val,
                                                    const std::size_t       k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return _translators_[k]->isMissingValue(translated_val);
    }


    /// returns the kth translator
    INLINE DBTranslator& DBTranslatorSet::translator(const std::size_t k) {
      return *(_translators_[k]);
    }


    /// returns the kth translator
    INLINE const DBTranslator& DBTranslatorSet::translator(const std::size_t k) const {
      return *(_translators_[k]);
    }


    /// returns the kth translator
    INLINE DBTranslator& DBTranslatorSet::translatorSafe(const std::size_t k) {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(_translators_[k]);
    }


    /// returns the kth translator
    INLINE const DBTranslator& DBTranslatorSet::translatorSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(_translators_[k]);
    }


    /// returns the domain size of the variables stored into the kth translator
    INLINE std::size_t DBTranslatorSet::domainSize(const std::size_t k) const {
      return _translators_[k]->domainSize();
    }


    /// returns the domain size of the variables stored into the kth translator
    INLINE std::size_t DBTranslatorSet::domainSizeSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->domainSize();
    }


    /// returns the variable stored into the kth translator
    INLINE const Variable& DBTranslatorSet::variable(const std::size_t k) const {
      return *(_translators_[k]->variable());
    }


    /// returns the variable stored into the kth translator
    INLINE const Variable& DBTranslatorSet::variableSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return *(_translators_[k]->variable());
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    INLINE bool DBTranslatorSet::needsReordering(const std::size_t k) const {
      return _translators_[k]->needsReordering();
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    INLINE bool DBTranslatorSet::needsReorderingSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->needsReordering();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    INLINE HashTable< std::size_t, std::size_t > DBTranslatorSet::reorder(const std::size_t k) {
      return _translators_[k]->reorder();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    INLINE HashTable< std::size_t, std::size_t > DBTranslatorSet::reorderSafe(const std::size_t k) {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->reorder();
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    INLINE std::size_t DBTranslatorSet::inputColumn(const std::size_t k) const {
      return _columns_[k];
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    INLINE std::size_t DBTranslatorSet::inputColumnSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Column #" << k << "could not be found")

      return _columns_[k];
    }


    /// returns the largest input database column index read by the translators
    INLINE std::size_t DBTranslatorSet::highestInputColumn() const { return _highest_column_; }


    /// returns the number of translators stored into the set
    INLINE std::size_t DBTranslatorSet::nbTranslators() const { return _columns_.size(); }


    /// returns the number of translators stored into the set
    INLINE std::size_t DBTranslatorSet::size() const { return _columns_.size(); }

    /// returns the set of translators
    INLINE const std::vector< DBTranslator* >& DBTranslatorSet::translators() const {
      return _translators_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

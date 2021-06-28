/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief The set of translators stored into a row filter
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the translator set
    template < template < typename > class ALLOC >
    typename DBTranslatorSet< ALLOC >::allocator_type
       DBTranslatorSet< ALLOC >::getAllocator() const {
      return _columns_.get_allocator();
    }


    /// remove all the translators from the vector
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::clear() {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      for (auto translator: _translators_) {
        allocator.destroy(translator);
        allocator.deallocate(translator, 1);
      }

      _translators_.clear();
      _columns_.clear();
      _highest_column_ = std::size_t(0);
    }


    /// copy the content of another translator set that uses another allocator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::_copy_(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) {
      if (_translators_.size() != 0) clear();

      // resize the vectors used in the set. First, we reserve new memory. This
      // will keep the DBTranslatorSet in a correct state, even if the memory
      // allocation fails
      const std::size_t size = from._translators_.size();
      _translators_.reserve(size);
      _columns_.reserve(size);
      _translators_.resize(size);
      _columns_.resize(size);

      std::size_t i;
      try {
        for (i = std::size_t(0); i < size; ++i) {
          _translators_[i] = from._translators_[i]->clone(alloc);
          _columns_[i]     = from._columns_[i];
        }
      } catch (...) {
        _translators_.resize(i);
        clear();
        throw;
      }

      _highest_column_ = from._highest_column_;
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        _translators_(alloc),
        _columns_(allocator_type(alloc)) {
      GUM_CONSTRUCTOR(DBTranslatorSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       const DBTranslatorSet< ALLOC >&                          from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        _translators_(alloc),
        _columns_(alloc) {
      _copy_(from, alloc);

      GUM_CONS_CPY(DBTranslatorSet);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(const DBTranslatorSet< ALLOC >& from) :
        DBTranslatorSet< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(
       DBTranslatorSet< ALLOC >&&                               from,
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) :
        _translators_(std::move(from._translators_), alloc),
        _columns_(std::move(from._columns_), alloc), _highest_column_(from._highest_column_) {
      GUM_CONS_MOV(DBTranslatorSet);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::DBTranslatorSet(DBTranslatorSet< ALLOC >&& from) :
        DBTranslatorSet< ALLOC >(from, from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >* DBTranslatorSet< ALLOC >::clone(
       const typename DBTranslatorSet< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBTranslatorSet< ALLOC > > allocator(alloc);
      DBTranslatorSet< ALLOC >*         set = allocator.allocate(1);
      try {
        allocator.construct(set, *this, alloc);
      } catch (...) {
        allocator.deallocate(set, 1);
        throw;
      }
      return set;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >* DBTranslatorSet< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >::~DBTranslatorSet() {
      clear();
      GUM_DESTRUCTOR(DBTranslatorSet);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslatorSet< ALLOC >&
       DBTranslatorSet< ALLOC >::operator=(const DBTranslatorSet< ALLOC >& from) {
      if (this != &from) {
        clear();
        _copy_(from, this->getAllocator());
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE DBTranslatorSet< ALLOC >&
       DBTranslatorSet< ALLOC >::operator=(DBTranslatorSet< ALLOC >&& from) {
      if (this != &from) {
        clear();
        _translators_    = std::move(from._translators_);
        _columns_        = std::move(from._columns_);
        _highest_column_ = from._highest_column_;
      }

      return *this;
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >& DBTranslatorSet< ALLOC >::operator[](const std::size_t k) {
      return *(_translators_[k]);
    }


    /// returns the ith translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
       DBTranslatorSet< ALLOC >::operator[](const std::size_t k) const {
      return *(_translators_[k]);
    }


    /// inserts a new translator at the end of the translator set
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class Translator >
    std::size_t DBTranslatorSet< ALLOC >::insertTranslator(const Translator< ALLOC >& translator,
                                                           const std::size_t          column,
                                                           const bool unique_column) {
      // if the unique_column parameter is set to true and there exists already
      // another translator that parses the column, raise a DuplicateElement
      // exception
      const std::size_t size = _translators_.size();
      if (unique_column) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          if (_columns_[i] == column)
            GUM_ERROR(DuplicateElement,
                      "There already exists a DBTranslator that parses Column" << column)
        }
      }

      // reserve some place for the new translator
      _translators_.reserve(size + 1);
      _columns_.reserve(size + 1);

      // create and add the new translator
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      DBTranslator< ALLOC >*         new_translator = translator.clone(allocator);

      _translators_.resize(size + 1);
      _columns_.resize(size + 1);
      _translators_[size] = new_translator;
      _columns_[size]     = column;

      // update the highest column
      if (column > _highest_column_) _highest_column_ = column;

      return size;
    }


    /// inserts a new translator for a given variable in the translator set
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    std::size_t DBTranslatorSet< ALLOC >::insertTranslator(
       const Variable&                                          var,
       const std::size_t                                        column,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               unique_column) {
      // create the translator, depending on the type of the variable
      switch (var.varType()) {
        case VarType::Labelized: {
          const LabelizedVariable& xvar = static_cast< const LabelizedVariable& >(var);
          DBTranslator4LabelizedVariable< ALLOC > translator(xvar, missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Discretized: {
          const IDiscretizedVariable& xvar = static_cast< const IDiscretizedVariable& >(var);
          DBTranslator4DiscretizedVariable< ALLOC > translator(xvar, missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Range: {
          const RangeVariable&                xvar = static_cast< const RangeVariable& >(var);
          DBTranslator4RangeVariable< ALLOC > translator(xvar, missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        case VarType::Continuous: {
          const IContinuousVariable& xvar = static_cast< const IContinuousVariable& >(var);
          DBTranslator4ContinuousVariable< ALLOC > translator(xvar, missing_symbols);
          return insertTranslator(translator, column, unique_column);
        }

        default:
          GUM_ERROR(NotImplementedYet,
                    "The insertion of the translator for Variable "
                       << var.name()
                       << " is impossible because a translator "
                          "for such variable is not implemented yet")
      }
    }


    /// inserts a new translator for a given variable in the translator set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::insertTranslator(const Variable&   var,
                                                                  const std::size_t column,
                                                                  const bool        unique_column) {
      const std::vector< std::string, ALLOC< std::string > > missing;
      return this->insertTranslator(var, column, missing, unique_column);
    }


    /// erase the kth translator
    template < template < typename > class ALLOC >
    void DBTranslatorSet< ALLOC >::eraseTranslator(const std::size_t k, const bool k_is_input_col) {
      ALLOC< DBTranslator< ALLOC > > allocator(this->getAllocator());
      const std::size_t              nb_trans = _translators_.size();

      if (!k_is_input_col) {
        if (nb_trans < k) return;

        // remove the translator and its corresponding column
        allocator.destroy(_translators_[k]);
        allocator.deallocate(_translators_[k], 1);

        const std::size_t colk = _columns_[k];
        _translators_.erase(_translators_.begin() + k);
        _columns_.erase(_columns_.begin() + k);

        // if the highest column index corresponded to the kth translator,
        // we must recomput it
        if (_highest_column_ == colk) {
          _highest_column_ = std::size_t(0);
          for (const auto col: _columns_)
            if (_highest_column_ < col) _highest_column_ = col;
        }
      } else {
        // remove all the translators parsing the kth column
        auto iter_trans       = _translators_.rbegin();
        bool translator_found = false;
        for (auto iter_col = _columns_.rbegin(); iter_col != _columns_.rend();
             ++iter_col, ++iter_trans) {
          if (*iter_col == k) {
            // remove the translator and its corresponding column
            allocator.destroy(*iter_trans);
            allocator.deallocate(*iter_trans, 1);

            _translators_.erase((iter_trans + 1).base());
            _columns_.erase((iter_col + 1).base());
            translator_found = true;
          }
        }

        // if the highest column index corresponded to one of the translators
        // removed, we must recompute it
        if (translator_found && (k == _highest_column_)) {
          _highest_column_ = std::size_t(0);
          for (const auto col: _columns_)
            if (_highest_column_ < col) _highest_column_ = col;
        }
      }
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translate(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      return _translators_[k]->translate(row[_columns_[k]]);
    }


    /// ask the kth translator to translate a string in a row of the database
    template < template < typename > class ALLOC >
    template < template < typename > class OTHER_ALLOC >
    INLINE DBTranslatedValue DBTranslatorSet< ALLOC >::translateSafe(
       const std::vector< std::string, OTHER_ALLOC< std::string > >& row,
       const std::size_t                                             k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << " could not be found")

      return _translators_[k]->translate(row[_columns_[k]]);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string
           DBTranslatorSet< ALLOC >::translateBack(const DBTranslatedValue translated_val,
                                               const std::size_t       k) const {
      return _translators_[k]->translateBack(translated_val);
    }


    /// returns the original string that was translated into translated_val
    template < template < typename > class ALLOC >
    INLINE std::string
           DBTranslatorSet< ALLOC >::translateBackSafe(const DBTranslatedValue translated_val,
                                                   const std::size_t       k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return _translators_[k]->translateBack(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValue(const DBTranslatedValue translated_val,
                                                         const std::size_t       k) const {
      return _translators_[k]->isMissingValue(translated_val);
    }


    // indicates whether the kth translator considers a translated_val
    // as a missing value
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::isMissingValueSafe(const DBTranslatedValue translated_val,
                                                             const std::size_t       k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return _translators_[k]->isMissingValue(translated_val);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >& DBTranslatorSet< ALLOC >::translator(const std::size_t k) {
      return *(_translators_[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
                 DBTranslatorSet< ALLOC >::translator(const std::size_t k) const {
      return *(_translators_[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE DBTranslator< ALLOC >& DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(_translators_[k]);
    }


    /// returns the kth translator
    template < template < typename > class ALLOC >
    INLINE const DBTranslator< ALLOC >&
                 DBTranslatorSet< ALLOC >::translatorSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Translator #" << k << "could not be found")

      return *(_translators_[k]);
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::domainSize(const std::size_t k) const {
      return _translators_[k]->domainSize();
    }


    /// returns the domain size of the variables stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::domainSizeSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->domainSize();
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable& DBTranslatorSet< ALLOC >::variable(const std::size_t k) const {
      return *(_translators_[k]->variable());
    }


    /// returns the variable stored into the kth translator
    template < template < typename > class ALLOC >
    INLINE const Variable& DBTranslatorSet< ALLOC >::variableSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return *(_translators_[k]->variable());
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::needsReordering(const std::size_t k) const {
      return _translators_[k]->needsReordering();
    }


    // indicates whether a reordering is needed to make the kth translator
    // sorted by lexicographical order
    template < template < typename > class ALLOC >
    INLINE bool DBTranslatorSet< ALLOC >::needsReorderingSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->needsReordering();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t, std::size_t, ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorder(const std::size_t k) {
      return _translators_[k]->reorder();
    }


    // performs a reordering of the dictionary and returns a mapping
    // from the old translated values to the new ones.
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t, std::size_t, ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslatorSet< ALLOC >::reorderSafe(const std::size_t k) {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Variable #" << k << "could not be found")

      return _translators_[k]->reorder();
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::inputColumn(const std::size_t k) const {
      return _columns_[k];
    }


    /** @brief returns the column of the input database that will be written
     * in the kth column of the DatabaseTable */
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::inputColumnSafe(const std::size_t k) const {
      if (_translators_.size() <= k)
        GUM_ERROR(UndefinedElement, "Column #" << k << "could not be found")

      return _columns_[k];
    }


    /// returns the largest input database column index read by the translators
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::highestInputColumn() const {
      return _highest_column_;
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::nbTranslators() const {
      return _columns_.size();
    }


    /// returns the number of translators stored into the set
    template < template < typename > class ALLOC >
    INLINE std::size_t DBTranslatorSet< ALLOC >::size() const {
      return _columns_.size();
    }

    /// returns the set of translators
    template < template < typename > class ALLOC >
    INLINE const std::vector< DBTranslator< ALLOC >*, ALLOC< DBTranslator< ALLOC >* > >&
                 DBTranslatorSet< ALLOC >::translators() const {
      return _translators_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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
 * @brief The implementation of tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/database/databaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DatabaseTable< ALLOC >::DatabaseTable(
       const typename DatabaseTable< ALLOC >::template MissingValType< XALLOC >& missing_symbols,
       const DBTranslatorSet< ALLOC >&                                           translators,
       const typename DatabaseTable< ALLOC >::allocator_type&                    alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(
           missing_symbols,
           std::vector< std::string, ALLOC< std::string > >(),
           alloc),
        _translators_(translators, alloc) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string, ALLOC< std::string > > var_names(translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size; ++i) {
          var_names[i] = _translators_.translator(i).variable()->name();
        }
        setVariableNames(var_names, false);
      }

      GUM_CONSTRUCTOR(DatabaseTable);
    }


    // default constructor
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >::DatabaseTable(
       const DBTranslatorSet< ALLOC >&                        translators,
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(
           std::vector< std::string, ALLOC< std::string > >(),
           std::vector< std::string, ALLOC< std::string > >(),
           alloc),
        _translators_(translators, alloc) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string, ALLOC< std::string > > var_names(translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size; ++i) {
          var_names[i] = _translators_.translator(i).variable()->name();
        }
        setVariableNames(var_names, false);
      }

      GUM_CONSTRUCTOR(DatabaseTable);
    }


    // copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::DatabaseTable(
       const DatabaseTable< ALLOC >&                          from,
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(from, alloc),
        _translators_(from._translators_, alloc), _ignored_cols_(from._ignored_cols_) {
      GUM_CONS_CPY(DatabaseTable);
    }


    // copy constructor
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::DatabaseTable(const DatabaseTable< ALLOC >& from) :
        DatabaseTable< ALLOC >(from, from.getAllocator()) {}


    // move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::DatabaseTable(
       DatabaseTable< ALLOC >&&                               from,
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(std::move(from), alloc),
        _translators_(std::move(from._translators_), alloc),
        _ignored_cols_(std::move(from._ignored_cols_)) {
      GUM_CONS_MOV(DatabaseTable);
    }


    // move constructor
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::DatabaseTable(DatabaseTable< ALLOC >&& from) :
        DatabaseTable< ALLOC >(std::move(from), from.getAllocator()) {}


    // virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >* DatabaseTable< ALLOC >::clone(
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) const {
      ALLOC< DatabaseTable< ALLOC > > allocator(alloc);
      DatabaseTable< ALLOC >*         new_db = allocator.allocate(1);
      try {
        allocator.construct(new_db, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_db, 1);
        throw;
      }

      return new_db;
    }


    // virtual copy constructor
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >* DatabaseTable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    // destructor
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::~DatabaseTable() {
      GUM_DESTRUCTOR(DatabaseTable);
    }


    // copy operator
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >& DatabaseTable< ALLOC >::operator=(const DatabaseTable< ALLOC >& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue, ALLOC >::operator=(from);
        _translators_                                       = from._translators_;
        _ignored_cols_                                      = from._ignored_cols_;
      }

      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >& DatabaseTable< ALLOC >::operator=(DatabaseTable< ALLOC >&& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue, ALLOC >::operator=(std::move(from));
        _translators_                                       = std::move(from._translators_);
        _ignored_cols_                                      = std::move(from._ignored_cols_);
      }

      return *this;
    }


    // a method to process the rows of the database in multithreading
    template < template < typename > class ALLOC >
    template < typename Functor1, typename Functor2 >
    void DatabaseTable< ALLOC >::_threadProcessDatabase_(Functor1& exec_func, Functor2& undo_func) {
      // compute the number of threads to execute the code
      const std::size_t nb_threads = this->nbProcessingThreads_();

      // if there is just one thread, let it process all the rows
      if (nb_threads == 1) {
        const std::size_t db_size = this->nbRows();
        exec_func(std::size_t(0), db_size, 0);
        return;
      }

      // if there are multiple threads, compute the ranges of rows they should process
      const std::vector< std::pair< std::size_t, std::size_t > > ranges =
         this->rangesProcessingThreads_(nb_threads);

      // here, we shall create the threads, but also one std::exception_ptr
      // for each thread. This will allow us to catch the exception raised
      // by the threads
      std::vector< std::thread > threads;
      threads.reserve(nb_threads);
      std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

      // create a lambda that will execute exec_func while catching its exceptions
      auto real_exec_func
         = [&exec_func](std::size_t begin,
                        std::size_t end,
                        std::size_t index,
                        std::exception_ptr& exc) -> void {
        try {
          exec_func(begin, end, index);
        } catch (...) { exc = std::current_exception(); }
      };

      // launch the threads
      for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
        threads.push_back(std::thread(std::ref(real_exec_func),
                                      ranges[i].first,
                                      ranges[i].second,
                                      i,
                                      std::ref(func_exceptions[i])));
      }

      // wait for the threads to complete their executions
      std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

      // now, check if one exception has been raised
      bool exception_raised = false;
      for (const auto& exc: func_exceptions) {
        if (exc != nullptr) {
          exception_raised = true;
          break;
        }
      }

      if (exception_raised) {
        // create a lambda that will execute undo_func while catching
        // its exceptions
        auto real_undo_func
           = [&undo_func](std::size_t begin,
                          std::size_t end,
                          std::size_t index,
                          std::exception_ptr& exc) -> void {
          try {
            undo_func(begin, end, index);
          } catch (...) { exc = std::current_exception(); }
        };

        // launch the repair threads
        threads.clear();
        std::vector< std::exception_ptr > undo_func_exceptions(nb_threads, nullptr);
        for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
          // we just need to repair the threads that did not raise exceptions
          if (func_exceptions[i] == nullptr)
            threads.push_back(std::thread(std::ref(real_undo_func),
                                          ranges[i].first,
                                          ranges[i].second,
                                          i,
                                          std::ref(undo_func_exceptions[i])));
        }

        // wait for the threads to complete their executions
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        // rethrow the exception
        for (const auto& exc: func_exceptions) {
          if (exc != nullptr) { std::rethrow_exception(exc); }
        }
      }
    }


    /// insert a new translator into the database
    template < template < typename > class ALLOC >
    std::size_t DatabaseTable< ALLOC >::insertTranslator(const DBTranslator< ALLOC >& translator,
                                                         const std::size_t            input_column,
                                                         const bool unique_column) {
      // check that there is no ignored_column corresponding to column
      if (_ignored_cols_.exists(input_column))
        GUM_ERROR(OperationNotAllowed,
                  "Column " << input_column << " is marked as being ignored. "
                            << "So it is forbidden to create a translator for that column.")

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end,
                                             std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin,
                                    std::size_t end,
                                    std::size_t index) -> void {
      };

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos
         = _translators_.insertTranslator(translator, input_column, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->variable_names_.push_back(translator.variable()->name());
      } catch (...) {
        _translators_.eraseTranslator(pos);
        throw;
      }

      // if the databaseTable is not empty, fill the column of the database
      // corresponding to the translator with missing values
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        const DBTranslatedValue missing = _translators_[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda = [this, missing](std::size_t begin,
                                           std::size_t end,
                                           std::size_t index) -> void {
          std::size_t i = begin;
          try {
            for (; i < end; ++i) {
              this->rows_[i].row().push_back(missing);
              this->has_row_missing_val_[i] = IsMissing::True;
            }
          } catch (...) {
            for (std::size_t j = begin; j < i; ++j)
              this->rows_[i].row().pop_back();
            throw;
          }
        };

        auto undo_fill_lambda = [this](std::size_t begin,
                                       std::size_t end,
                                       std::size_t index) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->rows_[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }


    /// insert a new translator into the database
    template < template < typename > class ALLOC >
    std::size_t DatabaseTable< ALLOC >::insertTranslator(const Variable&   var,
                                                         const std::size_t input_column,
                                                         const bool        unique_column) {
      // check that there is no ignored_column corresponding to column
      if (_ignored_cols_.exists(input_column))
        GUM_ERROR(OperationNotAllowed,
                  "Column " << input_column << " is marked as being ignored. "
                            << "So it is forbidden to create a translator for that column.")

      // if the databaseTable is not empty, we should fill the column of the
      // database corresponding to the new translator with missing values. But, the
      // current method assumes that the list of missing values is empty. Hence, it
      // should raise an exception
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        GUM_ERROR(MissingValueInDatabase,
                  "inserting a new translator into a database creates a new column "
                     << "with missing values. However, you did not define any symbol for "
                     << "such values.")
      }

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end,
                                             std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin,
                                    std::size_t end,
                                    std::size_t index) -> void {
      };

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos = _translators_.insertTranslator(var, input_column, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->variable_names_.push_back(var.name());
      } catch (...) {
        _translators_.eraseTranslator(pos);
        throw;
      }

      return pos;
    }


    /// insert a new translator into the database
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    std::size_t DatabaseTable< ALLOC >::insertTranslator(
       const Variable&                                   var,
       const std::size_t                                 input_column,
       std::vector< std::string, XALLOC< std::string > > missing_symbols,
       const bool                                        unique_column) {
      // check that there is no ignored_column corresponding to column
      if (_ignored_cols_.exists(input_column))
        GUM_ERROR(OperationNotAllowed,
                  "Column " << input_column << " is marked as being ignored. "
                            << "So it is forbidden to create a translator for that column.")

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end,
                                             std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin,
                                    std::size_t end,
                                    std::size_t index) -> void {
      };

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos
         = _translators_.insertTranslator(var, input_column, missing_symbols, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->variable_names_.push_back(var.name());
      } catch (...) {
        _translators_.eraseTranslator(pos);
        throw;
      }

      // if the databaseTable is not empty, fill the column of the database
      // corresponding to the translator with missing values
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        const DBTranslatedValue missing = _translators_[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda = [this, missing](std::size_t begin,
                                           std::size_t end,
                                           std::size_t index) -> void {
          std::size_t i = begin;
          try {
            for (; i < end; ++i) {
              this->rows_[i].row().push_back(missing);
              this->has_row_missing_val_[i] = IsMissing::True;
            }
          } catch (...) {
            for (std::size_t j = begin; j < i; ++j)
              this->rows_[i].row().pop_back();
            throw;
          }
        };

        auto undo_fill_lambda = [this](std::size_t begin,
                                       std::size_t end,
                                       std::size_t index) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->rows_[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }


    /** @brief returns the indices corresponding either to the kth translator
     * or to all those that parse the kth column of the input dataset
     *
     * @warning the indices are sorted by deacreasing order */
    template < template < typename > class ALLOC >
    INLINE typename DatabaseTable< ALLOC >::template DBVector< std::size_t >
       DatabaseTable< ALLOC >::_getKthIndices_(const std::size_t k,
                                               const bool        k_is_input_col) const {
      const std::size_t nb_trans = _translators_.size();
      if (!k_is_input_col) {
        if (k < nb_trans)
          return DBVector< std::size_t >{k};
        else
          return DBVector< std::size_t >();
      } else {
        DBVector< std::size_t > trans;
        for (std::size_t i = std::size_t(0), kk = nb_trans - 1; i < nb_trans; ++i, --kk) {
          if (_translators_.inputColumn(kk) == k) trans.push_back(kk);
        }
        return trans;
      }
    }


    // erases the kth translator or all those parsing the kth column of
    // the input dataset
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::eraseTranslators(const std::size_t k, const bool k_is_input_col) {
      for (const auto kk: _getKthIndices_(k, k_is_input_col)) {
        // erase the translator of index kk and the corresponding variable
        // name. If there remains no more translator in the translator set,
        // rows_ should become empty
        this->variable_names_.erase(this->variable_names_.begin() + kk);
        if (this->variable_names_.empty()) {
          IDatabaseTable< DBTranslatedValue, ALLOC >::eraseAllRows();
        } else {
          const std::size_t nb_trans = _translators_.size();

          auto erase_lambda = [this, nb_trans, kk](std::size_t begin,
                                                   std::size_t end,
                                                   std::size_t index) -> void {
            for (std::size_t i = begin; i < end; ++i) {
              auto& row = this->rows_[i].row();
              if (this->_translators_.isMissingValue(row[kk], kk)) {
                bool has_missing_val = false;
                for (std::size_t j = std::size_t(0); j < nb_trans; ++j) {
                  if ((j != kk) && this->_translators_.isMissingValue(row[j], j)) {
                    has_missing_val = true;
                    break;
                  }
                }
                if (!has_missing_val) this->has_row_missing_val_[i] = IsMissing::False;
              }
              row.erase(row.begin() + kk);
            }
          };

          auto undo_erase_lambda = [](std::size_t begin,
                                      std::size_t end,
                                      std::size_t index) -> void {
          };

          // launch the threads executing the lambdas
          this->_threadProcessDatabase_(erase_lambda, undo_erase_lambda);
        }
        _translators_.eraseTranslator(kk);
      }
    }


    /// change the translator of a database column
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::changeTranslator(const DBTranslator< ALLOC >& new_translator,
                                                  const std::size_t            k,
                                                  const bool                   k_is_input_col) {
      // get the index of the column in the database. If it is not found, indicate that
      // the substitution is impossible
      const auto db_k = _getKthIndices_(k, k_is_input_col);
      if (db_k >= _translators_.size()) {
        GUM_ERROR(OutOfBounds,
                  "the translator at position " << k << '/' << db_k <<
                  "cannot be found.");
      }

      // if the dataset does not contain any data, we can safely substitute the old translator
      // by the new one
      if (this->empty()) {
        // keep into account the name of the new translator
        this->variable_names_[db_k] = new_translator.variable()->name();

        // substitute int the stransltor's set the old translator by the new one
        _translators_.substituteTranslator(new_translator, db_k);

        return;
      }

      // get the translator and check that it is not lossy: as, here, there are some data,
      // we cannot always ensure that there won't be some loss of information substituting
      // one translator by another
      const DBTranslator< ALLOC >& old_translator = _translators_[db_k];
      if (!old_translator.isLossless()) {
        // for the moment, we consider that it is impossible to substitute lossy translators
        // because we may have already lost information that are necessary for the new
        // translator
        GUM_ERROR(OperationNotAllowed,
                  "Lossy translators cannot yet be substituted by other translators");
      }

      const std::size_t  nb_threads = this->nbProcessingThreads_();

      // how missing values will be translated
      std::pair<DBTranslatedValue, DBTranslatedValue>
         miss_mapping(old_translator.missingValue(), new_translator.missingValue());

      // Now, we should compute the mapping from the values and missing symbols of the old
      // translator to those of the new one.

      // When the database already contains some data, we must ensure that we will be able to
      // substitute the old translator by the new one without loosing any information. Possible
      // loss of information may occur in the following cases:
      // 1/ if the set of missing symbols of the old translator is not a singleton and some of its
      //    missing symbols do not belong to the set of missing symbols of the new translator.
      //    In this case, the translation of this symbol by the new translator should either raise
      //    an exception because the new translator does not know how to handle it, or should
      //    produce a DBTranslatedValue if the new translator thinks this is an observed value.
      //    Now, the problem is that when observing a missing symbol in the database, we have no
      //    way to determine to which above case this should correspond. Hence the substitution
      //    cannot be made unambiguously.
      // 2/ if the set of (non-missing) values of the old translator is not included in the one
      //    of the new translator
      // If one of these cases occur, before performing the translation, we must parse the content
      // of the database: if case 1/ obtains and if the database contains some missing symbols,
      // then we cannot unambiguously substitute the old translator by the new one, hence an error.
      // If case 2/ obtains, we must check that all the observed values currently stored into the
      // database also belong to the set of values the new translator is capable of translating.
      if (!this->empty()) {
        // to test case 1, we first determine whether the dataset contains some
        // missing values
        bool has_missing_value = false;
        {
          std::vector< int > missing_values(nb_threads, 0);

          // a lambda to parse all the translated values for missing symbols
          auto get_lambda = [this, db_k, &missing_values](std::size_t begin,
                                                          std::size_t end,
                                                          std::size_t index) -> void {
            for (std::size_t i = begin; i < end; ++i) {
              auto& row = this->rows_[i].row();
              if (this->_translators_.isMissingValue(row[db_k], db_k)) {
                missing_values[index] = 1;
                return;
              }
            }
          };

          auto undo_get_lambda = [](std::size_t begin, std::size_t end, std::size_t index) -> void {
          };

          // launch the threads executing the lambdas
          this->_threadProcessDatabase_(get_lambda, undo_get_lambda);

          // if has_missing_values has at least one value 1, there are missing values
          for (const auto x: missing_values) {
            if (x) {
              has_missing_value = true;
              break;
            }
          }
        }

        // test for case 1/
        const auto old_missing_symbols      = old_translator.missingSymbols();
        const auto new_missing_symbols      = new_translator.missingSymbols();
        const bool multiple_missing_symbols = old_missing_symbols.size() > 1;
        const bool old_miss_included        = old_missing_symbols.isSubsetOrEqual(new_missing_symbols);
        if (has_missing_value && multiple_missing_symbols && !old_miss_included) {
          // here, we know that the the database contains missing values
          // and we cannot unambiguously perform the translator's substitution
          GUM_ERROR(OperationNotAllowed,
                    "it is impossible to substitute the translator because "
                    "the database contains some missing values that cannot be "
                    "substituted unambiguously");
        }

        // if the database contains some missing values, two cases can obtain:
        // a/ old_miss_included is true, in which case all the old missing values
        //    will be translated as missing values in the new translator.
        //    In this case, there is no translation problem.
        // b/ old_miss_included is false. In this case, we know that there is only
        //    one old missing symbol, which is not inluded in the set of missing
        //    symbols of the new translator. If we can translate its symbol as a
        //    "proper" value in the new translator, that's ok, otherwise we cannot
        //    perform the substitution.
        if (has_missing_value && !old_miss_included) {
          try {
            new_translator.translate(*(old_translator.missingSymbols().begin()));
          }
          catch (Exception&) {
            GUM_ERROR(OperationNotAllowed,
                      "it is impossible to substitute the translator because "
                      "the database contains some missing values that cannot be "
                      "substituted");
          }
        }

        // compute the mapping of the missing symbol if this one does not corresponds
        // to a missing value in the new translator
        if (has_missing_value && !old_miss_included) {
            miss_mapping.second =
             new_translator.translate(*(old_translator.missingSymbols().begin()));
        }

        // test for case 2/ (if the set of (non-missing) values of the old translator is
        // not included in the one of the new translator)

        // now, parse the database and check that all the values contained in the
        // database can be translated
        std::vector< int > unmapped(nb_threads, 0);

        // a lambda to parse all the translated values
        auto check_lambda = [this, db_k, old_translator, new_translator,
                             &unmapped](std::size_t begin,
                                        std::size_t end, std::size_t index) -> void {
          const DBTranslatedValue old_miss = old_translator.missingValue();
          for (std::size_t i = begin; i < end; ++i) {
            auto& row = this->rows_[i].row();
            if (row[db_k] != old_miss) {
              try {
                new_translator.translate(old_translator.translateBack(row[db_k]));
              } catch (Exception&) {
                // ok, here, the translation is impossible
                unmapped[index] = 1;
                return;
              }
            }
          }
        };

        auto undo_check_lambda = [](std::size_t begin,
                                    std::size_t end,
                                    std::size_t index) -> void {};

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(check_lambda, undo_check_lambda);

        // if unmapped has at least one value 1, there are values that we don't know how to translate
        for (const auto x: unmapped) {
          if (x) {
            GUM_ERROR(OperationNotAllowed,
                      "The database contains some values that cannot be translated "
                      "using the new translator");
          }
        }
      }

      // here, we know that we can perform the translator's substitution, so
      // let's do it
      auto change_lambda = [this, db_k, old_translator, new_translator,
                            miss_mapping](std::size_t begin,
                                          std::size_t end,
                                          std::size_t index) -> void {
        const DBTranslatedValue old_miss = old_translator.missingValue();
        for (std::size_t i = begin; i < end; ++i) {
          auto& row = this->rows_[i].row();
          if (row[db_k] == old_miss) {
            row[db_k] = miss_mapping.second;
          } else {
            new_translator.translate(old_translator.translateBack(row[db_k]));
          }
        }
      };

      auto undo_change_lambda = [](std::size_t begin,
                                   std::size_t end,
                                   std::size_t index) -> void {};

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(change_lambda, undo_change_lambda);

      // keep into account the name of the new translator
      this->variable_names_[db_k] = new_translator.variable()->name();

      // substitute int the stransltor's set the old translator by the new one
      _translators_.substituteTranslator(new_translator, db_k);
    }








    /// change the translator of a database column
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::changeTranslator(const Variable&   var,
                                                  const std::size_t k,
                                                  const bool        k_is_input_col) {

    }










    /// returns the set of translators
    template < template < typename > class ALLOC >
    INLINE const DBTranslatorSet< ALLOC >& DatabaseTable< ALLOC >::translatorSet() const {
      return _translators_;
    }


    /** @brief returns the index corresponding either to the kth translator or
     * to that of the first translator parsing the kth column of the
     * input dataset */
    template < template < typename > class ALLOC >
    INLINE std::size_t DatabaseTable< ALLOC >::_getKthIndex_(const std::size_t k,
                                                             const bool k_is_input_col) const {
      if (k_is_input_col) {
        const std::size_t nb_trans = _translators_.size();
        for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
          if (_translators_.inputColumn(i) == k) { return i; }
        }
        return nb_trans + 1;
      } else {
        return k;
      }
    }


    /// returns the kth translator of the database
    template < template < typename > class ALLOC >
    const DBTranslator< ALLOC >&
       DatabaseTable< ALLOC >::translator(const std::size_t k, const bool k_is_input_col) const {
      // find the position of the translator that we look for. This
      // is variable kk below
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " translators, so Translator #" << k
                                        << " does not exist")
        }
      }

      return _translators_.translator(kk);
    }


    /// returns the kth variable of the database
    template < template < typename > class ALLOC >
    const Variable& DatabaseTable< ALLOC >::variable(const std::size_t k,
                                                     const bool        k_is_input_col) const {
      // find the position of the translator that contains the variable.
      // This is variable kk below
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that "
                       << "corresponds to Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #" << k
                                        << " does not exist")
        }
      }

      return _translators_.variable(kk);
    }


    /// sets the names of the variables
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::setVariableNames(
       const std::vector< std::string, ALLOC< std::string > >& names,
       const bool                                              from_external_object) {
      const std::size_t size     = names.size();
      const std::size_t nb_trans = _translators_.size();
      if (!from_external_object) {
        if (nb_trans != size) {
          GUM_ERROR(SizeError,
                    "the number of variable's names (i.e., "
                       << size << ") does not correspond to the number of columns of the "
                       << "database table (i.e.," << nb_trans << ")")
        }

        // update the translator names
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          _translators_.translator(i).setVariableName(names[i]);
        }
      } else {
        if (nb_trans && (_translators_.highestInputColumn() >= size)) {
          GUM_ERROR(SizeError,
                    "the names vector has "
                       << size << " elements whereas it should have at least "
                       << (_translators_.highestInputColumn() + 1)
                       << "elements so that each translator is assigned a name")
        }

        // update the translator names
        for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
          _translators_.translator(i).setVariableName(names[_translators_.inputColumn(i)]);
        }
      }

      // update variable_names_ using the newly assigned translators names
      this->variable_names_.resize(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        this->variable_names_[i] = _translators_.variable(i).name();
      }
    }


    /** @brief indicates that we should ignore the kth column of the original
     * database when inserting new rows */
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::ignoreColumn(const std::size_t k, const bool k_is_input_col) {
      // indicate that the column will be forbidden. If the column is already
      // forbidden, do nothing. But if the column is assigned to a translator
      // that does not exist, raise an UndefinedElement exception
      const std::size_t nb_trans = _translators_.size();
      if (k_is_input_col) {
        if (_ignored_cols_.exists(k)) return;
        _ignored_cols_.insert(k);
      } else {
        if (k < nb_trans) {
          _ignored_cols_.insert(_translators_.inputColumn(k));
        } else {
          GUM_ERROR(UndefinedElement,
                    "It is impossible to ignore the column parsed by Translator #"
                       << k << "because there exist only " << nb_trans << " translators")
        }
      }

      // remove all the translators corresponding to k
      eraseTranslators(k, k_is_input_col);
    }


    /// returns  the set of ignored columns
    template < template < typename > class ALLOC >
    const typename DatabaseTable< ALLOC >::template DBVector< std::size_t >
       DatabaseTable< ALLOC >::ignoredColumns() const {
      const std::size_t nb_trans = _translators_.size();

      if (nb_trans == std::size_t(0)) { return DBVector< std::size_t >{std::size_t(0)}; }

      // get the columns handled by the translators, sorted by increasing order
      DBVector< std::size_t > cols(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i)
        cols[i] = _translators_.inputColumn(i);
      std::sort(cols.begin(), cols.end());

      // create a vector with all the possible input columns
      const std::size_t       highest = _translators_.highestInputColumn() + 1;
      DBVector< std::size_t > ignored_cols(highest);
      std::iota(ignored_cols.begin(), ignored_cols.end(), 0);

      // remove from ignored_cols the elements of cols
      for (std::size_t i = std::size_t(0), ii = highest - 1, k = std::size_t(0), kk = nb_trans - 1;
           i < highest;
           ++i, --ii) {
        if (cols[kk] == ii) {
          ignored_cols.erase(ignored_cols.begin() + ii);
          while ((k < nb_trans) && (cols[kk] == ii)) {
            --kk;
            ++k;
          }
          if (k == nb_trans) break;
        }
      }

      // add the column past the last translator
      ignored_cols.push_back(highest);

      return ignored_cols;
    }


    /// returns the set of columns parsed
    template < template < typename > class ALLOC >
    const typename DatabaseTable< ALLOC >::template DBVector< std::size_t >
       DatabaseTable< ALLOC >::inputColumns() const {
      const std::size_t nb_trans = _translators_.size();
      if (nb_trans == std::size_t(0)) { return DBVector< std::size_t >(); }

      DBVector< std::size_t > input_cols(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        input_cols[i] = _translators_.inputColumn(i);
      }
      return input_cols;
    }


    /// returns the domain size of the kth variable
    template < template < typename > class ALLOC >
    std::size_t DatabaseTable< ALLOC >::domainSize(const std::size_t k,
                                                   const bool        k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that "
                       << "corresponds to Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #" << k
                                        << " does not exist")
        }
      }

      return _translators_.domainSize(kk);
    }


    /// returns the domain sizes of all the variables in the database table
    template < template < typename > class ALLOC >
    INLINE std::vector< std::size_t, ALLOC< std::size_t > >
           DatabaseTable< ALLOC >::domainSizes() const {
      const std::size_t          nb_trans = _translators_.size();
      std::vector< std::size_t > dom(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        dom[i] = _translators_.domainSize(i);
      }
      return dom;
    }


    // indicates whether a reordering is needed to make the kth
    // translator sorted by lexicographical order
    template < template < typename > class ALLOC >
    bool DatabaseTable< ALLOC >::needsReordering(const std::size_t k,
                                                 const bool        k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " translators, so Translator #" << k
                                        << " does not exist")
        }
      }

      return _translators_.needsReordering(kk);
    }


    // performs a reordering of the kth translator or of the first
    // translator corresponding to the kth column of the input database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::reorder(const std::size_t k, const bool k_is_input_col) {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " translators, so Translator #" << k
                                        << " does not exist")
        }
      }

      // if the translator is not designed for a discrete variable, there
      // is no reordering to apply
      if (_translators_.translator(kk).getValType() != DBTranslatedValueType::DISCRETE) return;

      // get the translation to perform
      auto updates = _translators_.reorder(kk);
      if (updates.empty()) return;

      std::size_t                                      size = updates.size();
      std::vector< std::size_t, ALLOC< std::size_t > > new_values(size);
      for (const auto& update: updates) {
        if (update.first >= size) {
          size = update.first + 1;
          new_values.resize(size);
        }
        new_values[update.first] = update.second;
      }

      // apply the translations
      auto newtrans_lambda = [this, kk, &new_values](std::size_t begin,
                                                     std::size_t end,
                                                     std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i) {
          auto& elt = this->rows_[i][kk].discr_val;
          if (elt != std::numeric_limits< std::size_t >::max()) elt = new_values[elt];
        }
      };

      auto undo_newtrans_lambda = [](std::size_t begin,
                                     std::size_t end,
                                     std::size_t index) -> void {
      };

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(newtrans_lambda, undo_newtrans_lambda);
    }


    /// performs a reordering of all the columns
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::reorder() {
      const std::size_t nb_trans = _translators_.size();
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i)
        reorder(i, false);
    }


    /// insert a new row at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRow(
       const std::vector< std::string, ALLOC< std::string > >& new_row) {
      // check that the row can be fully translated, i.e., it contains enough
      // columns to be translated
      const std::size_t row_size = new_row.size();
      if (row_size == std::size_t(0)) return;

      if (_translators_.highestInputColumn() >= row_size) {
        GUM_ERROR(SizeError,
                  "the new row has " << row_size
                                     << " columns whereas the database requires at least "
                                     << (_translators_.highestInputColumn() + 1) << " columns")
      }

      // convert the new_row into a row of DBTranslatedValue
      const std::size_t        nb_trans = _translators_.size();
      Row< DBTranslatedValue > dbrow;
      dbrow.reserve(nb_trans);
      bool has_missing_val = false;
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        const DBTranslatedValue new_val(_translators_.translate(new_row, i));
        if (_translators_.isMissingValue(new_val, i)) has_missing_val = true;
        dbrow.pushBack(std::move(new_val));
      }

      this->insertRow(std::move(dbrow), has_missing_val ? IsMissing::True : IsMissing::False);
    }


    /** @brief check that a row's values are compatible with those of the
     * translators' variables */
    template < template < typename > class ALLOC >
    bool DatabaseTable< ALLOC >::_isRowCompatible_(
       const typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >& row) const {
      // check that the size of the row corresponds to that of the translators
      const std::size_t row_size = row.size();
      if (row_size != _translators_.size()) return false;

      const auto& translators = _translators_.translators();
      for (std::size_t i = std::size_t(0); i < row_size; ++i) {
        switch (translators[i]->getValType()) {
          case DBTranslatedValueType::DISCRETE:
            if ((row[i].discr_val >= translators[i]->domainSize())
                && (row[i].discr_val != std::numeric_limits< std::size_t >::max()))
              return false;
            break;

          case DBTranslatedValueType::CONTINUOUS: {
            const IContinuousVariable& var
               = static_cast< const IContinuousVariable& >(*(translators[i]->variable()));
            if (((var.lowerBoundAsDouble() > (double)row[i].cont_val)
                 || (var.upperBoundAsDouble() < (double)row[i].cont_val))
                && (row[i].cont_val != std::numeric_limits< float >::max()))
              return false;
            break;
          }

          default:
            GUM_ERROR(NotImplementedYet, "Translated value type not supported yet")
        }
      }

      return true;
    }


    /// insert a new DBRow at the end of the database
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::insertRow(
       typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >&& new_row,
       const typename DatabaseTable< ALLOC >::IsMissing                     contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!_isRowCompatible_(new_row)) {
        if (new_row.size() != _translators_.size()) {
          GUM_ERROR(SizeError,
                    "The new row has " << new_row.size()
                                       << " elements whereas the database table has "
                                       << _translators_.size() << " columns")
        } else {
          GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRow(std::move(new_row),
                                                            contains_missing_data);
    }


    /// insert a new row at the end of the database
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::insertRow(
       const typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >& new_row,
       const typename DatabaseTable< ALLOC >::IsMissing contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!_isRowCompatible_(new_row)) {
        if (new_row.size() != _translators_.size()) {
          GUM_ERROR(SizeError,
                    "The new row has " << new_row.size()
                                       << " elements whereas the database table has "
                                       << _translators_.size() << " columns")
        } else {
          GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRow(new_row, contains_missing_data);
    }


    // insert a new DBRow of DBCells at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRow(
       const typename DatabaseTable< ALLOC >::template Row< DBCell >& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }

    // insert a new DBRow of DBCells at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRow(
       typename DatabaseTable< ALLOC >::template Row< DBCell >&& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       typename DatabaseTable< ALLOC >::template Matrix< DBTranslatedValue >&& rows,
       const typename DatabaseTable< ALLOC >::template DBVector< IsMissing >&
          rows_have_missing_vals) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for (const auto& new_row: rows) {
        if (!_isRowCompatible_(new_row)) {
          if (new_row.size() != _translators_.size()) {
            GUM_ERROR(SizeError,
                      "The new row has " << new_row.size()
                                         << " elements whereas the database table has "
                                         << _translators_.size() << " columns")
          } else {
            GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
          }
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRows(std::move(rows),
                                                             rows_have_missing_vals);
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       const typename DatabaseTable< ALLOC >::template Matrix< DBTranslatedValue >& new_rows,
       const typename DatabaseTable< ALLOC >::template DBVector< IsMissing >&
          rows_have_missing_vals) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for (const auto& new_row: new_rows) {
        if (!_isRowCompatible_(new_row)) {
          if (new_row.size() != _translators_.size()) {
            GUM_ERROR(SizeError,
                      "The new row has " << new_row.size()
                                         << " elements whereas the database table has "
                                         << _translators_.size() << " columns")
          } else {
            GUM_ERROR(InvalidArgument, "the new row is not compatible with the current translators")
          }
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRows(new_rows, rows_have_missing_vals);
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       typename DatabaseTable< ALLOC >::template Matrix< DBCell >&& new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       const typename DatabaseTable< ALLOC >::template Matrix< DBCell >& new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }


    /// erase the content of the database, including the names of the variables
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::clear() {
      _translators_.clear();
      _ignored_cols_.clear();
      IDatabaseTable< DBTranslatedValue, ALLOC >::clear();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

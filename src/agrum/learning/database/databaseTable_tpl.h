
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
 * @brief The implementation of tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/databaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DatabaseTable< ALLOC >::DatabaseTable(
       const typename DatabaseTable< ALLOC >::template MissingValType< XALLOC >&
                                                              missing_symbols,
       const DBTranslatorSet< ALLOC >&                        translators,
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(
           missing_symbols,
           std::vector< std::string, ALLOC< std::string > >(),
           alloc),
        __translators(translators, alloc) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string, ALLOC< std::string > > var_names(
           translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size;
             ++i) {
          var_names[i] = __translators.translator(i).variable()->name();
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
        __translators(translators, alloc) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string, ALLOC< std::string > > var_names(
           translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size;
             ++i) {
          var_names[i] = __translators.translator(i).variable()->name();
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
        __translators(from.__translators, alloc),
        __ignored_cols(from.__ignored_cols) {
      GUM_CONS_CPY(DatabaseTable);
    }


    // copy constructor
    template < template < typename > class ALLOC >
    INLINE
       DatabaseTable< ALLOC >::DatabaseTable(const DatabaseTable< ALLOC >& from) :
        DatabaseTable< ALLOC >(from, from.getAllocator()) {}


    // move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DatabaseTable< ALLOC >::DatabaseTable(
       DatabaseTable< ALLOC >&&                               from,
       const typename DatabaseTable< ALLOC >::allocator_type& alloc) :
        IDatabaseTable< DBTranslatedValue, ALLOC >(std::move(from), alloc),
        __translators(std::move(from.__translators), alloc),
        __ignored_cols(std::move(from.__ignored_cols)) {
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
    DatabaseTable< ALLOC >& DatabaseTable< ALLOC >::
                            operator=(const DatabaseTable< ALLOC >& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue, ALLOC >::operator=(from);
        __translators = from.__translators;
        __ignored_cols = from.__ignored_cols;
      }

      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    DatabaseTable< ALLOC >& DatabaseTable< ALLOC >::
                            operator=(DatabaseTable< ALLOC >&& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue, ALLOC >::operator=(std::move(from));
        __translators = std::move(from.__translators);
        __ignored_cols = std::move(from.__ignored_cols);
      }

      return *this;
    }


    // a method to process the rows of the database in multithreading
    template < template < typename > class ALLOC >
    template < typename Functor1, typename Functor2 >
    void DatabaseTable< ALLOC >::__threadProcessDatabase(Functor1& exec_func,
                                                         Functor2& undo_func) {
      // compute the number of threads to execute the code, the number N of
      // rows that each thread should process and the number of rows that
      // would remain after each thread has processed its N rows. For instance,
      // if the database has 105 rows and there are 10 threads, each thread
      // should process 10 rows and there would remain 5 rows
      const std::size_t db_size = this->_rows.size();
      std::size_t       nb_threads = db_size / this->_min_nb_rows_per_thread;
      if (nb_threads < 1)
        nb_threads = 1;
      else if (nb_threads > this->_max_nb_threads)
        nb_threads = this->_max_nb_threads;
      std::size_t nb_rows_par_thread = db_size / nb_threads;
      std::size_t rest_rows = db_size - nb_rows_par_thread * nb_threads;

      // if there is just one thread, let it process all the rows
      if (nb_threads == 1) {
        exec_func(std::size_t(0), db_size);
        return;
      }

      // here, we shall create the threads, but also one std::exception_ptr
      // for each thread. This will allow us to catch the exception raised
      // by the threads
      std::vector< std::thread > threads;
      threads.reserve(nb_threads);
      std::vector< std::exception_ptr > func_exceptions(nb_threads, nullptr);

      // create a lambda that will execute exec_func while catching its exceptions
      auto real_exec_func = [&exec_func](std::size_t         begin,
                                         std::size_t         end,
                                         std::exception_ptr& exc) -> void {
        try {
          exec_func(begin, end);
        } catch (...) { exc = std::current_exception(); }
      };

      // launch the threads
      std::size_t begin_index = std::size_t(0);
      for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
        std::size_t end_index = begin_index + nb_rows_par_thread;
        if (rest_rows != std::size_t(0)) {
          ++end_index;
          --rest_rows;
        }
        threads.push_back(std::thread(std::ref(real_exec_func),
                                      begin_index,
                                      end_index,
                                      std::ref(func_exceptions[i])));
        begin_index = end_index;
      }

      // wait for the threads to complete their executions
      std::for_each(
         threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

      // now, check if one exception has been raised
      bool exception_raised = false;
      for (const auto& exc : func_exceptions) {
        if (exc != nullptr) {
          exception_raised = true;
          break;
        }
      }

      if (exception_raised) {
        // create a lambda that will execute undo_func while catching
        // its exceptions
        auto real_undo_func = [&undo_func](std::size_t         begin,
                                           std::size_t         end,
                                           std::exception_ptr& exc) -> void {
          try {
            undo_func(begin, end);
          } catch (...) { exc = std::current_exception(); }
        };

        // launch the repair threads
        threads.clear();
        begin_index = std::size_t(0);
        std::vector< std::exception_ptr > undo_func_exceptions(nb_threads,
                                                               nullptr);
        for (std::size_t i = std::size_t(0); i < nb_threads; ++i) {
          std::size_t end_index = begin_index + nb_rows_par_thread;
          if (rest_rows != std::size_t(0)) {
            ++end_index;
            --rest_rows;
          }
          // we just need to repair the threads that did not raise exceptions
          if (func_exceptions[i] == nullptr)
            threads.push_back(std::thread(std::ref(real_undo_func),
                                          begin_index,
                                          end_index,
                                          std::ref(undo_func_exceptions[i])));
          begin_index = end_index;
        }

        // wait for the threads to complete their executions
        std::for_each(
           threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        // rethrow the exception
        for (const auto& exc : func_exceptions) {
          if (exc != nullptr) { std::rethrow_exception(exc); }
        }
      }
    }


    /// insert a new translator into the database
    template < template < typename > class ALLOC >
    std::size_t DatabaseTable< ALLOC >::insertTranslator(
       const DBTranslator< ALLOC >& translator,
       const std::size_t            input_column,
       const bool                   unique_column) {
      // check that there is no ignored_column corresponding to column
      if (__ignored_cols.exists(input_column))
        GUM_ERROR(
           OperationNotAllowed,
           "Column "
              << input_column << " is marked as being ignored. "
              << "So it is forbidden to create a translator for that column.");

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->_rows[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin, std::size_t end) -> void {};

      // launch the threads executing the lambdas
      this->__threadProcessDatabase(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos =
         __translators.insertTranslator(translator, input_column, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->_variable_names.push_back(translator.variable()->name());
      } catch (...) {
        __translators.eraseTranslator(pos);
        throw;
      }

      // if the databaseTable is not empty, fill the column of the database
      // corresponding to the translator with missing values
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        const DBTranslatedValue missing = __translators[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda = [this, missing](std::size_t begin,
                                           std::size_t end) -> void {
          std::size_t i = begin;
          try {
            for (; i < end; ++i) {
              this->_rows[i].row().push_back(missing);
              this->_has_row_missing_val[i] = IsMissing::True;
            }
          } catch (...) {
            for (std::size_t j = begin; j < i; ++j)
              this->_rows[i].row().pop_back();
            throw;
          }
        };

        auto undo_fill_lambda = [this](std::size_t begin,
                                       std::size_t end) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->_rows[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->__threadProcessDatabase(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }


    /// insert a new translator into the database
    template < template < typename > class ALLOC >
    std::size_t
       DatabaseTable< ALLOC >::insertTranslator(const Variable&   var,
                                                const std::size_t input_column,
                                                const bool        unique_column) {
      // check that there is no ignored_column corresponding to column
      if (__ignored_cols.exists(input_column))
        GUM_ERROR(
           OperationNotAllowed,
           "Column "
              << input_column << " is marked as being ignored. "
              << "So it is forbidden to create a translator for that column.");

      // if the databaseTable is not empty, we should fill the column of the
      // database corresponding to the new translator with missing values. But, the
      // current method assumes that the list of missing values is empty. Hence, it
      // should raise an exception
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        GUM_ERROR(
           MissingValueInDatabase,
           "inserting a new translator into a database creates a new column "
              << "with missing values. However, you did not define any symbol for "
              << "such values.");
      }

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->_rows[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin, std::size_t end) -> void {};

      // launch the threads executing the lambdas
      this->__threadProcessDatabase(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos =
         __translators.insertTranslator(var, input_column, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->_variable_names.push_back(var.name());
      } catch (...) {
        __translators.eraseTranslator(pos);
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
      if (__ignored_cols.exists(input_column))
        GUM_ERROR(
           OperationNotAllowed,
           "Column "
              << input_column << " is marked as being ignored. "
              << "So it is forbidden to create a translator for that column.");

      // reserve some place for the new column in the records of the database
      const std::size_t new_size = this->nbVariables() + 1;

      // create the lambda for reserving some memory for the new column
      // and the one that undoes what it performed if some thread executing
      // it raised an exception
      auto reserve_lambda = [this, new_size](std::size_t begin,
                                             std::size_t end) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->_rows[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda = [](std::size_t begin, std::size_t end) -> void {};

      // launch the threads executing the lambdas
      this->__threadProcessDatabase(reserve_lambda, undo_reserve_lambda);

      // insert the translator into the translator set
      const std::size_t pos = __translators.insertTranslator(
         var, input_column, missing_symbols, unique_column);

      // insert the name of the translator's variable to the set of variable names
      try {
        this->_variable_names.push_back(var.name());
      } catch (...) {
        __translators.eraseTranslator(pos);
        throw;
      }

      // if the databaseTable is not empty, fill the column of the database
      // corresponding to the translator with missing values
      if (!IDatabaseTable< DBTranslatedValue, ALLOC >::empty()) {
        const DBTranslatedValue missing = __translators[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda = [this, missing](std::size_t begin,
                                           std::size_t end) -> void {
          std::size_t i = begin;
          try {
            for (; i < end; ++i) {
              this->_rows[i].row().push_back(missing);
              this->_has_row_missing_val[i] = IsMissing::True;
            }
          } catch (...) {
            for (std::size_t j = begin; j < i; ++j)
              this->_rows[i].row().pop_back();
            throw;
          }
        };

        auto undo_fill_lambda = [this](std::size_t begin,
                                       std::size_t end) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->_rows[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->__threadProcessDatabase(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }


    /** @brief returns the indices corresponding either to the kth translator
     * or to all those that parse the kth column of the input dataset
     *
     * @warning the indices are sorted by deacreasing order */
    template < template < typename > class ALLOC >
    INLINE typename DatabaseTable< ALLOC >::template DBVector< std::size_t >
       DatabaseTable< ALLOC >::__getKthIndices(const std::size_t k,
                                               const bool k_is_input_col) const {
      const std::size_t nb_trans = __translators.size();
      if (!k_is_input_col) {
        if (k < nb_trans)
          return DBVector< std::size_t >{k};
        else
          return DBVector< std::size_t >();
      } else {
        DBVector< std::size_t > trans;
        for (std::size_t i = std::size_t(0), kk = nb_trans - 1; i < nb_trans;
             ++i, --kk) {
          if (__translators.inputColumn(kk) == k) trans.push_back(kk);
        }
        return trans;
      }
    }


    // erases the kth translator or all those parsing the kth column of
    // the input dataset
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::eraseTranslators(const std::size_t k,
                                                  const bool k_is_input_col) {
      for (const auto kk : __getKthIndices(k, k_is_input_col)) {
        // erase the translator of index kk and the corresponding variable
        // name. If there remains no more translator in the translator set,
        // _rows should become empty
        this->_variable_names.erase(this->_variable_names.begin() + kk);
        if (this->_variable_names.empty()) {
          IDatabaseTable< DBTranslatedValue, ALLOC >::eraseAllRows();
        } else {
          const std::size_t nb_trans = __translators.size();

          auto erase_lambda = [this, nb_trans, kk](std::size_t begin,
                                                   std::size_t end) -> void {
            for (std::size_t i = begin; i < end; ++i) {
              auto& row = this->_rows[i].row();
              if (this->__translators.isMissingValue(row[kk], kk)) {
                bool has_missing_val = false;
                for (std::size_t j = std::size_t(0); j < nb_trans; ++j) {
                  if ((j != kk) && this->__translators.isMissingValue(row[j], j)) {
                    has_missing_val = true;
                    break;
                  }
                }
                if (!has_missing_val)
                  this->_has_row_missing_val[i] = IsMissing::False;
              }
              row.erase(row.begin() + kk);
            }
          };

          auto undo_erase_lambda = [](std::size_t begin, std::size_t end) -> void {
          };

          // launch the threads executing the lambdas
          this->__threadProcessDatabase(erase_lambda, undo_erase_lambda);
        }
        __translators.eraseTranslator(kk);
      }
    }


    /// returns the set of translators
    template < template < typename > class ALLOC >
    INLINE const DBTranslatorSet< ALLOC >&
                 DatabaseTable< ALLOC >::translatorSet() const {
      return __translators;
    }


    /** @brief returns the index corresponding either to the kth translator or
     * to that of the first translator parsing the kth column of the
     * input dataset */
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DatabaseTable< ALLOC >::__getKthIndex(const std::size_t k,
                                             const bool k_is_input_col) const {
      if (k_is_input_col) {
        const std::size_t nb_trans = __translators.size();
        for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
          if (__translators.inputColumn(i) == k) { return i; }
        }
        return nb_trans + 1;
      } else {
        return k;
      }
    }


    /// returns the kth translator of the database
    template < template < typename > class ALLOC >
    const DBTranslator< ALLOC >&
       DatabaseTable< ALLOC >::translator(const std::size_t k,
                                          const bool        k_is_input_col) const {
      // find the position of the translator that we look for. This
      // is variable kk below
      const std::size_t nb_trans = __translators.size();
      const std::size_t kk = __getKthIndex(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k);
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans
                                        << " translators, so Translator #" << k
                                        << " does not exist");
        }
      }

      return __translators.translator(kk);
    }


    /// returns the kth variable of the database
    template < template < typename > class ALLOC >
    const Variable&
       DatabaseTable< ALLOC >::variable(const std::size_t k,
                                        const bool        k_is_input_col) const {
      // find the position of the translator that contains the variable.
      // This is variable kk below
      const std::size_t nb_trans = __translators.size();
      const std::size_t kk = __getKthIndex(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that "
                       << "corresponds to Column " << k);
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #"
                                        << k << " does not exist");
        }
      }

      return __translators.variable(kk);
    }


    /// sets the names of the variables
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::setVariableNames(
       const std::vector< std::string, ALLOC< std::string > >& names,
       const bool from_external_object) {
      const std::size_t size = names.size();
      const std::size_t nb_trans = __translators.size();
      if (!from_external_object) {
        if (nb_trans != size) {
          GUM_ERROR(SizeError,
                    "the number of variable's names (i.e., "
                       << size
                       << ") does not correspond to the number of columns of the "
                       << "database table (i.e.," << nb_trans << ")");
        }

        // update the translator names
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          __translators.translator(i).setVariableName(names[i]);
        }
      } else {
        if (nb_trans && (__translators.highestInputColumn() >= size)) {
          GUM_ERROR(SizeError,
                    "the names vector has "
                       << size << " elements whereas it should have at least "
                       << (__translators.highestInputColumn() + 1)
                       << "elements so that each translator is assigned a name");
        }

        // update the translator names
        for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
          __translators.translator(i).setVariableName(
             names[__translators.inputColumn(i)]);
        }
      }

      // update _variable_names using the newly assigned translators names
      this->_variable_names.resize(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        this->_variable_names[i] = __translators.variable(i).name();
      }
    }


    /** @brief indicates that we should ignore the kth column of the original
     * database when inserting new rows */
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::ignoreColumn(const std::size_t k,
                                              const bool        k_is_input_col) {
      // indicate that the column will be forbidden. If the column is already
      // forbidden, do nothing. But if the column is assigned to a translator
      // that does not exist, raise an UndefinedElement exception
      const std::size_t nb_trans = __translators.size();
      if (k_is_input_col) {
        if (__ignored_cols.exists(k)) return;
        __ignored_cols.insert(k);
      } else {
        if (k < nb_trans) {
          __ignored_cols.insert(__translators.inputColumn(k));
        } else {
          GUM_ERROR(UndefinedElement,
                    "It is impossible to ignore the column parsed by Translator #"
                       << k << "because there exist only " << nb_trans
                       << " translators");
        }
      }

      // remove all the translators corresponding to k
      eraseTranslators(k, k_is_input_col);
    }


    /// returns  the set of ignored columns
    template < template < typename > class ALLOC >
    const typename DatabaseTable< ALLOC >::template DBVector< std::size_t >
       DatabaseTable< ALLOC >::ignoredColumns() const {
      const std::size_t nb_trans = __translators.size();

      if (nb_trans == std::size_t(0)) {
        return DBVector< std::size_t >{std::size_t(0)};
      }

      // get the columns handled by the translators, sorted by increasing order
      DBVector< std::size_t > cols(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i)
        cols[i] = __translators.inputColumn(i);
      std::sort(cols.begin(), cols.end());

      // create a vector with all the possible input columns
      const std::size_t       highest = __translators.highestInputColumn() + 1;
      DBVector< std::size_t > ignored_cols(highest);
      std::iota(ignored_cols.begin(), ignored_cols.end(), 0);

      // remove from ignored_cols the elements of cols
      for (std::size_t i = std::size_t(0),
                       ii = highest - 1,
                       k = std::size_t(0),
                       kk = nb_trans - 1;
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
      const std::size_t nb_trans = __translators.size();
      if (nb_trans == std::size_t(0)) { return DBVector< std::size_t >(); }

      DBVector< std::size_t > input_cols(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        input_cols[i] = __translators.inputColumn(i);
      }
      return input_cols;
    }


    /// returns the domain size of the kth variable
    template < template < typename > class ALLOC >
    std::size_t
       DatabaseTable< ALLOC >::domainSize(const std::size_t k,
                                          const bool        k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size();
      const std::size_t kk = __getKthIndex(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that "
                       << "corresponds to Column " << k);
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #"
                                        << k << " does not exist");
        }
      }

      return __translators.domainSize(kk);
    }


    /// returns the domain sizes of all the variables in the database table
    template < template < typename > class ALLOC >
    INLINE std::vector< std::size_t, ALLOC< std::size_t > >
           DatabaseTable< ALLOC >::domainSizes() const {
      const std::size_t          nb_trans = __translators.size();
      std::vector< std::size_t > dom(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        dom[i] = __translators.domainSize(i);
      }
      return dom;
    }


    // indicates whether a reordering is needed to make the kth
    // translator sorted by lexicographical order
    template < template < typename > class ALLOC >
    bool DatabaseTable< ALLOC >::needsReordering(const std::size_t k,
                                                 const bool k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size();
      const std::size_t kk = __getKthIndex(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k);
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans
                                        << " translators, so Translator #" << k
                                        << " does not exist");
        }
      }

      return __translators.needsReordering(kk);
    }


    // performs a reordering of the kth translator or of the first
    // translator corresponding to the kth column of the input database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::reorder(const std::size_t k,
                                         const bool        k_is_input_col) {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size();
      const std::size_t kk = __getKthIndex(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that "
                       << "parses Column " << k);
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans
                                        << " translators, so Translator #" << k
                                        << " does not exist");
        }
      }

      // if the translator is not designed for a discrete variable, there
      // is no reordering to apply
      if (__translators.translator(kk).getValType()
          != DBTranslatedValueType::DISCRETE)
        return;

      // get the translation to perform
      auto updates = __translators.reorder(kk);
      if (updates.empty()) return;

      std::size_t                                      size = updates.size();
      std::vector< std::size_t, ALLOC< std::size_t > > new_values(size);
      for (const auto& update : updates) {
        if (update.first >= size) {
          size = update.first + 1;
          new_values.resize(size);
        }
        new_values[update.first] = update.second;
      }

      // apply the translations
      auto newtrans_lambda = [this, kk, &new_values](std::size_t begin,
                                                     std::size_t end) -> void {
        for (std::size_t i = begin; i < end; ++i) {
          auto& elt = this->_rows[i][kk].discr_val;
          if (elt != std::numeric_limits< std::size_t >::max())
            elt = new_values[elt];
        }
      };

      auto undo_newtrans_lambda = [](std::size_t begin, std::size_t end) -> void {
      };

      // launch the threads executing the lambdas
      this->__threadProcessDatabase(newtrans_lambda, undo_newtrans_lambda);
    }


    /// performs a reordering of all the columns
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::reorder() {
      const std::size_t nb_trans = __translators.size();
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

      if (__translators.highestInputColumn() >= row_size) {
        GUM_ERROR(SizeError,
                  "the new row has "
                     << row_size
                     << " columns whereas the database requires at least "
                     << (__translators.highestInputColumn() + 1) << " columns");
      }

      // convert the new_row into a row of DBTranslatedValue
      const std::size_t        nb_trans = __translators.size();
      Row< DBTranslatedValue > dbrow;
      dbrow.reserve(nb_trans);
      bool has_missing_val = false;
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        const DBTranslatedValue new_val(__translators.translate(new_row, i));
        if (__translators.isMissingValue(new_val, i)) has_missing_val = true;
        dbrow.pushBack(std::move(new_val));
      }

      this->insertRow(std::move(dbrow),
                      has_missing_val ? IsMissing::True : IsMissing::False);
    }


    /** @brief check that a row's values are compatible with those of the
     * translators' variables */
    template < template < typename > class ALLOC >
    bool DatabaseTable< ALLOC >::__isRowCompatible(
       const typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >&
          row) const {
      // check that the size of the row corresponds to that of the translators
      const std::size_t row_size = row.size();
      if (row_size != __translators.size()) return false;

      const auto& translators = __translators.translators();
      for (std::size_t i = std::size_t(0); i < row_size; ++i) {
        switch (translators[i]->getValType()) {
          case DBTranslatedValueType::DISCRETE:
            if ((row[i].discr_val >= translators[i]->domainSize())
                && (row[i].discr_val != std::numeric_limits< std::size_t >::max()))
              return false;
            break;

          case DBTranslatedValueType::CONTINUOUS: {
            const IContinuousVariable& var =
               static_cast< const IContinuousVariable& >(
                  *(translators[i]->variable()));
            if (((var.lowerBoundAsDouble() > (double)row[i].cont_val)
                 || (var.upperBoundAsDouble() < (double)row[i].cont_val))
                && (row[i].cont_val != std::numeric_limits< float >::max()))
              return false;
            break;
          }

          default:
            GUM_ERROR(NotImplementedYet,
                      "Translated value type not supported yet");
        }
      }

      return true;
    }


    /// insert a new DBRow at the end of the database
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::insertRow(
       typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >&&
                                                        new_row,
       const typename DatabaseTable< ALLOC >::IsMissing contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!__isRowCompatible(new_row)) {
        if (new_row.size() != __translators.size()) {
          GUM_ERROR(SizeError,
                    "The new row has "
                       << new_row.size()
                       << " elements whereas the database table has "
                       << __translators.size() << " columns");
        } else {
          GUM_ERROR(InvalidArgument,
                    "the new row is not compatible with the current translators");
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRow(std::move(new_row),
                                                            contains_missing_data);
    }


    /// insert a new row at the end of the database
    template < template < typename > class ALLOC >
    INLINE void DatabaseTable< ALLOC >::insertRow(
       const typename DatabaseTable< ALLOC >::template Row< DBTranslatedValue >&
                                                        new_row,
       const typename DatabaseTable< ALLOC >::IsMissing contains_missing_data) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if (!__isRowCompatible(new_row)) {
        if (new_row.size() != __translators.size()) {
          GUM_ERROR(SizeError,
                    "The new row has "
                       << new_row.size()
                       << " elements whereas the database table has "
                       << __translators.size() << " columns");
        } else {
          GUM_ERROR(InvalidArgument,
                    "the new row is not compatible with the current translators");
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRow(new_row,
                                                            contains_missing_data);
    }


    // insert a new DBRow of DBCells at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRow(
       const typename DatabaseTable< ALLOC >::template Row< DBCell >& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet");
    }

    // insert a new DBRow of DBCells at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRow(
       typename DatabaseTable< ALLOC >::template Row< DBCell >&& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet");
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       typename DatabaseTable< ALLOC >::template Matrix< DBTranslatedValue >&&
          rows,
       const typename DatabaseTable< ALLOC >::template DBVector< IsMissing >&
          rows_have_missing_vals) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for (const auto& new_row : rows) {
        if (!__isRowCompatible(new_row)) {
          if (new_row.size() != __translators.size()) {
            GUM_ERROR(SizeError,
                      "The new row has "
                         << new_row.size()
                         << " elements whereas the database table has "
                         << __translators.size() << " columns");
          } else {
            GUM_ERROR(
               InvalidArgument,
               "the new row is not compatible with the current translators");
          }
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRows(
         std::move(rows), rows_have_missing_vals);
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       const typename DatabaseTable< ALLOC >::template Matrix< DBTranslatedValue >&
          new_rows,
       const typename DatabaseTable< ALLOC >::template DBVector< IsMissing >&
          rows_have_missing_vals) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for (const auto& new_row : new_rows) {
        if (!__isRowCompatible(new_row)) {
          if (new_row.size() != __translators.size()) {
            GUM_ERROR(SizeError,
                      "The new row has "
                         << new_row.size()
                         << " elements whereas the database table has "
                         << __translators.size() << " columns");
          } else {
            GUM_ERROR(
               InvalidArgument,
               "the new row is not compatible with the current translators");
          }
        }
      }

      IDatabaseTable< DBTranslatedValue, ALLOC >::insertRows(
         new_rows, rows_have_missing_vals);
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       typename DatabaseTable< ALLOC >::template Matrix< DBCell >&& new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet");
    }


    /// insert a set of new DBRows at the end of the database
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::insertRows(
       const typename DatabaseTable< ALLOC >::template Matrix< DBCell >&
          new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet");
    }


    /// erase the content of the database, including the names of the variables
    template < template < typename > class ALLOC >
    void DatabaseTable< ALLOC >::clear() {
      __translators.clear();
      __ignored_cols.clear();
      IDatabaseTable< DBTranslatedValue, ALLOC >::clear();
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

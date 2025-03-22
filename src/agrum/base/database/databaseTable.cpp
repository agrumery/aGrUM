/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief The class representing a tabular database stored in RAM
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/database/databaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/databaseTable_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // default constructor
    DatabaseTable::DatabaseTable(const typename DatabaseTable::MissingValType& missing_symbols,
                                 const DBTranslatorSet&                        translators) :
        IDatabaseTable< DBTranslatedValue >(missing_symbols, std::vector< std::string >()),
        _translators_(translators) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string > var_names(translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size; ++i) {
          var_names[i] = _translators_.translator(i).variable()->name();
        }
        setVariableNames(var_names, false);
      }

      GUM_CONSTRUCTOR(DatabaseTable);
    }

    // default constructor
    DatabaseTable::DatabaseTable(const DBTranslatorSet& translators) :
        IDatabaseTable< DBTranslatedValue >(std::vector< std::string >(),
                                            std::vector< std::string >()),
        _translators_(translators) {
      if (translators.size()) {
        // set the variables names according to those of the translators
        std::vector< std::string > var_names(translators.size());
        for (std::size_t i = std::size_t(0), size = translators.size(); i < size; ++i) {
          var_names[i] = _translators_.translator(i).variable()->name();
        }
        setVariableNames(var_names, false);
      }

      GUM_CONSTRUCTOR(DatabaseTable);
    }

    // virtual copy constructor
    DatabaseTable* DatabaseTable::clone() const { return new DatabaseTable(*this); }

    // copy operator
    DatabaseTable& DatabaseTable::operator=(const DatabaseTable& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue >::operator=(from);
        _translators_  = from._translators_;
        _ignored_cols_ = from._ignored_cols_;
      }

      return *this;
    }

    // move constructor
    DatabaseTable& DatabaseTable::operator=(DatabaseTable&& from) {
      if (this != &from) {
        IDatabaseTable< DBTranslatedValue >::operator=(std::move(from));
        _translators_  = std::move(from._translators_);
        _ignored_cols_ = std::move(from._ignored_cols_);
      }

      return *this;
    }

    /// insert a new translator into the database
    std::size_t DatabaseTable::insertTranslator(const DBTranslator& translator,
                                                const std::size_t   input_column,
                                                const bool          unique_column) {
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
      auto reserve_lambda
          = [this, new_size](std::size_t begin, std::size_t end, std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda
          = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

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
      if (!IDatabaseTable< DBTranslatedValue >::empty()) {
        const DBTranslatedValue missing = _translators_[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda
            = [this, missing](std::size_t begin, std::size_t end, std::size_t index) -> void {
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

        auto undo_fill_lambda
            = [this](std::size_t begin, std::size_t end, std::size_t index) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->rows_[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }

    /// insert a new translator into the database
    std::size_t DatabaseTable::insertTranslator(const Variable&   var,
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
      if (!IDatabaseTable< DBTranslatedValue >::empty()) {
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
      auto reserve_lambda
          = [this, new_size](std::size_t begin, std::size_t end, std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda
          = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

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
    std::size_t DatabaseTable::insertTranslator(const Variable&            var,
                                                const std::size_t          input_column,
                                                std::vector< std::string > missing_symbols,
                                                const bool                 unique_column) {
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
      auto reserve_lambda
          = [this, new_size](std::size_t begin, std::size_t end, std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i)
          this->rows_[i].row().reserve(new_size);
      };

      auto undo_reserve_lambda
          = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

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
      if (!IDatabaseTable< DBTranslatedValue >::empty()) {
        const DBTranslatedValue missing = _translators_[pos].missingValue();

        // create the lambda for adding a new column filled wih a missing value
        auto fill_lambda
            = [this, missing](std::size_t begin, std::size_t end, std::size_t index) -> void {
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

        auto undo_fill_lambda
            = [this](std::size_t begin, std::size_t end, std::size_t index) -> void {
          for (std::size_t i = begin; i < end; ++i)
            this->rows_[i].row().pop_back();
        };

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(fill_lambda, undo_fill_lambda);
      }

      return pos;
    }

    // erases the kth translator or all those parsing the kth column of
    // the input dataset
    void DatabaseTable::eraseTranslators(const std::size_t k, const bool k_is_input_col) {
      for (const auto kk: _getKthIndices_(k, k_is_input_col)) {
        // erase the translator of index kk and the corresponding variable
        // name. If there remains no more translator in the translator set,
        // rows_ should become empty
        this->variable_names_.erase(this->variable_names_.begin() + kk);
        if (this->variable_names_.empty()) {
          IDatabaseTable< DBTranslatedValue >::eraseAllRows();
        } else {
          const std::size_t nb_trans = _translators_.size();

          auto erase_lambda =
              [this, nb_trans, kk](std::size_t begin, std::size_t end, std::size_t index) -> void {
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

          auto undo_erase_lambda
              = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

          // launch the threads executing the lambdas
          this->_threadProcessDatabase_(erase_lambda, undo_erase_lambda);
        }
        _translators_.eraseTranslator(kk);
      }
    }

    /// change the translator of a database column
    void DatabaseTable::changeTranslator(DBTranslator&     new_translator,
                                         const std::size_t k,
                                         const bool        k_is_input_col) {
      // get the index of the column in the database. If it is not found, indicate that
      // the substitution is impossible
      const auto db_k = _getKthIndices_(k, k_is_input_col);
      if (db_k.empty()) {
        GUM_ERROR(OutOfBounds, "the translator at position " << k << " cannot be found.");
      }
      const std::size_t kk = db_k[db_k.size() - 1];
      if (kk >= _translators_.size()) {
        GUM_ERROR(OutOfBounds, "the translator at position " << k << " cannot be found.");
      }


      // if the dataset does not contain any data, we can safely substitute the old translator
      // by the new one
      if (this->empty()) {
        // keep into account the name of the new translator
        this->variable_names_[kk] = new_translator.variable()->name();

        // substitute int the stransltor's set the old translator by the new one
        _translators_.changeTranslator(new_translator, kk);

        return;
      }

      // get the translator and check that it is not lossy: as, here, there are some data,
      // we cannot always ensure that there won't be some loss of information substituting
      // one translator by another
      DBTranslator& old_translator = _translators_[kk];
      if (!old_translator.isLossless()) {
        // for the moment, we consider that it is impossible to substitute lossy translators
        // because we may have already lost information that are necessary for the new
        // translator
        GUM_ERROR(OperationNotAllowed,
                  "Lossy translators cannot yet be substituted by other translators");
      }

      const std::size_t nb_threads = this->nbProcessingThreads_();

      // how missing values will be translated
      std::pair< DBTranslatedValue, DBTranslatedValue > miss_mapping(old_translator.missingValue(),
                                                                     new_translator.missingValue());

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
          auto get_lambda = [this, kk, &missing_values](std::size_t begin,
                                                        std::size_t end,
                                                        std::size_t index) -> void {
            for (std::size_t i = begin; i < end; ++i) {
              auto& row = this->rows_[i].row();
              if (this->_translators_.isMissingValue(row[kk], kk)) {
                missing_values[index] = 1;
                return;
              }
            }
          };

          auto undo_get_lambda
              = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

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
        const bool old_miss_included = old_missing_symbols.isSubsetOrEqual(new_missing_symbols);
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
          } catch (Exception const&) {
            GUM_ERROR(OperationNotAllowed,
                      "it is impossible to substitute the translator because "
                      "the database contains some missing values that cannot be "
                      "substituted");
          }
        }

        // compute the mapping of the missing symbol if this one does not corresponds
        // to a missing value in the new translator
        if (has_missing_value && !old_miss_included) {
          miss_mapping.second
              = new_translator.translate(*(old_translator.missingSymbols().begin()));
        }

        // test for case 2/ (if the set of (non-missing) values of the old translator is
        // not included in the one of the new translator)

        // now, parse the database and check that all the values contained in the
        // database can be translated
        std::vector< int > unmapped(nb_threads, 0);

        // a lambda to parse all the translated values
        auto check_lambda
            = [this, kk, &old_translator, &new_translator, &unmapped](std::size_t begin,
                                                                      std::size_t end,
                                                                      std::size_t index) -> void {
          const auto old_miss = old_translator.missingValue().discr_val;
          for (std::size_t i = begin; i < end; ++i) {
            const auto& row = this->rows_[i].row();
            if (row[kk].discr_val != old_miss) {
              try {
                new_translator.translate(old_translator.translateBack(row[kk]));
              } catch (Exception const&) {
                // ok, here, the translation is impossible
                unmapped[index] = 1;
                return;
              }
            }
          }
        };

        auto undo_check_lambda
            = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

        // launch the threads executing the lambdas
        this->_threadProcessDatabase_(check_lambda, undo_check_lambda);

        // if unmapped has at least one value 1, there are values that we don't know how to
        // translate
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
      auto change_lambda
          = [this, kk, &old_translator, &new_translator, miss_mapping](std::size_t begin,
                                                                       std::size_t end,
                                                                       std::size_t index) -> void {
        const auto old_miss = old_translator.missingValue().discr_val;
        for (std::size_t i = begin; i < end; ++i) {
          auto& row = this->rows_[i].row();
          if (row[kk].discr_val == old_miss) {
            row[kk] = miss_mapping.second;
          } else {
            row[kk] = new_translator.translate(old_translator.translateBack(row[kk]));
          }
        }
      };

      auto undo_change_lambda
          = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(change_lambda, undo_change_lambda);

      // keep into account the name of the new translator
      this->variable_names_[kk] = new_translator.variable()->name();

      // substitute int the stransltor's set the old translator by the new one
      _translators_.changeTranslator(new_translator, kk);
    }

    /// change the translator of a database column
    void DatabaseTable::changeTranslator(const Variable&                   var,
                                         const std::size_t                 k,
                                         const bool                        k_is_input_col,
                                         const std::vector< std::string >& missing_symbols,
                                         const bool                        editable_dictionary,
                                         const std::size_t                 max_dico_entries) {
      // get the appropriate set of missing symbols
      std::vector< std::string > missing;
      if (missing_symbols.empty()) {
        // try to get the missing symbols of the current translator
        const auto db_k = _getKthIndices_(k, k_is_input_col);
        if (db_k.empty()) {
          GUM_ERROR(OutOfBounds, "the translator at position " << k << " cannot be found.");
        }
        const std::size_t kk = db_k[db_k.size() - 1];
        if (kk >= _translators_.size()) {
          GUM_ERROR(OutOfBounds, "the translator at position " << k << " cannot be found.");
        }

        const auto& miss = _translators_[kk].missingSymbols();
        missing.reserve(miss.size());
        for (const auto& miss: missing) {
          missing.push_back(miss);
        }
      } else {
        missing = missing_symbols;
      }

      // create the DBTranslator corresponding to the parameters
      DBTranslator* new_translator
          = DBTranslators::create(var, missing, editable_dictionary, max_dico_entries);

      try {
        changeTranslator(*new_translator, k, k_is_input_col);
      } catch (...) {
        // remove from memory new_translator
        delete new_translator;
        throw;
      }

      // remove from memory new_translator
      delete new_translator;
    }

    /// propose a set with translators better suited for the content of the database
    std::vector< std::pair< Idx, std::shared_ptr< DBTranslator > > >
        DatabaseTable::betterTranslators() const {
      std::vector< std::pair< Idx, std::shared_ptr< DBTranslator > > > better;

      for (Idx i = 0, size = _translators_.size(); i < size; ++i) {
        switch (_translators_[i].variable()->varType()) {
          // if the translator is discretized, range or continuous, we cannot
          // improve it
          case VarType::CONTINUOUS :
          case VarType::NUMERICAL :
          case VarType::DISCRETIZED :
          case VarType::RANGE : break;

          // if the translator can only translate integers ans all the numbers
          // are consecutive, prefer a range variable
          case VarType::INTEGER : {
            const IntegerVariable& var
                = static_cast< const IntegerVariable& >(*(_translators_[i].variable()));

            // check that the values in the domain are consecutive
            const auto& domain = var.integerDomain();
            if (domain.empty()) break;   // we cannot get a better translator
            int  prev = domain[0] - 1;
            bool ok   = true;
            for (const auto elt: domain) {
              if (elt != prev + 1) {
                ok = false;
                break;
              } else {
                prev = elt;
              }
            }

            // here, we know that the values are consecutive, hence we can
            // change the variable to a range variable
            if (ok) {
              RangeVariable new_var(var.name(),
                                    var.description(),
                                    domain[0],
                                    domain[domain.size() - 1]);

              // get the set of missing symbols
              const auto&                missing = _translators_[i].missingSymbols();
              std::vector< std::string > new_missing;
              new_missing.reserve(missing.size());
              for (const auto& miss: missing) {
                new_missing.push_back(miss);
              }
              DBTranslator4RangeVariable* new_trans
                  = new DBTranslator4RangeVariable(new_var, new_missing);
              better.push_back(std::pair< Idx, std::shared_ptr< DBTranslator > >(
                  i,
                  std::shared_ptr< DBTranslator >(new_trans)));
            }
            break;
          }

          // if the translator is a set of labels, check whether those are all
          // numbers. In this case, if they are integers and consecutive,
          // prefer a RangeVariable; if they are integers but not consecutive,
          // prefer an IntegerVariable, else check whether a continuous
          // variable could be ok
          case VarType::LABELIZED : {
            const LabelizedVariable& var
                = static_cast< const LabelizedVariable& >(*(_translators_[i].variable()));
            if (!var.domainSize()) break;   // we cannot get a better translator

            // get the numerical values of the labels
            Set< int >   int_values;
            Set< float > real_values;
            bool         ok = true;
            for (Idx i = 0, size = var.domainSize(); i < size; ++i) {
              const std::string& val = var.label(i);
              if (DBCell::isReal(val)) {
                if (DBCell::isInteger(val)) {
                  int_values.insert(std::stoi(val));
                } else {
                  real_values.insert(std::stof(val));
                }
              } else {
                ok = false;
                break;
              }
            }

            // if there are only numerical values, we can certainly do something
            if (ok) {
              // check whether an IntegerVariable or a Range variable would do
              if (real_values.empty()) {
                // get the values in increasing order
                std::vector< int > values;
                values.reserve(int_values.size());
                for (const auto val: int_values)
                  values.push_back(val);
                std::sort(values.begin(), values.end());

                // if all the values are consecutive, then a range variable
                // would be best
                int  prev        = values[0] - 1;
                bool consecutive = true;
                for (const auto elt: values) {
                  if (elt != prev + 1) {
                    consecutive = false;
                    break;
                  } else {
                    prev = elt;
                  }
                }

                if (consecutive) {
                  // here, we should create a range variable since all the
                  // values in the domain are consecutive
                  RangeVariable new_var(var.name(), var.description(), values[0], values.back());

                  // get the set of missing symbols
                  const auto&                missing = _translators_[i].missingSymbols();
                  std::vector< std::string > new_missing;
                  new_missing.reserve(missing.size());
                  for (const auto& miss: missing) {
                    new_missing.push_back(miss);
                  }
                  DBTranslator4RangeVariable* new_trans
                      = new DBTranslator4RangeVariable(new_var, new_missing);
                  better.push_back(std::pair< Idx, std::shared_ptr< DBTranslator > >(
                      i,
                      std::shared_ptr< DBTranslator >(new_trans)));
                } else {
                  // here, the values in the domain are not consecutive, hence
                  // it would be better to create an IntegerVariable
                  IntegerVariable new_var(var.name(), var.description(), values);

                  // get the set of missing symbols
                  const auto&                missing = _translators_[i].missingSymbols();
                  std::vector< std::string > new_missing;
                  new_missing.reserve(missing.size());
                  for (const auto& miss: missing) {
                    new_missing.push_back(miss);
                  }
                  DBTranslator4IntegerVariable* new_trans
                      = new DBTranslator4IntegerVariable(new_var, new_missing);
                  better.push_back(std::pair< Idx, std::shared_ptr< DBTranslator > >(
                      i,
                      std::shared_ptr< DBTranslator >(new_trans)));
                }
              } else {
                // here, a translator for continuous variable would be suited
                ContinuousVariable< float > new_var(var.name(), var.description());

                // get the set of missing symbols
                const auto&                missing = _translators_[i].missingSymbols();
                std::vector< std::string > new_missing;
                new_missing.reserve(missing.size());
                for (const auto& miss: missing) {
                  new_missing.push_back(miss);
                }
                DBTranslator4ContinuousVariable* new_trans
                    = new DBTranslator4ContinuousVariable(new_var, new_missing);
                better.push_back(std::pair< Idx, std::shared_ptr< DBTranslator > >(
                    i,
                    std::shared_ptr< DBTranslator >(new_trans)));
              }
            }
            break;
          }
        }
      }

      return better;
    }

    /// returns the kth translator of the database
    const DBTranslator& DatabaseTable::translator(const std::size_t k,
                                                  const bool        k_is_input_col) const {
      // find the position of the translator that we look for. This
      // is variable kk below
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that " << "parses Column " << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " translators, so Translator #" << k
                                        << " does not exist")
        }
      }

      return _translators_.translator(kk);
    }

    /// returns the kth variable of the database
    const Variable& DatabaseTable::variable(const std::size_t k, const bool k_is_input_col) const {
      // find the position of the translator that contains the variable.
      // This is variable kk below
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that " << "corresponds to Column "
                                                                       << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #" << k
                                        << " does not exist")
        }
      }

      return _translators_.variable(kk);
    }

    /// sets the names of the variables
    void DatabaseTable::setVariableNames(const std::vector< std::string >& names,
                                         const bool                        from_external_object) {
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
    void DatabaseTable::ignoreColumn(const std::size_t k, const bool k_is_input_col) {
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
    const typename DatabaseTable::template DBVector< std::size_t >
        DatabaseTable::ignoredColumns() const {
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
    const typename DatabaseTable::template DBVector< std::size_t >
        DatabaseTable::inputColumns() const {
      const std::size_t nb_trans = _translators_.size();
      if (nb_trans == std::size_t(0)) { return DBVector< std::size_t >(); }

      DBVector< std::size_t > input_cols(nb_trans);
      for (std::size_t i = std::size_t(0); i < nb_trans; ++i) {
        input_cols[i] = _translators_.inputColumn(i);
      }
      return input_cols;
    }

    /// returns the domain size of the kth variable
    std::size_t DatabaseTable::domainSize(const std::size_t k, const bool k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no variable in the database table that " << "corresponds to Column "
                                                                       << k)
        } else {
          GUM_ERROR(UndefinedElement,
                    "the database has " << nb_trans << " variables, so Variable #" << k
                                        << " does not exist")
        }
      }

      return _translators_.domainSize(kk);
    }

    // indicates whether a reordering is needed to make the kth
    // translator sorted by lexicographical order
    bool DatabaseTable::needsReordering(const std::size_t k, const bool k_is_input_col) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that " << "parses Column " << k)
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
    void DatabaseTable::reorder(const std::size_t k, const bool k_is_input_col) {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = _translators_.size();
      const std::size_t kk       = _getKthIndex_(k, k_is_input_col);

      // check if the translator exists
      if (nb_trans <= kk) {
        if (k_is_input_col) {
          GUM_ERROR(UndefinedElement,
                    "there is no translator in the database table that " << "parses Column " << k)
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

      std::size_t                size = updates.size();
      std::vector< std::size_t > new_values(size);
      for (const auto& update: updates) {
        if (update.first >= size) {
          size = update.first + 1;
          new_values.resize(size);
        }
        new_values[update.first] = update.second;
      }

      // apply the translations
      auto newtrans_lambda
          = [this, kk, &new_values](std::size_t begin, std::size_t end, std::size_t index) -> void {
        for (std::size_t i = begin; i < end; ++i) {
          auto& elt = this->rows_[i][kk].discr_val;
          if (elt != std::numeric_limits< std::size_t >::max()) elt = new_values[elt];
        }
      };

      auto undo_newtrans_lambda
          = [](std::size_t begin, std::size_t end, std::size_t index) -> void {};

      // launch the threads executing the lambdas
      this->_threadProcessDatabase_(newtrans_lambda, undo_newtrans_lambda);
    }

    /// insert a new row at the end of the database
    void DatabaseTable::insertRow(const std::vector< std::string >& new_row) {
      // check that the row can be fully translated, i.e., it contains enough
      // columns to be translated
      const std::size_t row_size = new_row.size();
      if (row_size == std::size_t(0)) return;

      if (_translators_.highestInputColumn() >= row_size) {
        GUM_ERROR(SizeError,
                  "the row #" << 1 + size() << " has " << row_size
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
    bool DatabaseTable::_isRowCompatible_(
        const typename DatabaseTable::template Row< DBTranslatedValue >& row) const {
      // check that the size of the row corresponds to that of the translators
      const std::size_t row_size = row.size();
      if (row_size != _translators_.size()) return false;

      const auto& translators = _translators_.translators();
      for (std::size_t i = std::size_t(0); i < row_size; ++i) {
        switch (translators[i]->getValType()) {
          case DBTranslatedValueType::DISCRETE :
            if ((row[i].discr_val >= translators[i]->domainSize())
                && (row[i].discr_val != std::numeric_limits< std::size_t >::max()))
              return false;
            break;

          case DBTranslatedValueType::CONTINUOUS : {
            const IContinuousVariable& var
                = static_cast< const IContinuousVariable& >(*(translators[i]->variable()));
            if (((var.lowerBoundAsDouble() > (double)row[i].cont_val)
                 || (var.upperBoundAsDouble() < (double)row[i].cont_val))
                && (row[i].cont_val != std::numeric_limits< float >::max()))
              return false;
            break;
          }

          default : GUM_ERROR(NotImplementedYet, "Translated value type not supported yet")
        }
      }

      return true;
    }

    // insert a new DBRow of DBCells at the end of the database
    void DatabaseTable::insertRow(const typename DatabaseTable::template Row< DBCell >& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }

    // insert a new DBRow of DBCells at the end of the database
    void DatabaseTable::insertRow(typename DatabaseTable::template Row< DBCell >&& new_row) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }

    /// insert a set of new DBRows at the end of the database
    void DatabaseTable::insertRows(
        typename DatabaseTable::template Matrix< DBTranslatedValue >&& rows,
        const typename DatabaseTable::template DBVector< IsMissing >&  rows_have_missing_vals) {
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

      IDatabaseTable< DBTranslatedValue >::insertRows(std::move(rows), rows_have_missing_vals);
    }

    /// insert a set of new DBRows at the end of the database
    void DatabaseTable::insertRows(
        const typename DatabaseTable::template Matrix< DBTranslatedValue >& new_rows,
        const typename DatabaseTable::template DBVector< IsMissing >& rows_have_missing_vals) {
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

      IDatabaseTable< DBTranslatedValue >::insertRows(new_rows, rows_have_missing_vals);
    }

    /// insert a set of new DBRows at the end of the database
    void DatabaseTable::insertRows(typename DatabaseTable::template Matrix< DBCell >&& new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }

    /// insert a set of new DBRows at the end of the database
    void DatabaseTable::insertRows(
        const typename DatabaseTable::template Matrix< DBCell >& new_rows) {
      GUM_ERROR(NotImplementedYet, "not implemented yet")
    }

    /// erase the content of the database, including the names of the variables
    void DatabaseTable::clear() {
      _translators_.clear();
      _ignored_cols_.clear();
      IDatabaseTable< DBTranslatedValue >::clear();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

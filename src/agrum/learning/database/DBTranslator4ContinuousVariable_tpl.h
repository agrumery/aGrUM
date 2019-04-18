
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
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <utility>
#  include <vector>
#  include <limits>

#  include <agrum/learning/database/DBTranslator4ContinuousVariable.h>
#  include <agrum/learning/database/DBCell.h>

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::CONTINUOUS,
                              missing_symbols,
                              fit_range,
                              1,
                              alloc),
        __variable("var", ""), __fit_range(fit_range) {
      // Here, if fit_range is set to false, and the range of the
      // random variable will remain (-inf,+inf). So all the missing symbols
      // that are numbers should be discarded since they lie in the domain
      // of the variable. On the other hand, if fit_range is true, each newly
      // observed value will update the range of the variable, so that, again,
      // all the missing symbols that are numbers should be discarded since
      // they always end up lying in the domain of the variable.
      for (auto iter = this->_missing_symbols.beginSafe();
           iter != this->_missing_symbols.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) { this->_missing_symbols.erase(iter); }
      }

      // the remaining symbols are not numbers. Take the first one as
      // the default missing symbols for back translations.
      if (!this->_missing_symbols.empty()) {
        __nonfloat_missing_symbol = *(this->_missing_symbols.begin());
      }

      // if fit_range is true, we shall be able to update the ranges of
      // the continuous variable. To indicate that we did not encountered any
      // value yet in the database, we fix the lower bound of __variable to +max
      if (__fit_range)
        __variable.setLowerBound(std::numeric_limits< float >::infinity());

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = __variable.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const bool fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::CONTINUOUS, fit_range, 1, alloc),
        __variable("var", ""), __fit_range(fit_range) {
      // if fit_range is true, we shall be able to update the ranges of
      // the continuous variable. To indicate that we did not encountered any
      // value yet in the database, we fix the lower bound of __variable to +max
      if (__fit_range)
        __variable.setLowerBound(std::numeric_limits< float >::infinity());

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = __variable.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// default constructor with a continuous variable as translator
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR, template < typename > class XALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const ContinuousVariable< GUM_SCALAR >&                  var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::CONTINUOUS,
                              missing_symbols,
                              fit_range,
                              1,
                              alloc),
        __variable(var.name(), var.description()), __fit_range(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = float(var.lowerBound());
      const float upper_bound = float(var.upperBound());
      __variable.setLowerBound(lower_bound);
      __variable.setUpperBound(upper_bound);

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      bool non_float_symbol_found = false;
      for (auto iter = this->_missing_symbols.beginSafe();
           iter != this->_missing_symbols.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->_missing_symbols.erase(iter);
          } else
            __status_float_missing_symbols.insert(*iter, false);
        } else if (!non_float_symbol_found) {
          non_float_symbol_found = true;
          __nonfloat_missing_symbol = *iter;
        }
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// default constructor with a continuous variable as translator
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const ContinuousVariable< GUM_SCALAR >& var,
       const bool                              fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::CONTINUOUS, fit_range, 1, alloc),
        __variable(var.name(), var.description()), __fit_range(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = float(var.lowerBound());
      const float upper_bound = float(var.upperBound());
      __variable.setLowerBound(lower_bound);
      __variable.setUpperBound(upper_bound);

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// default constructor with a IContinuous variable as translator
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const IContinuousVariable&                               var,
       const std::vector< std::string, XALLOC< std::string > >& missing_symbols,
       const bool                                               fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(DBTranslatedValueType::CONTINUOUS,
                              missing_symbols,
                              fit_range,
                              1,
                              alloc),
        __variable(var.name(), var.description()), __fit_range(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = float(var.lowerBoundAsDouble());
      const float upper_bound = float(var.upperBoundAsDouble());
      __variable.setLowerBound(lower_bound);
      __variable.setUpperBound(upper_bound);

      // remove all the missing symbols corresponding to a number between
      // lower_bound and upper_bound
      bool non_float_symbol_found = false;
      for (auto iter = this->_missing_symbols.beginSafe();
           iter != this->_missing_symbols.endSafe();
           ++iter) {
        if (DBCell::isReal(*iter)) {
          const float missing_val = std::stof(*iter);
          if ((missing_val >= lower_bound) && (missing_val <= upper_bound)) {
            this->_missing_symbols.erase(iter);
          } else
            __status_float_missing_symbols.insert(*iter, false);
        } else if (!non_float_symbol_found) {
          non_float_symbol_found = true;
          __nonfloat_missing_symbol = *iter;
        }
      }

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// default constructor with a IContinuous variable as translator
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const IContinuousVariable& var,
       const bool                 fit_range,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(
           DBTranslatedValueType::CONTINUOUS, fit_range, 1, alloc),
        __variable(var.name(), var.description()), __fit_range(fit_range) {
      // get the bounds of the range variable
      const float lower_bound = var.lowerBoundAsDouble();
      const float upper_bound = var.upperBoundAsDouble();
      __variable.setLowerBound(lower_bound);
      __variable.setUpperBound(upper_bound);

      // store a copy of the variable, that should be used by method variable ()
      __real_variable = var.clone();

      GUM_CONSTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const DBTranslator4ContinuousVariable< ALLOC >& from,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(from, alloc),
        __variable(from.__variable),
        __status_float_missing_symbols(from.__status_float_missing_symbols),
        __nonfloat_missing_symbol(from.__nonfloat_missing_symbol),
        __fit_range(from.__fit_range) {
      // store a copy of the variable, that should be used by method variable ()
      __real_variable = from.__real_variable->clone();

      GUM_CONS_CPY(DBTranslator4ContinuousVariable);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       const DBTranslator4ContinuousVariable< ALLOC >& from) :
        DBTranslator4ContinuousVariable< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       DBTranslator4ContinuousVariable< ALLOC >&& from,
       const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
          alloc) :
        DBTranslator< ALLOC >(std::move(from), alloc),
        __variable(std::move(from.__variable)),
        __status_float_missing_symbols(
           std::move(from.__status_float_missing_symbols)),
        __nonfloat_missing_symbol(std::move(from.__nonfloat_missing_symbol)),
        __fit_range(from.__fit_range) {
      // store a copy of the variable, that should be used by method variable ()
      __real_variable = from.__real_variable;
      from.__real_variable = nullptr;

      GUM_CONS_MOV(DBTranslator4ContinuousVariable);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >::DBTranslator4ContinuousVariable(
       DBTranslator4ContinuousVariable< ALLOC >&& from) :
        DBTranslator4ContinuousVariable< ALLOC >(std::move(from),
                                                 from.getAllocator()) {}


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >*
       DBTranslator4ContinuousVariable< ALLOC >::clone(
          const typename DBTranslator4ContinuousVariable< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< DBTranslator4ContinuousVariable< ALLOC > > allocator(alloc);
      DBTranslator4ContinuousVariable< ALLOC >* translator = allocator.allocate(1);
      try {
        allocator.construct(translator, *this, alloc);
      } catch (...) {
        allocator.deallocate(translator, 1);
        throw;
      }
      return translator;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4ContinuousVariable< ALLOC >*
           DBTranslator4ContinuousVariable< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE DBTranslator4ContinuousVariable<
       ALLOC >::~DBTranslator4ContinuousVariable() {
      if (__real_variable != nullptr) delete __real_variable;

      GUM_DESTRUCTOR(DBTranslator4ContinuousVariable);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >&
          DBTranslator4ContinuousVariable< ALLOC >::
          operator=(const DBTranslator4ContinuousVariable< ALLOC >& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(from);
        __variable = from.__variable;
        __status_float_missing_symbols = from.__status_float_missing_symbols;
        __nonfloat_missing_symbol = from.__nonfloat_missing_symbol;
        __fit_range = from.__fit_range;

        if (__real_variable != nullptr) delete __real_variable;
        __real_variable = from.__real_variable->clone();
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBTranslator4ContinuousVariable< ALLOC >&
          DBTranslator4ContinuousVariable< ALLOC >::
          operator=(DBTranslator4ContinuousVariable< ALLOC >&& from) {
      if (this != &from) {
        DBTranslator< ALLOC >::operator=(std::move(from));
        __variable = std::move(from.__variable);
        __status_float_missing_symbols =
           std::move(from.__status_float_missing_symbols);
        __nonfloat_missing_symbol = std::move(from.__nonfloat_missing_symbol);
        __fit_range = from.__fit_range;

        if (__real_variable != nullptr) delete __real_variable;
        __real_variable = from.__real_variable;
        from.__real_variable = nullptr;
      }

      return *this;
    }


    /// returns the translation of a string, as found in the current dictionary
    template < template < typename > class ALLOC >
    DBTranslatedValue DBTranslator4ContinuousVariable< ALLOC >::translate(
       const std::string& str) {
      // check if the string is actually a number
      if (!DBCell::isReal(str)) {
        if (this->isMissingSymbol(str)) {
          return DBTranslatedValue{std::numeric_limits< float >::max()};
        } else
          GUM_ERROR(TypeError,
                    "String \""
                       << str
                       << "\" cannot be translated because it is not a number");
      }

      // here we know that the string is a number
      const float number = std::stof(str);

      // if we are in the range of the variable, return the number
      if (__variable.belongs(number)) return DBTranslatedValue{number};

      // check that this is not a missing value
      if (this->isMissingSymbol(str)) {
        if (!__status_float_missing_symbols[str]) {
          __status_float_missing_symbols[str] = true;
        }
        return DBTranslatedValue{std::numeric_limits< float >::max()};
      }

      // check if we are allowed to update the domain of the variable
      if (!__fit_range) {
        GUM_ERROR(UnknownLabelInDatabase,
                  "String \"" << str
                              << "\" cannot be translated because it is "
                                 "out of the domain of the continuous variable");
      }

      // now, we can try to add str as a new bound of the range variable
      // if possible

      // if the variable is empty, set the min and max ranges. Here,
      // there is no need to check whether the new range would contain an
      // already translated missing symbol because this was already tested
      // in the above test.
      if (__variable.lowerBound() == std::numeric_limits< float >::infinity()) {
        __variable.setLowerBound(number);
        __variable.setUpperBound(number);
        return DBTranslatedValue{number};
      }

      // here, the domain is not empty. So we should update either the
      // lower bound or the upper bound of the variable, unless
      // a missing symbol lies within the new bounds and we have already
      // translated it.
      const float lower_bound = __variable.lowerBound();
      const float upper_bound = __variable.upperBound();
      if (number < lower_bound) {
        // check that there does not already exist a translated missing
        // value within the new bounds of the variable
        for (const auto& missing : __status_float_missing_symbols) {
          if (missing.second) {
            const float miss_val = std::stof(missing.first);
            if ((miss_val >= number) && (miss_val <= upper_bound)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \""
                           << str << "\" cannot be translated because "
                           << "it would induce a new domain containing an already "
                           << "translated missing symbol");
            }
          }
        }

        // remove all the missing symbols that were not translated yet and
        // that lie within the new bounds of the variable
        for (auto iter = __status_float_missing_symbols.beginSafe();
             iter != __status_float_missing_symbols.endSafe();
             ++iter) {
          if (iter.val() == false) {
            const float miss_val = std::stof(iter.key());
            if ((miss_val >= number) && (miss_val <= upper_bound)) {
              this->_missing_symbols.erase(iter.key());
              __status_float_missing_symbols.erase(iter);
            }
          }
        }

        // update the domain of the continuous variable
        __variable.setLowerBound(number);

        return DBTranslatedValue{number};
      } else {
        // check that there does not already exist a translated missing
        // value within the new bounds of the variable
        for (const auto& missing : __status_float_missing_symbols) {
          if (missing.second) {
            const float miss_val = std::stof(missing.first);
            if ((miss_val >= lower_bound) && (miss_val <= number)) {
              GUM_ERROR(OperationNotAllowed,
                        "String \""
                           << str << "\" cannot be translated because "
                           << "it would induce a new domain containing an already "
                           << "translated missing symbol");
            }
          }
        }

        // remove all the missing symbols that were not translated yet and
        // that lie within the new bounds of the variable
        for (auto iter = __status_float_missing_symbols.beginSafe();
             iter != __status_float_missing_symbols.endSafe();
             ++iter) {
          if (iter.val() == false) {
            const float miss_val = std::stof(iter.key());
            if ((miss_val >= lower_bound) && (miss_val <= number)) {
              this->_missing_symbols.erase(iter.key());
              __status_float_missing_symbols.erase(iter);
            }
          }
        }

        // update the domain of the continuous variable
        __variable.setUpperBound(number);

        return DBTranslatedValue{number};
      }
    }


    /// returns the original value for a given translation
    template < template < typename > class ALLOC >
    INLINE std::string DBTranslator4ContinuousVariable< ALLOC >::translateBack(
       const DBTranslatedValue translated_val) const {
      if (translated_val.cont_val == std::numeric_limits< float >::max()) {
        if (!__nonfloat_missing_symbol.empty()) return __nonfloat_missing_symbol;
        if (this->_missing_symbols.empty())
          return *(this->_missing_symbols.begin());
      }

      if ((translated_val.cont_val < __variable.lowerBound())
          || (translated_val.cont_val > __variable.upperBound())) {
        GUM_ERROR(UnknownLabelInDatabase,
                  "The back translation of "
                     << translated_val.cont_val
                     << " could not be found because the value is outside the "
                     << "domain of the continuous variable");
      }

      char buffer[100];
      sprintf(buffer, "%g", translated_val.cont_val);
      return std::string(buffer);
    }


    /// indicates whether the translations should be reordered
    template < template < typename > class ALLOC >
    INLINE bool DBTranslator4ContinuousVariable< ALLOC >::needsReordering() const {
      return false;
    }


    /// returns a mapping to reorder the current dictionary and updates it
    template < template < typename > class ALLOC >
    INLINE HashTable< std::size_t,
                      std::size_t,
                      ALLOC< std::pair< std::size_t, std::size_t > > >
           DBTranslator4ContinuousVariable< ALLOC >::reorder() {
      return HashTable< std::size_t,
                        std::size_t,
                        ALLOC< std::pair< std::size_t, std::size_t > > >();
    }


    /// returns the domain size of a variable corresponding to the translations
    template < template < typename > class ALLOC >
    INLINE std::size_t
           DBTranslator4ContinuousVariable< ALLOC >::domainSize() const {
      return std::numeric_limits< std::size_t >::max();
    }


    /// returns the variable stored into the translator
    template < template < typename > class ALLOC >
    INLINE const IContinuousVariable*
                 DBTranslator4ContinuousVariable< ALLOC >::variable() const {
      __real_variable->setLowerBoundFromDouble(__variable.lowerBound());
      __real_variable->setUpperBoundFromDouble(__variable.upperBound());
      return __real_variable;
    }


    /// returns the translation of a missing value
    template < template < typename > class ALLOC >
    INLINE DBTranslatedValue
           DBTranslator4ContinuousVariable< ALLOC >::missingValue() const {
      return DBTranslatedValue{std::numeric_limits< float >::max()};
    }


  } /* namespace learning */

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

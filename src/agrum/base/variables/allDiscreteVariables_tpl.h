/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


#include <algorithm>
#include <cstdio>
#include <iostream>

#include <agrum/base/variables/allDiscreteVariables.h>

namespace gum {
  template < typename GUM_SCALAR >
  std::unique_ptr< DiscreteVariable > fastVariable(std::string var_description,
                                                   Size        default_domain_size) {
    if (default_domain_size < 1)
      GUM_ERROR(InvalidArgument, "default_domain_size can not be less than 1")

    const std::string domain = "[" + std::to_string(default_domain_size) + "]";
    return fastVariable< GUM_SCALAR >(var_description, domain);
  }

  template < typename GUM_SCALAR >
  std::unique_ptr< DiscreteVariable > fastVariable(std::string        var_description,
                                                   const std::string& default_domain) {
    Size                       ds        = 0;
    long                       range_min = 0;
    long                       range_max = 1;
    std::vector< std::string > labels;
    std::vector< GUM_SCALAR >  ticks;
    std::string                name;

    std::vector< int >    values;
    std::vector< double > numerical_values;

    trim(var_description);

    if (default_domain.size() == 0) GUM_ERROR(InvalidArgument, "default_domain can not be empty")

    auto t = *default_domain.begin();
    if (t != '[' && t != '{')
      GUM_ERROR(InvalidArgument,
                "default_domain (" << default_domain << ") must start with '[' or '{'")

    if (var_description.find('[') == std::string::npos
        && var_description.find('{') == std::string::npos) {
      var_description += default_domain;
    }

    // [1,3,5]...
    if (*(var_description.rbegin()) == ']') {
      auto posBrack = var_description.find('[');
      if (posBrack != std::string::npos) {
        name = var_description.substr(0, posBrack);
        const auto& s_args
            = var_description.substr(posBrack + 1, var_description.size() - posBrack - 2);
        const auto& args = split(s_args, ",");
        if (args.empty()) {              // n[]
          GUM_ERROR(InvalidArgument, "Empty range for variable " << var_description)
        } else if (args.size() == 1) {   // n[4] or n[0:5.5:10]

          const auto& labels = split(args[0], ":");

          if (labels.size() == 3) {   // b{1.1:3.31:5}
            const double fmin = std::stod(labels[0]);
            const double fmax = std::stod(labels[1]);
            const int    nbr  = std::stoi(labels[2]);

            if (fmax <= fmin) { GUM_ERROR(InvalidArgument, "last<=first in " << var_description) }
            if (nbr <= 1) { GUM_ERROR(InvalidArgument, "nbr<=1 in " << var_description) }
            const double step    = double((fmax - fmin) / nbr);
            double       current = fmin;
            for (auto i = 0; i <= nbr; i += 1) {
              ticks.push_back(current);
              current += step;
            }
            ds = ticks.size();
          } else {   // n[4]
            int n = std::stoi(args[0]);
            if (n < 2)
              if (default_domain != "[1]")
                GUM_ERROR(InvalidArgument, n << " is not >=2 for variable " << var_description)
            ds        = static_cast< Size >(n);
            range_min = 0;
            range_max = long(ds) - 1;
          }
        } else if (args.size() == 2) {   // n[5,10]
          range_min = std::stol(args[0]);
          range_max = std::stol(args[1]);

          if (range_max < range_min)
            GUM_ERROR(InvalidArgument,
                      "Invalid range for variable " << var_description << ": max<min")
          if (range_max == range_min && default_domain != "[1]")
            GUM_ERROR(InvalidArgument,
                      "Invalid range for variable "
                          << var_description << ": max==min not allowed if default_domain_size>1")

          ds = static_cast< Size >(1 + range_max - range_min);
        } else {   // n[3.14,5,10,12]
          for (const auto& tick: args) {
            ticks.push_back(static_cast< GUM_SCALAR >(std::strtod(tick.c_str(), nullptr)));
          }
          ds = args.size() - 1;
        }
      }
      // var_description like "n{one|two|three}" or b{1.1:3.31:5}
    } else if (*(var_description.rbegin()) == '}') {
      auto posBrack = var_description.find('{');
      if (posBrack != std::string::npos) {
        name   = var_description.substr(0, posBrack);
        labels = split(var_description.substr(posBrack + 1, var_description.size() - posBrack - 2),
                       ":");
        if (labels.size() == 3) {   // b{1.1:3.31:5}
          const auto fmin = std::stod(labels[0]);
          const auto fmax = std::stod(labels[1]);
          const int  nbr  = std::stoi(labels[2]);

          if (fmax <= fmin) { GUM_ERROR(InvalidArgument, "last<=first in " << var_description) }
          if (nbr <= 1) { GUM_ERROR(InvalidArgument, "nbr<=1 in " << var_description) }
          const double step = double((fmax - fmin) / (nbr - 1));

          if ((trunc(step) == step) && (trunc(fmin) == fmin)
              && (trunc(fmax) == fmax)) {   // b{1:6:5} => IntegerVariable
            labels.clear();
            numerical_values.clear();
            int v = int(fmin);
            for (int i = 1; i <= nbr; i++) {
              labels.push_back(std::to_string(v));
              v += int(step);
            }
            ds = labels.size();
          } else {   // b{1.3:6.3:5} => NumericalDiscreteVariable
            labels.clear();
            numerical_values.clear();
            ds       = nbr;
            double v = fmin;
            numerical_values.push_back(v);
            for (auto i = 1; i < nbr - 1; i++) {
              v += step;
              numerical_values.push_back(v);
            }
            numerical_values.push_back(fmax);
          }
        } else {
          labels
              = split(var_description.substr(posBrack + 1, var_description.size() - posBrack - 2),
                      "|");
          if (labels.size() < 2) {
            if (labels.size() == 1
                && default_domain != "[1]")   // 1 is ok if default_domain_size==1
              GUM_ERROR(InvalidArgument, "Not enough labels in var_description " << var_description)
          }
          if (!hasUniqueElts(labels)) {
            GUM_ERROR(InvalidArgument, "Duplicate labels in var_description " << var_description)
          }
          ds = labels.size();
        }
      }
    } else {
      name = var_description;
    }

    if (ds == 0) {
      GUM_ERROR(InvalidArgument, "No value for variable " << var_description << ".")
    } else if (ds == 1) {
      if (default_domain != "[1]")
        GUM_ERROR(InvalidArgument,
                  "Only one value for variable " << var_description << " (2 at least are needed).")
    }

    if (!labels.empty()) {
      if (std::all_of(labels.cbegin(), labels.cend(), isInteger)) {
        for (const auto& label: labels)
          values.push_back(std::stoi(label));
        if (values.size() > 2) {   // there can be an enumeration of consecutive integers
          std::sort(values.begin(), values.end());
          range_min     = values[0];
          auto v        = range_min;
          bool is_range = true;
          for (const auto item: values) {
            if (item != v) { is_range = false; }
            v += 1;
          }
          if (is_range) {
            values.clear();   // not an IntegerVariable but rather a RangeVariable
            labels.clear();
            range_max = v - 1;
          }
        }
      } else if (std::all_of(labels.cbegin(), labels.cend(), isNumerical))
        for (const auto& label: labels)
          numerical_values.push_back(std::stod(label));
    }

    trim(name);

    if (!values.empty()) {
      return std::make_unique< IntegerVariable >(name, name, values);
    } else if (!numerical_values.empty()) {
      if (!std::all_of(numerical_values.cbegin(),
                       numerical_values.cend(),
                       gum::isfinite< double >)) {
        GUM_ERROR(DefaultInLabel, "Infinite value is not allowed for variable " << name)
      }
      return std::make_unique< NumericalDiscreteVariable >(name, name, numerical_values);
    } else if (!labels.empty()) {
      return std::make_unique< LabelizedVariable >(name, name, labels);
    } else if (!ticks.empty()) {
      if (!std::all_of(ticks.cbegin(), ticks.cend(), gum::isfinite< double >)) {
        GUM_ERROR(DefaultInLabel, "Infinite value is not allowed for variable " << name)
      }
      // if last character of name is a +, we remove the character from the name and set empirical
      // to True
      if (name.back() == '+') {
        name.pop_back();
        trim(name);
        return std::make_unique< DiscretizedVariable< GUM_SCALAR > >(name, name, ticks, true);
      } else {
        return std::make_unique< DiscretizedVariable< GUM_SCALAR > >(name, name, ticks);
      }
    } else {
      return std::make_unique< RangeVariable >(name, name, range_min, range_max);
    }
  }
}   // namespace gum

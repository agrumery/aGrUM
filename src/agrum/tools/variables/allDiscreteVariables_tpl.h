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

#include <cstdio>
#include <iostream>
#include <algorithm>

#include <agrum/tools/variables/allDiscreteVariables.h>

namespace gum {
  template < typename GUM_SCALAR >
  std::unique_ptr< DiscreteVariable > fastVariable(const std::string& var_description,
                                                   Size               default_domain_size) {
    if (default_domain_size == 0) GUM_ERROR(InvalidArgument, "default_domain_size can not be 0")

    Size                       ds        = default_domain_size;
    long                       range_min = 0;
    long                       range_max = long(ds) - 1;
    std::vector< std::string > labels;
    std::vector< GUM_SCALAR >  ticks;
    std::string                name;

    if (*(var_description.rbegin()) == ']') {
      auto posBrack = var_description.find('[');
      if (posBrack != std::string::npos) {
        name = var_description.substr(0, posBrack);
        const auto& s_args
           = var_description.substr(posBrack + 1, var_description.size() - posBrack - 2);
        const auto& args = split(s_args, ",");
        if (args.empty()) {   // n[]
          GUM_ERROR(InvalidArgument, "Empty range for variable " << var_description)
        } else if (args.size() == 1) {   // n[4]
          int n     = std::stoi(args[0]);
          if (n<2)
            GUM_ERROR(InvalidArgument, n<<" is not >=2 for variable " << var_description)
          ds        = static_cast< Size >(n);
          range_min = 0;
          range_max = long(ds) - 1;
        } else if (args.size() == 2) {   // n[5,10]
          range_min = std::stol(args[0]);
          range_max = std::stol(args[1]);

          if (range_max < range_min)
            GUM_ERROR(InvalidArgument,
                      "Invalid range for variable " << var_description << ": max<min")
          if (range_max == range_min && default_domain_size > 1)
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
    } else if (*(var_description.rbegin()) == '}') {   // var_description like "n{one|two|three}"
      auto posBrack = var_description.find('{');
      if (posBrack != std::string::npos) {
        name   = var_description.substr(0, posBrack);
        labels = split(var_description.substr(posBrack + 1, var_description.size() - posBrack - 2),
                       "|");
        if (labels.size() < 2) {
          if (labels.size() != default_domain_size)   // 1 is ok if default_domain_size==1
            GUM_ERROR(InvalidArgument, "Not enough labels in var_description " << var_description)
        }
        if (!hasUniqueElts(labels)) {
          GUM_ERROR(InvalidArgument, "Duplicate labels in var_description " << var_description)
        }
        ds = labels.size();
      }
    } else {
      name = var_description;
    }

    if (ds == 0) {
      GUM_ERROR(InvalidArgument, "No value for variable " << var_description << ".")
    } else if (ds == 1) {
      if (default_domain_size != 1)
        GUM_ERROR(InvalidArgument,
                  "Only one value for variable " << var_description << " (2 at least are needed).")
    }

    std::vector< int >    values;
    std::vector< double > double_values;
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
          double_values.push_back(std::stod(label));
    }

    trim(name);

    if (!values.empty()) {
      return std::make_unique< IntegerVariable >(name, name, values);
    } else if (!double_values.empty()) {
      return std::make_unique< NumericalDiscreteVariable >(name, name, double_values);
    } else if (!labels.empty()) {
      return std::make_unique< LabelizedVariable >(name, name, labels);
    } else if (!ticks.empty()) {
      return std::make_unique< DiscretizedVariable< GUM_SCALAR > >(name, name, ticks);
    } else {
      return std::make_unique< RangeVariable >(name, name, range_min, range_max);
    }
  }
}   // namespace gum
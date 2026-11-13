/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of the parsing of the GUM formats.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <set>
#include <string>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/io/GumJson.h>

namespace gum::detail {
  namespace {
    using json = nlohmann::json;

    /// builds the JSON value as nlohmann::json::parse does, but refuses an object (a msgpack map)
    /// with the same key twice: nlohmann keeps the last value, silently
    class UniqueKeysSax final: public nlohmann::json_sax< json > {
      public:
      explicit UniqueKeysSax(json& result) : _dom_(result, true) {}

      bool null() override { return _dom_.null(); }

      bool boolean(bool val) override { return _dom_.boolean(val); }

      bool number_integer(number_integer_t val) override { return _dom_.number_integer(val); }

      bool number_unsigned(number_unsigned_t val) override { return _dom_.number_unsigned(val); }

      bool number_float(number_float_t val, const string_t& s) override {
        return _dom_.number_float(val, s);
      }

      bool string(string_t& val) override { return _dom_.string(val); }

      bool binary(binary_t& val) override { return _dom_.binary(val); }

      bool start_object(std::size_t elements) override {
        _keys_.emplace_back();
        return _dom_.start_object(elements);
      }

      bool key(string_t& val) override {
        if (!_keys_.back().insert(val).second)
          GUM_ERROR(IOError, "the key '" << val << "' is written twice in an object")
        return _dom_.key(val);
      }

      bool end_object() override {
        _keys_.pop_back();
        return _dom_.end_object();
      }

      bool start_array(std::size_t elements) override { return _dom_.start_array(elements); }

      bool end_array() override { return _dom_.end_array(); }

      bool parse_error(std::size_t                        position,
                       const std::string&                 last_token,
                       const nlohmann::detail::exception& ex) override {
        return _dom_.parse_error(position, last_token, ex);   // throws (exceptions allowed)
      }

      private:
      /// the builder of nlohmann (the input adapter only types its optional lexer, unused here)
      nlohmann::detail::
          json_sax_dom_parser< json, nlohmann::detail::iterator_input_adapter< const char* > >
              _dom_;

      /// the keys already read in each object being read
      std::vector< std::set< std::string > > _keys_;
    };

    /// parses input with the format, the errors of nlohmann becoming IOError
    template < typename INPUT >
    json parse(INPUT&& input, json::input_format_t format, const char* what) {
      json          result;
      UniqueKeysSax sax(result);
      try {
        json::sax_parse(std::forward< INPUT >(input), &sax, format);
      } catch (const json::exception& e) {
        GUM_ERROR(IOError, "not a " << what << " (" << e.what() << ")")
      }
      return result;
    }
  }   // namespace

  json parseGumText(std::string_view text) {
    return parse(text, json::input_format_t::json, "JSON text");
  }

  json parseGumText(std::istream& input) {
    return parse(input, json::input_format_t::json, "JSON text");
  }

  json parseGumBinary(const std::vector< uint8_t >& payload) {
    return parse(payload, json::input_format_t::msgpack, "msgpack content");
  }
}   // namespace gum::detail

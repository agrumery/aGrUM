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


/**
 * @file
 * @brief The class for the NML penalty used in 3off2
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE KNML::KNML(const DBRowGeneratorParser&                                 parser,
                      const Prior&                                                prior,
                      const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                      const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        IndependenceTest(parser, prior, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(KNML);
    }


    /// default constructor
    INLINE KNML::KNML(const DBRowGeneratorParser&             parser,
                      const Prior&                            prior,
                      const Bijection< NodeId, std::size_t >& nodeId2columns) :
        IndependenceTest(parser, prior, nodeId2columns) {
      GUM_CONSTRUCTOR(KNML);
    }


    /// copy constructor
    INLINE KNML::KNML(const KNML& from) :
        IndependenceTest(from), _param_complexity_(from._param_complexity_) {
      GUM_CONS_CPY(KNML);
    }


    /// move constructor
    INLINE KNML::KNML(KNML&& from) :
        IndependenceTest(std::move(from)), _param_complexity_(std::move(from._param_complexity_)) {
      GUM_CONS_MOV(KNML);
    }


    /// virtual copy constructor
    INLINE KNML* KNML::clone() const { return new KNML(*this); }


    /// destructor
    INLINE KNML::~KNML() { GUM_DESTRUCTOR(KNML); }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

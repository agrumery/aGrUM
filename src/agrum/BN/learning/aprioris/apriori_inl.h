/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE Apriori::Apriori(const DatabaseTable&                    database,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        database_(&database),
        nodeId2columns_(nodeId2columns) {
      GUM_CONSTRUCTOR(Apriori);
    }


    /// copy constructor
    INLINE Apriori::Apriori(const Apriori& from) :
        weight_(from.weight_), database_(from.database_), nodeId2columns_(from.nodeId2columns_) {
      GUM_CONS_CPY(Apriori);
    }


    /// move constructor
    INLINE Apriori::Apriori(Apriori&& from) :
        weight_(from.weight_), database_(from.database_),
        nodeId2columns_(std::move(from.nodeId2columns_)) {
      GUM_CONS_MOV(Apriori);
    }


    /// destructor
    INLINE Apriori::~Apriori() { GUM_DESTRUCTOR(Apriori); }


    /// sets the weight of the a priori (kind of effective sample size)
    INLINE void Apriori::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds, "A negative weight (" << weight << ") is forbidden for an apriori");
      }
      weight_ = weight;
    }


    /// returns the weight assigned to the apriori
    INLINE double Apriori::weight() const { return weight_; }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

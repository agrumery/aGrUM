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
 * @brief the internal apriori for the BDeu score (N' / (r_i * q_i)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE AprioriBDeu::AprioriBDeu(const DatabaseTable&                    database,
                                    const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Apriori(database, nodeId2columns) {
      GUM_CONSTRUCTOR(AprioriBDeu);
    }


    /// copy constructor
    INLINE AprioriBDeu::AprioriBDeu(const AprioriBDeu& from) : Apriori(from) {
      GUM_CONS_CPY(AprioriBDeu);
    }


    /// move constructor
    INLINE AprioriBDeu::AprioriBDeu(AprioriBDeu&& from) : Apriori(std::move(from)) {
      GUM_CONS_MOV(AprioriBDeu);
    }


    /// virtual copy constructor
    INLINE AprioriBDeu* AprioriBDeu::clone() const { return new AprioriBDeu(*this); }


    /// destructor
    INLINE AprioriBDeu::~AprioriBDeu() { GUM_DESTRUCTOR(AprioriBDeu); }


    /// copy operator
    INLINE AprioriBDeu& AprioriBDeu::operator=(const AprioriBDeu& from) {
      Apriori::operator=(from);
      return *this;
    }


    /// move operator
    INLINE AprioriBDeu& AprioriBDeu::operator=(AprioriBDeu&& from) {
      Apriori::operator=(std::move(from));
      return *this;
    }


    /// sets the effective sample size N' (alias of setEffectiveSampleSize ())
    INLINE void AprioriBDeu::setWeight(const double weight) {
      if (weight < 0.0) {
        GUM_ERROR(OutOfBounds,
                  "A negative weight (" << weight << ") is forbidden for the BDeu apriori");
      }
      this->weight_ = weight;
    }


    /// sets the effective sample size N'
    INLINE void AprioriBDeu::setEffectiveSampleSize(const double weight) { setWeight(weight); }


    /// indicates whether an apriori is of a certain type
    INLINE bool AprioriBDeu::isOfType(const std::string& type) {
      return AprioriBDeuType::isOfType(type);
    }


    /// returns the type of the apriori
    INLINE const std::string& AprioriBDeu::getType() const { return AprioriBDeuType::type; }


    /// indicates whether the apriori is potentially informative
    INLINE bool AprioriBDeu::isInformative() const { return this->weight_ != 0.0; }


    /// returns the apriori vector all the variables in the idset
    INLINE void AprioriBDeu::addAllApriori(const IdCondSet& idset, std::vector< double >& counts) {
      // if the idset is empty or the weight is zero, the apriori is also empty
      if (idset.empty() || (this->weight_ == 0.0)) return;

      // otherwise, add the weight to all the cells in the counting vector
      const double weight = this->weight_ / counts.size();
      for (auto& count: counts)
        count += weight;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

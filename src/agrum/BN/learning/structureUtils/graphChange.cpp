
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
 * @brief A class to account for changes in a graph
 *
 * This class shall be used by learning algorithms to notify scores, structural
 * constraints, etc, that the learnt graph has been modified.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/learning/structureUtils/graphChange.h>

#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/structureUtils/graphChange_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  namespace learning {

    /// put the content of the GraphChange into a string
    std::string GraphChange::toString() const {
      std::stringstream stream;
      switch (type()) {
        case GraphChangeType::ARC_ADDITION:
          stream << "ArcAddition ( " << node1() << " , " << node2() << " )";
          return stream.str();

        case GraphChangeType::ARC_DELETION:
          stream << "ArcDeletion ( " << node1() << " , " << node2() << " )";
          return stream.str();

        case GraphChangeType::ARC_REVERSAL:
          stream << "ArcReversal ( " << node1() << " , " << node2() << " )";
          return stream.str();

        case GraphChangeType::EDGE_ADDITION:
          stream << "EdgeAddition ( " << node1() << " , " << node2() << " )";
          return stream.str();

        case GraphChangeType::EDGE_DELETION:
          stream << "EdgeDeletion ( " << node1() << " , " << node2() << " )";
          return stream.str();

        default:
          GUM_ERROR(OperationNotAllowed,
                    "this graph modification is not supported yet");
      }
    }

    /// put the content of the ArcAddition into a string
    std::string ArcAddition::toString() const {
      std::stringstream stream;
      stream << "ArcAddition ( " << node1() << " , " << node2() << " )";
      return stream.str();
    }

    /// put the content of the ArcDeletion into a string
    std::string ArcDeletion::toString() const {
      std::stringstream stream;
      stream << "ArcDeletion ( " << node1() << " , " << node2() << " )";
      return stream.str();
    }

    /// put the content of the ArcReversal into a string
    std::string ArcReversal::toString() const {
      std::stringstream stream;
      stream << "ArcReversal ( " << node1() << " , " << node2() << " )";
      return stream.str();
    }

    /// put the content of the EdgeAddition into a string
    std::string EdgeAddition::toString() const {
      std::stringstream stream;
      stream << "EdgeAddition ( " << node1() << " , " << node2() << " )";
      return stream.str();
    }

    /// put the content of the EdgeDeletion into a string
    std::string EdgeDeletion::toString() const {
      std::stringstream stream;
      stream << "EdgeDeletion ( " << node1() << " , " << node2() << " )";
      return stream.str();
    }

    /// a \c << operator for GraphChanges
    std::ostream& operator<<(std::ostream& stream, const GraphChange& change) {
      return stream << change.toString();
    }

    /// a \c << operator for ArcAddition
    std::ostream& operator<<(std::ostream& stream, const ArcAddition& change) {
      return stream << change.toString();
    }

    /// a \c << operator for ArcDeletion
    std::ostream& operator<<(std::ostream& stream, const ArcDeletion& change) {
      return stream << change.toString();
    }

    /// a \c << operator for ArcReversal
    std::ostream& operator<<(std::ostream& stream, const ArcReversal& change) {
      return stream << change.toString();
    }

    /// a \c << operator for EdgeAddition
    std::ostream& operator<<(std::ostream& stream, const EdgeAddition& change) {
      return stream << change.toString();
    }

    /// a \c << operator for EdgeDeletion
    std::ostream& operator<<(std::ostream& stream, const EdgeDeletion& change) {
      return stream << change.toString();
    }

  } /* namespace learning */

} /* namespace gum */

/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Source implementation of gum::ClassElementContainer
 *
 * @author Lionel TORTI
 *
 */

#include <agrum/prm/classElementContainer.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/classElementContainer.inl>
#endif //GUM_NO_INLINE

namespace gum {
  namespace prm {


// NodeId
// ClassElementContainer::_findNodeId() {
//   Set<ClassElementContainer*> set;
//   std::vector<NodeId> candidates;
//   set.insert(this);
//   _findAllSubtypes(set);
//   candidates.push_back(_dag().insertNode());
//   bool found = false;
//   for (short count = 0; count < 1000; ++count) {
//     for (Set<ClassElementContainer*>::iterator iter = set.begin();
//          iter != set.end(); ++iter) {
//       if ((**iter).exists(candidates.back())) {
//         found = true;
//         break;
//       }
//     }
//     if (found) {
//       candidates.push_back(_dag().insertNode());
//       found = false;
//     } else {
//       NodeId id = candidates.back();
//       candidates.pop_back();
//       for (std::vector<NodeId>::iterator iter = candidates.begin();
//            iter != candidates.end(); ++iter) {
//         _dag().eraseNode(*iter);
//       }
//       return id;
//     }
//   }
//   GUM_ERROR(FatalError, "could not find a common NodeId");
// }

    void
    ClassElementContainer::_copyIOFlags( const ClassElementContainer& c ) {
      typedef HashTable< std::string, std::pair<bool, bool> >::const_iterator Iter;

      for ( Iter iter = c.__IOFlags.begin(); iter != c.__IOFlags.end(); ++iter ) {
        _setIOFlag( get( iter.key() ), *iter );
      }
    }

    std::ostream&
    operator<<( std::ostream& output, const ClassElementContainer& container ) {
      std::string tab = "  ";
      output << "digraph \"" << container.name() << "\" {" << std::endl;

      for ( DAG::NodeIterator node_iter = container.dag().beginNodes();
            node_iter != container.dag().endNodes(); ++node_iter ) {
        if ( container.dag().children( *node_iter ).size() > 0 ) {
          const NodeSet& children = container.dag().children( *node_iter );

          for ( NodeSetIterator child_iter = children.begin();
                child_iter != children.end(); ++child_iter ) {
            output << tab << "\"" << container.get( *node_iter ).name() << "\" -> "
                   << "\"" << container.get( *child_iter ).name() << "\";" << std::endl;
          }
        } else if ( container.dag().parents( *node_iter ).size() == 0 ) {
          output << tab << "\"" << container.get( *node_iter ).name() << "\";" << std::endl;
        }
      }

      output << "}" << std::endl;
      return output;
    }


  } /* namespace prm */
} /* namespace gum */


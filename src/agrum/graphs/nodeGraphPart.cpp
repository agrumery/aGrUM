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
/** @file
 * @brief Source implementation of nodes sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/nodeGraphPart.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/nodeGraphPart.inl>
#endif //GUM_NOINLINE


namespace gum {


  ///////////////////// NodeGraphPart
  NodeGraphPart::NodeGraphPart( Size nodes_size ,
                                bool nodes_resize_policy ) :
    __nodes( nodes_size,nodes_resize_policy ),
    __max( 0 ) {
    GUM_CONSTRUCTOR( NodeGraphPart );
  }

  NodeGraphPart::NodeGraphPart( const NodeGraphPart& s ) :
    __nodes( s.__nodes ),__max( s.__max ) {
    GUM_CONS_CPY( NodeGraphPart );
  }

  NodeGraphPart::~NodeGraphPart() {
    GUM_DESTRUCTOR( NodeGraphPart );
  }

  void NodeGraphPart::populateNodes( const NodeGraphPart& s ) {
    clear();
    __nodes=s.__nodes;
    __max=s.__max;
  }

  std::string NodeGraphPart::toString() const {
    std::stringstream s;
    std::string res;
    bool first=true;
    s<<"{";

    for ( NodeSetIterator it=__nodes.begin();it!=__nodes.end();++it ) {
      if ( first ) {
        first=false;
      }
      else {
        s<<",";
      }

      s<<*it;
    }

    s<<"}";

    s >> res;
    return res;
  }

  std::ostream& operator<< ( std::ostream& stream, const NodeGraphPart& set ) {
    stream<<set.toString();
    return stream;
  }


} /* namespace gum */


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
 * @brief Inline implementation of StructuredInference.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    INLINE
    void
    StructuredInference::setPatternMining( bool b ) {
      __mining = b;
    }

    INLINE
    std::string
    StructuredInference::__str( const Instance* i, const Attribute* a ) const {
      return i->name() + __dot + a->safeName();
    }

    INLINE
    std::string
    StructuredInference::__str( const Instance* i, const Attribute& a ) const {
      return i->name() + __dot + a.safeName();
    }

    INLINE
    std::string
    StructuredInference::__str( const Instance* i, const SlotChain& a ) const {
      return i->name() + __dot + a.lastElt().safeName();
    }

    INLINE
    StructuredInference::RGData::~RGData() {
      GUM_DESTRUCTOR( StructuredInference::RGData );
    }

    INLINE
    StructuredInference::PData::~PData() {
      GUM_DESTRUCTOR( StructuredInference::PData );
    }

    INLINE
    std::string
    StructuredInference::name() const {
      return "StructuredInference";
    }

    INLINE
    GSpan&
    StructuredInference::gspan() {
      return *__gspan;
    }

    INLINE
    const GSpan&
    StructuredInference::gspan() const {
      return *__gspan;
    }

    INLINE
    void
    StructuredInference::__removeNode( StructuredInference::PData& data,
                                       NodeId id,
                                       Set<Potential<prm_float>*>& pool ) {
      data.graph.eraseNode( id );
      GUM_ASSERT( not data.graph.exists( id ) );
      data.mod.erase( id );
      GUM_ASSERT( not data.mod.exists( id ) );
      data.node2attr.eraseFirst( id );
      GUM_ASSERT( not data.node2attr.existsFirst( id ) );
      data.map.erase( id );
      GUM_ASSERT( not data.map.exists( id ) );
      data.vars.eraseFirst( id );
      GUM_ASSERT( not data.vars.existsFirst( id ) );
      data.inners().erase( id );
      GUM_ASSERT( not data.inners().exists( id ) );
      pool.erase( data.pots[id] );
      GUM_ASSERT( not pool.exists( data.pots[id] ) );
      data.pots.erase( id );
      GUM_ASSERT( not data.pots.exists( id ) );
    }

  } /* namespace prm */
} /* namespace gum */


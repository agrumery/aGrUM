/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it udnder the terms of the GNU General Public License as published by *
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


#include <agrum/prm/utils_prm.h>

namespace gum {
  namespace prm {

// Decompose a string in a vector of strings using "." as separators.
    void
    decomposePath( const std::string& path, std::vector<std::string>& v ) {
      size_t prev = 0;
      size_t length = 0;
      size_t idx_1 = path.find( "." );
      size_t idx_2 = path.find(PRMObject::LEFT_CAST() );

      if ( idx_2 == std::string::npos ) {
        // ignore safe names
        size_t idx = idx_1;

        while ( idx != std::string::npos ) {
          length = idx - prev;
          v.push_back( path.substr( prev, length ) );
          prev = idx + 1;
          idx = path.find( ".", prev );
        }
      } else {
        size_t tmp = 0;

        while ( idx_1 != std::string::npos ) {
          if ( idx_1 < idx_2 ) {
            length = idx_1 - prev;
            v.push_back( path.substr( prev, length ) );
            prev = idx_1 + 1;
            idx_1 = path.find( ".", prev );
          } else if ( idx_2 < idx_1 ) {
            tmp = path.find( PRMObject::RIGHT_CAST(), idx_2 );
            idx_1 = path.find( ".", tmp );
            idx_2 = path.find(PRMObject::LEFT_CAST(), tmp );
          }
        }
      }

      v.push_back( path.substr( prev, std::string::npos ) );
    }

    Potential<prm_float>* copyPotential( const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
                                         const Potential<prm_float>& source ) {
      const MultiDimImplementation<prm_float>* impl = source.content();
      Potential<prm_float>* p = 0;

      try {
        if ( dynamic_cast< const MultiDimReadOnly<prm_float>* >( impl ) ) {
          if ( dynamic_cast< const MultiDimNoisyORCompound<prm_float>* >( impl ) ) {
            p = new Potential<prm_float>( new MultiDimNoisyORCompound<prm_float>( bij, static_cast<const MultiDimNoisyORCompound<prm_float>&>( *impl ) ) );
          } else if ( dynamic_cast< const MultiDimNoisyORNet<prm_float>* >( impl ) ) {
            p = new Potential<prm_float>( new MultiDimNoisyORNet<prm_float>( bij, static_cast<const MultiDimNoisyORNet<prm_float>&>( *impl ) ) );
          } else if ( dynamic_cast< const aggregator::MultiDimAggregator<prm_float>* >( impl ) ) {
            p = new Potential<prm_float>( static_cast<MultiDimImplementation<prm_float>*>( impl->newFactory() ) );

            for ( MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter )
              p->add( *( bij.second( *iter ) ) );
          } else if ( dynamic_cast<const MultiDimBucket<prm_float>*>( impl ) ) {
            // This is necessary just to prevent non initialized arrays
            const_cast<MultiDimBucket<prm_float>*>( static_cast<const MultiDimBucket<prm_float>*>( impl ) )->compute();

            try {
              p = new Potential<prm_float>( new MultiDimBijArray<prm_float>( bij, static_cast<const MultiDimBucket<prm_float>*>( impl )->bucket() ) );
            } catch ( OperationNotAllowed& e ) {
              // This is an empty bucket, it happens if all variables were eliminated
              return new Potential<prm_float>();
            }
          } else {
            GUM_ERROR( FatalError, "encountered an unexpected MultiDim implementation" );
          }
        } else {
          if ( dynamic_cast< const MultiDimArray<prm_float>* >( impl ) ) {
            p = new Potential<prm_float>( new MultiDimBijArray<prm_float>( bij, static_cast< const MultiDimArray<prm_float>& >( *impl ) ) );
          } else if ( dynamic_cast<const MultiDimBijArray<prm_float>*>( impl ) ) {
            p = new Potential<prm_float>( new MultiDimBijArray<prm_float>( bij, static_cast<const MultiDimBijArray<prm_float>&>( *impl ) ) );
          } else if ( dynamic_cast< const MultiDimSparse<prm_float>* >( impl ) ) {
            GUM_ERROR( FatalError, "There is no MultiDimSparse in PRMs, normally..." );
          } else {
            // Just need to make the copy using the bijection but we only use multidim array
            GUM_ERROR( FatalError, "encountered an unexpected MultiDim implementation" );
          }
        }

        return p;
      } catch ( Exception& e ) {
        if ( p ) delete p;

        throw ;
      }
    }

// the function used to combine two tables
    Potential<prm_float>* multPotential( const Potential<prm_float>& t1,
                                         const Potential<prm_float>& t2 ) {
      return new Potential<prm_float> ( t1 * t2 );
    }

    void
    eliminateNode( const DiscreteVariable* var,
                   Set<Potential<prm_float>*>& pool,
                   Set<Potential<prm_float>*>& trash ) {
      Potential<prm_float>* pot = 0;
      Potential<prm_float>* tmp = 0;
      Set<const DiscreteVariable*> var_set;
      var_set.insert( var );
      Set<const Potential<prm_float>*> pots;

      for ( SetIterator<Potential<prm_float>*> iter = pool.begin(); iter != pool.end(); ++iter )
        if ( ( *iter )->contains( *var ) )
          pots.insert( *iter );

      if ( pots.size() == 0 ) {
        return;
      } else if ( pots.size() == 1 ) {
        tmp = const_cast<Potential<prm_float>*>( *( pots.begin() ) );
        pot = new Potential<prm_float>( projectSum( *tmp, var_set ) );
      } else {
        MultiDimCombinationDefault<float,Potential> Comb( multPotential );
        tmp = Comb.combine( pots );
        pot = new Potential<prm_float>( projectSum( *tmp, var_set ) );
        delete tmp;
      }

      for ( Set<const Potential<prm_float>*>::iterator iter = pots.begin(); iter != pots.end(); ++iter ) {
        pool.erase( const_cast<Potential<prm_float>*>( *iter ) );

        if ( trash.exists( const_cast<Potential<prm_float>*>( *iter ) ) ) {
          trash.erase( const_cast<Potential<prm_float>*>( *iter ) );
          delete const_cast<Potential<prm_float>*>( *iter );
        }
      }

      pool.insert( pot );
      trash.insert( pot );
    }

// void
// eliminateNode(const DiscreteVariable* var,
//               Set<Potential<prm_float>*>& pool,
//               Set<Potential<prm_float>*>& trash)
// {
//   MultiDimBucket<prm_float>* bucket = new MultiDimBucket<prm_float>();
//   Set< Potential<prm_float>* > toRemove;
//   for (SetIterator<Potential<prm_float>*> iter = pool.begin();
//        iter != pool.end(); ++iter )
//   {
//     if ((*iter)->contains(*var)) {
//       bucket->add(**iter);
//       toRemove.insert(*iter);
//     }
//   }
//   if (toRemove.empty()) {
//     delete bucket;
//   } else {
//     for (SetIterator<Potential<prm_float>*> iter = toRemove.begin();
//          iter != toRemove.end(); ++iter)
//       pool.erase( *iter );
//     for (Set<const DiscreteVariable*>::iterator jter =
//          bucket->allVariables().begin(); jter != bucket->allVariables().end();
//          ++jter )
//     {
//       try {
//         if ((*jter) != var) bucket->add( **jter );
//       } catch (NotFound&) {
//         // This can happen if since some DiscreteVariable are not represented
//         // as nodes in the undigraph (parents of input nodes)
//         bucket->add(**jter);
//       }
//     }
//     Potential<prm_float>* bucket_pot = new Potential<prm_float>( bucket );
//     trash.insert( bucket_pot );
//     pool.insert( bucket_pot );
//   }
//   if (bucket->domainSize() > INT_MAX)
//     GUM_TRACE_VAR(bucket->domainSize());
// }

  } /* namespace prm */
}

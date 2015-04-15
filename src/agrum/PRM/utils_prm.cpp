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

#include <agrum/PRM/utils_prm.h>

namespace gum {
  namespace prm {

    // Decompose a string in a vector of strings using "." as separators.
    void decomposePath(const std::string &path, std::vector<std::string> &v) {
      size_t prev = 0;
      size_t length = 0;
      size_t idx_1 = path.find(".");
      size_t idx_2 = path.find(PRMObject::LEFT_CAST());

      if (idx_2 == std::string::npos) {
        // ignore safe names
        size_t idx = idx_1;

        while (idx != std::string::npos) {
          length = idx - prev;
          v.push_back(path.substr(prev, length));
          prev = idx + 1;
          idx = path.find(".", prev);
        }
      } else {
        size_t tmp = 0;

        while (idx_1 != std::string::npos) {
          if (idx_1 < idx_2) {
            length = idx_1 - prev;
            v.push_back(path.substr(prev, length));
            prev = idx_1 + 1;
            idx_1 = path.find(".", prev);
          } else if (idx_2 < idx_1) {
            tmp = path.find(PRMObject::RIGHT_CAST(), idx_2);
            idx_1 = path.find(".", tmp);
            idx_2 = path.find(PRMObject::LEFT_CAST(), tmp);
          }
        }
      }

      v.push_back(path.substr(prev, std::string::npos));
    }

    // void
    // eliminateNode(const DiscreteVariable* var,
    //               Set<Potential<GUM_SCALAR>*>& pool,
    //               Set<Potential<GUM_SCALAR>*>& trash)
    // {
    //   MultiDimBucket<GUM_SCALAR>* bucket = new MultiDimBucket<GUM_SCALAR>();
    //   Set< Potential<GUM_SCALAR>* > toRemove;
    //   for (SetIterator<Potential<GUM_SCALAR>*> iter = pool.begin();
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
    //     for (SetIterator<Potential<GUM_SCALAR>*> iter = toRemove.begin();
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
    //     Potential<GUM_SCALAR>* bucket_pot = new Potential<GUM_SCALAR>( bucket );
    //     trash.insert( bucket_pot );
    //     pool.insert( bucket_pot );
    //   }
    //   if (bucket->domainSize() > INT_MAX)
    //     GUM_TRACE_VAR(bucket->domainSize());
    // }

  } /* namespace prm */
}

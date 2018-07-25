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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    AprioriDirichletFromDatabase2< ALLOC >::AprioriDirichletFromDatabase2(
      const DatabaseTable< ALLOC >&                                 learning_db,
      const DBRowGeneratorParser< ALLOC >&                          apriori_parser,
      const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >& nodeId2columns,
      const typename AprioriDirichletFromDatabase2< ALLOC >::allocator_type&
        alloc) :
        Apriori2< ALLOC >(apriori_parser.database(),
                          Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
                          alloc),
        __counter(
          apriori_parser,
          std::vector< std::pair< std::size_t, std::size_t >,
                       ALLOC< std::pair< std::size_t, std::size_t > > >(alloc),
          nodeId2columns,
          alloc) {
      // we check that the variables in the learning database also exist in the
      // apriori database and that they are precisely equal.
      const DatabaseTable< ALLOC >& apriori_db = apriori_parser.database();
      const auto&                   apriori_names = apriori_db.variableNames();
      const std::size_t             apriori_size = apriori_names.size();
      HashTable< std::string, std::size_t > names2col(apriori_size);
      for (std::size_t i = std::size_t(0); i < apriori_size; ++i)
        names2col.insert(apriori_names[i], i);

      const auto&       learning_names = learning_db.variableNames();
      const std::size_t learning_size = learning_names.size();
      HashTable< std::size_t, std::size_t > learning2apriori_index(learning_size);
      bool                                  different_index = false;
      for (std::size_t i = std::size_t(0); i < learning_size; ++i) {
        // get the column of the variable in the apriori database
        std::size_t apriori_col;
        try {
          apriori_col = names2col[learning_names[i]];
        } catch (...) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i]
                                << " could not be found in the apriori database");
        }

        // check that both variables are the same
        const Variable& learning_var = learning_db.variable(i);
        const Variable& apriori_var = apriori_db.variable(apriori_col);
        if (learning_var.varType() != apriori_var.varType()) {
          GUM_ERROR(DatabaseError,
                    "Variable "
                      << learning_names[i]
                      << " has not the same type in the learning database "
                         " and the apriori database");
        }
        if (learning_var.domain() != apriori_var.domain()) {
          GUM_ERROR(DatabaseError,
                    "Variable " << learning_names[i] << " has domain "
                                << learning_var.domain()
                                << " in the learning database and domain "
                                << apriori_var.domain()
                                << " in the aprioi database");
        }

        // save the mapping from i to col
        learning2apriori_index.insert(i, apriori_col);
        if (i != apriori_col) different_index = true;
      }

      // here we are guaranteed that the variables in the learning database
      // have their equivalent in the a priori database. Now, we should
      // fill the bijection from ids to columns
      if (!different_index) {
        this->_nodeId2columns = nodeId2columns;
      } else {
        if (nodeId2columns.empty()) {
          for (std::size_t i = std::size_t(0); i < learning_size; ++i) {
            this->_nodeId2columns.insert(NodeId(i), learning2apriori_index[i]);
          }
        } else {
          for (auto iter = nodeId2columns.begin(); iter != nodeId2columns.end();
               ++iter) {
            this->_nodeId2columns.insert(iter.first(),
                                         learning2apriori_index[iter.second()]);
          }
        }
      }

      // recreate the record counter with the appropriate node2col mapping
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
                              ranges(alloc);
      RecordCounter2< ALLOC > good_counter(
        apriori_parser, ranges, this->_nodeId2columns, alloc);
      __counter = std::move(good_counter);

      GUM_CONSTRUCTOR(AprioriDirichletFromDatabase2);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >::AprioriDirichletFromDatabase2(
      const AprioriDirichletFromDatabase2< ALLOC >& from,
      const typename AprioriDirichletFromDatabase2< ALLOC >::allocator_type&
        alloc) :
        Apriori2< ALLOC >(from, alloc),
        __counter(from.__counter, alloc) {
      GUM_CONS_CPY(AprioriDirichletFromDatabase2);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >::AprioriDirichletFromDatabase2(
      const AprioriDirichletFromDatabase2< ALLOC >& from) :
        AprioriDirichletFromDatabase2< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >::AprioriDirichletFromDatabase2(
      AprioriDirichletFromDatabase2< ALLOC >&& from,
      const typename AprioriDirichletFromDatabase2< ALLOC >::allocator_type&
        alloc) :
        Apriori2< ALLOC >(std::move(from), alloc),
        __counter(std::move(from.__counter), alloc) {
      GUM_CONS_MOV(AprioriDirichletFromDatabase2);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >::AprioriDirichletFromDatabase2(
      AprioriDirichletFromDatabase2< ALLOC >&& from) :
        AprioriDirichletFromDatabase2< ALLOC >(std::move(from),
                                               from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriDirichletFromDatabase2< ALLOC >*
      AprioriDirichletFromDatabase2< ALLOC >::clone(
        const typename AprioriDirichletFromDatabase2< ALLOC >::allocator_type&
          alloc) const {
      ALLOC< AprioriDirichletFromDatabase2< ALLOC > > allocator(alloc);
      AprioriDirichletFromDatabase2< ALLOC >* apriori = allocator.allocate(1);
      try {
        allocator.construct(apriori, *this, alloc);
      } catch (...) {
        allocator.deallocate(apriori, 1);
        throw;
      }

      return apriori;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >*
           AprioriDirichletFromDatabase2< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE
      AprioriDirichletFromDatabase2< ALLOC >::~AprioriDirichletFromDatabase2() {
      GUM_DESTRUCTOR(AprioriDirichletFromDatabase2);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >&
           AprioriDirichletFromDatabase2< ALLOC >::
           operator=(const AprioriDirichletFromDatabase2< ALLOC >& from) {
      if (this != &from) {
        Apriori2< ALLOC >::operator=(from);
        __counter = from.__counter;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase2< ALLOC >&
           AprioriDirichletFromDatabase2< ALLOC >::
           operator=(AprioriDirichletFromDatabase2< ALLOC >&& from) {
      if (this != &from) {
        Apriori2< ALLOC >::operator=(std::move(from));
        __counter = std::move(from.__counter);
      }
      return *this;
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool
      AprioriDirichletFromDatabase2< ALLOC >::isOfType(const std::string& type) {
      return AprioriDirichletType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string&
                 AprioriDirichletFromDatabase2< ALLOC >::getType() const {
      return AprioriDirichletType::type;
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriDirichletFromDatabase2< ALLOC >::addAllApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {
      const auto&       apriori = __counter.counts(idset);
      const std::size_t size = apriori.size();
      if (this->_weight != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * this->_weight;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i];
        }
      }
    }


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    void AprioriDirichletFromDatabase2< ALLOC >::addConditioningApriori(
      const IdSet2< ALLOC >&                  idset,
      std::vector< double, ALLOC< double > >& counts) {
      const auto&       apriori = __counter.counts(idset.conditionalIdSet());
      const std::size_t size = apriori.size();
      if (this->_weight != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * this->_weight;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i];
        }
      }
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

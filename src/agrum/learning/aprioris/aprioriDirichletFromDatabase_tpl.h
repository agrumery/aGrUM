
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
 * @brief A dirichlet priori: computes its N'_ijk from a database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    AprioriDirichletFromDatabase< ALLOC >::AprioriDirichletFromDatabase(
       const DatabaseTable< ALLOC >&        learning_db,
       const DBRowGeneratorParser< ALLOC >& apriori_parser,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
          nodeId2columns,
       const typename AprioriDirichletFromDatabase< ALLOC >::allocator_type&
          alloc) :
        Apriori< ALLOC >(apriori_parser.database(),
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
      RecordCounter< ALLOC > good_counter(
         apriori_parser, ranges, this->_nodeId2columns, alloc);
      __counter = std::move(good_counter);

      if (apriori_db.nbRows() == std::size_t(0))
        __internal_weight = 0.0;
      else
        __internal_weight = this->_weight / apriori_db.nbRows();

      GUM_CONSTRUCTOR(AprioriDirichletFromDatabase);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >::AprioriDirichletFromDatabase(
       const AprioriDirichletFromDatabase< ALLOC >& from,
       const typename AprioriDirichletFromDatabase< ALLOC >::allocator_type&
          alloc) :
        Apriori< ALLOC >(from, alloc),
        __counter(from.__counter, alloc),
        __internal_weight(from.__internal_weight) {
      GUM_CONS_CPY(AprioriDirichletFromDatabase);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >::AprioriDirichletFromDatabase(
       const AprioriDirichletFromDatabase< ALLOC >& from) :
        AprioriDirichletFromDatabase< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >::AprioriDirichletFromDatabase(
       AprioriDirichletFromDatabase< ALLOC >&& from,
       const typename AprioriDirichletFromDatabase< ALLOC >::allocator_type&
          alloc) :
        Apriori< ALLOC >(std::move(from), alloc),
        __counter(std::move(from.__counter), alloc),
        __internal_weight(from.__internal_weight) {
      GUM_CONS_MOV(AprioriDirichletFromDatabase);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >::AprioriDirichletFromDatabase(
       AprioriDirichletFromDatabase< ALLOC >&& from) :
        AprioriDirichletFromDatabase< ALLOC >(std::move(from),
                                              from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    AprioriDirichletFromDatabase< ALLOC >*
       AprioriDirichletFromDatabase< ALLOC >::clone(
          const typename AprioriDirichletFromDatabase< ALLOC >::allocator_type&
             alloc) const {
      ALLOC< AprioriDirichletFromDatabase< ALLOC > > allocator(alloc);
      AprioriDirichletFromDatabase< ALLOC >* apriori = allocator.allocate(1);
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
    INLINE AprioriDirichletFromDatabase< ALLOC >*
           AprioriDirichletFromDatabase< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >::~AprioriDirichletFromDatabase() {
      GUM_DESTRUCTOR(AprioriDirichletFromDatabase);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >&
           AprioriDirichletFromDatabase< ALLOC >::
           operator=(const AprioriDirichletFromDatabase< ALLOC >& from) {
      if (this != &from) {
        Apriori< ALLOC >::operator=(from);
        __counter = from.__counter;
        __internal_weight = from.__internal_weight;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    INLINE AprioriDirichletFromDatabase< ALLOC >&
           AprioriDirichletFromDatabase< ALLOC >::
           operator=(AprioriDirichletFromDatabase< ALLOC >&& from) {
      if (this != &from) {
        Apriori< ALLOC >::operator=(std::move(from));
        __counter = std::move(from.__counter);
        __internal_weight = from.__internal_weight;
      }
      return *this;
    }


    /// indicates whether an apriori is of a certain type
    template < template < typename > class ALLOC >
    INLINE bool
       AprioriDirichletFromDatabase< ALLOC >::isOfType(const std::string& type) {
      return AprioriDirichletType::isOfType(type);
    }


    /// returns the type of the apriori
    template < template < typename > class ALLOC >
    INLINE const std::string&
                 AprioriDirichletFromDatabase< ALLOC >::getType() const {
      return AprioriDirichletType::type;
    }


    /// indicates whether the apriori is potentially informative
    template < template < typename > class ALLOC >
    INLINE bool AprioriDirichletFromDatabase< ALLOC >::isInformative() const {
      return (this->_weight != 0.0);
    }


    /// sets the weight of the a priori (kind of effective sample size)
    template < template < typename > class ALLOC >
    INLINE void
       AprioriDirichletFromDatabase< ALLOC >::setWeight(const double weight) {
      Apriori< ALLOC >::setWeight(weight);
      if (__counter.database().nbRows() == 0.0)
        __internal_weight = 0.0;
      else
        __internal_weight = this->_weight / __counter.database().nbRows();
    }


    /// returns the apriori vector all the variables in the idset
    template < template < typename > class ALLOC >
    INLINE void AprioriDirichletFromDatabase< ALLOC >::addAllApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      if (this->_weight == 0.0) return;

      const auto&       apriori = __counter.counts(idset);
      const std::size_t size = apriori.size();
      if (__internal_weight != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * __internal_weight;
        }
      } else {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i];
        }
      }
    }


    /// returns the apriori vector over only the conditioning set of an idset
    template < template < typename > class ALLOC >
    void AprioriDirichletFromDatabase< ALLOC >::addConditioningApriori(
       const IdSet< ALLOC >&                   idset,
       std::vector< double, ALLOC< double > >& counts) {
      if (__internal_weight == 0.0) return;

      const auto&       apriori = __counter.counts(idset.conditionalIdSet());
      const std::size_t size = apriori.size();
      if (__internal_weight != 1.0) {
        for (std::size_t i = std::size_t(0); i < size; ++i) {
          counts[i] += apriori[i] * __internal_weight;
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

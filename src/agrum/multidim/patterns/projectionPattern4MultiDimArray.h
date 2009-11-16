/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */


// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use projectionPattern, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else

#ifndef GUM_PROJECTION_OPERATION
#warning to use projectionPattern, you must define GUM_PROJECTION_OPERATION
#endif
#ifndef GUM_MULTI_DIM_OPERATOR_TYPE
#warning to use projectionPattern, you must define GUM_MULTI_DIM_OPERATOR_TYPE
#endif
#ifndef GUM_MULTI_DIM_OPERATOR_NAME
#warning to use projectionPattern, you must define GUM_MULTI_DIM_OPERATOR_NAME
#endif



// ================================================================================
/// a specialized function for combining two multiDimArrays
// ================================================================================
#ifdef GUM_MULTI_DIM_OPERATOR_NAME
  template<typename T_DATA>
  GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>*
  GUM_MULTI_DIM_OPERATOR_NAME
     ( const GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>* table,
       const Set<const DiscreteVariable *>& proj_set,
       const T_DATA& neutral_element ) {
#endif

  // first, compute whether we should loop over table or over the projected
  // table first to get a faster algorithm.
    const Sequence<const DiscreteVariable *>& table_vars =
      table->variablesSequence ();
    bool need_swapping = table_vars.size() <= 2 * proj_set.size();

  
  if ( ! need_swapping ) {
    // Compute the variables that belong to both proj_set and table. Store the
    // domain size of the Cartesian product of these variables (result_domain_size)
    // as well as the domain size of the Cartesian product of the variables of
    // table that do not belong to proj_set (table_alone_domain_size). In addition,
    // store the number of increments in the computation loops at the end of the
    // function before which the variables of table cap proj_set need be
    // incremented (vector before incr).
    std::vector<Idx> table_and_result_offset;
    std::vector<Idx> table_and_result_domain;
    std::vector<Idx> before_incr;
    Idx tmp_before_incr = 1;
    bool has_before_incr = false;
    unsigned int nb_positive_before_incr = 0;
    Idx table_alone_domain_size = 1;
    Idx result_domain_size = 1;
    Idx table_domain_size = 1;
    Sequence<const DiscreteVariable *> result_varSeq;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            table_vars.begin(); iter != table_vars.end(); ++iter ) {
      table_domain_size *= (*iter)->domainSize();
      if ( proj_set.exists ( *iter ) ) {
        if ( has_before_incr ) {
          before_incr.push_back ( tmp_before_incr - 1 );
          has_before_incr = false;
          ++nb_positive_before_incr;
        }
        else {
          before_incr.push_back ( 0 );
        }
        table_and_result_domain.push_back ( (*iter)->domainSize() );
        table_and_result_offset.push_back ( result_domain_size );
        result_domain_size *= (*iter)->domainSize();
        tmp_before_incr = 1;
        result_varSeq << *iter;
      }
      else {
        tmp_before_incr *= (*iter)->domainSize();
        has_before_incr = true;
        table_alone_domain_size *= (*iter)->domainSize();
      }
    }
    std::vector<Idx> table_and_result_value = table_and_result_domain;
    std::vector<Idx> current_incr = before_incr;
    std::vector<Idx> table_and_result_down = table_and_result_offset;
    for ( unsigned int i = 0; i < table_and_result_down.size(); ++i ) {
      table_and_result_down[i] *= (table_and_result_domain[i] - 1);
    }

    // create a table "result" containing only the variables of the projection:
    // the variables are stored in the order in which they appear in "table"
    // Hence, ++ operations on an instantiation on table will more or less 
    // correspond to a ++ operation on an instantiation on result
    GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>* result =
      new GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>;
    result->beginMultipleChanges ();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            result_varSeq.begin(); iter != result_varSeq.end(); ++iter ) {
      *result << **iter; 
    } 
    result->endMultipleChanges ();
    result->fill ( neutral_element );
    
    
    // compute the projection: first loop over the variables X's in table that do
    // not belong to result and, for each value of these X's, loop over the
    // variables in both table and result. As such, in the internal loop, the
    // offsets of "result" need only be incremented as usually to parse
    // appropriately this table. For result, the problem is slightly more
    // complicated: in the outer for loop, we shall always reset resul_offset to 0.
    // For the inner loop, result_offset should be incremented (++) only when t1
    // before_incr[xxx] steps in the loop have already been made.
      
    // but before doing so, check whether there exist positive_before_incr. If
    // this is not the case, optimize by not using before_incr at all
    if ( ! nb_positive_before_incr ) {
      Idx table_offset = 0;
      Idx result_offset = 0;
      for (Idx i = 0; i < table_alone_domain_size; ++i ) {
        for (Idx j = 0; j < result_domain_size; ++j ) {
          { GUM_PROJECTION_OPERATION ( table, table_offset,
                                       result, result_offset ); }
          
          // update the offset of table and result
          ++table_offset;
          ++result_offset;
        }
        
        // update the offset of result
        result_offset = 0;
      }
    }
    else {
      // here there are positive before_incr and we should use them to know
      // when result_offset needs be changed
      Idx table_offset = 0;
      Idx result_offset = 0;
      for (Idx i = 0; i < table_domain_size; ++i ) {
        { GUM_PROJECTION_OPERATION ( table, table_offset,
                                     result, result_offset ); }
          
        // update the offset of table
        ++table_offset;

        // update the offset of result
        for ( unsigned int k = 0; k < current_incr.size(); ++k ) {
          // check if we need modify result_offset
          if ( current_incr[k] ) {
            --current_incr[k];
            break;
          }
          current_incr[k] = before_incr[k];
          
          // here we shall modify result_offset
          --table_and_result_value[k];
          if ( table_and_result_value[k] ) {
            result_offset += table_and_result_offset[k];
            break;
          }
          table_and_result_value[k] = table_and_result_domain[k];
          result_offset -= table_and_result_down[k];
        }
      }
    }
    
    return result;
  }
  else { // need_swapping = true
    
    // Compute the variables that are in t1 but not in t2. For these variables,
    // get the increment in the offset of t1 that would result from an increment
    // in one of these variables (vector t1_alone_offset). Also store the domain
    // size of these variables (vector t1_alone_domain) and, for the computation
    // loops at the end of this function, |variable| - the current values of these
    // variables (vector t1_alone_value). All these vectors reference the variables
    // of t1 \ t2 in the order in which they appear in seq1. Keep as well the size
    // of the Cartesian product of these variables.
    std::vector<Idx> table_alone_offset;
    std::vector<Idx> table_alone_domain;
    Idx offset = 1;
    Idx table_alone_domain_size = 1;
    HashTable<const DiscreteVariable*,Idx> var1offset ( table_vars.size() );
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            table_vars.begin(); iter != table_vars.end(); ++iter ) {
      if ( ! proj_set.exists ( *iter ) ) {
        table_alone_domain.push_back ( (*iter)->domainSize() );
        table_alone_offset.push_back ( offset );
        table_alone_domain_size *= (*iter)->domainSize();
      }
      var1offset.insert ( *iter, offset );
      offset *= (*iter)->domainSize();
    }
    std::vector<Idx> table_alone_value = table_alone_domain;
    std::vector<Idx> table_alone_down = table_alone_offset;
    for ( unsigned int i = 0; i < table_alone_down.size(); ++i )
      table_alone_down[i] *= ( table_alone_domain[i] - 1 );

    // Compute the same vectors for the variables that belong to both t1 and t2.
    // In this case, All these vectors reference the variables in the order in
    // which they appear in seq2. In addition, store the number of increments in
    // the computation loops at the end of the function before which the variables
    // of t1 cap t2 need be incremented (vector t1_and_t2_before incr). Similarly,
    // store the number of such increments currently still needed before the next
    // incrementation of the variables of t1 cap t2. Keep as well the size of the
    // Cartesian product of these variables.
    Sequence<const DiscreteVariable *> result_varSeq;
    std::vector<Idx> table_and_result_offset;
    std::vector<Idx> table_and_result_domain;
    Idx result_domain_size = 1;
    bool has_before_incr = false;
    bool found_proj_var = false;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            table_vars.begin(); iter != table_vars.end(); ++iter ) {
      if ( proj_set.exists ( *iter ) ) {
        table_and_result_domain.push_back ( (*iter)->domainSize() );
        table_and_result_offset.push_back ( var1offset[*iter] );
        found_proj_var = true;
        result_domain_size *= (*iter)->domainSize();
        result_varSeq << *iter;
      }
      else {
        if ( found_proj_var ) has_before_incr = true;
      }
    }
    std::vector<Idx> table_and_result_value = table_and_result_domain;
    std::vector<Idx> table_and_result_down = table_and_result_offset;
    for ( unsigned int i = 0; i < table_and_result_down.size(); ++i ) {
      table_and_result_down[i] *= (table_and_result_domain[i] - 1);
    }


    // create a table "result" containing only the variables of the projection:
    // the variables are stored in the order in which they appear in "table"
    // Hence, ++ operations on an instantiation on table will more or less 
    // correspond to a ++ operation on an instantiation on result
    GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>* result =
      new GUM_MULTI_DIM_OPERATOR_TYPE<T_DATA>;
    result->beginMultipleChanges ();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            result_varSeq.begin(); iter != result_varSeq.end(); ++iter ) {
      *result << **iter; 
    } 
    result->endMultipleChanges ();
    result->fill ( neutral_element );
     
    
    // compute the sum: first loop over the variables X's both in table and in
    // result and, for each value of these X's, loop over the variables that are
    // in table but not result. As such, in the internal loop, there is no
    // increment in the offset of "result", and in the outer loop, this offset
    // is incremented using a simple ++ operator.
    // For table, the problem is slightly more complicated: in the outer for loop,
    // we shall increment the variables of table cap result according to vectors
    // table_and_result_xxx. Each time a variable of these vectors has been
    // incremented up to its max, we shall put it down to 0 and increment the next
    // one, and so on. For the inner loop, this is similar except that we shall do
    // these operations only when before_incr[xxx] steps in the loop have
    // already been made.
      
    // but before doing so, check whether there exist positive_before_incr. If
    // this is not the case, optimize by not using before_incr at all
    if ( ! has_before_incr ) {
      Idx table_offset = 0;
      Idx result_offset = 0;
      for (Idx i = 0; i < result_domain_size; ++i ) {
        for (Idx j = 0; j < table_alone_domain_size; ++j ) {
          { GUM_PROJECTION_OPERATION ( table, table_offset,
                                       result, result_offset ); }
          
          // update the offset of table
          ++table_offset;
        }
        
        // update the offset of result
        ++result_offset;
      }
    }
    else {
      // here there are positive before_incr and we should use them to know
      // when result_offset needs be changed
      Idx table_offset = 0;
      Idx result_offset = 0;
      for (Idx j = 0; j < result_domain_size; ++j ) {
        for (Idx i = 0; i < table_alone_domain_size; ++i ) {
          { GUM_PROJECTION_OPERATION ( table, table_offset,
                                       result, result_offset ); }
          
          // update the increment of table for the inner loop
          for ( unsigned int k = 0; k < table_alone_value.size(); ++k ) {
            --table_alone_value[k];
            if ( table_alone_value[k] ) {
              table_offset += table_alone_offset[k];
              break;
            }
            table_alone_value[k] = table_alone_domain[k];
            table_offset -= table_alone_down[k];
          }
        }
        
        // update the offset of table for the outer loop
        for ( unsigned int k = 0; k < table_and_result_value.size(); ++k ) {
          --table_and_result_value[k];
          if ( table_and_result_value[k] ) {
            table_offset += table_and_result_offset[k];
            break;
          }
          table_and_result_value[k] = table_and_result_domain[k];
          table_offset -= table_and_result_down[k];
        }
        
        // update the offset of result for the outer loop
        ++result_offset;
      }
    }
    
    return result;
  }
}
  

#endif /* GUM_OPERATOR_PATTERN_ALLOWED */

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
 * @brief the pattern used by all "basename" binary operators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */


// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use operatorPattern4MultiDimBase.h, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else


// ================================================================================
/// a specialized function for combining two multiDimImplementations whose
/// real data type is unknown to us
// ================================================================================
#ifdef GUM_MULTI_DIM_OPERATOR_NAME
  template<typename T>
  MultiDimImplementation<T>*
  GUM_MULTI_DIM_OPERATOR_NAME  ( const MultiDimImplementation<T>* t1,
                                 const MultiDimImplementation<T>* t2 ) {
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_NAME_F
  template<typename T>
  
  GUM_MULTI_DIM_OPERATOR_NAME_F ( const MultiDimImplementation<T>* t1,
                                  const MultiDimImplementation<T>* t2,
                                  const T (*f) ( const T&, const T&) ) {
#endif

  // first, compute whether we should swap t1 and t2 to get a faster algorithm.
  // here is the idea: look at the for loops at the end of this function that
  // actually compute the result of t1 + t2. It can be seen that the overall
  // complexity is O( |t2\t1| x #(t2\t1) + |t1 cup t2| x #(t1  cap t2) ) where
  // |t| is the domain size of matrix t and #t is the number of variables in t.
  // As a consequence, the algorithm will be slightly faster if we minimize
  // the product |t2\t1| x #(t2\t1). So we compute A = |t2\t1| x #(t2\t1) and
  // B = |t1\t2| x #(t1\t2) and if B < A, we swap t1 and t2. If B < A, then
  // alpha x B < alpha x A, hence, it is sufficient to compare
  // A = |t2| x #(t2\t1) and B = |t1| x #(t1\t2) (using alpha = |t1 cap t2|)
  bool need_swapping = false; 

  // get the variables in both tables
  const Sequence<const DiscreteVariable *>& seq1 = t1->variablesSequence();
  const Sequence<const DiscreteVariable *>& seq2 = t2->variablesSequence();

  // compute the variables that are in t1 but not t2, and in t2 but not t1
  unsigned int nb_elt_t1_cup_t2 = 0;
  for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq1.begin();
        iter != seq1.end(); ++iter ) {
    if ( seq2.exists ( *iter ) ) ++nb_elt_t1_cup_t2;
  }
  
  float A = (float) t2->domainSize() * ( t2->nbrDim() - nb_elt_t1_cup_t2 );
  float B = (float) t1->domainSize() * ( t1->nbrDim() - nb_elt_t1_cup_t2 );
    
  if ( B < A ) need_swapping = true;
  
  
  if ( ! need_swapping ) {
    // Compute the variables that are in t2 but not in t1. Store these variables
    // into vector t2_alone_var. Also store the domain size of these variables
    // (vector t2_alone_domain) and, for the computation loops at the end of this
    // function, |variable| - the current values of these variables (vector
    // t2_alone_value). All these vectors reference the variables of t2 \ t1 in
    // the order in which they appear in seq2. Keep as well the size of the
    // Cartesian product of these variables.
    std::vector<const DiscreteVariable*> t2_alone_var;
    std::vector<Idx> t2_alone_domain;
    Idx t2_alone_domain_size = 1;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq2.begin();
          iter != seq2.end(); ++iter ) {
      if ( ! seq1.exists ( *iter ) ) {
        t2_alone_domain.push_back ( (*iter)->domainSize() );
        t2_alone_var.push_back ( *iter );
        t2_alone_domain_size *= (*iter)->domainSize();
      }
    }
    std::vector<Idx> t2_alone_value = t2_alone_domain;

    // Compute the same vectors for the variables that belong to both t1 and t2.
    // In this case, All these vectors reference the variables in the order in
    // which they appear in seq1. In addition, store the number of increments in
    // the computation loops at the end of the function before which the variables
    // of t1 cap t2 need be incremented (vector t1_and_t2_before incr). Similarly,
    // store the number of such increments currently still needed before the next
    // incrementation of the variables of t1 cap t2. Keep as well the size of the
    // Cartesian product of these variables.
    std::vector<const DiscreteVariable*> t1_and_t2_var;
    std::vector<Idx> t1_and_t2_domain;
    std::vector<Idx> t1_and_t2_before_incr;
    Idx before_incr = 1;
    bool has_before_incr = false;
    unsigned int nb_positive_before_incr = 0;
    Idx t1_domain_size = t1->domainSize();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq1.begin();
          iter != seq1.end(); ++iter ) {
      if ( seq2.exists ( *iter ) ) {
        t1_and_t2_domain.push_back ( (*iter)->domainSize() );
        t1_and_t2_var.push_back ( *iter );
        if ( has_before_incr ) {
          t1_and_t2_before_incr.push_back ( before_incr - 1 );
          has_before_incr = false;
          ++nb_positive_before_incr;
        }
        else
          t1_and_t2_before_incr.push_back ( 0 );
        before_incr = 1;
      }
      else {
        before_incr *= (*iter)->domainSize();
        has_before_incr = true;
      }
    }
    std::vector<Idx> t1_and_t2_value = t1_and_t2_domain;
    std::vector<Idx> t1_and_t2_current_incr = t1_and_t2_before_incr;

    // create a table "result" containing all the variables: the first variables
    // are those of t1, in the order of t1, the others are the remaining variables
    // belonging to t2. Hence, making a ++ operation on an instantiation on T will
    // correspond to a ++ operation on an instantiation on t1
    MultiDimArray<T>* result = new MultiDimArray<T>;
    result->beginMultipleChanges ();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq1.begin();
          iter != seq1.end(); ++iter ) {
      *result << **iter;
    }
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq2.begin();
          iter != seq2.end(); ++iter ) {
      if ( ! seq1.exists ( *iter ) ) {
        *result << **iter;
      }
    }
    result->endMultipleChanges ();

    
    // check if t1 and t2 are disjoint => we can optimize: parse all the variables
    // of t1, then all variables in t2
    if ( ! t1_and_t2_domain.size() ) {
      Instantiation result_inst (result);
      Instantiation t2_inst ( t2 );
      Instantiation t1_inst ( t1 );
      for (t2_inst.setFirst(); ! t2_inst.end(); ++t2_inst ) {
        for (t1_inst.setFirst(); ! t1_inst.end(); ++t1_inst ) {
          result->set ( result_inst,
                        GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                t2->get ( t2_inst ) ) );
          ++result_inst;
        }
      }
    }
    else {
      // compute the sum: first loop over the variables X's in t2 but not in t1
      // and, for each value of these X's, loop over the variables in both t1 and
      // t2. As such, in the internal loop, the instantiations of "result" and t1
      // need only be incremented as usually to parse appropriately these two
      // tables. For t2, the problem is slightly more complicated: in the outer
      // for loop, we shall increment the variables of t2 \ t1 according to vectors
      // t2_alone_xxx. Each time a variable of these vectors has been incremented
      // up to its max, we shall put it down to 0 and increment the next one, and
      // so on. For the inner loop, this is similar except that we shall do these
      // operations only when t1_and_t2_before_incr[xxx] steps in the loop have
      // already been made.
      
      // but before doing so, check whether there exist positive_before_incr. If
      // this is not the case, optimize by not using before_incr at all
      if ( ! nb_positive_before_incr ) {
        Instantiation t1_inst ( t1 );
        Instantiation t2_inst ( t2 );
        Instantiation result_inst ( result );
        for (Idx i = 0; i < t2_alone_domain_size; ++i ) {
          for (Idx j = 0; j < t1_domain_size; ++j ) {
            result->set ( result_inst,
                          GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                  t2->get ( t2_inst ) ) );
            
            // increment the instantiations of result and t1
            ++result_inst;
            ++t1_inst;
            
            // update the instantiation of t2
            for ( unsigned int k = 0; k < t1_and_t2_current_incr.size(); ++k ) {
              --t1_and_t2_value[k];
              if ( t1_and_t2_value[k] ) {
                t2_inst.incVar ( *(t1_and_t2_var[k]) );
                break;
              }
              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t2_inst.setFirstVar ( *(t1_and_t2_var[k]) );
            }
          }
      
          // update the instantiation for t1
          t1_inst.setFirst();
          
          // update the instantiation of t2 for the outer loop
          for ( unsigned int k = 0; k < t2_alone_value.size(); ++ k ) {
            --t2_alone_value[k];
            if ( t2_alone_value[k] ) {
              t2_inst.incVar ( *(t2_alone_var[k]) );
              break;
            }
            t2_alone_value[k] = t2_alone_domain[k];
            t2_inst.setFirstVar ( *(t2_alone_var[k] ) );
          }
        }
      }
      else {
        // here there are positive before_incr and we should use them to know
        // when t2_inst needs be changed
        Instantiation t1_inst ( t1 );
        Instantiation t2_inst ( t2 );
        Instantiation result_inst ( result );
        for (Idx i = 0; i < t2_alone_domain_size; ++i ) {
          for (Idx j = 0; j < t1_domain_size; ++j ) {
            result->set ( result_inst,
                          GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                  t2->get ( t2_inst ) ) );
            
            // increment the instantiations of result and t1
            ++result_inst;
            ++t1_inst;

            // update the instantiation of t2
            for ( unsigned int k = 0; k < t1_and_t2_current_incr.size(); ++k ) {
              // check if we need modify t2_inst
              if ( t1_and_t2_current_incr[k] ) {
                --t1_and_t2_current_incr[k];
                break;
              }
              t1_and_t2_current_incr[k] = t1_and_t2_before_incr[k];
              
              // here we shall modify t2_inst
              --t1_and_t2_value[k];
              if ( t1_and_t2_value[k] ) {
                t2_inst.incVar ( *(t1_and_t2_var[k]) );
                break;
              }
              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t2_inst.setFirstVar ( *(t1_and_t2_var[k] ) );
            }
          }
      
          // update the instantiation for t1
          t1_inst.setFirst ();
      
          // update the instantiation of t2 for the outer loop
          for ( unsigned int k = 0; k < t2_alone_value.size(); ++ k ) {
            --t2_alone_value[k];
            if ( t2_alone_value[k] ) {
              t2_inst.incVar ( *(t2_alone_var[k]) );
              break;
            }
            t2_alone_value[k] = t2_alone_domain[k];
            t2_inst.setFirstVar ( *(t2_alone_var[k]) );
          }
        }
      }
    }
  
    return result;
  }
  else { // need_swapping = true
    
    // Compute the variables that are in t1 but not in t2. Store these variables
    // in vector t1_alone_var. Also store the domain size of these variables
    // (vector t1_alone_domain) and, for the computation loops at the end of this
    // function, |variable| - the current values of these variables (vector
    // t1_alone_value). All these vectors reference the variables of t1 \ t2 in
    // the order in which they appear in seq1. Keep as well the size of the
    // Cartesian product of these variables.
    std::vector<const DiscreteVariable*> t1_alone_var;
    std::vector<Idx> t1_alone_domain;
     Idx t1_alone_domain_size = 1;
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq1.begin();
          iter != seq1.end(); ++iter ) {
      if ( ! seq2.exists ( *iter ) ) {
        t1_alone_domain.push_back ( (*iter)->domainSize() );
        t1_alone_var.push_back ( *iter );
        t1_alone_domain_size *= (*iter)->domainSize();
      }
    }
    std::vector<Idx> t1_alone_value = t1_alone_domain;

    // Compute the same vectors for the variables that belong to both t1 and t2.
    // In this case, All these vectors reference the variables in the order in
    // which they appear in seq2. In addition, store the number of increments in
    // the computation loops at the end of the function before which the variables
    // of t1 cap t2 need be incremented (vector t1_and_t2_before incr). Similarly,
    // store the number of such increments currently still needed before the next
    // incrementation of the variables of t1 cap t2. Keep as well the size of the
    // Cartesian product of these variables.
    std::vector<const DiscreteVariable*> t1_and_t2_var;
    std::vector<Idx> t1_and_t2_domain;
    std::vector<Idx> t1_and_t2_before_incr;
    Idx before_incr = 1;
    bool has_before_incr = false;
    unsigned int nb_positive_before_incr = 0;
    Idx t2_domain_size = t2->domainSize();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq2.begin();
          iter != seq2.end(); ++iter ) {
      if ( seq1.exists ( *iter ) ) {
        t1_and_t2_domain.push_back ( (*iter)->domainSize() );
        t1_and_t2_var.push_back ( *iter );
        if ( has_before_incr ) {
          t1_and_t2_before_incr.push_back ( before_incr - 1 );
          has_before_incr = false;
          ++nb_positive_before_incr;
        }
        else
          t1_and_t2_before_incr.push_back ( 0 );
        before_incr = 1;
      }
      else {
        before_incr *= (*iter)->domainSize();
        has_before_incr = true;
      }
    }
    std::vector<Idx> t1_and_t2_value = t1_and_t2_domain;
    std::vector<Idx> t1_and_t2_current_incr = t1_and_t2_before_incr;

    // create a table "result" containing all the variables: the first variables
    // are those of t2, in the order of t2, the others are the remaining variables
    // belonging to t1. Hence, making a ++ operation on an instantiation on T will
    // correspond to a ++ operation on an instantiation on t2
    MultiDimArray<T>* result = new MultiDimArray<T>;
    result->beginMultipleChanges ();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq2.begin();
          iter != seq2.end(); ++iter ) {
      *result << **iter;
    }
    for ( Sequence<const DiscreteVariable *>::const_iterator iter = seq1.begin();
          iter != seq1.end(); ++iter ) {
      if ( ! seq2.exists ( *iter ) ) {
        *result << **iter;
      }
    }
    result->endMultipleChanges ();

    
    // check if t1 and t2 are disjoint => we can optimize: parse all the variables
    // of t2, then all variables in t1
    if ( ! t1_and_t2_domain.size() ) {
      Instantiation result_inst (result);
      Instantiation t1_inst ( t1 );
      Instantiation t2_inst ( t2 );
      for (t1_inst.setFirst(); ! t1_inst.end(); ++t1_inst ) {
        for (t2_inst.setFirst(); ! t2_inst.end(); ++t2_inst ) {
          result->set ( result_inst,
                        GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                t2->get ( t2_inst ) ) );
          ++result_inst;
        }
      }
    }
    else {
      // compute the sum: first loop over the variables X's in t1 but not in t2
      // and, for each value of these X's, loop over the variables in both t1 and
      // t2. As such, in the internal loop, the instantiations of "result" and t2
      // need only be incremented as usually to parse appropriately these two
      // tables. For t1, the problem is slightly more complicated: in the outer
      // for loop, we shall increment the variables of t1 \ t2 according to vectors
      // t1_alone_xxx. Each time a variable of these vectors has been incremented
      // up to its max, we shall put it down to 0 and increment the next one, and
      // so on. For the inner loop, this is similar except that we shall do these
      // operations only when t1_and_t2_before_incr[xxx] steps in the loop have
      // already been made.
      
      // but before doing so, check whether there exist positive_before_incr. If
      // this is not the case, optimize by not using before_incr at all
      if ( ! nb_positive_before_incr ) {
        Instantiation t1_inst ( t1 );
        Instantiation t2_inst ( t2 );
        Instantiation result_inst ( result );
        for (Idx i = 0; i < t1_alone_domain_size; ++i ) {
          for (Idx j = 0; j < t2_domain_size; ++j ) {
            result->set ( result_inst,
                          GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                  t2->get ( t2_inst ) ) );
            
            // increment the instantiations of result and t2
            ++result_inst;
            ++t2_inst;
            
            // update the instantiation of t1
            for ( unsigned int k = 0; k < t1_and_t2_current_incr.size(); ++k ) {
              --t1_and_t2_value[k];
              if ( t1_and_t2_value[k] ) {
                t1_inst.setFirstVar ( *(t1_and_t2_var[k]) );
                break;
              }
              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t1_inst.setFirstVar ( *(t1_and_t2_var[k]) );
            }
          }
      
          // update the instantiation for t2 for the outer loop
          t2_inst.setFirst();
          
          // update the instantiation of t1 for the outer loop
          for ( unsigned int k = 0; k < t1_alone_value.size(); ++ k ) {
            --t1_alone_value[k];
            if ( t1_alone_value[k] ) {
              t1_inst.incVar ( *(t1_alone_var[k]) );
              break;
            }
            t1_alone_value[k] = t1_alone_domain[k];
            t1_inst.setFirstVar ( *(t1_alone_var[k] ) );
          }
        }
      }
      else {
        // here there are positive before_incr and we should use them to know
        // when t1_inst needs be changed
        Instantiation t1_inst ( t1 );
        Instantiation t2_inst ( t2 );
        Instantiation result_inst ( result );
        for (Idx i = 0; i < t1_alone_domain_size; ++i ) {
          for (Idx j = 0; j < t2_domain_size; ++j ) {
            result->set ( result_inst,
                          GUM_MULTI_DIM_OPERATOR( t1->get ( t1_inst ),
                                                  t2->get ( t2_inst ) ) );
            
            // update the instantiations of result and t2
            ++result_inst;
            ++t2_inst;
           
            // update the instantiation of t1
            for ( unsigned int k = 0; k < t1_and_t2_current_incr.size(); ++k ) {
              // check if we need modify t2_inst
              if ( t1_and_t2_current_incr[k] ) {
                --t1_and_t2_current_incr[k];
                break;
              }
              t1_and_t2_current_incr[k] = t1_and_t2_before_incr[k];
              
              // here we shall modify t2_inst
              --t1_and_t2_value[k];
              if ( t1_and_t2_value[k] ) {
                t1_inst.incVar ( *(t1_and_t2_var[k]) );
                break;
              }
              t1_and_t2_value[k] = t1_and_t2_domain[k];
              t1_inst.setFirstVar ( *(t1_and_t2_var[k] ) );
            }
          }
      
          // update the instantiation for t2 for the outer loop
          t2_inst.setFirst ();

          // update the instantiation of t1 for the outer loop
          for ( unsigned int k = 0; k < t1_alone_value.size(); ++ k ) {
            --t1_alone_value[k];
            if ( t1_alone_value[k] ) {
              t1_inst.incVar ( *(t1_alone_var[k]) );
              break;
            }
            t1_alone_value[k] = t1_alone_domain[k];
            t1_inst.setFirstVar ( *(t1_alone_var[k]) );
          }
        }
      }
    }
  
    return result;
  }
}
  

#endif /* GUM_OPERATOR_PATTERN_ALLOWED */

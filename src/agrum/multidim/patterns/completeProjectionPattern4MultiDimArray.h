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
 * @brief the pattern used by all the projections of multidim tables over all
 * their variables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */


// check if we allowed these patterns to be used
#ifndef GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

#warning To use completeProjectionPattern, you must define GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

#else
namespace gum{

// ================================================================================
/// a specialized function for projecting a multiDimArray over all its variables
// ================================================================================
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA
  template<typename T_DATA>
  T_DATA
  GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
    ( const MultiDimArray<T_DATA>* table,
      Instantiation* instantiation = 0 ) {
#endif
    

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA*
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template<typename T_DATA>
  T_DATA*
  GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
    ( const MultiDimArray<T_DATA*>* table,
      Instantiation* instantiation = 0 ) {
#endif
    

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA
  template<typename T_DATA>
  T_DATA
  GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME_F
    ( const MultiDimArray<T_DATA>* table,
      T_DATA (*f) ( const T_DATA&, const T_DATA&),
      Instantiation* instantiation = 0 ) {
#endif

    
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA*
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template<typename T_DATA>
  T_DATA*
  GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME_F
    ( const MultiDimArray<T_DATA*>* table,
      T_DATA* (*f) ( const T_DATA const*, const T_DATA const* ),
      Instantiation* instantiation = 0 ) {
#endif

    
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA
  template<typename T_DATA>
  T_DATA
  GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
    ( const MultiDimImplementation<T_DATA>* ttable,
      Instantiation* instantiation = 0 ) {
    const MultiDimArray<T_DATA>* table =
      reinterpret_cast<const MultiDimArray<T_DATA>*> (ttable);
#endif

    
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE T_DATA*
#define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
  template<typename T_DATA>
    T_DATA*
  GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
    ( const MultiDimImplementation<T_DATA*>* ttable,
      Instantiation* instantiation = 0 ) {
    const MultiDimArray<T_DATA*>* table =
      reinterpret_cast<const MultiDimArray<T_DATA*>*> (ttable);
#endif


  // first, compute the domain size of the table:
  Size nb_elt = table->domainSize ();

  // now, parse the table and compute the projection. Start with the
  // neutral element
  register GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* ptable =
    const_cast<GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE*>
    ( &( table->unsafeGet ( 0 ) ) );
  T_DATA current_val = GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL;

  if ( instantiation ) {
    T_DATA old_val = current_val;
    GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* current_ptr = ptable;
    GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* deb_ptr = ptable;

    for ( Size i = 0; i < nb_elt; ++i, ++ptable ) {
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
      GUM_MULTI_DIM_COMPLETE_PROJECTION ( &current_val, *ptable );
#else
      GUM_MULTI_DIM_COMPLETE_PROJECTION ( current_val, *ptable );
#endif
      if ( current_val != old_val ) {
        old_val = current_val;
        current_ptr = ptable;
      }
    }

    // put the table's variables into the instantiation
    instantiation->forgetMaster ();
    instantiation->clear ();
    const Sequence<const DiscreteVariable *>& table_vars =
      table->variablesSequence ();
    for ( Sequence<const DiscreteVariable *>::const_iterator iter =
            table_vars.begin(); iter != table_vars.end(); ++iter ) {
      instantiation->add ( **iter );
    }

    // return the result
    table->fromOffset ( *instantiation, current_ptr - deb_ptr );
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
    return *current_ptr;
#else
    return current_val;
#endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER */
  }
  else {    
#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER    
    T_DATA old_val = current_val;
    GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE* current_ptr = ptable;
    
    for ( Size i = 0; i < nb_elt; ++i, ++ptable ) {
      GUM_MULTI_DIM_COMPLETE_PROJECTION ( &current_val, *ptable );
      if ( current_val != old_val ) {
        old_val = current_val;
        current_ptr = ptable;
      }
    }
    return *current_ptr;
#else
    for ( Size i = 0; i < nb_elt; ++i, ++ptable ) {
      GUM_MULTI_DIM_COMPLETE_PROJECTION ( current_val, *ptable );
    }
    return current_val;
#endif /* GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER  */
  }
}


#undef GUM_MULTI_DIM_COMPLETE_PROJECTION_TYPE

#ifdef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER
#endif
  

} /* End of namespace gum */

#endif /* GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED */

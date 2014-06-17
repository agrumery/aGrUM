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
 * @brief The "non-meta-programming" set of translators stored into a row filter
 *
 * Basically, there are two ways to create the cell translators needed to parse
 * a database: either you know at compile time the columns of the database you
 * will wish to extract and you should definitely use the DBRowTranslatorSet
 * class to store the cell translators; or you know the columns to extract at
 * run time and you should use the DBRowTranslatorVector class. DBRowTranslatorSet
 * is a "meta-programming" based class that packs the cell filters in a most
 * efficient way (essentially, all methods can be inlined, which makes this class
 * the fastest one). DBRowTranslatorVectors are less efficient but are more
 * general. If all their cell translators are identical, translator's methods are
 * also inlined, otherwise, methods' calls induce virtual function overheads.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_TRANSLATOR_VECTOR_H
#define GUM_LEARNING_DB_ROW_TRANSLATOR_VECTOR_H


#include <vector>

#include <agrum/config.h>
#include <agrum/learning/database/column.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/filteredRow.h>
#include <agrum/learning/database/DBCellTranslator.h>


namespace gum {

  
  namespace learning {

    
    /** @class DBRowTranslatorVector
     * @ingroup learning_group
     * @brief the "non-meta-programming" class that includes cell translator packs
     * into row filters
     *
     * Basically, there are two ways to create the cell translators needed to parse
     * a database: either you know at compile time the columns of the database you
     * will wish to extract and you should definitely use the DBRowTranslatorSet
     * class to store the cell translators; or you know the columns to extract at
     * run time and you should use the DBRowTranslatorVector class.
     * DBRowTranslatorSet is a "meta-programming" based class that packs the cell
     * filters in a most efficient way (essentially, all methods can be inlined,
     * which makes this class the fastest one). DBRowTranslatorVectors are less
     * efficient but are more general. If all their cell translators are
     * identical, translator's methods are also inlined, otherwise, methods' calls
     * induce virtual function overheads.
     */
    template <typename Translator>
    class DBRowTranslatorVector {
    public:
  
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowTranslatorVector ();

      /// copy constructor
      DBRowTranslatorVector ( const DBRowTranslatorVector<Translator>& from );

      /// move constructor
      DBRowTranslatorVector ( DBRowTranslatorVector<Translator>&& from );

      /// destructor
      ~DBRowTranslatorVector () noexcept;
  
      /// @}
  

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowTranslatorVector<Translator>&
      operator= ( const DBRowTranslatorVector<Translator>& );

      /// move operator
      DBRowTranslatorVector<Translator>&
      operator= ( DBRowTranslatorVector<Translator>&& );      

      /// @}

 
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// inserts new translators at the end of the vector
      /** insert a new translator that will read columns deb_cols. If we wish to
       * insert several translators, use an nb_times different from 1. In this
       * case, the other translators will read columns of the database deduced
       * from deb_cols by applying increment ColsIncr. */
      template <typename Cols,
                typename ColsIncr = typename Make_Default_Incr<Cols>::type>
      void insertTranslator
      ( Cols deb_cols = Cols (),
        unsigned int nb_times = 1,
        ColsIncr incr  = ColsIncr () );

      /// inserts new translators at the end of the vector
      /** insert a new translator that will read columns deb_cols. If we wish to
       * insert several translators, use an nb_times different from 1. In this
       * case, the other translators will read columns of the database deduced
       * from deb_cols by applying increment ColsIncr. */
      template <typename NewTranslator,
                typename Cols,
                typename ColsIncr = typename Make_Default_Incr<Cols>::type>
      void insertTranslator
      ( const NewTranslator& translator,
        Cols deb_cols = Cols (),
        unsigned int nb_times = 1,
        ColsIncr incr  = ColsIncr () );

      /// inserts new translators at the end of the vector
      /** insert a new translator that will read only one column, namely deb_col.
       * If we wish to insert several translators, use an nb_times different
       * from 1. In this case, the other translators will read columns of the
       * database deduced from deb_col by applying increment "increment". */
      void insertTranslator ( unsigned int deb_col,
                              unsigned int nb_times = 1,
                              unsigned int increment  = 1 );

      /// execute all the translations on the current database row
      void translate ();

      /// initialize the cell filters by parsing once the database
      void initialize ();

      /** @brief indicates whether one of the translators needs an initial parsing
       * of the database to initialize itself */
      bool requiresInitialization () const noexcept;

      /// push back the number of modalities of the variables of the output columns
      void modalities ( std::vector<unsigned int>& modals ) const;

      /// sets the input row that shall be read by all the cell translators
      void setInputRow ( const DBRow& row ) noexcept;
      
      /// returns the current output row
      FilteredRow& outputRow () noexcept;

      /// remove all the translators from the vector
      void clear () noexcept;

      /// @}
  
  private:
      /// the vector of the cell filters
      std::vector<Translator*> __translators;

      /// the size of the output
      unsigned int __output_size { 0 };
    
      /// the output row into which the translators write their output
      FilteredRow __output_row;

    };


  } /* namespace learning */

  
} /* namespace gum */


// always include templated implementation
#include <agrum/learning/database/DBRowTranslatorVector.tcc>


#endif /* GUM_LEARNING_DB_ROW_TRANSLATOR_VECTOR_H */

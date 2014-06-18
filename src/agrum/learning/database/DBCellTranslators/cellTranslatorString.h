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
 * @brief A cell translator translating DBCells that contain string
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_CELL_TRANSLATOR_STRING_H
#define GUM_LEARNING_CELL_TRANSLATOR_STRING_H

#include <string>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/set.h>
#include <agrum/learning/database/DBCellTranslator.h>


namespace gum {

  
  namespace learning {


    /** @class CellTranslatorString
     * @ingroup learning_group
     * @brief A cell translator translating DBCells that contain string
     */
    class CellTranslatorString : public DBCellTranslator<1,1> {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      CellTranslatorString ();

      /// copy constructor
      CellTranslatorString ( const CellTranslatorString& from );

      /// move constructor
      CellTranslatorString ( CellTranslatorString&& from );

      /// virtual copy constructor
      virtual CellTranslatorString* copyFactory () final;
 
      /// destructor
      virtual ~CellTranslatorString ();
      
      /// @}

      
      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CellTranslatorString&
      operator= ( const CellTranslatorString& from );

      /// move operator
      CellTranslatorString&
      operator= ( CellTranslatorString&& from );

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// perform the translation
      void translate ();

      /// initialize the cell translator by a first database parsing
      /** If initialization is required, this method is called for each row
       * of the database used for initialization. */
      void initialize ();

      /// perform a post initialization after the database parsing
      void postInitialize ();

      /// add the number of modalities discovered in the database into a vector
      void modalities ( std::vector<unsigned int>& modal ) const noexcept;

      /// returns whether the translator needs a DB parsing to initialize itself
      bool requiresInitialization () const noexcept;

      /// returns a given value as stored within the database
      std::string translateBack ( unsigned int col,
                                  unsigned int translated_val );

      /// returns the name of the variable(s) the translator has processed
      void variableNames ( const std::vector<std::string>& db_var,
                           std::vector<std::string>& output_vars ) const;
      
    private:
      /// the next max translated value
      unsigned int __max_value { 0 };
      
      /// the set of strings (actually their indices) found so far
      gum::Bijection<int,unsigned int> __strings;
      
    };


  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_CELL_TRANSLATOR_STRING_H */

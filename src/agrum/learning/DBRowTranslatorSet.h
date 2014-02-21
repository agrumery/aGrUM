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
 * @brief The set of translators stored into a row filter
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H
#define GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H


#include <agrum/config.h>
#include <agrum/learning/cellTranslator.h>
#include <agrum/learning/translatorCreate.h>


namespace gum {

  
  namespace learning {


#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /** @brief the wrapper of Create<>s that will be used to translate
     * a set of database cells */ 
    template<int Idx, typename... Translators>
    struct BasicDBRowTranslatorSet;


    /** @brief the end of the recursive definition of BasicDBRowTranslatorSet:
     * should do nothing */
    template <int Idx>
    class BasicDBRowTranslatorSet<Idx> {
    public:
      static constexpr unsigned int output_size = Idx;
  
      BasicDBRowTranslatorSet () noexcept {}
      BasicDBRowTranslatorSet( const BasicDBRowTranslatorSet<Idx>& ) noexcept {}
      void setInputRow ( const DBRow& ) noexcept {}
      void setOutputRow ( FilteredRow& ) noexcept {}
      void translate () noexcept {}
    };


    /** @brief the implementation of the wrapper of Create<>s (used to
     * translate the cells of a DBRow) */
    template <int Idx, typename Translator, typename... OtherTranslators>
    class BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...> :
      public BasicDBRowTranslatorSet
               < Idx + std::remove_reference<Translator>::type::output_size,
                 OtherTranslators... > {
    public:
      
      /// the type of the subsequent Create<>s to apply
      using NextTranslators = BasicDBRowTranslatorSet
        < Idx + std::remove_reference<Translator>::type::output_size,
          OtherTranslators... >;

      /// the number of columns written by all the BasicDBRowTranslatorSets
      static constexpr unsigned int output_size = NextTranslators::output_size;


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{
 
    protected:
      
      /// default constructor
      /** @param first_translator the Create<> for the first translator to apply
       * @param next_translators all the other Create<>s to apply */
      BasicDBRowTranslatorSet ( const Translator& first_translator,
                                const OtherTranslators&... next_translators );

      /// copy constructor
      BasicDBRowTranslatorSet
      ( const BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...>& from );
  
    public:
      
     /// destructor
      ~BasicDBRowTranslatorSet () noexcept;
 
      /// @}

  
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// sets the input row to filter
      void setInputRow ( const DBRow& row ) noexcept;

      /// sets the output rows of the translators
      void setOutputRow ( FilteredRow& output_row ) noexcept;

      /// perform the translation of a database row
      void translate ();

      /// @}

      
    private:
      /// the Create<> translator that should be executed
      Translator __translator;

      /// only DBRowTranslatorSets can create BasicDBRowTranslatorSets
      template <typename... Translators>
      friend class DBRowTranslatorSet;

    };

#endif // DOXYGEN_SHOULD_SKIP_THIS




    /** @class DBRowTranslatorSet
     * @ingroup learning_group
     * @brief the very class that includes cell translator packs into row filters
     *
     * This class enables to pack cell translators into row filters. To make it
     * easy to use, you should prefer using its helper creation function
     * make_translators. The following code shows how to simply create a
     * DBRowTranslatorSet:
     * @code
     * auto my_filter = make_translators
     * ( Create<Translate, Col<3> > (),   // use Translate to translate column 3
     *   Create<Translate, Col<5>, 4> (), // use Translate to translate cols 5 to 8
     *   Create<Translate2, Col<6,7> > (),// translate cols 6 and 7 and get the
     *                                    // result into 1 col of the output row
     *   Create<Translate2, Col<10,11>, 4, Incr<1,2> > ()
     * );
     * @endcode
     */
    template <typename... Translators>
    class DBRowTranslatorSet : BasicDBRowTranslatorSet<0,Translators...> {
    public:
  
      using TranslatorSet = BasicDBRowTranslatorSet<0,Translators...>;

      /// the size of the output filtered row
      static constexpr unsigned int output_size = TranslatorSet::output_size;

      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @aram translators the set of CellTranslators to apply */
      DBRowTranslatorSet ( const Translators&...  translators );

      /// copy constructor
      DBRowTranslatorSet ( const DBRowTranslatorSet<Translators...>& from );

      /// destructor
      ~DBRowTranslatorSet () noexcept;
  
      /// @}
  
  
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// execute all the translations on the current database row
      void translate ();

      /// sets the input row that shall be read by all the cell translators
      void setInputRow ( const DBRow& row ) noexcept;
      
      /// returns the current output row
      FilteredRow& outputRow () noexcept;

      /// @}
  
    private:
      /// the output row into which the translators write their output
      FilteredRow __output_row;

    };


    
    /// a function to create easily a DBRowTranslatorSet
    /** Below is an example of the use of this function. Basically, all the
     * arguments are Create<> translators.
     * @code
     * auto my_filter = make_translators
     * ( Create<Translate, Col<3> > (), // use Translate to translate column 3
     *   Create<Translate, Col<5>, 4> (), // use Translate to translate cols 5 to 8
     *   Create<Translate2, Col<6,7> > (), // translate cols 6 and 7 and get the
     *                                     // result into 1 col of the output row
     *   Create<Translate2, Col<10,11>, 4, Incr<1,2> > ()
     * );
     * @endcode
     */
    template<typename... Args>
    constexpr DBRowTranslatorSet<Args...>
    make_translators ( const Args&... args ) {
      return DBRowTranslatorSet<Args...> ( args... );
    }


  } /* namespace learning */

  
} /* namespace gum */


// always include the template implementation
#include <agrum/learning/DBRowTranslatorSet.tcc>

    
#endif /* GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H */


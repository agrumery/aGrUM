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
/**
 * @file
 * @brief Inline implementation of SkoolReader.
 *
 * @author Lionel TORTI
 */
// ============================================================================

//to ease automatic parsing
#include <agrum/prm/skool/SkoolReader.h>

namespace gum {

  namespace prm {

    namespace skool {

      INLINE
      SkoolReader::SkoolReader() {
        GUM_CONSTRUCTOR ( SkoolReader );
        __parseDone = false;
        __prmTake = false;
        __parser = 0;
      }


      INLINE
      SkoolReader::~SkoolReader() {
        GUM_DESTRUCTOR ( SkoolReader );
        if ( __parseDone )
          delete __parser;
        if ( ! __prmTake )
          delete __factory.prm();
      }

      INLINE
      void
      SkoolReader::readFile ( const std::string& file ) {
        if ( ! __parseDone ) {
          Scanner s ( file.c_str() );
          __parser = new Parser ( &s );
          __parser->setFactory ( &__factory );
          __parser->setClassPath ( __class_path );
          __parser->setCurrentDirectory( file.substr(0, file.find_last_of('/')+1) );
          try {
            __parser->Parse();
            __parseDone = true;
          } catch ( gum::Exception &e ) {
            GUM_SHOWERROR ( e );
          }
        }
      }

      INLINE
      void
      SkoolReader::readString ( const std::string & string ) {
        if ( ! __parseDone ) {
          Scanner s ( (unsigned char*) string.c_str(), (int) ( string.length() ) );
          __parser = new Parser ( &s );
          __parser->setFactory ( &__factory );
          __parser->setClassPath ( __class_path );

          try {
            __parser->Parse();
            __parseDone = true;
          } catch ( gum::Exception &e ) {
            GUM_SHOWERROR ( e );
          }
        }
      }


      INLINE
      gum::prm::PRM*
      SkoolReader::prm() {
        __prmTake = true;
        return __factory.prm();
      }
      
      INLINE
      const gum::prm::PRM* 
      SkoolReader::prm() const {
        return __factory.prm();
      }

/// @{
/// publishing Errors API
      INLINE
      unsigned int SkoolReader::errLine ( unsigned int i ) const {
        if ( __parseDone ) return __parser->errors().line ( i );
        else GUM_ERROR ( OperationNotAllowed, "Skool file not parsed yet" );
      }

      INLINE
      unsigned int SkoolReader::errCol ( unsigned int i ) const {
        if ( __parseDone ) return __parser->errors().col ( i );
        else GUM_ERROR ( OperationNotAllowed, "Skool file not parsed yet" );
      }

      INLINE
      std::wstring SkoolReader::errFilename ( unsigned int i ) const {
        if ( __parseDone ) return __parser->errors().filename ( i );
        else GUM_ERROR ( OperationNotAllowed, "Skool file not parsed yet" );
      }

      INLINE
      bool SkoolReader::errIsError ( unsigned int i ) const {
        if ( __parseDone ) return __parser->errors().is_error ( i );
        else GUM_ERROR ( OperationNotAllowed, "Skool file not parsed yet" );
      }

      INLINE
      std::string SkoolReader::errMsg ( unsigned int i ) const {
        if ( __parseDone ) return std::string ( narrow ( __parser->errors().msg ( i ) ) );
        else GUM_ERROR ( OperationNotAllowed, "Skool file not parsed yet" );
      }

      INLINE
      void SkoolReader::showElegantErrors() const {
        if ( __parseDone ) __parser->errors().showElegantErrors();
        else return;
      }

      INLINE
      void SkoolReader::showElegantErrorsAndWarnings() const {
        if ( __parseDone ) __parser->errors().showElegantErrorsAndWarnings();
        else return;
      }

      INLINE
      void SkoolReader::showErrorCounts() const {
        if ( __parseDone ) __parser->errors().showSyntheticResults();
        else return;
      }

      INLINE
      Size  SkoolReader::errors() const {
        return ( ! __parseDone ) ? ( Size ) 0 : __parser->errors().error_count;
      }

      INLINE
      Size  SkoolReader::warnings() const {
        return ( ! __parseDone ) ? ( Size ) 0 : __parser->errors().warning_count;
      }

      INLINE
      const ErrorsContainer & SkoolReader::getErrorsContainer() const
      {
        return __parser->errors();
      }
/// @}

    } /* namespace skool */
  } /* namespace prm */
} /* namespace gum */
// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 

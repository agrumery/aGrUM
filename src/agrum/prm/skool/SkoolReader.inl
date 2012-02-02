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
      int
      SkoolReader::readFile ( const std::string& file ) {
        size_t lastSlashIndex = file.find_last_of ( '/' );
        Directory dir ( file.substr ( 0, lastSlashIndex+1 ) );

        if ( ! dir.isValid() ) {
          __errors.addException ( "directory doesn't exist.", dir.path() );
          return __errors.count();
        }

        string basename = file.substr ( lastSlashIndex+1 );

        string absFilename = dir.absolutePath() + basename;

        try {
          if ( __parser && __parser->getImports().exists ( absFilename ) )
            return __errors.count();;

          Scanner s ( absFilename.c_str() );

          if ( ! __parseDone ) {
            __parser = new Parser ( &s );
            __parser->setFactory ( &__factory );
            __parser->setClassPath ( __class_path );
          }
          else
            __parser->scanner = &s;

          __parser->setCurrentDirectory ( dir.absolutePath() );

          __parser->addImport ( absFilename );

          __parser->Parse();

          __parseDone = true;

          __errors += __parser->errors();
        }
        catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          __errors.addException ( e.content(), file );
        }

        return __parser->errors().count();
      }

      INLINE
      int
      SkoolReader::readString ( const std::string & string ) {
        // errors += parser.errors
        try {
          Scanner s ( ( unsigned char* ) string.c_str(), ( int ) ( string.length() ) );
          __parser = new Parser ( &s );
          __parser->setFactory ( &__factory );
          __parser->setClassPath ( __class_path );

          __parser->Parse();
          __parseDone = true;
          __errors += __parser->errors();
        }
        catch ( gum::Exception &e ) {
          GUM_SHOWERROR ( e );
          __errors.addException ( e.content(), "" );
        }

        return __parser->errors().count();
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
        return __errors.line ( i );
      }

      INLINE
      unsigned int SkoolReader::errCol ( unsigned int i ) const {
        return __errors.col ( i );
      }

      INLINE
      std::wstring SkoolReader::errFilename ( unsigned int i ) const {
        return __errors.filename ( i );
      }

      INLINE
      bool SkoolReader::errIsError ( unsigned int i ) const {
        return __errors.is_error ( i );
      }

      INLINE
      std::string SkoolReader::errMsg ( unsigned int i ) const {
        return gum::narrow ( __errors.msg ( i ) );
      }

      INLINE
      void SkoolReader::showElegantErrors() const {
        __errors.showElegantErrors();
      }

      INLINE
      void SkoolReader::showElegantErrorsAndWarnings() const {
        __errors.showElegantErrorsAndWarnings();
      }

      INLINE
      void SkoolReader::showErrorCounts() const {
        __errors.showSyntheticResults();
      }

      INLINE
      Size  SkoolReader::errors() const {
        return __errors.error_count;
      }

      INLINE
      Size  SkoolReader::warnings() const {
        return __errors.warning_count;
      }

      INLINE
      const ErrorsContainer & SkoolReader::errorsContainer() const {
        return __errors;
      }

/// @}

    } /* namespace skool */
  } /* namespace prm */
} /* namespace gum */

// ============================================================================
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

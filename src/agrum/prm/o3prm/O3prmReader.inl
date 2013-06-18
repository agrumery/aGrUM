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
 * @brief Inline implementation of O3prmReader.
 *
 * @author Lionel TORTI
 */
// ============================================================================

//to ease automatic parsing
#include <agrum/prm/o3prm/O3prmReader.h>

namespace gum {

  namespace prm {

    namespace o3prm {

      INLINE
      O3prmReader::O3prmReader() {
        GUM_CONSTRUCTOR ( O3prmReader );
        __parseDone = false;
        __prmTake = false;
        __parser = 0;
      }


      INLINE
      O3prmReader::~O3prmReader() {
        GUM_DESTRUCTOR ( O3prmReader );

        if ( __parseDone )
          delete __parser;

        if ( ! __prmTake )
          delete __factory.prm();
      }

      INLINE
      int
      O3prmReader::readFile ( const std::string& file ) {
        size_t lastSlashIndex = file.find_last_of ( '/' );
        Directory dir ( file.substr ( 0, lastSlashIndex+1 ) );

        if ( ! dir.isValid() ) {
          __errors.addException ( "directory doesn't exist.", dir.path() );
          return __errors.count();
        }

        std::string basename = file.substr ( lastSlashIndex+1 );

        std::string absFilename = dir.absolutePath() + basename;

        try {
          if ( __parser && __parser->getImports().exists ( absFilename ) )
            return __errors.count();;

          Scanner s ( absFilename.c_str() );

          if ( ! __parseDone ) {
            __parser = new Parser ( &s );
            __parser->setFactory ( &__factory );
            __parser->setClassPath ( __class_path );
          } else
            __parser->scanner = &s;

          __parser->setCurrentDirectory ( dir.absolutePath() );

          __parser->addImport ( absFilename );

          __parser->Parse();

          __parseDone = true;

          __errors += __parser->errors();
        } catch ( gum::Exception& e ) {
          GUM_SHOWERROR ( e );
          __errors.addException ( e.content(), file );
        }

        return __parser->errors().count();
      }

      INLINE
      int
      O3prmReader::readString ( const std::string& st ) {
        // errors += parser.errors
        try {
          Scanner s ( ( unsigned char* ) st.c_str(), ( int ) ( st.size() ) );
          __parser = new Parser ( &s );
          __parser->setFactory ( &__factory );
          __parser->setClassPath ( __class_path );

          __parser->Parse();
          __parseDone = true;
          __errors += __parser->errors();
        } catch ( gum::Exception& e ) {
          GUM_SHOWERROR ( e );
          __errors.addException ( e.content(), "" );
        }

        return __parser->errors().count();
      }


      INLINE
      gum::prm::PRM*
      O3prmReader::prm() {
        __prmTake = true;
        return __factory.prm();
      }

      INLINE
      const gum::prm::PRM*
      O3prmReader::prm() const {
        return __factory.prm();
      }

/// publishing Errors API
      INLINE
      unsigned int O3prmReader::errLine ( unsigned int i ) const {
        return __errors.line ( i );
      }

      INLINE
      unsigned int O3prmReader::errCol ( unsigned int i ) const {
        return __errors.col ( i );
      }

      INLINE
      std::wstring O3prmReader::errFilename ( unsigned int i ) const {
        return __errors.filename ( i );
      }

      INLINE
      bool O3prmReader::errIsError ( unsigned int i ) const {
        return __errors.is_error ( i );
      }

      INLINE
      std::string O3prmReader::errMsg ( unsigned int i ) const {
        return gum::narrow ( __errors.msg ( i ) );
      }

      INLINE
      void O3prmReader::showElegantErrors() const {
        __errors.showElegantErrors();
      }

      INLINE
      void O3prmReader::showElegantErrorsAndWarnings() const {
        __errors.showElegantErrorsAndWarnings();
      }

      INLINE
      void O3prmReader::showErrorCounts() const {
        __errors.showSyntheticResults();
      }

      INLINE
      Size  O3prmReader::errors() const {
        return __errors.error_count;
      }

      INLINE
      Size  O3prmReader::warnings() const {
        return __errors.warning_count;
      }

      INLINE
      const ErrorsContainer& O3prmReader::errorsContainer() const {
        return __errors;
      }

    } /* namespace o3prm */
  } /* namespace prm */
} /* namespace gum */

// ============================================================================
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

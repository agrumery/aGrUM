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
namespace gum {
namespace prm {
namespace skool {

INLINE
SkoolReader::SkoolReader() {
  GUM_CONSTRUCTOR( SkoolReader );
  __class_path.push_back(".");
  __parseDone = false;
}


INLINE
SkoolReader::~SkoolReader() {
  GUM_DESTRUCTOR( SkoolReader );
  delete __parser;
}

INLINE
void
SkoolReader::readFile(const std::string& file) {
  if ( ! __parseDone ) {
    Scanner s(file.c_str());
    __parser = new Parser(&s);
    __parser->setFactory(&__factory);
    __parser->setClassPath(__class_path);
    try {
      __parser->Parse();
    __parseDone=true;
    } catch ( gum::Exception &e ) {
      GUM_SHOWERROR( e );
    }
  }
}

INLINE
gum::prm::PRM*
SkoolReader::prm() {
  return __factory.prm();
}

/// @{
/// publishing Errors API
INLINE
unsigned int SkoolReader::errLine( unsigned int i ) {
  if ( __parseDone ) return __parser->errors->line( i );
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
unsigned int SkoolReader::errCol( unsigned int i ) {
  if ( __parseDone ) return __parser->errors->col( i );
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
std::wstring SkoolReader::errFilename( unsigned int i ) {
  if ( __parseDone ) return __parser->errors->filename( i );
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
bool SkoolReader::errIsError( unsigned int i ) {
  if ( __parseDone ) return __parser->errors->is_error( i );
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
std::string SkoolReader::errMsg( unsigned int i ) {
  if ( __parseDone ) return std::string( narrow( __parser->errors->msg( i ) ) );
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
void SkoolReader::showElegantErrors() {
  if ( __parseDone ) __parser->errors->showElegantErrors();
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
void SkoolReader::showElegantErrorsAndWarnings() {
  if ( __parseDone ) __parser->errors->showElegantErrorsAndWarnings();
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
void SkoolReader::showErrorCounts() {
  if ( __parseDone ) __parser->errors->showSyntheticResults();
  else GUM_ERROR( OperationNotAllowed,"BIF file not parsed yet" );
}

INLINE
Size  SkoolReader::errors() {
  return (! __parseDone )?( Size )0:__parser->errors->error_count;
}

INLINE
Size  SkoolReader::warnings() {
  return (! __parseDone )?( Size )0:__parser->errors->warning_count;
}

/// @}

} /* namespace skool */
} /* namespace prm */
} /* namespace gum */
// ============================================================================

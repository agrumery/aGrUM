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
#include <agrum/BN/io/BIFIO.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


extern bool GUM_BIF_parser( gum::BayesNet<float>&, const std::string& );


namespace gum {


  /* ============================================================================ */
  /* ===                          GUM_BIF_READER                              === */
  /* ============================================================================ */
  BIFReader::BIFReader() {
    GUM_CONSTRUCTOR( BIFReader );
  }

  BIFReader::~BIFReader() {
    GUM_DESTRUCTOR( BIFReader );
  }

  BayesNet<float>*
  BIFReader::read( std::string filePath ) {
    BayesNet<float>* bn = new BayesNet<float>();
    bool b=false;

    try {
      b = GUM_BIF_parser( *bn, filePath );
    } catch ( gum::SyntaxError& e ) {
      delete bn;
      throw( e );
    } catch ( gum::IOError& e ) {
      delete bn;
      throw( e );
    }

    if ( b ) {
      return bn;
    } else {
      delete bn;
      return NULL;
    }
  }


} /* namespace gum */


#endif // DOXYGEN_SHOULD_SKIP_THIS

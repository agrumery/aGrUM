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
/**
 * @file
 * @brief This file contains definition for a PRMexplorer (read-only access to
 *PRM)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

class PRMexplorer {
  private:
  gum::prm::PRM<double>* __prm;

  public:
  void loadPRM(std::string file,bool verbose=false) {//, const std::string& package = "" ) {
    std::string package="";

    std::stringstream stream;
    gum::prm::o3prm::O3prmReader<double> reader;

    auto nbErr = reader.readFile( file, package);
    reader.showElegantErrorsAndWarnings( stream );
    if ( nbErr > 0 ) {
      reader.showErrorCounts( stream );
      GUM_ERROR( gum::FatalError, stream.str() );
    }
    std::cout << stream.str() << std::endl;

    __prm = reader.prm();
  }

  std::vector<std::string> classes() {
    std::vector<std::string> v;

    for ( auto c : __prm->classes() )
      v.push_back( c->name() );

    return v;
  }
};

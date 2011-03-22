/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/core/dir_utils.h>
// ============================================================================
#include <iostream>
#include <string>
#include <dirent.h>

#define TEST_RESSOURCES string("../../../src/testunits/ressources/")

namespace gum {
namespace tests {

using namespace gum;

class DirTestSuite: public CxxTest::TestSuite {
  std::string oldWorkingDir;
	
  public:
    void setUp() {
		// Save current working directory
		char oldWD[255];
		if ( getcwd(oldWD, 255) != NULL )
			oldWorkingDir = std::string(oldWD);
		
		// Change working directory
		if ( chdir("../../../src/testunits/ressources/dir/") != 0 )
			std::cerr << "Error : Can not go to ../../../src/testunits/ressources/dir/" << std::endl;
    }

    void tearDown() {
		// Return to previous working directory
		if ( oldWorkingDir.empty() || chdir(oldWorkingDir.c_str()) != 0 )
			std::cerr << "Warning : Can not go to previous working directory." << std::endl;
    }
    
    void testValid() {
		TS_ASSERT( Directory("./").isValid() );
		TS_ASSERT( Directory("../").isValid() );
		TS_ASSERT( Directory("../dir/").isValid() );
		TS_ASSERT( Directory("/usr/").isValid() );
		TS_ASSERT( Directory("/usr/../usr/").isValid() );
		TS_ASSERT( ! Directory("./undossierauhasard/").isValid() );
		TS_ASSERT( ! Directory("/undossierauhasard/").isValid() );
		TS_ASSERT( ! Directory("undossierauhasard/").isValid() );
		TS_ASSERT( ! Directory("../dir1/").isValid() );
	}
	
    void testPath() {
		TS_ASSERT( Directory("./").path() == "./" );
		TS_ASSERT( Directory("../").path() == "../" );
		TS_ASSERT( Directory("../dir/").path() == "../dir/" );
		TS_ASSERT( Directory("/usr/").path() == "/usr/" );
		TS_ASSERT( Directory("/usr/../usr/").path() == "/usr/../usr/" );
		TS_ASSERT( Directory("./undossierauhasard/").path() == "./undossierauhasard/" );
	}
	
    void testAbsolutePath() {
		TS_ASSERT( Directory("./undossierauhasard/").absolutePath() == "" );
		TS_ASSERT( Directory("./").absolutePath() == Directory("../dir/").absolutePath() );
		TS_ASSERT( Directory("/usr/").absolutePath() == Directory("/usr/../usr/").absolutePath() );
		TS_ASSERT( Directory("./dir1/").absolutePath() == Directory("./dir2/../dir1/").absolutePath() );
		TS_ASSERT( Directory("./dir1/").absolutePath() != Directory("./dir2/").absolutePath() );
		TS_ASSERT( Directory("./dir1/").absolutePath() != Directory("./dir1/../dir2/").absolutePath() );
	}
    
    void testEntries() {
		std::vector<std::string> entries;
		
		Directory d0("../undossierauhasard/");
		entries = d0.entries();
		TS_ASSERT( entries.size() == 0 );
		
		Directory d1("./");
		entries = d1.entries();
		TS_ASSERT( entries.size() == 5 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir1" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir2" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir3" ) != entries.end() );
		
		Directory d2("../dir/");
		entries = d2.entries();
		TS_ASSERT( entries.size() == 5 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir1" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir2" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir3" ) != entries.end() );
		
		Directory d3("./dir1/../");
		entries = d3.entries();
		TS_ASSERT( entries.size() == 5 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir1" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir2" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "dir3" ) != entries.end() );
		
		Directory d4("./dir1/");
		entries = d4.entries();
		TS_ASSERT( entries.size() == 3 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file1.txt" ) != entries.end() );
		
		Directory d5("../dir/dir2/");
		entries = d5.entries();
		TS_ASSERT( entries.size() == 4 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file1.txt" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file2.txt" ) != entries.end() );
		
		Directory d6("./dir1/../dir3/");
		entries = d6.entries();
		TS_ASSERT( entries.size() == 5 );
		TS_ASSERT( find( entries.begin(), entries.end(), "." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), ".." ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file1.txt" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file2.txt" ) != entries.end() );
		TS_ASSERT( find( entries.begin(), entries.end(), "file3.txt" ) != entries.end() );
	}
	
    void testParent() {
		TS_ASSERT( Directory("./undossierauhasard/").parent().path() == "" );
		TS_ASSERT( Directory("./").parent().absolutePath() == Directory("../dir/").parent().absolutePath() );
		TS_ASSERT( Directory("./").parent().absolutePath() == Directory("../").absolutePath() );
		TS_ASSERT( Directory("./dir1/").parent().absolutePath() == Directory("./dir1/../dir2/").parent().absolutePath() );
		TS_ASSERT( Directory("./dir1/").parent().absolutePath() != Directory("./dir1/../").parent().absolutePath() );
	}
};

} // namespace tests
} // namespace gum
// ============================================================================

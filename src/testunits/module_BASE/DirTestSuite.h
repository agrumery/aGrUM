/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <iostream>
#include <string>

#ifdef HAVE_DIRENT_H
#  include <dirent.h>
#else
#  include <agrum/tools/core/mvsc/dirent.h>
#endif   // HAVE_DIRENT_H

#include <agrum/tools/core/utils_misc.h>
namespace gum_tests {

  class DirTestSuite: public CxxTest::TestSuite {
    std::string oldWorkingDir;

    public:
    void setUp() {
      // Save current working directory
      char oldWD[PATH_MAX];

      if (getcwd(oldWD, PATH_MAX) != nullptr) oldWorkingDir = std::string(oldWD);

      // Change working directory
      if (chdir(XGET_RESSOURCES_PATH(dir)) != 0) {
        GUM_TRACE("Error : Can not go to " << XGET_RESSOURCES_PATH(dir));
        TS_ASSERT(false);
      }
    }

    void tearDown() {
      // Return to previous working directory
      if (oldWorkingDir.empty() || chdir(oldWorkingDir.c_str()) != 0) {
        GUM_TRACE("Warning : Can not go to previous working directory.");
        TS_ASSERT(false);
      }
    }

    void testValid() {
      TS_ASSERT(gum::Directory("./").isValid());
      TS_ASSERT(gum::Directory("../").isValid());
      TS_ASSERT(gum::Directory("../dir/").isValid());
      TS_ASSERT(!gum::Directory("./undossierauhasard/").isValid());
      TS_ASSERT(!gum::Directory("/undossierauhasard/").isValid());
      TS_ASSERT(!gum::Directory("undossierauhasard/").isValid());
      TS_ASSERT(!gum::Directory("../dir1/").isValid());
    }

    void testPath() {
      TS_ASSERT_EQUALS(gum::Directory("./").path(), "./");
      TS_ASSERT_EQUALS(gum::Directory("../").path(), "../");
      TS_ASSERT_EQUALS(gum::Directory("../dir/").path(), "../dir/");
      TS_ASSERT_EQUALS(gum::Directory("/usr/").path(), "/usr/");
      TS_ASSERT_EQUALS(gum::Directory("/usr/../usr/").path(), "/usr/../usr/");
      TS_ASSERT_EQUALS(gum::Directory("./undossierauhasard/").path(), "./undossierauhasard/");
    }

    void testAbsolutePath() {
      TS_ASSERT_EQUALS(gum::Directory("./undossierauhasard/").absolutePath(), "");
      TS_ASSERT_EQUALS(gum::Directory("./").absolutePath(),
                       gum::Directory("../dir/").absolutePath());
      TS_ASSERT(gum::Directory("/usr/").absolutePath()
                == gum::Directory("/usr/../usr/").absolutePath());
      TS_ASSERT(gum::Directory("./dir1/").absolutePath()
                == gum::Directory("./dir2/../dir1/").absolutePath());
      TS_ASSERT(gum::Directory("./dir1/").absolutePath()
                != gum::Directory("./dir2/").absolutePath());
      TS_ASSERT(gum::Directory("./dir1/").absolutePath()
                != gum::Directory("./dir1/../dir2/").absolutePath());
    }

    void testEntries() {
      std::vector< std::string > entries;

      gum::Directory d0("../undossierauhasard/");
      entries = d0.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)0);

      gum::Directory d1("./");
      entries = d1.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)5);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir1"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir2"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir3"), entries.end());

      gum::Directory d2("../dir/");
      entries = d2.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)5);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir1"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir2"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir3"), entries.end());

      gum::Directory d3("./dir1/../");
      entries = d3.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)5);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir1"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir2"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "dir3"), entries.end());

      gum::Directory d4("./dir1/");
      entries = d4.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)3);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file1.txt"), entries.end());

      gum::Directory d5("../dir/dir2/");
      entries = d5.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)4);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file1.txt"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file2.txt"), entries.end());

      gum::Directory d6("./dir1/../dir3/");
      entries = d6.entries();
      TS_ASSERT_EQUALS(entries.size(), (gum::Size)5);
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), ".."), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file1.txt"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file2.txt"), entries.end());
      TS_ASSERT_DIFFERS(find(entries.begin(), entries.end(), "file3.txt"), entries.end());
    }

    void testParent() {
      TS_ASSERT_EQUALS(gum::Directory("./undossierauhasard/").parent().path(), "");
      TS_ASSERT(gum::Directory("./").parent().absolutePath()
                == gum::Directory("../dir/").parent().absolutePath());
      TS_ASSERT(gum::Directory("./").parent().absolutePath()
                == gum::Directory("../").absolutePath());
      TS_ASSERT(gum::Directory("./dir1/").parent().absolutePath()
                == gum::Directory("./dir1/../dir2/").parent().absolutePath());
      TS_ASSERT(gum::Directory("./dir1/").parent().absolutePath()
                != gum::Directory("./dir1/../").parent().absolutePath());
    }
  };

}   // namespace gum_tests

/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef TESTSUITE_UTILS_H
#define TESTSUITE_UTILS_H

#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/threads/threads.h>

#define xstrfy(s) strfy(s)
#define strfy(x)  #x

#define GET_RESSOURCES_PATH(relpath) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" relpath
#define XGET_RESSOURCES_PATH(x)      xstrfy(GUM_SRC_PATH) "/testunits/ressources/" xstrfy(x)

#define FASTVARDBL gum::fastVariable< double >

namespace gum_tests {
  // Scope of a test that reads or changes gum::Configuration: the configuration is saved on entry
  // and restored on exit, even if an assertion throws. It also starts from the defaults of the
  // schema, so that the test does not depend on an `agrum.ini` found in the working directory.
  struct ConfigurationGuard {
    ConfigurationGuard() {
      gum::Configuration::instance().push();
      gum::Configuration::instance().reset();
    }

    ~ConfigurationGuard() { gum::Configuration::instance().pop(); }

    ConfigurationGuard(const ConfigurationGuard&)            = delete;
    ConfigurationGuard& operator=(const ConfigurationGuard&) = delete;
  };

  // Scope of a test that changes the global number of threads (gum::setNumberOfThreads): the
  // number is restored on exit, even if an assertion throws.
  struct NumberOfThreadsGuard {
    NumberOfThreadsGuard() : _saved_(gum::getNumberOfThreads()) {}

    ~NumberOfThreadsGuard() { gum::setNumberOfThreads(_saved_); }

    NumberOfThreadsGuard(const NumberOfThreadsGuard&)            = delete;
    NumberOfThreadsGuard& operator=(const NumberOfThreadsGuard&) = delete;

    private:
    unsigned int _saved_;
  };

  inline const std::string waiter[]{"[*  ]", "[ * ]", "[  *]", "[ * ]"};
  inline const std::string backst = "\b\b\b\b\b";

  inline void begin_test_waiting() {
    std::cout << waiter[0];
    std::flush(std::cout);
  }

  inline void test_waiting(int s = -1) {
    static int ss = 0;

    if (s == -1) s = ss++;
    std::cout << backst << waiter[s % 4];
    std::flush(std::cout);
  }

  inline void end_test_waiting() {
    std::cout << backst;
    std::flush(std::cout);
  }

  inline std::string getTempFilePath(const std::string& suffix = "") {
    static std::atomic< int > counter{0};
    auto                      path = std::filesystem::temp_directory_path()
                                   / ("gum_test_" + std::to_string(++counter) + suffix);
    return path.string();
  }

  // A file of the temporary directory (see getTempFilePath), removed at the end of the scope.
  // Declare it before the streams that open it: they must be closed before the removal (Windows).
  struct TemporaryFile {
    explicit TemporaryFile(const std::string& suffix = "") : path(getTempFilePath(suffix)) {}

    ~TemporaryFile() {
      std::error_code ec;   // the file may not have been created
      std::filesystem::remove(path, ec);
    }

    TemporaryFile(const TemporaryFile&)            = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;

    const std::string path;
  };

  inline void str2file(const std::string& filename, const std::string& message) {
    std::ofstream outFile;
    outFile.open(filename);
    outFile << message << std::endl;
    outFile.close();
  }
}   // namespace gum_tests
#endif   // TESTSUITE_UTILS_H

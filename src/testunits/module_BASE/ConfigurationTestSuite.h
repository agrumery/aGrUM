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

#pragma once


#include <atomic>
#include <clocale>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/threads/threads.h>

#include <agrum/base/core/utils_string.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct ConfigurationTestSuite {
    public:
    static std::string defaults() {
      return R"ini(# free comment
[main]
### int :: a number
number = 4
### float :: a real
real = 1e-6
### bool :: a flag
Flag = False
### on|off|auto :: a mode
mode = auto
### str :: a text
text = \text{do}(
undocumented = whatever

[other]
### str :: a color
Color = #FF7F64
)ini";
    }

    static inline std::atomic< unsigned int > seenThreads{0};

    static std::string tempFile() { return gum::getUniqueFileName(); }

    static std::string readFile(const std::string& filename) {
      std::ifstream     f(filename);
      std::stringstream s;
      s << f.rdbuf();
      return s.str();
    }
  };

  GUM_TEST(ConfigurationGetAndDocumentation) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_EQ(c.get("main", "number"), "4");
    CHECK_EQ(c.get("main", "text"), "\\text{do}(");
    CHECK_EQ(c.get("other", "color"), "#FF7F64");

    CHECK_EQ(c.doc("main", "number"), "int :: a number");
    CHECK_EQ(c.doc("main", "real"), "float :: a real");
    CHECK_EQ(c.doc("main", "flag"), "bool :: a flag");
    CHECK_EQ(c.doc("main", "mode"), "on|off|auto :: a mode");
    CHECK_EQ(c.doc("main", "undocumented"), "str :: (undocumented)");

    CHECK_EQ(c.sections(), std::vector< std::string >({"main", "other"}));
    CHECK_EQ(c.schema().size(), std::size_t(7));
    CHECK_EQ(c.schema().front().key, "number");
    CHECK_EQ(c.schema().front().defaultValue, "4");
  }

  GUM_TEST(ConfigurationCaseSensitivity) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    // keys are not case-sensitive ...
    CHECK_EQ(c.get("main", "FLAG"), "False");
    CHECK_EQ(c.get("other", "COLOR"), c.get("other", "color"));
    CHECK(c.has("main", "Number"));
    c.set("main", "NUMBER", "5");
    CHECK_EQ(c.get("main", "number"), "5");

    // ... but sections are
    CHECK(!c.has("Main", "number"));
    CHECK_THROWS_AS(c.get("Main", "number"), const gum::SyntaxError&);
  }

  GUM_TEST(ConfigurationReadOnlyStructure) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_THROWS_AS(c.get("nosection", "number"), const gum::SyntaxError&);
    CHECK_THROWS_AS(c.get("main", "nokey"), const gum::SyntaxError&);
    CHECK_THROWS_AS(c.set("nosection", "number", "1"), const gum::SyntaxError&);
    CHECK_THROWS_AS(c.set("main", "nokey", "1"), const gum::SyntaxError&);
    CHECK_THROWS_AS(c.doc("main", "nokey"), const gum::SyntaxError&);
    CHECK(!c.has("main", "nokey"));
  }

  GUM_TEST(ConfigurationTypeValidationAndCanonicalForm) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_THROWS_AS(c.set("main", "number", "abc"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.set("main", "number", "1.5"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.set("main", "real", "abc"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.set("main", "flag", "maybe"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.set("main", "mode", "other"), const gum::InvalidArgument&);
    CHECK_EQ(c.get("main", "number"), "4");   // a refused value changes nothing

    c.set("main", "number", "-12");
    CHECK_EQ(c.get("main", "number"), "-12");
    c.set("main", "number", "+7");
    CHECK_EQ(c.getTyped< std::int64_t >("main", "number"), 7);

    for (const auto* yes: {"true", "TRUE", "1", "on", "Yes"}) {
      c.set("main", "flag", yes);
      CHECK_EQ(c.get("main", "flag"), "True");
    }
    for (const auto* no: {"false", "FALSE", "0", "Off", "no"}) {
      c.set("main", "flag", no);
      CHECK_EQ(c.get("main", "flag"), "False");
    }

    c.set("main", "mode", "ON");
    CHECK_EQ(c.get("main", "mode"), "on");   // casing of the declaration

    c.set("main", "text", "anything goes");
    CHECK_EQ(c.get("main", "text"), "anything goes");
  }

  GUM_TEST(ConfigurationTyped) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_EQ(c.getTyped< std::int64_t >("main", "number"), 4);
    CHECK_EQ(c.getTyped< double >("main", "number"), 4.0);   // an int is a valid float
    CHECK_EQ(c.getTyped< double >("main", "real"), 1e-6);
    CHECK_EQ(c.getTyped< bool >("main", "flag"), false);
    c.set("main", "flag", "yes");
    CHECK_EQ(c.getTyped< bool >("main", "flag"), true);
    CHECK_EQ(c.getTyped< std::string >("main", "number"), "4");
    CHECK_EQ(c.getTyped< std::string >("main", "mode"), "auto");

    CHECK_THROWS_AS(c.getTyped< bool >("main", "number"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.getTyped< std::int64_t >("main", "real"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.getTyped< double >("main", "text"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(c.getTyped< bool >("main", "nokey"), const gum::SyntaxError&);
  }

  GUM_TEST(ConfigurationFloatsDoNotDependOnTheLocale) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    // only a decimal or scientific notation with a '.' is accepted
    for (const auto* ok: {"0.5", "-2.25", ".5", "1e-3", "+1.5E2", "3"}) {
      CHECK_NOTHROW(c.set("main", "real", ok));
    }
    for (const auto* ko: {"0,5", "1.5x", "0x10", "nan", "inf", "1e999", ""}) {
      CHECK_THROWS_AS(c.set("main", "real", ko), const gum::InvalidArgument&);
    }

    // the decimal separator of the locale of the program is ignored (see gum::parseDecimal)
    const std::string previous = std::setlocale(LC_NUMERIC, nullptr);
    if (std::setlocale(LC_NUMERIC, "fr_FR.UTF-8") != nullptr
        || std::setlocale(LC_NUMERIC, "de_DE.UTF-8") != nullptr) {
      c.set("main", "real", "0.25");
      CHECK_EQ(c.getTyped< double >("main", "real"), 0.25);
      CHECK_EQ(std::get< double >(c.getValue("main", "real")), 0.25);
    }
    std::setlocale(LC_NUMERIC, previous.c_str());
  }

  GUM_TEST(ConfigurationSetIsCancelledWhenAHookThrows) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    int nbCalls = 0;
    c.addHook([&nbCalls](const gum::Configuration& conf) {
      ++nbCalls;
      if (conf.get("main", "number") == "13") GUM_ERROR(gum::OperationNotAllowed, "unlucky")
    });

    c.set("main", "number", "5");
    CHECK_EQ(nbCalls, 1);

    // the exception of the hook is the one received, the previous value is back and the hooks have
    // been run again with it
    CHECK_THROWS_AS(c.set("main", "number", "13"), const gum::OperationNotAllowed&);
    CHECK_EQ(c.get("main", "number"), "5");
    CHECK_EQ(nbCalls, 3);

    // without hook, nothing can fail
    CHECK_NOTHROW(c.set("main", "number", "13", true));
    CHECK_EQ(c.get("main", "number"), "13");
  }

  GUM_TEST(ConfigurationDefaultsMustBeValid) {
    const std::string h = "[s]\n";

    CHECK_THROWS_AS(gum::Configuration("[s]\n### int :: x\nk = 1\n### int\nl = 2\n"),
                    const gum::SyntaxError&);       // missing '::'
    CHECK_THROWS_AS(gum::Configuration(h + "### integer :: x\nk = 1\n"),
                    const gum::SyntaxError&);       // unknown type
    CHECK_THROWS_AS(gum::Configuration(h + "### int :: \nk = 1\n"),
                    const gum::SyntaxError&);       // empty documentation
    CHECK_THROWS_AS(gum::Configuration(h + "### a||b :: x\nk = a\n"),
                    const gum::SyntaxError&);       // empty item in an enumeration
    CHECK_THROWS_AS(gum::Configuration(h + "### int :: x\n# comment\nk = 1\n"),
                    const gum::SyntaxError&);       // metadata not immediately followed by a key
    CHECK_THROWS_AS(gum::Configuration(h + "### int :: x\n\nk = 1\n"),
                    const gum::SyntaxError&);       // idem, blank line
    CHECK_THROWS_AS(gum::Configuration(h + "### int :: x\n"),
                    const gum::SyntaxError&);       // metadata at the end of file
    CHECK_THROWS_AS(gum::Configuration("### int :: x\nk = 1\n"),
                    const gum::SyntaxError&);       // key outside of any section
    CHECK_THROWS_AS(gum::Configuration(h + "k = 1\nk = 2\n"),
                    const gum::SyntaxError&);       // duplicate key
    CHECK_THROWS_AS(gum::Configuration(h + "k = 1\n[s]\n"),
                    const gum::SyntaxError&);       // duplicate section
    CHECK_THROWS_AS(gum::Configuration(h + "just a line\n"),
                    const gum::SyntaxError&);       // not a key
    CHECK_THROWS_AS(gum::Configuration(h + "### int :: x\nk = abc\n"),
                    const gum::InvalidArgument&);   // default violating its own type
    CHECK_THROWS_AS(gum::Configuration(h + "### deprecated -> s.nokey :: x\nold = 1\n"),
                    const gum::SyntaxError&);       // deprecated -> unknown target
    CHECK_THROWS_AS(gum::Configuration(h
                                       + "### int :: x\nnew = 1\n"
                                         "### deprecated -> s.new :: x\nmid = 1\n"
                                         "### deprecated -> s.mid :: x\nold = 1\n"),
                    const gum::SyntaxError&);   // deprecated -> deprecated

    // four '#' is a free comment, not a metadata line
    CHECK_NOTHROW(gum::Configuration(h + "#### int ::\nk = 1\n"));
    // comments with ';', ':' as a separator, windows line ends
    CHECK_NOTHROW(gum::Configuration("; c\r\n[s]\r\n### int :: x\r\nk : 1\r\n"));
  }

  GUM_TEST(ConfigurationDeprecatedKey) {
    gum::Configuration c("[s]\n### int :: the new one\nnew = 1\n"
                         "### deprecated -> s.new :: replaced\nold = 0\n");

    CHECK(c.takeNotices().empty());

    c.set("s", "old", "5");
    auto notices = c.takeNotices();
    REQUIRE_EQ(notices.size(), std::size_t(1));
    CHECK(notices[0].kind == gum::Configuration::Notice::Kind::DEPRECATION);
    CHECK_EQ(c.get("s", "new"), "5");

    CHECK_EQ(c.get("s", "old"), "5");
    CHECK_EQ(c.getTyped< std::int64_t >("s", "old"), 5);
    CHECK_EQ(c.takeNotices().size(), std::size_t(1));   // the same notice is queued once

    CHECK_EQ(c.doc("s", "old"), "deprecated -> s.new :: replaced");
    CHECK_THROWS_AS(c.set("s", "old", "abc"), const gum::InvalidArgument&);

    // a deprecated key is hidden
    CHECK_EQ(c.toString(), "[s]\n  new = 5");
    CHECK_EQ(c.diff(), "[s]\n  new = 5");
    CHECK_EQ(c.grep("old"), "");
  }

  GUM_TEST(ConfigurationDisplay) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_EQ(c.diff(), "");
    CHECK_EQ(c.toString(),
             "[main]\n  number = 4\n  real = 1e-6\n  flag = False\n  mode = auto\n"
             "  text = \\text{do}(\n  undocumented = whatever\n[other]\n  color = #FF7F64");

    c.set("main", "number", "9");
    c.set("other", "color", "red");
    CHECK_EQ(c.diff(), "[main]\n  number = 9\n[other]\n  color = red");

    // a section matching the search is displayed as a whole
    CHECK_EQ(c.grep("OTHER"), "[other]\n  color = red");
    CHECK_EQ(c.grep("num"), "[main]\n  number = 9");
    CHECK_EQ(c.grep("nothing like that"), "");
  }

  GUM_TEST(ConfigurationResetPushPopAndHooks) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    int nbHooks = 0;
    c.addHook([&nbHooks](const gum::Configuration& conf) {
      ++nbHooks;
      (void)conf.get("main", "number");   // a hook may read the configuration
    });

    c.set("main", "number", "1");
    CHECK_EQ(nbHooks, 1);
    c.set("main", "number", "2", true);
    CHECK_EQ(nbHooks, 1);   // no_hook

    c.push();
    c.set("main", "number", "3");
    c.set("main", "text", "t");
    CHECK_EQ(nbHooks, 3);
    c.pop();
    CHECK_EQ(nbHooks, 4);
    CHECK_EQ(c.get("main", "number"), "2");
    CHECK_EQ(c.get("main", "text"), "\\text{do}(");

    CHECK_THROWS_AS(c.pop(), const gum::OperationNotAllowed&);

    c.reset();
    CHECK_EQ(nbHooks, 5);
    CHECK_EQ(c.get("main", "number"), "4");
    CHECK_EQ(c.diff(), "");

    // invalid value: nothing changes, no hook
    CHECK_THROWS_AS(c.set("main", "number", "x"), const gum::InvalidArgument&);
    CHECK_EQ(nbHooks, 5);
  }

  GUM_TEST(ConfigurationSaveAndLoad) {
    const auto filename = ConfigurationTestSuite::tempFile();

    {
      gum::Configuration c(ConfigurationTestSuite::defaults());
      c.set("main", "number", "9");
      c.set("main", "flag", "yes");
      c.set("other", "color", "red");
      c.save(filename);
    }
    CHECK_EQ(ConfigurationTestSuite::readFile(filename),
             "[main]\n  number = 9\n  flag = True\n[other]\n  color = red\n");

    gum::Configuration c(ConfigurationTestSuite::defaults());
    int                nbHooks = 0;
    c.addHook([&nbHooks](const gum::Configuration&) { ++nbHooks; });
    c.load(filename);
    CHECK_EQ(nbHooks, 1);
    CHECK_EQ(c.get("main", "number"), "9");
    CHECK_EQ(c.getTyped< bool >("main", "flag"), true);
    CHECK_EQ(c.get("other", "color"), "red");
    CHECK(c.takeNotices().empty());
    CHECK_EQ(ConfigurationTestSuite::readFile(filename),
             "[main]\n  number = 9\n  flag = True\n[other]\n  color = red\n");   // not rewritten

    CHECK_THROWS_AS(c.load(filename + ".nosuchfile"), const gum::IOError&);
    CHECK_THROWS_AS(c.save("/nosuchdirectory/nosuchfile"), const gum::IOError&);

    std::filesystem::remove(filename);
  }

  GUM_TEST(ConfigurationLoadWithProblems) {
    const auto filename = ConfigurationTestSuite::tempFile();
    {
      std::ofstream f(filename);
      f << "[nosection]\nk = 1\n[main]\nNUMBER = 12\nnokey = 1\nreal = abc\nflag = on\n";
    }

    gum::Configuration c(ConfigurationTestSuite::defaults());
    CHECK_NOTHROW(c.load(filename));   // problems do not stop the loading ...
    CHECK_EQ(c.get("main", "number"), "12");
    CHECK_EQ(c.get("main", "real"), "1e-6");
    CHECK_EQ(c.get("main", "flag"), "True");

    const auto notices = c.takeNotices();   // ... they are reported ...
    CHECK_EQ(notices.size(), std::size_t(3));
    for (const auto& n: notices)
      CHECK(n.kind == gum::Configuration::Notice::Kind::FILE);

    // ... and the file is left as it was
    CHECK_EQ(ConfigurationTestSuite::readFile(filename),
             "[nosection]\nk = 1\n[main]\nNUMBER = 12\nnokey = 1\nreal = abc\nflag = on\n");

    {
      std::ofstream f(filename);
      f << "k = 1\n";
    }
    CHECK_THROWS_AS(c.load(filename), const gum::SyntaxError&);   // key outside of any section

    std::filesystem::remove(filename);
  }

  GUM_TEST(ConfigurationSaveMigratesDeprecatedKeys) {
    const auto filename = ConfigurationTestSuite::tempFile();
    const auto defaults = std::string("[s]\n### int :: the answer\nnewkey = 1\n"
                                      "### deprecated -> s.newkey :: renamed\noldkey = 0\n");
    {
      std::ofstream f(filename);
      f << "[s]\noldkey = 42\n";
    }

    gum::Configuration c(defaults);
    c.load(filename);
    CHECK_EQ(c.get("s", "newkey"), "42");
    CHECK_EQ(c.takeNotices().size(), std::size_t(1));
    CHECK_EQ(ConfigurationTestSuite::readFile(filename),
             "[s]\noldkey = 42\n");   // load never writes

    c.save(filename);                 // the deprecated key is replaced by its replacement
    CHECK_EQ(ConfigurationTestSuite::readFile(filename), "[s]\n  newkey = 42\n");

    std::filesystem::remove(filename);
  }

  GUM_TEST(ConfigurationSameNoticeIsQueuedOnce) {
    gum::Configuration c("[s]\n### int :: the answer\nnewkey = 1\n"
                         "### deprecated -> s.newkey :: renamed\noldkey = 0\n");

    for (int i = 0; i < 100; ++i)
      c.get("s", "oldkey");
    CHECK_EQ(c.takeNotices().size(), std::size_t(1));
    CHECK(c.takeNotices().empty());

    c.get("s", "oldkey");   // once taken, the notice can be raised again
    CHECK_EQ(c.takeNotices().size(), std::size_t(1));
  }

  GUM_TEST(ConfigurationLoadIsAtomic) {
    const auto filename = ConfigurationTestSuite::tempFile();
    {
      std::ofstream f(filename);
      f << "[main]\nnumber = 12\nthis line is malformed\nflag = on\n";
    }

    gum::Configuration c(ConfigurationTestSuite::defaults());
    int                nbHooks = 0;
    c.addHook([&nbHooks](const gum::Configuration&) { ++nbHooks; });

    CHECK_THROWS_AS(c.load(filename), const gum::SyntaxError&);
    // nothing has been applied (not even the lines before the malformed one), nor reported
    CHECK_EQ(c.diff(), "");
    CHECK_EQ(nbHooks, 0);
    CHECK(c.takeNotices().empty());
    CHECK_EQ(ConfigurationTestSuite::readFile(filename),
             "[main]\nnumber = 12\nthis line is malformed\nflag = on\n");   // not rewritten

    std::filesystem::remove(filename);
  }

  GUM_TEST(ConfigurationGetValue) {
    gum::Configuration c("[s]\n### int :: an int\nn = 4\n### float :: a float\nx = 1e-6\n"
                         "### bool :: a bool\nb = no\n### a|B :: an enum\ne = b\n"
                         "### str :: a str\nt = text\n"
                         "### deprecated -> s.n :: renamed\nold = 0\n");

    CHECK_EQ(std::get< std::int64_t >(c.getValue("s", "n")), 4);
    CHECK_EQ(std::get< double >(c.getValue("s", "x")), 1e-6);
    CHECK_EQ(std::get< bool >(c.getValue("s", "b")), false);
    CHECK_EQ(std::get< std::string >(c.getValue("s", "e")), "B");
    CHECK_EQ(std::get< std::string >(c.getValue("s", "t")), "text");

    c.set("s", "b", "ON");
    CHECK_EQ(std::get< bool >(c.getValue("s", "b")), true);

    // a deprecated key gives the value of its replacement
    CHECK_EQ(std::get< std::int64_t >(c.getValue("s", "old")), 4);
    CHECK_EQ(c.takeNotices().size(), std::size_t(1));

    CHECK_THROWS_AS(c.getValue("s", "nokey"), const gum::SyntaxError&);
    CHECK_THROWS_AS(c.getValue("nosection", "n"), const gum::SyntaxError&);
  }

  GUM_TEST(ConfigurationMetaAndDescribe) {
    using Kind = gum::Configuration::Kind;
    gum::Configuration c(ConfigurationTestSuite::defaults());

    CHECK_EQ(gum::Configuration::kindName(Kind::INT), "int");
    CHECK_EQ(gum::Configuration::kindName(Kind::FLOAT), "float");
    CHECK_EQ(gum::Configuration::kindName(Kind::BOOL), "bool");
    CHECK_EQ(gum::Configuration::kindName(Kind::STR), "str");
    CHECK_EQ(gum::Configuration::kindName(Kind::ENUM), "enum");
    CHECK_EQ(gum::Configuration::kindName(Kind::DEPRECATED), "deprecated");

    const auto mode = c.meta("main", "mode");
    CHECK(mode.kind == Kind::ENUM);
    CHECK_EQ(mode.joinedValues(), "on|off|auto");
    CHECK_EQ(c.meta("main", "number").joinedValues(), "");

    // nothing customized: the whole configuration is displayed, after a comment
    CHECK_EQ(c.describe(), "# no customized property\n" + c.toString());
    c.set("main", "number", "9");
    CHECK_EQ(c.describe(), c.diff());
  }

  GUM_TEST(ConfigurationConcurrentAccess) {
    gum::Configuration c(ConfigurationTestSuite::defaults());

    std::atomic< bool >        ok{true};
    std::atomic< int >         nbHooks{0};
    std::vector< std::thread > threads;
    c.addHook([&nbHooks](const gum::Configuration& conf) {
      ++nbHooks;
      (void)conf.get("main", "number");
    });

    for (int t = 0; t < 4; ++t) {
      threads.emplace_back([&c, &ok, t] {
        for (int i = 0; i < 200; ++i) {
          try {
            if (t % 2 == 0) {
              c.set("main", "number", std::to_string(i));
            } else {
              const auto v = c.getTyped< std::int64_t >("main", "number");
              if (v < 0 || v >= 200) ok = false;
              (void)c.toString();
            }
          } catch (...) { ok = false; }
        }
      });
    }
    for (auto& th: threads)
      th.join();

    CHECK(ok.load());
    CHECK_EQ(nbHooks.load(), 400);
  }

  GUM_TEST(ConfigurationSingleton) {
    auto& c = gum::Configuration::instance();
    CHECK_EQ(&c, &gum::Configuration::instance());

    // the embedded defaults are the ones of defaults.ini
    CHECK(c.has("core", "maxNumberOfThreads"));
    CHECK(c.has("core", "jgumIndent"));
    CHECK(c.has("core", "allow_modification_when_saving"));
    CHECK_EQ(
        c.doc("core", "maxNumberOfThreads"),
        "int :: default maximum number of threads used by aGrUM (-1 = adapted to the machine)");

    CHECK(!gum::Configuration::userFileName().empty());
  }

#ifndef GUM_FOR_SWIG
  GUM_TEST(ConfigurationOfAgrumDoesNotContainTheKeysOfPyAgrum) {
    // the sections of pyAgrum are added to the defaults of aGrUM only when pyAgrum is compiled
    auto& conf = gum::Configuration::instance();
    CHECK(conf.has("core", "maxNumberOfThreads"));
    CHECK(!conf.has("notebook", "node_bgcolor"));
    CHECK_EQ(gum::Configuration::userFileName(), "agrum.ini");
  }
#endif

  GUM_TEST(ConfigurationNoVisibleKeyStartsWithDefault) {
    for (const auto& entry: gum::Configuration::instance().schema()) {
      if (entry.meta.kind == gum::Configuration::Kind::DEPRECATED) continue;
      CHECK_MESSAGE(entry.key.rfind("default_", 0) != 0,
                    entry.section << "." << entry.key << ": 'default' is implied in defaults.ini");
    }
  }

  GUM_TEST(ConfigurationDeprecatedKeysOfTheDefaultsAreWellFormed) {
    // nothing to check once the deprecated keys are removed from defaults.ini
    auto& conf = gum::Configuration::instance();
    for (const auto& entry: conf.schema()) {
      if (entry.meta.kind != gum::Configuration::Kind::DEPRECATED) continue;
      const auto name = entry.section + "." + entry.key;

      CHECK_MESSAGE(!entry.meta.doc.empty(), name << " has no message");
      REQUIRE_MESSAGE(conf.has(entry.meta.targetSection, entry.meta.targetKey),
                      name << " targets an unknown key");
      CHECK_MESSAGE(conf.meta(entry.meta.targetSection, entry.meta.targetKey).kind
                        != gum::Configuration::Kind::DEPRECATED,
                    name << " targets a deprecated key");
      CHECK_EQ(conf.get(entry.section, entry.key),
               conf.get(entry.meta.targetSection, entry.meta.targetKey));
    }
    conf.takeNotices();   // the accesses above have queued deprecation notices
  }

  GUM_TEST(ConfigurationSingletonAppliesTheNumberOfThreads) {
    auto&              conf   = gum::Configuration::instance();
    const unsigned int before = gum::getNumberOfThreads();

    conf.push();
    conf.set("core", "maxNumberOfThreads", "3");
    CHECK_EQ(gum::getNumberOfThreads(), 3u);
    conf.set("core", "maxNumberOfThreads", "2");
    CHECK_EQ(gum::getNumberOfThreads(), 2u);

    // no hook: the configuration changes, not the library
    conf.set("core", "maxNumberOfThreads", "5", true);
    CHECK_EQ(gum::getNumberOfThreads(), 2u);
    conf.runHooks();
    CHECK_EQ(gum::getNumberOfThreads(), 5u);

    // a negative value gives back the number of threads provided by the machine
    conf.set("core", "maxNumberOfThreads", "-1");
    CHECK_GE(gum::getNumberOfThreads(), 1u);

    conf.pop();
    gum::setNumberOfThreads(before);
  }

  GUM_TEST(ConfigurationUserHooksRunAfterTheBuiltInOnes) {
    auto& conf = gum::Configuration::instance();

    // a hook of the singleton cannot be removed: it must only capture what outlives the test
    ConfigurationTestSuite::seenThreads = 0;
    conf.addHook([](const gum::Configuration&) {
      ConfigurationTestSuite::seenThreads = gum::getNumberOfThreads();
    });
    // hooks are never called when registered, only after a change
    CHECK_EQ(ConfigurationTestSuite::seenThreads.load(), 0u);

    conf.push();
    conf.set("core", "maxNumberOfThreads", "4");
    CHECK_EQ(ConfigurationTestSuite::seenThreads.load(), 4u);   // the built-in hook has already run
    conf.pop();
  }

}   // namespace gum_tests

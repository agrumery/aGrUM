############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
TEMPORARY DIAGNOSTIC (2026-09-07): audits every pyAgrum leaf module for the
GUM_PUBLIC-vs-PYGUM_SHARED_PUBLIC visibility bug (see commits cf75ee306,
ef844b9a3, 5e0dfe594) -- a BASE/BN class tagged plain GUM_PUBLIC (or left
untagged) instead of PYGUM_SHARED_PUBLIC/GUM_SHARED_PUBLIC never gets
exported from core's _pyagrumcpp.so under BUILD_PYTHON (-fvisibility=hidden
blanks it), so any leaf module (id/mrf/cn/cm/prm) whose SWIG-generated or
inline/template code needs that symbol fails with "undefined symbol" at
import time -- ImportError, not a runtime crash inside a specific function,
because typeinfo/vtable data relocations are resolved at dlopen time even
under lazy binding.

The existing test harness (testsOnPython.py) builds one dict comprehension
that imports every test module eagerly and aborts entirely on the FIRST
ImportError, so CI has only ever revealed one broken class per run --
three round-trips so far (GreedyHillClimbing/K2/etc., then ParamEstimator/
Score/Prior/etc.). This script instead imports each leaf submodule
independently and keeps going, so a single CI run reports every currently
broken export at once instead of one at a time.

Usage: run with the SAME Python interpreter pyAgrum was installed into,
after `act install`/`act pipinstall` succeeds (Linux only -- macOS/Windows
don't reproduce this: macOS tolerates unresolved symbols via lazy binding,
Windows fails at LINK time instead, already visible in the build log).
Exits 0 if every module imports cleanly, 1 otherwise (does not fail the
build on its own when invoked with `|| true`; see build_linux.sh).

Remove this script (and its call site in wheelhouse/build_linux.sh) once
the audit is complete and CI has been green for a while.
"""

import importlib
import sys

MODULES = [
    "pyagrum",
    "pyagrum.influence_diagram",
    "pyagrum.markov_random_field",
    "pyagrum.credal_net",
    "pyagrum.causal_model",
    "pyagrum.prm",
    "pyagrum.ktbn",
]


def main() -> int:
    print("** pyAgrum DEBUG-IMPORTS: auditing leaf module imports for missing exports")
    failures = []
    for name in MODULES:
        try:
            importlib.import_module(name)
        except ImportError as exc:
            print(f"** pyAgrum DEBUG-IMPORTS: FAIL {name}: {exc}")
            failures.append(name)
        else:
            print(f"** pyAgrum DEBUG-IMPORTS: OK   {name}")

    if failures:
        print(
            f"** pyAgrum DEBUG-IMPORTS: {len(failures)}/{len(MODULES)} module(s) "
            f"failed to import: {', '.join(failures)}"
        )
        return 1

    print(f"** pyAgrum DEBUG-IMPORTS: all {len(MODULES)} modules imported cleanly")
    return 0


if __name__ == "__main__":
    sys.exit(main())

############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import time
import platform

from .utils import *

from .ActBuilderAgrum import ActBuilderAgrum


class ActBuilderDocDoxygen(ActBuilderAgrum):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self):
    self.check_compiler_and_maker()
    if platform.system() == "Windows" and self.current["compiler"] != "mingw64":
      error("No doxygen support on windows with this compiler")
      return False

    if self.current["mode"].lower() != "release":
      warn("Doxygen is only supported in release mode. The documentation will then be built in release mode.")
      self.current["mode"] = "Release"

    return True

  def build(self) -> bool:
    self.modules = set(cfg.modules.keys())

    prefix = "build ⮕ " + self.current["mode"] + " ⮕ "
    self.run_start(prefix)

    err = False
    gc = gm = gb = 0
    t0 = time.time()

    safe_cd(self.current, "build")
    safe_cd(self.current, "aGrUM")
    safe_cd(self.current, cfg.buildPath[self.current["mode"]])

    self.run_start(prefix + "cmake")
    cmake_cde = self.build_cmake()
    err = err or 0 < self.execFromLine(cmake_cde, checkRC=False)

    t1 = time.time()
    self.run_start(prefix + "make")
    make_cde = self.build_make()
    err = err or 0 < self.execFromLine(make_cde, checkRC=False)
    t2 = time.time()

    gc = t1 - t0
    gm = t2 - t1
    safe_cd(self.current, "..")
    safe_cd(self.current, "..")
    safe_cd(self.current, "..")

    self.conclude(gc, gm)
    if not err:
      self.run_done(prefix)
    else:
      self.run_failed(prefix)

    return not err

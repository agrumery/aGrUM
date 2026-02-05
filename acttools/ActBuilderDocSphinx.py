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

from .utils import *

from .ActBuilder import ActBuilder


class ActBuilderDocSphinx(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self):
    return True

  def build(self) -> bool:
    self.run_start()
    if self.current["build"] != "doc-only":
      self.run_start("Compiling pyAgrum")
      buildcurrent = self.current.copy()
      buildcurrent["action"] = "lib"
      buildcurrent["taret"] = "pyAgrum"
      buildcurrent["mode"] = "Release"
      from .ActBuilderPyAgrum import ActBuilderPyAgrum

      b = ActBuilderPyAgrum(buildcurrent)
      b.build()
      self.run_done("Compiling pyAgrum")

    self.run_start("Sphinxing pyAgrum")
    safe_cd(self.current, "wrappers")
    safe_cd(self.current, "pyagrum")
    safe_cd(self.current, "doc")
    self.execFromLine("make")
    safe_cd(self.current, "..")
    safe_cd(self.current, "..")
    safe_cd(self.current, "..")
    self.run_done("Sphinxing pyAgrum")

    self.run_done()

    return True

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

import tempfile
import subprocess

from .utils import *

from .ActBuilder import ActBuilder
from .ActBuilderWheel import ActBuilderWheel


class ActBuilderPipInstall(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self):
    return True

  def build(self) -> bool:
    self.run_start()

    tmp = tempfile.mkdtemp(prefix="act")
    self.run_start(f"Building wheel in {tmp}")
    newcurrent = self.current.copy()
    newcurrent["destination"] = tmp
    builder = ActBuilderWheel(newcurrent)
    if not builder.check_consistency():
      self.warn("FAILED")
      return False
    if not builder.build():
      self.warn("FAILED")
      return False
    self.run_done(f"Wheel  : {builder.wheel_path}")
    # use pip to install the wheel
    self.run_start(f"Installing wheel via pip")
    cmd = [sys.executable, "-m", "pip", "install", "--force-reinstall", builder.wheel_path]
    self.run_start(f"Running command: {' '.join(cmd)}")
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    stdout, _ = proc.communicate()
    if proc.returncode != 0:
      self.run_failed(f"Installing wheel via pip : failed")
      print(stdout)
      return False
    self.run_done(f"Wheel installed.")
    return True

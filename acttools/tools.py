############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import os
import sys
from subprocess import PIPE, Popen

from typing import Optional

from .configuration import cfg
from .utils import critic


def cmdline(command: str) -> str:
  process = Popen(
    args=command,
    stdout=PIPE,
    shell=True
  )
  return process.communicate()[0].decode()


#################################################################################################
# find make, python3
def is_tool(prog: str, longpath=False) -> Optional[str]:
  progw = prog + ".exe"
  for dirname in os.environ['PATH'].split(os.pathsep):
    if os.path.exists(os.path.join(dirname, prog)):
      return prog if not longpath else '"' + os.path.join(dirname, prog) + '"'
    if os.path.exists(os.path.join(dirname, progw)):
      return progw if not longpath else '"' + os.path.join(dirname, progw) + '"'
  return None


def check_tools() -> tuple[str, str, str, str, str]:
  exe_py = f'"{sys.executable}"'

  version, subversion, _ = cmdline(
    exe_py + ' -c "import platform;print(platform.python_version())"').split(".")

  if version == "2":
    critic('python2 is not supported anymore. Please use pyAgrum 0.21.x.')
  else:
    if int(subversion) < 9:
      critic('python<3.9 is not supported anymore.')
  cfg.python_version = f"{version}.{subversion}"

  exe_cmake = is_tool("cmake")
  if exe_cmake is None:
    critic("No <cmake> utility found. Exit")

  exe_make = is_tool("mingw32-make.exe")
  if exe_make is None:
    exe_make = is_tool("make")
  if exe_make is None:
    exe_make = is_tool("msbuild")
  if exe_make is None:
    critic("No <make> utility found. Exit")

  exe_clangformat = None
  if is_tool("clang-format"):
    exe_clangformat = "clang-format"
  if exe_clangformat is None:
    for version in ['', '-13.0', '-12.0', '-11.0']:
      if is_tool(f"clang-format{version}"):
        exe_clangformat = f"clang-format{version}"
        break

  exe_msbuild = is_tool("msbuild")

  return exe_py, exe_cmake, exe_make, exe_clangformat, exe_msbuild

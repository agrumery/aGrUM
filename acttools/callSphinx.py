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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

from subprocess import PIPE, Popen, STDOUT

from .configuration import cfg
from .utils import notif, safe_cd


def _callSphinx(current: dict[str, str], cmd: str):
  if not current["dry_run"]:
    proc = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
    out = proc.stdout.readlines()
    for line in out:
      try:
        print(line.decode("utf-8"), end="")
      except ValueError:
        print(str(line), end="")
  else:
    notif("[" + cmd + "]")


def callSphinx(current: dict[str, str]):
  if current["build"] != "doc-only":
    notif("Compiling pyAgrum")
    _callSphinx(current, f"{cfg.python} act lib pyAgrum release --no-fun")

  notif("Sphinxing pyAgrum")
  safe_cd(current, "wrappers")
  safe_cd(current, "pyagrum")
  safe_cd(current, "doc")
  _callSphinx(current, "make")

  safe_cd(current, "..")
  safe_cd(current, "..")
  safe_cd(current, "..")

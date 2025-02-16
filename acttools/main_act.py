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

import time
import platform

from . import *
from .utils import notif


def main():
  #
  # options management
  initParams()
  # persistent values of current are used as default for options
  current = getCurrent()
  configureOptions(current)

  (options, args) = parseCommandLine()
  mvscForced = False
  if platform.system() == "Windows" and options.compiler.startswith("mvsc"):
    options.static_lib = True
    options.no_fun = True
    mvscForced = True

  mingwForced = False
  if platform.system() == "Windows" and options.compiler == "mingw64":
    options.no_fun = True
    mingwForced = True

  pyAgrumForced = False
  if "pyAgrum" in args:
    options.static_lib = True
    pyAgrumForced = True

  # colors, fun & verbosity partly controlled by options (--no-fun, ...)
  configureOutputs(options)

  about()

  if mvscForced:
    notif("Options [static] and [no-fun] forced by option [mvsc*]")

  if mingwForced:
    notif("Options [no-fun] forced by option [mingw64]")

  if pyAgrumForced:
    notif("Options [static] forced by target [pyAgrum]")

  configureTools()

  # check current consistency and update it if necessary
  checkCurrent(current, options, args)

  #
  # from now, current contains the specification
  if current['verbose']:
    print()
    notif("Invocation :")
    print(getInvocation(current))
    print()

  # looking at special commands (clean/show/etc.)
  gc = gm = gb = 0
  if not specialActions(current):
    # creating "classical" compilation from cmake/make/postprocess

    for target in current["targets"]:
      safe_cd(current, "build")
      safe_cd(current, target)
      safe_cd(current, current["mode"])

      t0 = time.time()
      if options.build == "all":
        buildCmake(current, target)
      t1 = time.time()
      if options.build != "no-make":
        buildMake(current, target)
      t2 = time.time()
      buildPost(current, target)
      t3 = time.time()
      gc += t1 - t0
      gm += t2 - t1
      gb += t3 - t2

      safe_cd(current, "..")
      safe_cd(current, "..")
      safe_cd(current, "..")

  return gc, gm, gb

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

import pickle

from .configuration import cfg
from .invocation import showInvocation
from .modules import check_modules
from .tests import checkAndWriteTests
from .utils import error, notif, critic, setifyString


def parseCommandLine():
  a = cfg.parser.parse_args()
  return a, a.cmds


def getCurrent() -> dict[str, str]:
  current = {}
  try:
    with open(cfg.configFile, "rb") as fp:
      shlv = pickle.load(fp)
  except FileNotFoundError:
    shlv = {}

  for key in cfg.default:  # .iterkeys():
    current[key] = cfg.default[key]
    if key not in cfg.non_persistent and key in shlv:
      current[key] = shlv[key]

  return current


def setCurrent(current: dict[str, str]):
  shlv = {}
  for key in current.keys():
    if key not in cfg.non_persistent:
      shlv[key] = current[key]

  with open(cfg.configFile, "wb") as fp:
    pickle.dump(shlv, fp)


def checkCurrent(current: dict[str, str], options: dict[str, str], args: list[str]):
  # helper
  def update(current: dict[str, str], key, val, test):
    if test and current[key] != val:
      current[key] = val
    return test

  # end of helper

  # fixing options
  for opt, value in options.__dict__.items():
    if opt == "cmds":  # cmds are not options
      continue
    if opt not in current:
      error(f"Options not known : {opt} in {current.keys()}")

    update(current, opt, value, current[opt] != value)

  # fixing possible "\" from compiler
  current["destination"] = current["destination"].replace("\\", "/")

  bT = bA = bM = False
  # fixing args
  for ar in args:
    t = setifyString(ar)
    arg = "+".join(t)
    if update(current, "targets", t, t.issubset(cfg.targets)):
      if bT:
        error(f"Targets overwritten by [{'+'.join(t)}]")
      bT = True
      continue
    if update(current, "action", arg, arg in cfg.actions):
      if bA:
        error(f"Action overwritten by [{arg}]")
      bA = True
      continue
    if update(current, "mode", arg, arg in cfg.modes):
      if bM:
        error(f"Mode overwritten by [{arg}]")
      bM = True
      continue

    critic(f"arg [{arg}] unknown")

  checkConsistency(current)

  if not options.noSaveParams:
    setCurrent(current)
  showInvocation(current)


def checkConsistency(current: dict[str, str]):
  has_notif = False

  # helper
  def check_aGrumTest(option, current):
    if current[option]:
      prefix = f"Option [{option}] acts only"
      if current["targets"] != {"aGrUM"}:
        has_notif = True
        notif(prefix + " on target [aGrUM].")
      if current["action"] != "test":
        critic(f"{prefix} on action [test] (not on [{current['action']}]).")

  # end of helper

  # test for only one target
  if current["action"] == "test":
    # check -t and -m
    check_modules(current)
    checkAndWriteTests(current)

    if len(current["targets"]) > 1:
      first = "aGrUM" if "aGrUM" in current["targets"] else list(current["targets"])[0]
      has_notif = True
      notif("Action [test] on only one target : selecting [" + first + "]")
      current["targets"] = [first]

  if current["stats"] and current["oneByOne"]:
    has_notif = True
    notif("Options [stats] and [oneByOne] are mutually exclusive")

  check_aGrumTest("oneByOne", current)
  if current["coverage"] and current["mode"] != "debug":
    error("Option [coverage] can only be used with [debug] builds.")
    current["coverage"] = False

  if current["action"] == "package":
    critic("Action [package] is not implemented yed")

  if has_notif:
    print("")

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

import platform
import pickle
import argparse


from .configuration import cfg
from .utils import *
from .configuration import init_params, configure_cli_options
from .configuration import configure_colors
from .invocation import show_invocation


def extract_cmd_from_args(current: dict[str, str | bool], args: set[str]) -> None:
  """
  Update current with the command found in args.

  Parameters
  ----------
  current : dict[str, str]
    The current configuration dictionary.
  args : set[str]
    The set of so-called commands in CLI args (with no orders).
  """

  # helper
  def _update(key: str, val: str, test: bool) -> bool:
    # only 2 rules for modifiying keys :
    if val.lower() == "agrum":
      val = "aGrUM"
    elif val.lower() == "pyagrum":
      val = "pyAgrum"

    if test and current[key] != val:
      current[key] = val
    return test

  # end of helper
  bT = bA = bM = False
  # fixing args
  for t in args:
    if _update("target", t, t.lower() in cfg.targets):
      if bT:
        error(f"Target overwritten by [{t}]")
      bT = True
      continue
    if _update("action", t, t in cfg.actions):
      if bA:
        error(f"Action overwritten by [{t}]")
      bA = True
      continue
    if _update("mode", t, t in cfg.modes):
      if bM:
        error(f"Mode overwritten by [{t}]")
      bM = True
      continue

    critic(f"arg [{t}] unknown")


def update_options_from_args(current: dict[str, str | bool], options: argparse.Namespace) -> None:
  """
  Update current with the options parsed from the command line.

  Parameters
  ----------
  current : dict[str, str]
    The current configuration dictionary.
  options : argparse.Namespace
    The options parsed from the command line.
  """
  for opt, value in options.__dict__.items():
    if opt == "cmds":  # cmds are not options
      continue
    if opt not in current:
      error(f"Options not known : {opt} in {current.keys()}")

    if value is not None:  # only update if value is not None
      current[opt] = value

  adapt_options_from_context(options, current)


def adapt_options_from_context(options: argparse.Namespace, args: set[str]) -> None:
  """
  Adapt (change) options from the context (platform, target, etc.)

  Parameters
  ----------
  options
  : argparse.Namespace
    The options parsed from the command line.
  args
  """
  if platform.system() == "Windows" and options.compiler.startswith("mvsc"):
    options.static_lib = True
    # options.no_fun = True
    notif("Options [static] forced by option [mvsc*]")
  elif platform.system() == "Windows" and options.compiler == "mingw64":
    options.no_fun = True
    notif("Options [no-fun] forced by platform")


def show_pickle() -> None:
  """
  Show the content of the pickle file used to store the current configuration.
  """
  main = ["target", "action", "mode"]
  try:
    with open(cfg.configFile, "rb") as fp:
      shlv = pickle.load(fp)
      notif("Current configuration loaded from pickle:")
      notif()
      for key in main:
        if key in shlv:
          warn(f"  [{key}]: {shlv[key]}")
        else:
          error(f"  [{key}] not set")
      notif()
      for key in sorted(shlv.keys()):
        if key not in main:
          notif(f"  [{key}]: {shlv[key]}")
  except FileNotFoundError:
    error(f"Configuration file {cfg.configFile} not found.")


def load_current_from_pickle() -> dict[str, str]:
  current = {}
  shlv = {}
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


def save_current_to_pickle(current: dict[str, str | bool]) -> None:
  """
  Save the current configuration to a pickle file.

  Parameters
  ----------
  current : dict[str, str]
    The current configuration dictionary.
  """
  shlv = {k: current[k] for k in current if k not in cfg.non_persistent}
  with open(cfg.configFile, "wb") as fp:
    pickle.dump(shlv, fp, protocol=pickle.HIGHEST_PROTOCOL)


def main() -> int:
  init_params()

  # CLI options and default management
  current = load_current_from_pickle()
  configure_cli_options(current)
  configure_colors(current["no_fun"])
  about()

  options = cfg.parser.parse_args()
  extract_cmd_from_args(current, options.cmds)
  update_options_from_args(current, options)

  original_current = current.copy()  # keep a copy of the original current before any modification

  if current["verbose"]:
    show_invocation(current)

  # adapt_options_from_context(options, args)
  builder = None
  res = 0

  match current["action"]:
    case "show":
      show_pickle()
    case "clean" | "purge":
      from .ActBuilderCleaning import ActBuilderCleaning

      builder = ActBuilderCleaning(current)
    case "show":
      from .ActBuilderShowConfig import ActBuilderShowConfig

      builder = ActBuilderShowConfig(current)
    case "guideline":
      from .ActBuilderGuideline import ActBuilderGuideline

      builder = ActBuilderGuideline(current)
    case "wheel" | "nightly_wheel":
      from .ActBuilderWheel import ActBuilderWheel

      builder = ActBuilderWheel(current)
    case "doc":
      if current["target"] == "aGrUM":
        from .ActBuilderDocDoxygen import ActBuilderDocDoxygen

        builder = ActBuilderDocDoxygen(current)
      elif current["target"] == "pyAgrum":
        from .ActBuilderDocSphinx import ActBuilderDocSphinx

        builder = ActBuilderDocSphinx(current)
      else:
        error(f"Unknown target [{current['target']}]")
        res = 1
    case "build" | "install" | "test" | "lib":
      if current["target"] == "aGrUM":
        from .ActBuilderAgrum import ActBuilderAgrum

        builder = ActBuilderAgrum(current)
      elif current["target"] == "pyAgrum":
        from .ActBuilderPyAgrum import ActBuilderPyAgrum

        builder = ActBuilderPyAgrum(current)
      else:
        error(f"Unknown target [{current['target']}]")
        res = 2
    case "pipinstall":
      from .ActBuilderPipInstall import ActBuilderPipInstall

      builder = ActBuilderPipInstall(current)

    case _:
      show_invocation(current)
      res = 3

  if builder is not None:
    if builder.check_consistency():
      if not current["noSaveParams"]:
        notif("Saving current configuration.")
        if not current["dry_run"]:
          save_current_to_pickle(original_current)
      if builder.build():
        notif("Build successful 🎉                    ")
      else:
        error("Build failed 😭                        ")
        res = 100
    else:
      if not current["verbose"]:  # already shown if verbose
        show_invocation(current)
        res = 3

  return res

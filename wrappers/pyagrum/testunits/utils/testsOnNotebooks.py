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

from typing import List

from nbconvert.preprocessors.execute import ExecutePreprocessor, CellExecutionError
import concurrent.futures
import logging
import nbformat
import random
import time
import traceback
import os
import glob
import sys
import warnings

from .gumTestOutput import notif, warn, error

warnings.filterwarnings("ignore", category=RuntimeWarning)

os.environ["PYDEVD_DISABLE_FILE_VALIDATION"] = "1"

# Width reserved for the notebook name in the live display.
_NB_NAME_WIDTH = 44

# Loggers that are noisy during headless notebook execution.
_NOISY_LOGGERS = ("nbconvert", "jupyter_client", "jupyter_core", "traitlets")


def processNotebook(notebook_filename: str) -> tuple[int, str, float]:
  # Silence noisy loggers — we are in a worker subprocess.
  for name in _NOISY_LOGGERS:
    logging.getLogger(name).setLevel(logging.CRITICAL)

  err = 0
  MAX_KERNEL_RETRIES = 3
  starttime = time.time()
  try:
    for attempt in range(MAX_KERNEL_RETRIES):
      try:
        ep = ExecutePreprocessor(timeout=5000, kernel_name="python3")
        ep.log_output = False
        with open(notebook_filename, encoding="utf-8") as f:
          nb = nbformat.read(f, as_version=4)
        nb["cells"].insert(
          0,
          nbformat.from_dict(
            {
              "outputs": [],
              "execution_count": 1,
              "metadata": {"collapsed": False, "editable": True, "deletable": True},
              "cell_type": "code",
              "source": 'import os,sys\nsys.path.insert(0, os.path.abspath("../../../build/pyAgrum/release/wrappers"))',
            }
          ),
        )
        # Redirect worker stdout/stderr to /dev/null so that any remaining
        # chatter from the kernel or nbconvert does not pollute the display.
        with open(os.devnull, "w") as _devnull:
          _saved = sys.stdout, sys.stderr
          sys.stdout = sys.stderr = _devnull
          try:
            ep.preprocess(nb, {"metadata": {"path": "../doc/sphinx/notebooks/"}})
          finally:
            sys.stdout, sys.stderr = _saved
        break
      except RuntimeError as e:
        if str(e) in {"Kernel died before replying to kernel_info", "Kernel didn't respond in 60 seconds"}:
          if attempt < MAX_KERNEL_RETRIES - 1:
            time.sleep(random.randint(5, 10) / 10.0)
          else:
            raise CellExecutionError(traceback=str(e.__traceback__), ename=str(e), evalue="")
        else:
          raise CellExecutionError(traceback=str(e.__traceback__), ename=str(e), evalue="")
  except CellExecutionError:
    err = 1
    errorfilename = "../../../" + time.strftime("%Y%m%d_%H%M_") + os.path.basename(notebook_filename) + ".log"
    with open(errorfilename, "w") as errfn:
      traceback.print_exc(file=errfn)

  duration = time.time() - starttime
  return err, os.path.basename(notebook_filename), duration


def _nb_line(icon: str, dur_str: str, name: str) -> str:
  """Format one notebook status line for the live display."""
  return f"  {icon} [{dur_str}] {name[: _NB_NAME_WIDTH]:{_NB_NAME_WIDTH}}"


def runNotebooks(lonb: list[str] = None):
  # 80-Applications_ipywidgets.ipynb requires ipywidgets and a live kernel display;
  # it cannot run headlessly under nbconvert.
  excludes = {"80-Applications_ipywidgets.ipynb"}

  if lonb is None:
    lonb = sorted(
      [
        filename
        for filename in glob.glob("../doc/sphinx/notebooks/*.ipynb")
        if os.path.basename(filename) not in excludes
      ]
    )
  else:
    includes = set(lonb)
    lonb = sorted(
      [
        filename
        for filename in glob.glob("../doc/sphinx/notebooks/*.ipynb")
        if (bn := os.path.basename(filename)) not in excludes and bn in includes
      ]
    )

  start_time = time.time()

  # Closure state: results keyed by future id to avoid re-calling f.result() in callbacks.
  _results: dict[int, tuple[int, str, float]] = {}
  _display_count = 0

  _SEP = "─" * (_NB_NAME_WIDTH + 16)
  futures = []

  def done(fn):
    nonlocal _display_count
    _results[id(fn)] = fn.result()

    # Move cursor up to overwrite previous block (separator + one line per
    # future + separator = 2 + len(futures) lines), then erase to end.
    if _display_count > 0:
      sys.stdout.write(f"\033[{2 + len(futures)}A\033[J")
      sys.stdout.flush()
    _display_count += 1

    print(_SEP)
    for f in futures:
      if id(f) in _results:
        e, name, dur = _results[id(f)]
        icon = "💥" if e else "✅"
        print(_nb_line(icon, f"{dur:7.2f}s", name))
      elif f.running():
        print(_nb_line("⏳", " ...... ", os.path.basename(f.filename)))
      else:
        print(_nb_line("💤", "        ", os.path.basename(f.filename)))
    print(_SEP)

  with concurrent.futures.ProcessPoolExecutor() as executor:
    for notebook_filename in lonb:
      fut = executor.submit(processNotebook, notebook_filename)
      fut.add_done_callback(done)
      fut.filename = notebook_filename
      futures.append(fut)
    concurrent.futures.wait(futures)

  errs = 0
  total_time = 0
  for f in futures:
    e, _, d = _results[id(f)]
    total_time += d
    errs += e

  elapsed_time = time.time() - start_time
  notif()
  notif(f"## Profiling : {total_time * 1000:.2f} ms ##")
  notif(f"## Duration  : {elapsed_time * 1000:.2f} ms ##")
  (error if errs > 0 else notif)(f"Failed {errs} of {len(lonb)} tests")
  notif(f"Success rate: {100 * (1 - errs / len(lonb) if len(lonb) != 0 else 1):.2f}%")
  (error if errs > 0 else notif)("Notebook Test Suite Error : " + str(errs))
  return errs


if __name__ == "__main__":
  runNotebooks()
  sys.exit(0)

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
import concurrent
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


def processNotebook(notebook_filename: str) -> tuple[int, str, float]:
  err = 0
  res = "ok"

  starttime = time.time()
  try:
    while True:
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
        ep.preprocess(nb, {"metadata": {"path": "../doc/sphinx/notebooks/"}})
        break
      except RuntimeError as e:
        if str(e) == "Kernel died before replying to kernel_info" or str(e) == "Kernel didn't respond in 60 seconds":
          time.sleep(random.randint(5, 10) / 10.0)
        else:
          raise CellExecutionError(traceback=str(e.__traceback__), ename=str(e), evalue="")
  except CellExecutionError:
    err = 1
    errorfilename = "../../../" + time.strftime("%Y%m%d_%H%M_") + os.path.basename(notebook_filename) + ".log"
    with open(errorfilename, "w") as errfn:
      traceback.print_exc(file=errfn)

    # Strip the timestamp prefix (YYYYMMDD_HHMM_) to show just the notebook name.
    _bn = os.path.basename(errorfilename)
    _parts = _bn.split("_", 2)
    res = (_parts[2] if len(_parts) == 3 else _bn) + " [ERROR]"

  duration = time.time() - starttime
  res = f"[ {duration:8.2f}s ] {os.path.basename(notebook_filename)[0:40]:40} {res}"

  return err, res, duration


def runNotebooks(lonb: List[str] = None):
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

  futures = []

  def done(fn):
    nonlocal _display_count
    _results[id(fn)] = fn.result()

    if _display_count > 0:
      print(f"\033[{3 + len(futures)}A")
    _display_count += 1

    print("=" * 58)
    for f in futures:
      if id(f) in _results:
        _, res, _ = _results[id(f)]
        print(res)
      else:
        state = "..." if f.running() else "zzz"
        print(f"[ ......... ] {os.path.basename(f.filename)[0:40]:40} {state}]")
    print("=" * 58)

  executor = concurrent.futures.ProcessPoolExecutor(None)
  for notebook_filename in lonb:
    fut = executor.submit(processNotebook, notebook_filename)
    fut.add_done_callback(done)
    fut.filename = notebook_filename
    futures.append(fut)
  concurrent.futures.wait(futures)

  time.sleep(1)

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

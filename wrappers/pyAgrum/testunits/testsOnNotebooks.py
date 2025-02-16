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
import zmq

os.environ['PYDEVD_DISABLE_FILE_VALIDATION'] = "1"


def processNotebook(notebook_filename):
  err = 0
  res = "ok"

  starttime = time.time()
  try:
    while True:
      try:
        ep = ExecutePreprocessor(timeout=5000, kernel_name='python3')
        ep.log_output = False
        with open(notebook_filename) as f:
          nb = nbformat.read(f, as_version=4)
        nb['cells'].insert(0, nbformat.from_dict({
          'outputs': [],
          'execution_count': 1,
          'metadata': {
            'collapsed': False,
            'editable': True,
            'deletable': True
          },
          'cell_type': 'code',
          'source': 'import os,sys\nsys.path.insert(0, os.path.abspath("../../../build/pyAgrum/release/wrappers"))'
        }))
        ep.preprocess(nb, {'metadata': {'path': '../doc/sphinx/notebooks/'}})
        break
      except RuntimeError as e:
        if str(e) == "Kernel died before replying to kernel_info" or str(e) == "Kernel didn't respond in 60 seconds":
          time.sleep(random.randint(5, 10) / 10.0)
        else:
          raise CellExecutionError(traceback=str(e.__traceback__), ename=str(e), evalue="")
  except CellExecutionError:
    err = 1
    errorfilename = "../../../" + \
                    time.strftime("%Y%m%d_%H%M_") + \
                    os.path.basename(notebook_filename) + ".log"
    with open(errorfilename, 'w') as errfn:
      traceback.print_exc(file=errfn)

    res = os.path.basename(errorfilename).split("-")[0] + "-...ipynb.log"

  duration = time.time() - starttime
  res = f"[ {duration:8.2f}s ] {os.path.basename(notebook_filename)[0:40]:40} {res}"

  return err, res, duration


futures = []


def done(fn):
  global futures

  if done.firstTime:
    done.firstTime = False
  else:
    print(f"\033[{3 + len(futures)}A")
  print("=" * 58)
  for f in futures:
    if f.done():
      e, res, _ = f.result()
      print(res)
    else:
      if f.running():
        state = "..."
      else:
        state = "zzz"
      print(f"[ ......... ] {os.path.basename(f.filename)[0:40]:40} {state}]")
  print("=" * 58)


done.firstTime = True


def runNotebooks(lonb: List[str] = None):
  global futures

  excludes = {"80-Applications_ipywidgets.ipynb"}
  if lonb is None:
    # slow notebooks
    lonb = sorted([filename for filename in glob.glob("../doc/sphinx/notebooks/*.ipynb")
                   if os.path.basename(filename) not in excludes])
  else:
    includes = set(lonb)
    lonb = sorted([filename for filename in glob.glob("../doc/sphinx/notebooks/*.ipynb")
                   if (bn := os.path.basename(filename)) not in excludes and bn in includes])

  start_time = time.time()

  # concurrent
  futures = []
  executor = concurrent.futures.ProcessPoolExecutor(None)
  for notebook_filename in lonb:
    fut = executor.submit(processNotebook, notebook_filename)
    fut.add_done_callback(done)
    fut.filename = notebook_filename
    futures.append(fut)
  concurrent.futures.wait(futures)
  sys.exit()

  time.sleep(1)

  errs = 0
  total_time = 0
  for f in futures:
    e, r, d = f.result()
    total_time += d
    errs += e
  elapsed_time = time.time() - start_time
  print()
  print(f"## Profiling : {total_time * 1000:.2f} ms ##")
  print(f"## Duration  : {elapsed_time * 1000:.2f} ms ##")
  print(f"Failed {errs} of {len(lonb)} tests")
  print(f"Success rate: {100 * (1 - errs / len(lonb) if len(lonb)!=0 else 1):.2f}%")

  print("Python Test Suite Error : " + str(errs))
  return errs


if __name__ == "__main__":
  runNotebooks()
  sys.exit(0)

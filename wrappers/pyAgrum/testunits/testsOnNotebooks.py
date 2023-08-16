# (c) Copyright 2015-2023 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

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

if __name__ == "__main__":
  print("Please use 'act test -t quick|all pyAgrum release'.")
  sys.exit(0)

os.environ['PYDEVD_DISABLE_FILE_VALIDATION']="1"
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
        state="..."
      else:
        state="zzz"        
      print(f"[ ......... ] {os.path.basename(f.filename)[0:40]:40} {state}]")  
  print("=" * 58)


done.firstTime = True


def runNotebooks():
  global futures

  l = []
  # slow notebooks
  excludes = {}
  for filename in glob.glob("../doc/sphinx/notebooks/*.ipynb"):
    if not os.path.basename(filename) in excludes:
      l.append(filename)
  
  startTime = time.time()

  # concurrent
  futures = []
  executor = concurrent.futures.ProcessPoolExecutor(None)
  for notebook_filename in sorted(l):
    fut = executor.submit(processNotebook, notebook_filename)
    fut.add_done_callback(done)
    fut.filename = notebook_filename
    futures.append(fut)
  concurrent.futures.wait(futures)

  time.sleep(1)

  errs = 0
  totaltime = 0
  for f in futures:
    e, r, d = f.result()
    totaltime += d
    errs += e
  elapsedTime = time.time() - startTime
  print()
  print("## Profiling : {} ms ##".format(int(totaltime * 1000)))
  print("## Duration  : {} ms ##".format(int(elapsedTime * 1000)))
  print("Failed {} of {} tests".format(errs, len(list)))
  print("Success rate: {}%".format(int(100 * (1 - errs / len(list)))))

  print("Python Test Suite Error : " + str(errs))
  return errs

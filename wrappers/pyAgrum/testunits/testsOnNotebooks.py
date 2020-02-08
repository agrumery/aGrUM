#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

from nbconvert.preprocessors.execute import ExecutePreprocessor, CellExecutionError
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
  
def processeNotebook(notebook_filename):
  err = 0
  res = "ok"

  try:
    while True:
      try:
        starttime = time.time()
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
            'source': 'import os,sys\nsys.path.insert(0, os.path.abspath("../../../build/release/wrappers"))'
        }))
        ep.preprocess(nb, {'metadata': {'path': '../pyLibs/notebooks/'}})
        break
      except RuntimeError as e:
        #print("zmq error on "+os.path.basename(notebook_filename)+"... restarting")
        if str(e)=="Kernel died before replying to kernel_info":
          time.sleep(random.randint(5, 10) / 10.0)
        else:
          raise CellExecutionError(e.__traceback__)
  except CellExecutionError:
    err = 1
    errorfilename = "../../../" + \
        time.strftime("%Y%m%d_%H%M_") + \
        os.path.basename(notebook_filename) + ".log"
    with open(errorfilename, 'w') as errfn:
      traceback.print_exc(file=errfn)
    
    res = os.path.basename(errorfilename).split("-")[0]+"-...ipynb.log"

  duration = time.time()-starttime
  res = f"[ {duration:8.2f}s ] {os.path.basename(notebook_filename)[0:40]:40} {res}"

  return err, res, duration


futures = []


def done(fn):
  global futures

  if done.firstTime:
    done.firstTime = False
  else:
    print(f"\033[{3+len(futures)}A")
  print("="*58)
  for f in futures:
    if (f.running()):
      print(f"[ ......... ] {os.path.basename(f.filename)[0:40]:40} ...")
      allok = False
    else:
      e, res, _ = f.result()
      print(res)
  print("=" * 58)


done.firstTime = True


def runNotebooks():
  global futures

  errs = 0

  list = []
  # slow notebooks
  excludes = {"11-structuralLearning.ipynb",
              "12-parametersLearningWithPandas.ipynb",
              "14-LearningAndEssentialGraphs.ipynb",
              "17-Chi2AndScoresFromBNLearner.ipynb",
              "23-ApproximateInference.ipynb",
              "26-klForBns.ipynb",
              "25-samplingInference.ipynb",
              "31-o3prm.ipynb"}

  excludes = {}
  for filename in glob.glob("../pyLibs/notebooks/*.ipynb"):
    if not os.path.basename(filename) in excludes:
      list.append(filename)

  startTime = time.time()

  # sequential
  # for notebook_filename in sorted(list):
  #  errs+=processeNotebook(notebook_filename)

  # concurrent
  futures = []
  executor = concurrent.futures.ProcessPoolExecutor(None)
  for notebook_filename in sorted(list):
    fut = executor.submit(processeNotebook, notebook_filename)
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
  return err

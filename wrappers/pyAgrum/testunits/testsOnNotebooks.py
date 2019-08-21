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

if sys.version_info >= (3, 0):
  import concurrent


def processeNotebook(notebook_filename):
  # creating some randomness
  #time.sleep(random.randint(5, 10) / 10.0)

  err = 0
  print(os.path.basename(notebook_filename) + " ... ")
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
      except RuntimeError:
        #print("zmq error on "+os.path.basename(notebook_filename)+"... restarting")
        time.sleep(random.randint(5, 10) / 10.0)
  except:
    err = 1
    errorfilename = "../../../" + \
        time.strftime("%Y%m%d_%H%M_") + \
        os.path.basename(notebook_filename) + ".log"
    with open(errorfilename, 'w') as err:
      traceback.print_exc(file=err)
    res = "error (see " + os.path.basename(errorfilename) + ")"
    
  duration = time.time()-starttime
  res = f"[ {duration:8.2f}s ] {os.path.basename(notebook_filename):50} {res}"
  
  return err,res

futures=[]
def done(fn):
  global futures
  for i in range(5):
    print(" ")
  print("=================================================")
  for f in futures:
    if (f.running()):
      print(f"[ ......... ] {os.path.basename(f.filename):50} ...")
    else:
      e, res = f.result()
      print(res)

def runNotebooks():
  global futures

  if sys.version_info < (3, 0):
    print("No notebook tests in python2")
    return 0

  errs = 0

  list = []
  excludes = {}
  #{"23-ApproximateInference.ipynb", "11-structuralLearning.ipynb",
  #            "learningClassifier.ipynb", "17-Chi2AndScoresFromBNLearner.ipynb", "25-samplingInference.ipynb"}
  for filename in glob.glob("../pyLibs/notebooks/*.ipynb"):
    if not filename in excludes:
      list.append(filename)

  startTime = time.time()

  # sequential
  # for notebook_filename in sorted(list):
  #  errs+=processeNotebook(notebook_filename)

  # concurrent
  futures = []
  executor = concurrent.futures.ProcessPoolExecutor(None)
  for notebook_filename in sorted(list):
    fut=executor.submit(processeNotebook, notebook_filename)
    fut.add_done_callback(done)
    fut.filename = notebook_filename
    futures.append(fut)    
  concurrent.futures.wait(futures)
  
  errs = 0
  for f in futures:
    e, r = f.result()
    errs+=e
  #errs = sum([f.result() for f in futures])

  elapsedTime = time.time() - startTime

  print()
  print("----------------------------------------------------------------------")
  print("## Profiling : {} ms ##".format(int(elapsedTime * 1000)))
  print("Failed {} of {} tests".format(errs, len(list)))
  print("Success rate: {}%".format(int(100 * (1 - errs / len(list)))))

  print("Python Test Suite Error : " + str(errs))
  return err

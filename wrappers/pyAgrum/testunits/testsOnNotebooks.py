#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import sys

if sys.version_info >= (3, 0):
  import concurrent

import glob
import os
import traceback
import time

import nbformat
from nbconvert.preprocessors.execute import ExecutePreprocessor, CellExecutionError


def processeNotebook(notebook_filename):
  err = 0

  print(os.path.basename(notebook_filename) + " ... ")
  res = "ok"

  try:
    ep = ExecutePreprocessor(timeout=5000, kernel_name='python3')
    with open(notebook_filename) as f:
      nb = nbformat.read(f, as_version=4)
    nb['cells'].insert(0, nbformat.from_dict({
      'outputs'        : [],
      'execution_count': 1,
      'metadata'       : {
        'collapsed': False,
        'editable' : True,
        'deletable': True
      },
      'cell_type'      : 'code',
      'source'         : 'import os,sys\nsys.path.insert(0, os.path.abspath("../../../build/release/wrappers"))'
    }))
    ep.preprocess(nb, {'metadata': {'path': '../notebooks/'}})
  except CellExecutionError as e:
    res = ""
    print("Error")
    err = 1
    print("-" * 60)
    traceback.print_exc(file=sys.stdout)
    print("-" * 60)

  print(os.path.basename(notebook_filename) + " " + res)
  return err


def runNotebooks():
  if sys.version_info < (3, 0):
    print("No notebook tests in python2")
    return 0

  errs = 0

  list = []
  for filename in glob.glob("../notebooks/*.ipynb"):
    list.append(filename)

  startTime = time.time()

  # sequential
  # for notebook_filename in sorted(list):
  #  errs+=processeNotebook(notebook_filename)

  ## concurrent
  executor = concurrent.futures.ProcessPoolExecutor(None)
  futures = [executor.submit(processeNotebook, notebook_filename) for notebook_filename in sorted(list)]
  concurrent.futures.wait(futures)
  errs = sum([f.result() for f in futures])

  elapsedTime = time.time() - startTime

  print()
  print("----------------------------------------------------------------------")
  print("## Profiling : {} ms ##".format(int(elapsedTime * 1000)))
  print("Failed {} of {} tests".format(errs, len(list)))
  print("Success rate: {}%".format(int(100 * (1 - errs / len(list)))))

  print("Python Test Suite Error : " + str(errs))
  return err

#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import glob
import os
import sys
import traceback
import time


import nbformat
from nbconvert.preprocessors.execute import ExecutePreprocessor, CellExecutionError

errs = 0

list = []
for filename in glob.glob("../notebooks/*.ipynb"):
  list.append(filename)

startTime = time.time()
for notebook_filename in sorted(list):
  print(os.path.basename(notebook_filename)+" ... ",end='',flush=True)
  res="ok"

  try:
    ep = ExecutePreprocessor(timeout=600, kernel_name='python3')
    with open(notebook_filename) as f:
      nb = nbformat.read(f, as_version=4)
    nb['cells'].insert(0,nbformat.from_dict({
      'outputs': [],
      'execution_count': 1,
      'metadata': {'collapsed': False,
                   'editable': True,
                   'deletable': True},
      'cell_type': 'code',
      'source': 'import os,sys\nsys.path.insert(0, os.path.abspath("../../../build/release/wrappers"))'}))
    ep.preprocess(nb, {'metadata': {'path': '../notebooks/'}})
  except CellExecutionError as e:
    res=""
    print("Error")
    errs += 1
    print("-" * 60)
    traceback.print_exc(file=sys.stdout)
    print("-" * 60)

  print(res)

elapsedTime = time.time() - startTime

print()
print("----------------------------------------------------------------------")
print("## Profiling : {} ms ##".format(int(elapsedTime * 1000)))
print("Failed {} of {} tests".format(errs,len(list)))
print("Success rate: {}%".format(int(100*(1-errs/len(list)))))

print("Python Test Suite Error : " + str(errs))

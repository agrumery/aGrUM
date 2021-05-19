#!/usr/bin/python
# -*- coding: utf-8 -*-
# ***************************************************************************
# *   Copyright (c) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   "act" is free software; you can redistribute it and/or modify         *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# **************************************************************************

import types
import inspect
import sys
from os.path import join, dirname

gumPath = join(dirname(sys.argv[0]), "build/release/wrappers")


class PyAgrumDocCoverage:
  def __init__(self, verbose):
    self._verbose = verbose
    self.nbClass = 0
    self.nbMeth = 0
    self.nbFunc = 0

    self.undocClass = []
    self.undocFunc = []
    self.undocMeth = []

    self.partialDocClass = []
    self.partialDocFunc = []
    self.partialDocMeth = []

  def _isNotValid(self, msg):
    # deprecated does not follow the rules of validity for documentation
    if msg.strip().startswith("Deprecated"):
      return False

    # msg les than 3 lines are not valid
    if msg.count("\n") < 3:
      return True

    if "'PyObject *'" in msg:
      return True

    return False

  def _prefix(self, name):
    return " " * (4 * (name.count(".") - 1)) + "- "

  def _checkFunctionDoc(self, name, func):
    res = "check"
    is_meth = name.count(".") > 1

    if is_meth:
      self.nbMeth += 1
    else:
      self.nbFunc += 1

    if not hasattr(func, "__doc__") or func.__doc__ is None:
      if is_meth:
        self.undocMeth.append(name)
      else:
        self.undocFunc.append(name)
      res = "no doc"
    else:

      if self._isNotValid(func.__doc__):
        if is_meth:
          self.partialDocMeth.append(name)
        else:
          self.partialDocFunc.append(name)

        res = "partial"

    if self._verbose:
      print(self._prefix(name) + name + " : " + res)

  def _checkClassDoc(self, name, clas):
    res = "check"

    self.nbClass += 1

    if not hasattr(clas, "__doc__") or clas.__doc__ is None:
      self.undocClass.append(name)
      res = "no doc"
    else:
      if self._isNotValid(clas.__doc__):
        self.partialDocClass.append(name)
        res = "partial"

    if self._verbose:
      print(self._prefix(name) + name + " : " + res)

  def _ignoredClass(self, name, clas):
    if not gumPath in sys.path:
      sys.path.insert(0, gumPath)
    import pyAgrum as gum

    if issubclass(clas, gum.GumException):
      return True
    return False

  def _traversal(self, entities, container):
    if not gumPath in sys.path:
      sys.path.insert(0, gumPath)
    import pyAgrum as gum

    for entity in entities:
      if entity[0] != '_':
        completeEntityName = container + "." + entity
        instEntity = eval(completeEntityName)
        if type(instEntity) is types.FunctionType:
          self._checkFunctionDoc(completeEntityName, instEntity)
        elif inspect.isclass(instEntity):
          if not self._ignoredClass(completeEntityName, instEntity):
            self._checkClassDoc(completeEntityName, instEntity)
            self._traversal(dir(instEntity), completeEntityName)

  def checkMissingDocs(self):
    if not gumPath in sys.path:
      sys.path.insert(0, gumPath)
    import pyAgrum as gum

    self.nbClass = 0
    self.nbMeth = 0
    self.nbFunc = 0

    self.undocClass = []
    self.undocFunc = []
    self.undocMeth = []

    self.partialDocClass = []
    self.partialDocFunc = []
    self.partialDocMeth = []

    self._traversal(dir(gum), "gum")

    pc = 1.0 - (len(self.undocClass) + len(self.partialDocClass)) / (1.0 * self.nbClass)
    pm = 1.0 - (len(self.undocMeth) + len(self.partialDocMeth)) / (1.0 * self.nbMeth)
    pf = 1.0 - (len(self.undocFunc) + len(self.partialDocFunc)) / (1.0 * self.nbFunc)

    print()
    print('Documentation in pyAgrum {}'.format(gum.__version__))

    print("  Classes   : coverage={:6.2f}% [{}]".format(pc * 100.0, self.nbClass))
    if self._verbose:
      print("---------")
      print("  - nbr of classes : " + str(self.nbClass))
      print("  - nbr of partially documented classes : " + str(len(self.partialDocClass)))
      print("\n    + ".join([""] + self.partialDocClass))
      print()
      print("  - nbr of undocumented classes : " + str(len(self.undocClass)))
      print("\n    + ".join([""] + self.undocClass))

    print("  Methods   : coverage={:6.2f}% [{}]".format(pm * 100.0, self.nbMeth))
    if self._verbose:
      print("---------")
      print("  - nbr of methods: " + str(self.nbMeth))
      print("  - nbr of partially documented methods : " + str(len(self.partialDocMeth)))
      print("\n    + ".join([""] + self.partialDocMeth))
      print()
      print("  - nbr of undocumented methods : " + str(len(self.undocMeth)))
      print("\n    + ".join([""] + self.undocMeth))

    print("  Functions : coverage={:6.2f}% [{}]".format(pf * 100.0, self.nbFunc))
    if self._verbose:
      print("-----------")
      print("  - nbr of functions: " + str(self.nbFunc))
      print("  - nbr of partially documented functions : " + str(len(self.partialDocFunc)))
      print("\n    + ".join([""] + self.partialDocFunc))
      print()
      print("  - nbr of undocumented functions : " + str(len(self.undocFunc)))
      print("\n    + ".join([""] + self.undocFunc))
    print()

    return len(self.undocClass) + len(self.partialDocClass) + \
           len(self.undocMeth) + len(self.partialDocMeth) + \
           len(self.undocFunc) + len(self.partialDocFunc)


def computeNbrError(showFunct):
  parser = PyAgrumDocCoverage(verbose=showFunct)
  return parser.checkMissingDocs()


if __name__ == "__main__":
  # execute only if run as a script
  parser = PyAgrumDocCoverage(verbose=True)
  print("\nNbr of documentation errors: {}".format(parser.checkMissingDocs()))

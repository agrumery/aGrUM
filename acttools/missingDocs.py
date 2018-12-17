import types
import inspect
import sys
from os.path import join, dirname

gumPath=join(dirname(sys.argv[0]), "build/release/wrappers")

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
      if func.__doc__.count("\n") <= 3:
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
      if clas.__doc__.count("\n") <= 3:
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


    pc = 1 - (len(self.undocClass) + len(self.partialDocClass)) / (1.0 * self.nbClass)
    pm = 1 - (len(self.undocMeth) + len(self.partialDocMeth)) / (1.0 * self.nbMeth)
    pf = 1 - (len(self.undocFunc) + len(self.partialDocFunc)) / (1.0 * self.nbFunc)

    print()
    print(f'Documentation in pyAgrum {gum.__version__}')

    print(f"  Classes   : coverage={pc * 100:6.2f}%")
    if self._verbose:
      print("---------")
      print("  - nbr of classes : " + str(self.nbClass))
      print("  - nbr of partially documented classes : " + str(len(self.partialDocClass)))
      print("\n    + ".join([""] + self.partialDocClass))
      print()
      print("  - nbr of undocumented classes : " + str(len(self.undocClass)))
      print("\n    + ".join([""] + self.undocClass))

    print(f"  Methods   : coverage={pm * 100:6.2f}%")
    if self._verbose:
      print("---------")
      print("  - nbr of methods: " + str(self.nbMeth))
      print("  - nbr of partially documented methods : " + str(len(self.partialDocMeth)))
      print("\n    + ".join([""] + self.partialDocMeth))
      print()
      print("  - nbr of undocumented methods : " + str(len(self.undocMeth)))
      print("\n    + ".join([""] + self.undocMeth))

    print(f"  Functions : coverage={pf * 100:6.2f}%")
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
  print(f"\nNbr of documentation errors: {parser.checkMissingDocs()}")

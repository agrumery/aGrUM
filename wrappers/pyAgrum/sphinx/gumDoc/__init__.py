import glob
from os import path


def xfile(afile):
  with open(afile, "r") as fh:
    exec(fh.read())


for docfile in glob.glob(path.dirname(__file__) + "/doc*.py"):
  xfile(docfile)
  print(docfile + " added.")

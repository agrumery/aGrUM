import pyAgrum

def xfile(afile):
  with open(afile, "r") as fh:
    exec(fh.read())

xfile('gumDoc/arc.py')
xfile('gumDoc/bayesNet.py')


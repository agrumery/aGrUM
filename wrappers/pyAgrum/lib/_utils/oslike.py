# native remplacements for os-like facilities
# (c) Pierre-Henri Wuillemin 2014
# From found versions on the net
import functools as ft, itertools as it, operator as op, os

def wc_l(fname):
  """
  imitate wc -l for linux os
  """
  res=0

  with open(fname, 'r') as f:
    reader= ft.partial(f.read, 131072)
    counter= op.methodcaller('count', '\n')
    res=sum(map(counter, iter(reader, ''))) # it.imap in python2

  print(res)

def head(filename, count=10,dest=None):
    """
    imitate head command for linux os
    """
    lines=[]

    with open(filename, 'r') as f:
        lines = [f.readline() for line in range(1, count+1)]

    if dest is None:
      print("".join(lines))
    else:
      with open(dest,'w') as f:
        f.write("".join(lines))

def rm(filename):
  """
  remove the file if existing. No interaction (rm -f)
  """
  if os.path.isfile(filename):
    os.unlink(filename)

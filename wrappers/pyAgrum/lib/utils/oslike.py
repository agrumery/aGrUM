# native remplacements for os-like facilities
# (c) Pierre-Henri Wuillemin 2014
# From found versions on the net
import functools as ft, itertools as it, operator as op, os

def wc_l(fname):
  """
  imitate wc -l for linux os
  """
  res=0
  
  with open(fname, 'rb') as f:
    reader= ft.partial(f.read, 131072)
    counter= op.methodcaller('count', '\n')
    res=sum(it.imap(counter, iter(reader, '')))
    
  return res
  
def head(filename, count=10):
    """
    imitate head command for linux os
    """
    lines=[]
    
    with open(filename, 'r') as f:
        lines = [f.readline() for line in xrange(1, count+1)]
    
    return filter(len, lines)

def rm(filename):
  """
  remove the file if existing. No interaction (rm -f)
  """
  if os.path.isfile(filename):
    os.unlink(filename)
  
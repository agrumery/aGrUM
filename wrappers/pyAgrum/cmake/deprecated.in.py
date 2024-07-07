# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
Deprecated for older pyAgrum
"""
import warnings
import functools



def deprecated_arg(newA: str, oldA: str, version: str):
  """
  Annotation of a function when changing the name of an argument of the function

  Example
  ------
  @gum.deprecated_arg("x","old_x","1.8")
  def f(x:int):
    return 2*X

  Parameters
  ----------
  newA:str
    the new name of the argument
  oldA:str
    the old name of the argument
  version:str
    the version of pyAgrum
  """

  def deco(f):
    @functools.wraps(f)
    def wrapper(*args, **kwargs):
      if oldA in kwargs:
        if newA in kwargs:
          warnings.warn(
            f"""
** pyAgrum : argument '{oldA}' is deprecated since '{version}', '{newA}' is used instead.
""", DeprecationWarning, stacklevel=2)
          kwargs.pop(oldA)
        else:
          warnings.warn(
            f"""
** pyAgrum : argument '{oldA}' is deprecated since '{version}', please use '{newA}' is instead.
""", DeprecationWarning, stacklevel=2)
          kwargs[newA] = kwargs.pop(oldA)
      return f(*args, **kwargs)

    return wrapper

  return deco

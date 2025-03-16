############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

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

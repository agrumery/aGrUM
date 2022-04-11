# -*- coding: utf-8 -*-
#(c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!

"""
This file defines the specific exception for causal package
"""

from ._types import NameSet

class HedgeException(Exception):
  """
  Represents an hedge exception for a causal query

  Parameters
  ----------
  msg: str
  observables: NameSet
  gs: ???
  """

  def __init__(self, msg: str, observables: NameSet, gs):
    """
    Represents an hedge exception for a causal query

    Parameters
    ----------
    msg: str
    observables: NameSet
    gs: ???
    """
    self.message = msg
    self.type = "HedgeException"
    self.observables = observables
    self.gs = gs
    super().__init__(self.message)


class UnidentifiableException(Exception):
  """
  Represents an unidentifiability for a causal query

  Parameters
  ----------
  msg: str
  """

  def __init__(self, msg):
    """
    Represents an unidentifiability for a causal query

    Parameters
    ----------
    msg: str
    """
    self.message = msg
    self.type = "Unidentifiable"
    super().__init__(self.message)

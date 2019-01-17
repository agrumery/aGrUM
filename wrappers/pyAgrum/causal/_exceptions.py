from ._types import *

class HedgeException(Exception):
  """
  Represents an hedge exception for a causal query

  :param msg: str
  :param observables: NameSet
  :param gs: ???
  """

  def __init__(self, msg: str, observables: NameSet, gs):
    self.message = msg
    self.type = "HedgeException"
    self.observables = observables
    self.gs = gs


class UnidentifiableException(Exception):
  """
  Represents an unidentifiability for a causal query
  """

  def __init__(self, msg):
    self.message = msg
    self.type = "Unidentifiable"


import warnings

warnings.warn(
  """
** pyAgrum : module 'pyAgrum.lib.mn2graph' is deprecated. Please import pyAgrum.lib.mrf2graph instead.
""", DeprecationWarning, stacklevel=2)

from pyAgrum.lib.mrf2graph import *

# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

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

"""

__author__ = "Pierre-Henri Wuillemin, Nikola Matevski, Louise Mouillé"
__copyright__ = "(c) 2020 PARIS"

from .bnclassifier import BNClassifier
from .discretizer import BNDiscretizer
from ._MBCalcul import compileMarkovBlanket
import sys

if sys.version_info < (3, 0):
  print("[pyAgrum] python 3 is needed for pyAgrum's skbn package.")
  sys.exit(1)

__all__ = ['BNClassifier', 'BNDiscretizer', 'compileMarkovBlanket']

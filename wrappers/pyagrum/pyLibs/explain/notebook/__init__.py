"""
This module provides functions for visualizing Shapley values.
"""

__author__ = "Pierre-Henri Wuillemin"
__copyright__ = "(c) 2019-2025 PARIS"

from ._bar import bar
from ._beeswarm import beeswarm
from ._waterfall import waterfall
from ._showShapValues import showShapValues

__all__ = [
    'bar',
    'beeswarm',
    'waterfall',
    'showShapValues'
]
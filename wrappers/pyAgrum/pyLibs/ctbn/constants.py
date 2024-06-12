from typing import List, Union, NewType

"""
This file contains constants and defines types.
"""

ALPHA: float = 0.95
INFINITY: float = 10e9
REJECT = True
FAIL_TO_REJECT = False

Domain: type = List[str]
NodeId: type = NewType("NodeId", int)
NameOrId: type = Union[str, NodeId]

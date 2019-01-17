from __future__ import annotations

from typing import TypeVar, NewType
from typing import Union, Optional, Iterable
from typing import Set, List, Tuple, Dict

import pyAgrum as gum

DirectedModel = Union[gum.BayesNet, gum.DAG, 'CausalModel']

NodeId = int # NewType("NodeId",int)
NodeList = List[NodeId]
NodeSet = Set[NodeId]

ArcSet = Set[Tuple[NodeId, NodeId]]
NameSet = Set[str]

LatentDescriptor = Tuple[str, Tuple[str, str]]
LatentDescriptorList = List[LatentDescriptor]

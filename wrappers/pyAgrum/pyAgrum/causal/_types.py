# -*- coding: utf-8 -*-
#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

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
This file defines the types for the causal package
"""
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

# requires: matplotlib
import pyagrum as gum
import pyagrum.explain as expl

bn = gum.fastBN("A->B->Y;C->Y;Y->D")
expl.showInformation(bn, evs={"A": 1})  # entropy in the nodes, mutual information on the arcs

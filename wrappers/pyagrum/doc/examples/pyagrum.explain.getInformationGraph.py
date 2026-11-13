# requires: pandas
import pyagrum as gum
import pyagrum.explain as expl

bn = gum.fastBN("A->B->Y;C->Y;Y->D")
g = expl.getInformationGraph(bn, evs={"A": 1})  # a pydot graph
sorted(n.get_name().strip('"') for n in g.get_nodes() if n.get_name() not in ("node", "edge", "graph"))
# ['A', 'B', 'C', 'D', 'Y']

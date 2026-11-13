# requires: pandas
import pyagrum as gum
import pyagrum.explain as expl

bn = gum.fastBN("A->B->Y;C->Y;Y->D")
g = expl.generalizedMarkovBlanket(bn, "Y", 1)  # a pydot graph
sorted(n.get_name().strip('"') for n in g.get_nodes())  # the Markov blanket of Y
# ['B', 'C', 'D', 'Y']

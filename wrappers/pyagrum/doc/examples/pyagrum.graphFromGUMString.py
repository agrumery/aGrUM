import pyagrum as gum

s = gum.fastDAG("A->B->C").saveGUMstring()
g = gum.graphFromGUMString(s)
type(g).__name__
# 'DAG'
sorted(g.arcs())
# [(0, 1), (1, 2)]

import pyagrum as gum

type(gum.fastGraph("A-B-C")).__name__
# 'UndiGraph'
type(gum.fastGraph("A->B->C")).__name__
# 'DAG'
type(gum.fastGraph("A->B->C->A")).__name__  # a directed cycle
# 'DiGraph'
type(gum.fastGraph("A->B-C")).__name__
# 'PDAG'
type(gum.fastGraph("A o-> B")).__name__
# 'MarkedGraph'
g = gum.fastGraph("'Visit Asia'->X")
sorted(g.nameFromId(n) for n in g.nodes())
# ['Visit Asia', 'X']

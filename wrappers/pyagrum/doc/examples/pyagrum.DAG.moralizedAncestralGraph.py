import pyagrum as gum

dag = gum.fastDAG("A->C;B->C;C->D;E->D")
mag = dag.moralizedAncestralGraph({dag.idFromName("C")})
sorted(dag.nameFromId(i) for i in mag.nodes())  # C and its ancestors
# ['A', 'B', 'C']
sorted(sorted(dag.nameFromId(i) for i in e) for e in mag.edges())
# [['A', 'B'], ['A', 'C'], ['B', 'C']]

import pyagrum as gum

dag = gum.fastDAG("A->B->C")
gum.saveGraph(dag, "dag.jgum")  # .jgum, .bgum or .pkl
sorted(gum.loadGraph("dag.jgum").arcs())
# [(0, 1), (1, 2)]

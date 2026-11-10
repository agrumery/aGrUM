import pyagrum as gum

dag = gum.fastDAG("A->B")
print(dag.saveGUMstring(indent=-1))
# {"type":"DAG","GumJsonVersion":"1.0","nodes":[0,1],"names":{"0":"A","1":"B"},"arcs":[[0,1]],"edges":[],"markedEdges":[]}

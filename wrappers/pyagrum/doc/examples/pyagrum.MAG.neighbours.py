import pyagrum as gum

dag = gum.fastDAG("A->S<-B;B->C")
mag = gum.MAG.fromDAG(dag, set(), {1})  # S selected: A--B->C
mag.neighbours(2)  # the undirected edges only
# {0}

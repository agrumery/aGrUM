import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.adjacencyMatrix().tolist()
# [[0, 1, 1], [0, 0, 1], [0, 0, 0]]

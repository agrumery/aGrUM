import pyagrum as gum

bn = gum.fastBN("A->B<-C;B->D")
mb = gum.MarkovBlanket(bn, "A")
mb.adjacencyMatrix().tolist()
# [[0, 1, 0], [0, 0, 0], [0, 1, 0]]

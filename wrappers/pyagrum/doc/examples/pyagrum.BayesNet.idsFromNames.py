import pyagrum as gum

bn = gum.fastBN("A->B->C<-D")
bn.idsFromNames({"C": ["B", "D"]})
# {2: [1, 3]}

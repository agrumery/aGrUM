import pyagrum as gum

bn = gum.fastBN("A->B;A->C;B->C")
bn.children("A")
# {1, 2}
bn.children(["A", "B"])
# {1, 2}

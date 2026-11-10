import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
dot = cm.toDot()
dot.startswith("digraph")
# True
'"A" -> "B"' in dot
# True

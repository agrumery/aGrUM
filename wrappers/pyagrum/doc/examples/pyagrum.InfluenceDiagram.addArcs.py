import pyagrum as gum

diag = gum.fastID("C;*D;$U")
diag.addArcs([("C", "D"), ("C", "U"), ("D", "U")])
sorted(diag.arcs())
# [(0, 1), (0, 2), (1, 2)]

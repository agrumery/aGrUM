import pyagrum as gum

diag = gum.fastID("C;*D;$U")
diag.addArc("C", "U")
diag.addArc("D", "U")
sorted(diag.arcs())
# [(0, 2), (1, 2)]
diag.utility("U").names
# ('U', 'C', 'D')

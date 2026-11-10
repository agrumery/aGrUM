import pyagrum as gum

diag = gum.fastID("C;*D")
u = diag.addUtilityNode(gum.LabelizedVariable("U", "utility", 1))
diag.addArc("C", "U")
diag.addArc("D", "U")
diag.isUtilityNode(u), diag.utilityNodeSize()
# (True, 1)
diag.utility(u).names
# ('U', 'C', 'D')

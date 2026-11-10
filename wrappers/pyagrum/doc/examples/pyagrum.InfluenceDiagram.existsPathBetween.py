import pyagrum as gum

diag = gum.fastID("A->*D->$U;B")
diag.existsPathBetween("A", "U")
# True
diag.existsPathBetween("U", "A")
# False
diag.existsPathBetween("A", "B")
# False

import pyagrum as gum

diag = gum.fastID("A->B->C->*D->$U")
diag.minimalCondSet("C", ["A", "B"])
# {1}

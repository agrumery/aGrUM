import pyagrum as gum

check = gum.checkFastSyntax("A->B")
check.error(gum.FastSyntax.DiGraph)  # valid
# ''
check.error(gum.FastSyntax.UndiGraph) != ""
# True

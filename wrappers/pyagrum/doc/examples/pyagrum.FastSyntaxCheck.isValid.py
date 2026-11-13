import pyagrum as gum

check = gum.checkFastSyntax("A[3]->B")
check.isValid(gum.FastSyntax.BayesNet)
# True
check.isValid(gum.FastSyntax.MarkovRandomField)  # no arc in a Markov random field
# False

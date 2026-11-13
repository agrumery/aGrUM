import pyagrum as gum

gum.checkFastSyntax("A{yes|no}->B").isValid(gum.FastSyntax.BayesNet)
# True

import pyagrum as gum

bn1 = gum.fastBN("A->B")
bn2 = gum.fastBN("A->C")
q = bn1.cpt("A").toVarsIn(bn2)  # same values, but over the variable A of bn2
q.variable(0) == bn2.variable("A")
# True
(q + bn2.cpt("A")).names
# ('A',)

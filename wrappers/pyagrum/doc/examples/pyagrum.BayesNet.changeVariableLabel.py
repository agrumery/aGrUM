import pyagrum as gum

bn = gum.fastBN("A{yes|no}->B")
bn.changeVariableLabel("A", "yes", "oui")
bn.variable("A").labels()
# ('oui', 'no')

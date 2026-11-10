import pyagrum as gum

bn = gum.fastBN("A{yes|no}->B[3]")
bn.toFast()
# 'A{yes|no}->B[3]'

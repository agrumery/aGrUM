import pyagrum as gum

bn = gum.fastBN("A{yes|no}->B[3]")
bn.toFast()
# 'A{yes|no}->B[3]'

# a name is quoted when it would not be read back as is otherwise
gum.fastBN("'X-ray'[3]->Visit Asia").toFast()
# "'X-ray'[3]->Visit Asia[2]"

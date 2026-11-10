import pyagrum as gum

bn = gum.fastBN("A->B")
bn.variable("A").setDescription("the cause")
bn.variable("A").description()
# 'the cause'

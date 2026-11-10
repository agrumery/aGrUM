import pyagrum as gum

bn = gum.fastBN("A->C<-B")
bn.dim()  # 1 + 1 + 4 free parameters
# 6

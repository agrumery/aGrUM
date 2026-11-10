import pyagrum as gum

cn = gum.CredalNet(gum.fastBN("A[3]->B"), gum.fastBN("A[3]->B"))
cn.domainSize(cn.src_bn().idFromName("A"))
# 3

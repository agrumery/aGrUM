import pyagrum as gum

id_ = gum.fastID("*D->$U<-C;C->D")
sorted(id_.namesFromIds(id_.parents("U")))
# ['C', 'D']

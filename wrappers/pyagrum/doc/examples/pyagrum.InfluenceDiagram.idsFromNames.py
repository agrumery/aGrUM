import pyagrum as gum

id_ = gum.fastID("*D->$U<-C;C->D")
id_.idsFromNames({"U": ("C", "D")})
# {1: (2, 0)}

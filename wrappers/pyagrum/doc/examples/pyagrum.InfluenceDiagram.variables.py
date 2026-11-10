import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
sorted(v.name() for v in diag.variables())
# ['C', 'D', 'U']
sorted(v.name() for v in diag.variables(["D", "U"]))
# ['D', 'U']

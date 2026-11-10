import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.changeVariableName("C", "Weather")
sorted(diag.names())
# ['D', 'U', 'Weather']
diag.idFromName("Weather")
# 0

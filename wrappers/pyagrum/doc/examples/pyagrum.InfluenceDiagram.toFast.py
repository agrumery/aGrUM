import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
diag.toFast()
# 'C[2]->*D[2];C->$U[1];D->U'

# a name is quoted when it would not be read back as is otherwise
gum.fastID("'my state'->*'my-decision'").toFast()
# "my state[2]->*'my-decision'[2]"

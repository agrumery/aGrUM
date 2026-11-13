import pyagrum as gum

gum.fastVariable("c{red|green}").toFast()
# 'c{red|green}'
gum.fastVariable("r[1,4]").toFast()
# 'r[1,4]'
gum.fastVariable("'X-ray'{yes|no}").toFast()  # the name is quoted only if needed
# "'X-ray'{yes|no}"

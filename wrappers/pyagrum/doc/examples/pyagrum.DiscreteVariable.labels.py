import pyagrum as gum

gum.fastVariable("c{red|green|blue}").labels()
# ('red', 'green', 'blue')
gum.fastVariable("r[1,4]").labels()
# ('1', '2', '3', '4')

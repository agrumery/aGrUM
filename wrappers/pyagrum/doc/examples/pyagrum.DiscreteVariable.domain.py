import pyagrum as gum

gum.fastVariable("c{red|green|blue}").domain()
# '{red|green|blue}'
gum.fastVariable("x[0:10:5]").domain()
# '<[0;2[,[2;4[,[4;6[,[6;8[,[8;10]>'

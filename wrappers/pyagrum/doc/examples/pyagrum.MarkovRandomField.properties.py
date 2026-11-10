import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.setProperty("author", "me")
"author" in mrf.properties()
# True
mrf.property("author")
# 'me'

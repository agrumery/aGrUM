import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.setProperty("author", "me")
mrf.existsProperty("author")
# True
mrf.existsProperty("license")
# False

# Makefile for Sphinx documentation
#
WINKEYWORD=MINGW

TOTALUNAME := $(shell uname)
UNAME := $(findstring $(WINKEYWORD),$(TOTALUNAME))

ifeq ($(UNAME), $(WINKEYWORD))
  SPHINXBUILD   = sphinx-build.exe
endif
ifneq ($(UNAME), $(WINKEYWORD))
  SPHINXBUILD   = python3 `which sphinx-build`
endif

# Internal variables.
PAPEROPT_a4     = -D latex_paper_size=a4
PAPEROPT_letter = -D latex_paper_size=letter

PAPER         =
RTDDIR      = ../../../../build/pyAgrum/release/wrappers/pyagrum/docs
SITEDIR      = Web

# You can set these variables from the command line.
DEFAULTOPTS   = -a -E -w sphinx_warnings.txt 

RTDOPTS     = -j auto $(DEFAULTOPTS) -c . 
SITEOPTS       = $(DEFAULTOPTS) -c $(SITEDIR) $(SITEDIR)

ALLRTDOPTS   =  $(PAPEROPT_$(PAPER)) $(RTDOPTS) 


AGRUMRTDDIR = "{agrum}/build/pyAgrum/release/wrappers/pyagrum/docs"

# the i18n builder cannot share the environment and doctrees with the others
I18NRTDOPTS  = $(PAPEROPT_$(PAPER)) $(RTDOPTS) .

.PHONY: help
help:
	@echo "SPHINX Makefile for '$(UNAME)' using '$(SPHINXBUILD)'"
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  rtd       to make standalone HTML files for the ReadTheDocs site"
	@echo "  site       to make standalone markdown files for web site"
	@echo "  linkcheck  to check all external links for integrity"
	@echo "  doctest    to run all doctests embedded in the documentation (if enabled)"
	@echo "  coverage   to run coverage check of the documentation (if enabled)"
	@echo "  dummy      to check syntax errors of document sources"

.PHONY: clean
clean:
	rm -rf $(RTDDIR)/*
	rm -rf $(SITEDIR)
	rm -rf $(SITEDIR).zip


.PHONY: html rtd
rtd: html

html: clean
	$(SPHINXBUILD) -b html $(ALLRTDOPTS) . $(RTDDIR)
	@echo
	@echo "Build finished. The HTML pages are in $(AGRUMRTDDIR)/html."

.PHONY: site
site: clean
	@echo "Building the site in $(SITEDIR)..."
	python3 webbuild.py $(SITEDIR)
	
.PHONY: dirhtml
dirhtml:
	$(SPHINXBUILD) -b dirhtml $(ALLRTDOPTS) $(RTDDIR)/dirhtml
	@echo
	@echo "Build finished. The HTML pages are in $(AGRUMRTDDIR)/dirhtml."

.PHONY: singlehtml
singlehtml:
	$(SPHINXBUILD) -b singlehtml $(ALLRTDOPTS) $(RTDDIR)/singlehtml
	@echo
	@echo "Build finished. The HTML page is in $(RTDDIR)/singlehtml."

.PHONY: pickle
pickle:
	$(SPHINXBUILD) -b pickle $(ALLRTDOPTS) $(RTDDIR)/pickle
	@echo
	@echo "Build finished; now you can process the pickle files."

.PHONY: json
json:
	$(SPHINXBUILD) -b json $(ALLRTDOPTS) $(RTDDIR)/json
	@echo
	@echo "Build finished; now you can process the JSON files."

.PHONY: htmlhelp
htmlhelp:
	$(SPHINXBUILD) -b htmlhelp $(ALLRTDOPTS) $(RTDDIR)/htmlhelp
	@echo
	@echo "Build finished; now you can run HTML Help Workshop with the" \
	      ".hhp project file in $(RTDDIR)/htmlhelp."

.PHONY: qthelp
qthelp:
	$(SPHINXBUILD) -b qthelp $(ALLRTDOPTS) $(RTDDIR)/qthelp
	@echo
	@echo "Build finished; now you can run "qcollectiongenerator" with the" \
	      ".qhcp project file in $(RTDDIR)/qthelp, like this:"
	@echo "# qcollectiongenerator $(RTDDIR)/qthelp/pyagrum.qhcp"
	@echo "To view the help file:"
	@echo "# assistant -collectionFile $(RTDDIR)/qthelp/pyagrum.qhc"

.PHONY: applehelp
applehelp:
	$(SPHINXBUILD) -b applehelp $(ALLRTDOPTS) $(RTDDIR)/applehelp
	@echo
	@echo "Build finished. The help book is in $(RTDDIR)/applehelp."
	@echo "N.B. You won't be able to view it unless you put it in" \
	      "~/Library/Documentation/Help or install it in your application" \
	      "bundle."

.PHONY: devhelp
devhelp:
	$(SPHINXBUILD) -b devhelp $(ALLRTDOPTS) $(RTDDIR)/devhelp
	@echo
	@echo "Build finished."
	@echo "To view the help file:"
	@echo "# mkdir -p $$HOME/.local/share/devhelp/pyAgrum"
	@echo "# ln -s $(RTDDIR)/devhelp $$HOME/.local/share/devhelp/pyAgrum"
	@echo "# devhelp"

.PHONY: epub
epub:
	$(SPHINXBUILD) -b epub $(ALLRTDOPTS) $(RTDDIR)/epub
	@echo
	@echo "Build finished. The epub file is in $(RTDDIR)/epub."

.PHONY: epub3
epub3:
	$(SPHINXBUILD) -b epub3 $(ALLRTDOPTS) $(RTDDIR)/epub3
	@echo
	@echo "Build finished. The epub3 file is in $(RTDDIR)/epub3."

.PHONY: latex
latex:
	$(SPHINXBUILD) -b latex $(ALLRTDOPTS) $(RTDDIR)/latex
	@echo
	@echo "Build finished; the LaTeX files are in $(AGRUMRTDDIR)/latex."
	@echo "Run \`make' in that directory to run these through (pdf)latex" \
	      "(use \`make latexpdf' here to do that automatically)."

.PHONY: latexpdf
latexpdf:
	$(SPHINXBUILD) -b latex $(ALLRTDOPTS) $(RTDDIR)/latex
	@echo "Running LaTeX files through pdflatex..."
	$(MAKE) -C $(RTDDIR)/latex all-pdf
	@echo "pdflatex finished; the PDF files are in $(AGRUMRTDDIR)/latex."

.PHONY: latexpdfja
latexpdfja:
	$(SPHINXBUILD) -b latex $(ALLRTDOPTS) $(RTDDIR)/latex
	@echo "Running LaTeX files through platex and dvipdfmx..."
	$(MAKE) -C $(RTDDIR)/latex all-pdf-ja
	@echo "pdflatex finished; the PDF files are in $(AGRUMRTDDIR)/latex."

.PHONY: text
text:
	$(SPHINXBUILD) -b text $(ALLRTDOPTS) $(RTDDIR)/text
	@echo
	@echo "Build finished. The text files are in $(AGRUMRTDDIR)/text."

.PHONY: man
man:
	$(SPHINXBUILD) -b man $(ALLRTDOPTS) $(RTDDIR)/man
	@echo
	@echo "Build finished. The manual pages are in $(AGRUMRTDDIR)/man."

.PHONY: texinfo
texinfo:
	$(SPHINXBUILD) -b texinfo $(ALLRTDOPTS) $(RTDDIR)/texinfo
	@echo
	@echo "Build finished. The Texinfo files are in $(AGRUMRTDDIR)/texinfo."
	@echo "Run \`make' in that directory to run these through makeinfo" \
	      "(use \`make info' here to do that automatically)."

.PHONY: info
info:
	$(SPHINXBUILD) -b texinfo $(ALLRTDOPTS) $(RTDDIR)/texinfo
	@echo "Running Texinfo files through makeinfo..."
	make -C $(RTDDIR)/texinfo info
	@echo "makeinfo finished; the Info files are in $(AGRUMRTDDIR)/texinfo."

.PHONY: gettext
gettext:
	$(SPHINXBUILD) -b gettext $(I18NRTDOPTS) $(RTDDIR)/locale
	@echo
	@echo "Build finished. The message catalogs are in $(AGRUMRTDDIR)/locale."

.PHONY: changes
changes:
	$(SPHINXBUILD) -b changes $(ALLRTDOPTS) $(RTDDIR)/changes
	@echo
	@echo "The overview file is in $(RTDDIR)/changes."

.PHONY: linkcheck
linkcheck:
	$(SPHINXBUILD) -b linkcheck $(ALLRTDOPTS) $(RTDDIR)/linkcheck
	@echo
	@echo "Link check complete; look for any errors in the above output " \
	      "or in $(RTDDIR)/linkcheck/output.txt."

.PHONY: doctest
doctest:
	$(SPHINXBUILD) -b doctest $(ALLRTDOPTS) $(RTDDIR)/doctest
	@echo "Testing of doctests in the sources finished, look at the " \
	      "results in $(RTDDIR)/doctest/output.txt."

.PHONY: coverage
coverage:
	$(SPHINXBUILD) -b coverage $(ALLRTDOPTS) $(RTDDIR)/coverage
	@echo "Testing of coverage in the sources finished, look at the " \
	      "results in $(RTDDIR)/coverage/python.txt."

.PHONY: xml
xml:
	$(SPHINXBUILD) -b xml $(ALLRTDOPTS) $(RTDDIR)/xml
	@echo
	@echo "Build finished. The XML files are in $(AGRUMRTDDIR)/xml."

.PHONY: pseudoxml
pseudoxml:
	$(SPHINXBUILD) -b pseudoxml $(ALLRTDOPTS) $(RTDDIR)/pseudoxml
	@echo
	@echo "Build finished. The pseudo-XML files are in $(AGRUMRTDDIR)/pseudoxml."

.PHONY: dummy
dummy:
	$(SPHINXBUILD) -b dummy $(ALLRTDOPTS) $(RTDDIR)/dummy
	@echo
	@echo "Build finished. Dummy builder generates no files."

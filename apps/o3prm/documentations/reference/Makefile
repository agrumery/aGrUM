# ============================================================
# makefile for easy LaTeX compilation
# CG -- version du 21/03/2005
# ============================================================

.PHONY : clean all fig dot pdf

PDFLATEX=pdflatex -shell-escape

all: o3prm-reference.pdf

o3prm-reference.pdf: o3prm-reference.tex  o3prm-reference.bib reference.sty clstholi2.cls
	${PDFLATEX} o3prm-reference.tex 
	bibtex o3prm-reference.aux 
	${PDFLATEX} o3prm-reference.tex &&\
	${PDFLATEX} o3prm-reference.tex

bib: o3prm-reference.bib
	bibtex o3prm-reference.aux

clean:
	-rm -f  *~ *idx *ilg *ind o3prm-reference.pdf \
	*.aux *.log *.out *.toc *.bbl *blg


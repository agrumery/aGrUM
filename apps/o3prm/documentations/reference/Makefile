# ============================================================
# makefile for easy LaTeX compilation
# CG -- version du 21/03/2005
# ============================================================

.PHONY : clean all fig dot pdf

LATEX=pdflatex -shell-escape

all: o3prm-reference.pdf

pdf:
	cp -f figures-sources/*pdf figures/

full: *.tex dot fig pdf
	${LATEX} o3prm-reference.tex &&\
	bibtex o3prm-reference.aux &&\
	${LATEX} o3prm-reference.tex &&\
	${LATEX} o3prm-reference.tex

o3prm-reference.pdf: *.tex
	${LATEX} o3prm-reference.tex &&\
	mv -f o3prm-reference.pdf $@

bib: o3prm-reference.bib
	rm -f o3prm-reference.bbl
	bibtex o3prm-reference.aux

clean:
	-rm -f  *~ *idx *ilg *ind o3prm-reference.pdf \
	*.aux *.log *.out *.toc *.bbl *blg


# Exercise 2 report makefile, TDT4258
#
# This makefile runs bibtex automatically when needed, and always compiles
# twice to get citations correct. Also includes a clean target.

main.pdf : main.tex meta.tex title.tex content/*.tex main.blg main.bbl
	pdflatex main.tex
	pdflatex main.tex # compile twice to get citations correct

main.blg main.bbl : references.bib main.aux
	bibtex main.aux

main.aux :
	pdflatex main.tex # Recreate main.aux after doing a clean

.PHONY : clean
clean :
	-rm -rf main.pdf main.aux main.out main.log main.blg main.bbl

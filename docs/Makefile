SRC=CZM_Manual

all : $(SRC).pdf

.PHONY : clean

$(SRC).pdf : $(SRC).tex $(SRC).bib
	pdflatex $(SRC)
	bibtex $(SRC)
	pdflatex $(SRC)

clean :
	rm -rf $(SRC).pdf $(SRC).log $(SRC).aux $(SRC).out $(SRC).toc $(SRC).bbl $(SRC).blg

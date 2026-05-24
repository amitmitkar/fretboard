all:fretboard

fretboard: FretBoard.o main.o
	g++ -o fretboard FretBoard.o main.o


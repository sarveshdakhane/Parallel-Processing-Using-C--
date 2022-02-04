# Entfernenn Sie das '#' in der n�chsten Zeile, um Compileroptimierungen
# einzuschalten.
#OPT = -O

all: heat ViewMatrix.class

heat: heat.cpp solver-jacobi.cpp
	mpic++ $(OPT) -fopenmp -o heat heat.cpp solver-jacobi.cpp

ViewMatrix.class: ViewMatrix.java
	javac ViewMatrix.java

clean:
	rm -f *.o *.c~ heat Matrix.txt
	rm -f ViewMatrix.class
 

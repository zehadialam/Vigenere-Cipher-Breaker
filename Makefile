vigenereCipherBreaker: vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o
	g++ -o vigenereCipherBreaker vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o

vigenereCipherBreaker.o: vigenereCipherBreaker.cpp nGramScorer.h vigenereCipher.h
	g++ -c vigenereCipherBreaker.cpp -std=c++17

nGramScorer.o: nGramScorer.cpp nGramScorer.h
	g++ -c nGramScorer.cpp -std=c++17

vigenereCipher.o: vigenereCipher.cpp vigenereCipher.h
	g++ -c vigenereCipher.cpp -std=c++17

clean:
	/bin/rm -f *.o
	/bin/rm -f vigenereCipherBreaker

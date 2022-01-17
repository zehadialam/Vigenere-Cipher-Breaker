vigenereCipherBreaker: vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o
	g++-11 -o vigenereCipherBreaker vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o -lpthread -O2 -Wextra

vigenereCipherBreaker.o: vigenereCipherBreaker.cpp nGramScorer.h vigenereCipher.h
	g++-11 -c vigenereCipherBreaker.cpp -std=c++17 -O2 -Wextra

nGramScorer.o: nGramScorer.cpp nGramScorer.h
	g++-11 -c nGramScorer.cpp -std=c++17 -O2 -Wextra

vigenereCipher.o: vigenereCipher.cpp vigenereCipher.h
	g++-11 -c vigenereCipher.cpp -std=c++17 -O2 -Wextra

clean:
	/bin/rm -f *.o
	/bin/rm -f vigenereCipherBreaker

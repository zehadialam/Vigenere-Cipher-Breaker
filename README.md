# Vigenère cipher breaker

A C++ implementation of Practical Cryptography's <a href="https://github.com/jameslyons/python_cryptanalysis/blob/master/break_vigenere.py">Vigenère cipher breaker</a> for improved speed, along with my own additions and features. More information about the algorithm that the program uses to break the Vigenère cipher can be found <a href="http://practicalcryptography.com/cryptanalysis/stochastic-searching/cryptanalysis-vigenere-cipher-part-2/">here</a> and <a href="http://practicalcryptography.com/cryptanalysis/text-characterisation/quadgrams/">here</a>.<br><br>
More information on the Vigenère cipher itself can be found <a href="https://medium.com/@pushkarkadam1994/cryptography-vigen%C3%A8re-cipher-e188fcb96bae">here</a>

## Running the program
### UNIX
To run the program in a UNIX environment, ensure that GCC is installed, so that the source code files can be compiled. Navigate to the directory where the files are saved and execute the following command:
```
$ make
```
This will execute the following commands, leading to the compiled program:
```
g++ -c vigenereCipherBreaker.cpp -std=c++17
g++ -c nGramScorer.cpp -std=c++17
g++ -c vigenereCipher.cpp -std=c++17
g++ -o vigenereCipherBreaker vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o
```
### Windows
To run the program on a Windows machine, use the Windows Subsystem for Linux.<br>
The Microsoft website has <a href="https://docs.microsoft.com/en-us/windows/wsl/install-win10">documentation</a> on how to do this.

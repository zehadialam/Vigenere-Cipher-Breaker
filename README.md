# Vigenère cipher breaker

A C++ implementation of Practical Cryptography's <a href="https://github.com/jameslyons/python_cryptanalysis/blob/master/break_vigenere.py">Vigenère cipher breaker</a> for improved speed, along with my own additions and features. More information about the algorithm that the program uses to break the Vigenère cipher can be found <a href="http://practicalcryptography.com/cryptanalysis/stochastic-searching/cryptanalysis-vigenere-cipher-part-2/">here</a> and <a href="http://practicalcryptography.com/cryptanalysis/text-characterisation/quadgrams/">here</a>.<br><br>
More information on the Vigenère cipher itself can be found <a href="https://medium.com/@pushkarkadam1994/cryptography-vigen%C3%A8re-cipher-e188fcb96bae">here</a>

## Running the program
### UNIX
To run the program in a UNIX environment, ensure that GCC is installed, so that the source code files can be compiled. This can be checked by running the following command:
```
gcc --version
```
If GCC is installed, there will be a message like the following (with the version indicated):
```
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
To compile the program, navigate to the directory where the files are saved and execute the following command:
```
make
```
This will execute the following commands, leading to the compiled program:
```
g++ -c vigenereCipherBreaker.cpp -std=c++17
g++ -c nGramScorer.cpp -std=c++17
g++ -c vigenereCipher.cpp -std=c++17
g++ -o vigenereCipherBreaker vigenereCipherBreaker.o nGramScorer.o vigenereCipher.o
```
To run the program, use the following syntax:
```
./vigenereCipherBreaker [ciphertext] [min keylength] [max keylength] [verbosemode]
```
The ciphertext command-line argument should be a string surrounded by double-quotes. <br>
The keylengths should be integer values. <br>
Verbose mode should be given a value of 1 or 0 to indicate true or false, respectively.
### Windows
It is encouraged to use the Windows Subsystem for Linux (WSL) to run the program on a Windows machine.<br>
The Microsoft website has <a href="https://docs.microsoft.com/en-us/windows/wsl/install-win10">documentation</a> on how to do this.<br>
Once WSL has been set up, the instructions provided for UNIX can be followed to run the program. 
## Example Runs
The following are a few examples of what running the program is like:
<img src="https://github.com/zehadialam/vigenere-cipher-breaker/blob/main/screenshots/Example1.png" width=40%>

# Cryptography-in-CPlusPlus

This repository contains a C++ implementation of Practical Cryptography's Vigenère cipher breaker, which is written in Python. Practical Cryptography's algorithm for breaking the Vigenère cipher, although very effective, runs slowly as a Python program. I was compelled to modify the program, so that it would run faster. I first tried to use Cython, but that did not result in any noticeable difference. I finally decided to rewrite the entire program in C++. The difference in speed is very significant.

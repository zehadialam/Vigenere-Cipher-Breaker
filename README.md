# A C++ Implementation of Practical Cryptography's Vigenère cipher breaker

This repository contains a C++ implementation of Practical Cryptography's Vigenère cipher breaker, written in Python. Practical Cryptography's algorithm for breaking the Vigenère cipher, although very effective, runs slowly as a Python program. I felt compelled to modify it, so that it would run faster. I initially tried to use Cython, but that did not result in any noticeable difference. I finally decided to rewrite the entire program in C++. The difference in speed is very significant.

# Pascallite Compiler

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)

## General info
This is a collaborated project for CS 4301 taught by Dr. Mark Motl (Angelo State University).
This compiler contains two stages and is for educational purposes only.

The Pascallite Compiler is a Compiler in C++ that parses Pascallite code and returns a RAMM assembly language object file and a listing file.

## Technologies
* RAMM (Representative Auxiliary Machine Model) is a simulated computer that was developed to demonstrate the various properties and techniques of programming a fixed-word-length computer. The RAMM computer is a computer program that when executed simulates the actions of a computer. The technique of simulating a computer with a computer program is very useful in studying the characteristics and properties of a computer without having to go to the expense of actually building one. For more information on RAMM please visit the [official page](https://www.cs.angelo.edu/ramm/).

* GCC, the [Gnu Compiler Collection](https://gcc.gnu.org/)


## Setup
To compile this project use this command in terminal†	 : ```g++ -o <e.g. stage0> -g -Wall -std=c++11 <e.g. stage0>.cpp``` or use the included makefile.

To run this project use this command in terminal†	 : ```./<compiled stage e.g. stage1> <input data file e.g. 145.dat> <output listing file e.g. 145.lst> <output object file e.g. 145.asm> ```

†GCC, the [Gnu Compiler Collection](https://gcc.gnu.org/) must be installed for this program to run correctly.



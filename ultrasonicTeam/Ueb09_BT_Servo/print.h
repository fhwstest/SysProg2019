/*
 * print.h
 *
 * Created: 02.05.2018 09:36:04
 *  Author: Felix
 */

#include <avr/io.h>
#include <stdio.h>

#ifndef print_H
#define print_H

void initPrint();
void print(char* message);

void initPrintf();
void printBuf(char* str, int* arr, int length);

#endif
# Embedded Assessment
This project implements a byte-wise frame parser using a state machine.
Frame Format:
SOF | CMD | LEN | PAYLOAD | CHECKSUM

 SOF = 0xAA
 CMD = Command byte
 LEN = Payload length
 PAYLOAD = Variable-length data
 CHECKSUM = XOR of CMD, LEN and PAYLOAD bytes

## Build

gcc -Wall -std=c99 main.c -o parser

## Run

./parser

## Example Input

7
AA
01
03
10
20
30
02

## Example Output

FRAME OK

Command : 0x01
Length  : 3
Payload : 0x10 0x20 0x30

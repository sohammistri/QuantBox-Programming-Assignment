#! /bin/bash
g++ -std=c++17 main.cpp Decoder.cpp CreateOrderBook.cpp  -lstdc++ -o main
./main $1 $2
#pragma once

#include <chrono>
#include <random>
using namespace std;

// Prosty generator liczb pseudolosowych na mt19937 z biblioteki <random> i mikrosekundach od EPOCH z biblioteki <chrono>.
// Zakres losowania jest lewostronnie domknięty.
class BetterRandom {
private:
	mt19937 generator;
	uniform_int_distribution<int> rozklad;
public:
	inline BetterRandom(int min, int max) { // Generator z zakresu
		generator = mt19937((unsigned int) chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());
		rozklad = uniform_int_distribution<int>(min, max - 1);
	}
	inline int rand() {
		return rozklad(generator);
	}
	inline mt19937 getGen() { // zwraca zainicjowany generator na potrzeby shuffle
		return generator;
	}
};
#pragma once
#ifndef BASE_H_
#define BASE_H_
//included libs in simulator
#include<iostream>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<bitset>
#include<cmath>
#include<ctime>
#include<stdint.h>

using namespace std;

//the bits of address
const auto LENGTH = 34;
const auto VALID = 1;
const auto TAG = 13;
const auto LINE = 9;
const auto OFFSET = 12;

//the size of different fields
const auto TAG_SIZE = 8192;	//2^13
const auto CACHE_SIZE = 2097152;	//2^21
const auto CACHE_LINE = 512;	//2^9
const auto LINE_SIZE = 4096;	//2^12
const auto MEMORY_LINE = 4;	//16/4

//define the W/R lantency
const auto WRITE_TIME = 500;
const auto READ_TIME = 200;
const auto WR_TIME = 20;

//the vector that stores the  important data(W/R, Offset, W/R size) of trace
//vector<vector<uint64_t>> op;

typedef struct CACHE *Cache;
typedef struct BLOCK *Block;

struct CACHE {
	uint64_t time;
	uint64_t hit;
	Block *block;
};

struct BLOCK {
	int dirty;
	int valid;
	bitset<TAG> tag;
	int count;	//calculate LRU
};

#endif

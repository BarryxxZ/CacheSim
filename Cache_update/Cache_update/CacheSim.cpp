#include"base.h"

//to change decimal address into binary format
bitset<LENGTH> change_Add(uint64_t address) {
	uint64_t result = address % 17179869184;
	bitset<LENGTH> bit(result);
	return bit;
}

//to find LRU index
int LRU(Cache cache, int pos) {
	int MAX = 0;
	for (int i = 0; i < CACHE_LINE; i++) {
		MAX = max(MAX, cache->block[i]->count);
	}
	for (int i = 0; i < CACHE_LINE; i++) {
		if (cache->block[i]->count == MAX) {
			return i;
		}
	}
	return pos;
}

//to get random index
int getRandom() {
	srand((unsigned int)time(0));
	return rand() % (CACHE_LINE - 1);
}

/*Function to write data to cache
  tag => bitset<TAG> btag;
  block_index => bitset<LINE> index;
  The replacement strategy currently used is LRU.
  If you want to use Random, please comment out the code commented as "for LRU"
  and remove the comment commented on the code as "for Random"
*/
void Write_update(Cache cache, uint64_t address) {
	bitset<TAG> btag;
	bitset<LINE> index;
	bitset<LENGTH> bit = change_Add(address);
	for (int i = 0; i < CACHE_LINE; i++) {
		cache->block[i]->count++;
	}
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		btag[i - (LINE + OFFSET - 1)] = bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		index[i - (OFFSET - 1)] = bit[i];
	}
	int pos = index.to_ulong();
	int ran = LRU(cache, pos);	//for LRU
	//int ran = getRandom();	//for Random
	if (cache->block[pos]->valid == 1 && cache->block[pos]->tag == btag) {
		cache->hit++;
		cache->block[pos]->dirty = 1;
		cache->block[pos]->count = 0;	//for LRU
	}
	else {
		if (cache->block[pos]->tag != NULL) {
			/**if (cache->block[ran]->dirty == 1) {
				cache->time += WRITE_TIME;
			}**/
			//cache->time += READ_TIME;
			cache->block[ran]->valid = 1;
			cache->block[ran]->dirty = 1;
			cache->block[ran]->tag = btag;
			cache->block[ran]->count = 0;	//for LRU
		}
		if (cache->block[pos]->tag == NULL) {
			/**if (cache->block[pos]->dirty == 1) {
				cache->time += WRITE_TIME;
			}**/
			//cache->time += READ_TIME;
			cache->block[pos]->valid = 1;
			cache->block[pos]->dirty = 1;
			cache->block[pos]->tag = btag;
			cache->block[pos]->count = 0;	//for LRU
		}
	}
	cache->time += WR_TIME;
}

/*Function to write data to cache
  tag => bitset<TAG> btag;
  block_index => bitset<LINE> index;
  The replacement strategy currently used is LRU.
  If you want to use Random, please comment out the code commented as "for LRU"
  and remove the comment commented on the code as "for Random"
*/
void Write(Cache cache, uint64_t address, int n) {
	bitset<TAG> btag;
	bitset<LINE> index;
	bitset<LENGTH> bit = change_Add(address);
	bool isWrite = false;
	for (int i = 0; i < CACHE_LINE; i++) {
		cache->block[i]->count++;
	}
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		btag[i - (LINE + OFFSET - 1)] = bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		index[i - (OFFSET - 1)] = bit[i];
	}
	int pos = index.to_ulong();
	int ran = LRU(cache, pos);	//for LRU
	//int ran = getRandom();	//for Random
	if (cache->block[pos]->valid == 1 && cache->block[pos]->tag == btag) {
		cache->hit++;
		cache->block[pos]->dirty = 1;
		cache->time += WR_TIME;
		cache->block[pos]->count = 0;	//for LRU
	}
	else {
		for (int i = 0; i < n; i++) {
			if (cache->block[pos]->valid == 1) {
				if (cache->block[(ran + i) % CACHE_LINE]->dirty == 1) {
					isWrite = true;
				}
				if ((ran + i) >= CACHE_LINE) {
					bitset<13> ctag((btag.to_ulong() + 1) % TAG_SIZE);
					btag = ctag;
				}
				cache->time += WR_TIME;
				cache->block[(ran + i) % CACHE_LINE]->valid = 1;
				cache->block[(ran + i) % CACHE_LINE]->dirty = 1;
				cache->block[(ran + i) % CACHE_LINE]->tag = btag;
				cache->block[ran]->count = 0;	//for LRU
			}
			if (cache->block[pos]->valid == 0) {
				if (cache->block[(pos + i) % CACHE_LINE]->dirty == 1) {
					isWrite = true;
				}
				if ((pos + i) >= CACHE_LINE) {
					bitset<13> ctag((btag.to_ulong() + 1) % TAG_SIZE);
					btag = ctag;
				}
				cache->time += WR_TIME;
				cache->block[(pos + i) % CACHE_LINE]->valid = 1;
				cache->block[(pos + i) % CACHE_LINE]->dirty = 1;
				cache->block[(pos + i) % CACHE_LINE]->tag = btag;
				cache->block[ran]->count = 0;	//for LRU
			}
		}
		if (isWrite) {
			cache->time += WRITE_TIME;
		}
		cache->time += READ_TIME;
	}
}

/*Optimized function to read
  for each W/R, there are (W/R_size)/Block_size addresses.
  we write/read all addresses at the same time
  and all addresses perform the same operation at the same time if they have a common operation
  ex: to access cache at the same time => W/R time
      all hit => to 
*/
void Read_update(Cache cache, uint64_t address) {
	bitset<TAG> btag;
	bitset<LINE> index;
	bitset<LENGTH> bit = change_Add(address);
	for (int i = 0; i < CACHE_LINE; i++) {
		cache->block[i]->count++;
	}
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		btag[i - (LINE + OFFSET - 1)] = bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		index[i - (OFFSET - 1)] = bit[i];
	}
	int pos = index.to_ulong();
	int ran = LRU(cache, pos);	//for LRU
	//int ran = getRandom();	//for Random
	if (cache->block[pos]->valid == 1 && cache->block[pos]->tag == btag) {
		cache->hit++;
		cache->block[pos]->count = 0;	//for LRU
	}
	else {
		if (cache->block[pos]->tag != NULL) {
			if (cache->block[ran]->dirty == 1) {
				//cache->time += WRITE_TIME;
				cache->block[ran]->dirty = 0;
			}
			//cache->time += READ_TIME;
			cache->block[ran]->valid = 1;
			cache->block[ran]->tag = btag;
			cache->block[ran]->count = 0;	//for LRU
		}
		if (cache->block[pos]->tag == NULL) {
			/**if (cache->block[pos]->dirty == 1) {
				cache->time += WRITE_TIME;
			}**/
			//cache->time += READ_TIME;
			cache->block[pos]->valid = 1;
			cache->block[pos]->tag = btag;
			cache->block[pos]->count = 0;	//for LRU
		}
		cache->time += WR_TIME;
	}
	cache->time += WR_TIME;
}

/*Function to read data from cache
  tag => bitset<TAG> btag;
  block_index => bitset<LINE> index;
  The replacement strategy currently used is random replacement.
  If you want to use LRU, please comment out the code commented as "for Random"
  and remove the comment commented on the code as "for LRU"
*/
void Read(Cache cache, uint64_t address, int n) {
	bitset<TAG> btag;
	bitset<LINE> index;
	bitset<LENGTH> bit = change_Add(address);
	bool isRead = false;
	for (int i = 0; i < CACHE_LINE; i++) {
		cache->block[i]->count++;
	}
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		btag[i - (LINE + OFFSET - 1)] = bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		index[i - (OFFSET - 1)] = bit[i];
	}
	int pos = index.to_ulong();
	int ran = LRU(cache, pos);	//for LRU
	//int ran = getRandom();	//for Random
	if (cache->block[pos]->valid == 1 && cache->block[pos]->tag == btag) {
		cache->hit++;
		cache->time += WR_TIME;
		cache->block[pos]->count = 0;	//for LRU
	}
	else {
		for (int i = 0; i < n; i++) {
			if (cache->block[pos]->valid == 1) {
				if (cache->block[(ran + i) % CACHE_LINE]->dirty == 1) {
					isRead = true;
					cache->block[(ran + i) % CACHE_LINE]->dirty = 0;
				}
				if ((ran + i) >= CACHE_LINE) {
					bitset<13> ctag((btag.to_ulong() + 1) % TAG_SIZE);
					btag = ctag;
				}
				cache->time += WR_TIME;
				cache->block[(ran + i) % CACHE_LINE]->valid = 1;
				cache->block[(ran + i) % CACHE_LINE]->tag = btag;
				cache->block[ran]->count = 0;	//for LRU
			}
			if (cache->block[pos]->valid == 0) {
				if (cache->block[(pos + i) % CACHE_LINE]->dirty == 1) {
					isRead = true;
					cache->block[(pos + i) % CACHE_LINE]->dirty = 0;
				}
				if ((pos + i) >= CACHE_LINE) {
					bitset<13> ctag((btag.to_ulong() + 1) % TAG_SIZE);
					btag = ctag;
				}
				cache->time += WR_TIME;
				cache->block[(pos + i) % CACHE_LINE]->valid = 1;
				cache->block[(pos + i) % CACHE_LINE]->tag = btag;
				cache->block[ran]->count = 0;	//for LRU
			}
		}
		if (isRead) {
			cache->time += WRITE_TIME;
		}
		cache->time += READ_TIME;
	}
}

int hit_num(Cache cache, uint64_t address) {
	int result = 0;
	bitset<TAG> hit_tag;
	bitset<LINE> hit_index;
	bitset<LENGTH> hit_bit = change_Add(address);
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		hit_tag[i - (LINE + OFFSET - 1)] = hit_bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		hit_index[i - (OFFSET - 1)] = hit_bit[i];
	}
	int pos = (hit_index.to_ulong());
	if (cache->block[pos]->valid == 1 && cache->block[pos]->tag == hit_tag) {
		result++;
	}
	return result;
}

int dirty_num(Cache cache, uint64_t address){
	bitset<TAG> dirty_tag;
	bitset<LINE> hit_index;
	bitset<LENGTH> hit_bit = change_Add(address);
	for (int i = LINE + OFFSET - 1; i < LENGTH - 1; i++) {
		dirty_tag[i - (LINE + OFFSET - 1)] = hit_bit[i];
	}
	for (int i = OFFSET - 1; i < OFFSET + LINE - 1; i++) {
		hit_index[i - (OFFSET - 1)] = hit_bit[i];
	}
	int pos = (hit_index.to_ulong());
	if (cache->block[LRU(cache, pos)]->dirty == 1) {
		return 1;
	}
	return 0;
}

//to initialize new cache
Cache newCache() {
	Cache cache;
	cache = (Cache)malloc(sizeof(struct CACHE));
	cache->hit = 0;
	cache->time = 0;
	cache->block = (Block*)malloc(sizeof(Block) * CACHE_LINE);
	for (int i = 0; i < CACHE_LINE; i++)
	{
		cache->block[i] = (Block)malloc(sizeof(struct BLOCK));
		cache->block[i]->dirty = 0;
		cache->block[i]->valid = 0;
		cache->block[i]->tag = NULL;
		cache->block[i]->count = 0;
	}
	return cache;
}

void destroy(Cache cache) {
	if (cache != NULL) {
		for (int i = 0; i < CACHE_LINE; i++) {
			free(cache->block[i]);
		}
		free(cache->block);
		free(cache);
	}
}

int main() {
	Cache cache = newCache();
	char filepath[100];
	string line;
	ifstream in_file;
	uint64_t sum = 0, sum_w = 0, new_sum = 0;
	uint64_t sum_data = 0, w_data = 0;
	cout << "Please input the path that you want to use!" << endl;
	cin >> filepath;
	in_file.open(filepath, ios::in);
	while (in_file.fail()) {
		cout << "ERROR! Please check the path that you input!" << endl;
		cin >> filepath;
		in_file.open(filepath, ios::in);
	}
	while (getline(in_file, line)) {
		stringstream sin(line);
		string field;
		int flag = 0;
		bool hit_flag = true;
		int dirt_flag = 0;
		vector<uint64_t> op;
		while (getline(sin, field, ',')) {
			flag++;
			if (flag == 4) {
				if (field == "Write") {
					op.push_back(1);
				}
				else {
					op.push_back(0);
				}
			}
			if (flag == 5 || flag == 6) {
				op.push_back(_atoi64(field.c_str()));
			}
		}
		int data = op[2];
		sum_data += data;
		int num = data % LINE_SIZE == 0 ? data / LINE_SIZE : (data / LINE_SIZE + 1);
		int c_num = num % 4 == 0 ? num / 4 : (num / 4 + 1);
		uint64_t address = op[1];
		uint64_t current_ad = 0;
		if (op[0] == 1) {
			w_data += data;
			for (int i = 0; i < num; i++) {
				current_ad = address + i * LINE_SIZE;
				//Write(cache, current_ad, num);
				Write_update(cache, current_ad);
				if (hit_num(cache, current_ad) == 0) {
					hit_flag = false;
					if (dirty_num(cache, current_ad) == 1) {
						dirt_flag++;
					}
				}
				sum++;
				sum_w++;
			}
		}
		if (op[0] == 0) {
			for (int i = 0; i < num; i++) {
				current_ad = address + i * LINE_SIZE;
				//Read(cache, current_ad, num);
				Read_update(cache, current_ad);
				if (hit_num(cache, current_ad) == 0) {
					hit_flag = false;
					if (dirty_num(cache, current_ad) == 1) {
						dirt_flag++;
					}
				}
				sum++;
			}
		}
		new_sum += c_num;
		dirt_flag = dirt_flag % 4 == 0 ? dirt_flag / 4 : (dirt_flag / 4 + 1);
		if (hit_flag == false) {
				cache->time += (READ_TIME * c_num);
		}
		cache->time += (WRITE_TIME*dirt_flag);
		vector<uint64_t>().swap(op);
	}
	in_file.close();
	cout << "The average response time = " << (float)cache->time / sum << endl;
	cout << "The ratio of write size = " << (double)w_data / sum_data * 100 << endl;
	cout << "The ratio of write = " << (double)sum_w / sum * 100 << endl;
	cout << "The hit ratio = " << (double)cache->hit / sum * 100 << endl;	//not update
	destroy(cache);
	system("pause");
	return 0;
}
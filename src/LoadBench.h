#ifndef LOAD_BENCH_H
#define LOAD_BENCH_H

#include <string>
#include <vector>
#include "Customer.h"

using namespace std;

class LoadBench {
public:
	LoadBench(string path) : path(path) {};
	~LoadBench() {};
	bool load(vector<Customer*> &staticCustomer, vector<Customer*> &dynamicCustomer, Customer &depot, float &capacity);
private:
	string path;
};

#endif
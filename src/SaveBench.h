#ifndef SAVE_BENCH_H
#define SAVE_BENCH_H

#include<vector>
#include "Customer.h"
#include<string>

using namespace std;

class SaveBench {
public:
	SaveBench(vector<Customer*> staticCustomer, vector<Customer*> dynamicCustomer, Customer depot, float capacity, int timeSlotNum); 
	~SaveBench() {};
	void save(string path);
private:
	vector<Customer*> dynamicCustomer;
	vector<Customer*> staticCustomer;
	Customer depot;
	float capacity;
	int timeSlotNum;
};

#endif // ! SAVE_BENCH_H


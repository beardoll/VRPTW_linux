#ifndef _ALNS_H
#define _ALNS_H
#include "Car.h"
#include<vector>

class ALNS{  // �㷨��
public:
	ALNS(vector<Customer*> allCustomer, Customer depot, float capacity, int maxIter);
	~ALNS(){};
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd);
	void run(vector<Car*> &finalCarSet, float &finalCost);  // �����㷨���൱���㷨��main()����
private:
	vector<Customer*> allCustomer;
	Customer depot;
	float capacity;
	int maxIter;
};

#endif
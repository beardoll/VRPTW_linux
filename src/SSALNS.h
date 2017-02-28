#ifndef _SSLANS_H
#define _SSALNS_H
#include<vector>
#include "Car.h"
#include<thread>
#include<mutex>

class SSALNS{  // SSALNS�㷨
public:
	SSALNS(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter);
	~SSALNS();
	void shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float maxd, float maxt, float maxquantity);
	void randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q);
	void worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float DTpara[]);  
	void greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[]);
	void regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[]);
	void run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck);  // �����㷨���൱���㷨��main()����
private:
	vector<Customer*> waitCustomer;     // ������Ĺ˿�
	vector<Car*> originPlan;   // ��ʼ�ƻ�
	Customer depot;
	float capacity;
    int maxIter;
};

#endif

#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H

#include "Car.h"

using namespace std;

float random(float start, float end);
template<class T1, class T2> bool ascendSort(pair<T1, T2> x, pair<T1, T2> y) {
	// ��������
	// �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
	return x.first < y.first;
}
template<class T1, class T2>  bool descendSort(pair<T1, T2> x, pair<T1, T2> y) {
	// �ݼ�����
	// �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
	return x.first > y.first;
}
vector<float> randomVec(int num);
vector<int> getRandom(int lb, int ub, int m, vector<int> &restData);
	// ��÷�Χ��lb��ub��m�����ظ�������
	// ʣ����������restData��
int roulette(vector<float> probability);
	// ����probability��Ӧ�������㷨�õ�������������ֵ���ɢֵ
	// ��Ƕ��probability���й�һ���ĺ���
int roulette(float *probability, int num);
	// ����probability��Ӧ�������㷨�õ�������������ֵ���ɢֵ
	// ������Ǹ��������ͷָ���Լ��ܹ��ĸ��ʷֲ�����
	// ��Ƕ��probability���й�һ���ĺ���
inline void withdrawPlan(vector<Car*> &Plan);    // ���ټƻ�
inline vector<Car*> copyPlan(vector<Car*> Plan); // ���Ƽƻ�
inline void deleteCustomerSet(vector<Customer*> &customerSet);            // ɾ��customerSet
inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet);  // ����customerSet


// ģ�庯��������������ʵ��
inline void withdrawPlan(vector<Car*> &Plan){  
	// ���ټƻ�
	vector<Car*>::iterator carIter;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
		delete(*carIter);
	}
	Plan.clear();
}

inline vector<Car*> copyPlan(vector<Car*> Plan) {
	// ���Ƽƻ�
	vector<Car*>::iterator carIter;
	vector<Car*> outputPlan;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++) {
		Car* newCar = new Car(**carIter);
		outputPlan.push_back(newCar);
	}
	return outputPlan;
}

inline void deleteCustomerSet(vector<Customer*> &customerSet){   // ɾ��CustomerSet
	vector<Customer*>::iterator iter;
	for(iter = customerSet.begin(); iter < customerSet.end(); iter++) {
		delete(*iter);
	}
	customerSet.clear();
}

inline vector<Customer*> copyCustomerSet(vector<Customer*> customerSet){  // ����customerSet
	vector<Customer*> outputCust;
	vector<Customer*>::iterator custIter;
	for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
		Customer *newCust = new Customer(**custIter);
		outputCust.push_back(newCust);
	}
	return outputCust;
}

#endif

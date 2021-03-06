#include "PublicFunction.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>
#include<sstream>
#include<iostream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>
#include "Matrix.h"
#include "ALNS.h"

bool ascendSortForCustId(Customer* item1, Customer* item2) {
	return item1->id < item2->id;
}

float random(float start, float end){
	// 产生start到end之间的随机数
	// some times using (int)output will get the boundary value "end" !!
	return (float)(start+(end-start)*rand()/(RAND_MAX+1.0));
}

vector<float> randomVec(int num){  // 产生num个随机数，它们的和为1
	float rest = 1;  // 初始余量为1
	vector<float> output;
	if(num == 1) {
		output.push_back(1);
	} else {
		for(int i=0; i<num - 1; i++) {
			float temp = random(0, rest); // 产生随机数
			output.push_back(temp);
			rest -= temp;
		}
		output.push_back(rest);
	}
	return output;
}

vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
	// 产生m个不同的，范围为[lb, ub)的随机数
	// restData, 除了返回值外剩余的数值
	restData.resize(0);
	for(int i=0; i<ub-lb; i++) {
		restData.push_back(i+lb);
	}
	int total = ub-lb;
	vector<int> outputData(0);
	for(int j=0; j<m; j++) {
		vector<int>::iterator iter = restData.begin();
		int num = (int)rand() % total; // 0-total-1, if normal 
		num = min(num, total - 1);     // avoid exceeding the right side
		iter += num;
		int temp = *(iter);
		outputData.push_back(temp);
		restData.erase(iter);
		total--;
	}
	return outputData;
}

int roulette(vector<float> probability) {
	// 轮盘算法
	// 随机选出一个整数k (from 0 to |probability|)。
	vector<float>::iterator floatIter;
	float sumation1 = accumulate(probability.begin(), probability.end(), 0.0f); // 求和
	for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
		*floatIter /= sumation1;  // 归一化
	}
	int totalNum = probability.end() - probability.begin();  // 总数目
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	floatIter = probability.begin();
	while((sumation < randFloat) && (floatIter < probability.end())) {
		k++;
		sumation += *(floatIter++);
	}
	k = min(k-1, totalNum-1); // avoid k is larger than totalNum
	k = max(k, 0);            // when randFloat = 0, here k < 0 will happen
	return k;
}

int roulette(float *probability, int num) {
	// 轮盘算法
	// 随机选出一个整数k (from 0 to |probability|)。
	// 一共有num个概率分布
	int i;
	float sumation1 = 0; // 求和
	for(i=0; i<num; i++) {
		sumation1 += probability[i];
	}
	for(i=0; i<num; i++) {
		probability[i] /= sumation1;  // 归一化
	}
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	while((sumation < randFloat) && (k < num)) {
		k++;
		sumation += probability[k];
	}
	k = min(k - 1, num - 1);   // avoid k is larger than num
	k = max(k, 0);             // when randFloat = 0, here k < 0 will happen
	return k;
}



void seperateCustomer(vector<Customer*> originCustomerSet, vector<Customer*> &staticCustomer, vector<Customer*> &dynamicCustomer, float dynamicism) {
	sort(originCustomerSet.begin(), originCustomerSet.end(), ascendSortForCustId);
	int customerAmount = originCustomerSet.size();
	int dynamicNum = (int)floor(customerAmount*dynamicism);  // 动态到达的顾客数量
	vector<int> staticPos;           // 静态到达的顾客节点在originCustomerSet中的定位
	vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);   // 动态到达的BHs在BHs集合下的坐标
	vector<Customer*>::iterator iter;
	staticCustomer.resize(0);
	dynamicCustomer.resize(0);
	vector<Customer*> originCopy = copyCustomerSet(originCustomerSet);
	for (iter=originCopy.begin(); iter < originCopy.end(); iter++) {
		int count = iter - originCopy.begin();  // 当前顾客节点于originCustomerSet中的定位
													   // 这里默认originCustomerSet是按id升序排列
		vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count); // 寻找count是否是dynamicPos中的元素
		if (iter2 != dynamicPos.end()) {   // 在dynamicPos集合中
			(*iter)->prop = 1;
			dynamicCustomer.push_back(*iter);
		}
		else {
			(*iter)->prop = 0;
			staticCustomer.push_back(*iter);
		}
	}
}

void loadData(TiXmlElement *element, float data) {
	ostringstream buffer;
	string ss;
	buffer << data;
	ss = buffer.str();
	element->LinkEndChild(new TiXmlText(ss.c_str()));
	buffer.str("");
}

void computeBest(vector<Car*> carSet, vector<Car*> &bestRoute, float &bestCost){
	Customer depot = carSet[0]->getRearNode();
	float capacity = carSet[0]->getCapacity();
	vector<Customer*> allCustomer;
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        cout << "Loading the " << carIter - carSet.begin() << "car data!" << endl;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			allCustomer.push_back(*custIter);
		}
	}
	depot.priority = 0;
	depot.startTime = 0;
	depot.serviceTime = 0;
	depot.arrivedTime = 0;
	depot.prop = 0;
	ALNS alg(allCustomer, depot, capacity, 25000);
    alg.run(bestRoute, bestCost);
	deleteCustomerSet(allCustomer);
}

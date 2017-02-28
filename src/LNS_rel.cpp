#include "LNS_rel.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<numeric>
#include<sstream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>
#include "PublicFunction.h"
// #include<hash_set>

// namespace __gnu_cxx{
// 	template<>
// 	struct hash<std::string>
// 	{
// 		hash<char*> h;
// 		size_t operator() (const std::string &s) {
// 			return h(s.c_str());
// 		}
// 	};
// }

using namespace std;
// using namespace __gnu_cxx;

void getAllCustomerInOrder(vector<Car*> originCarSet, vector<int> &customerNum, vector<Customer*> &allCustomerInOrder){
	// ��ȡCar���������еĹ˿�
	// customerNum: ����������Ĺ˿�����
	// allCustomer: ���еĹ˿ͽڵ�
	int i=0;
	for(vector<Car*>::iterator it1 = originCarSet.begin(); it1 < originCarSet.end(); it1++){
		if(i==0){  // ���customerNum��û��Ԫ�أ�����Ҫ�ۼ�
			customerNum.push_back((*it1)->getCustomerNum()); 
		}else{     // ���������Ҫ�ۼ�
			customerNum.push_back((*it1)->getCustomerNum() + customerNum[i-1]);
		}
		i++;
		vector<Customer*> customerSet = (*it1)->getAllCustomer(); // ÿ������������Ĺ˿�
		for(vector<Customer*>::iterator it2=customerSet.begin(); it2<customerSet.end(); it2++){
			Customer *temp = new Customer;
			*temp = **it2;
			allCustomerInOrder.push_back(temp);   // ����˿ͽڵ����
			// customerSet���ص������ã�����Ϊ�˷�ֹ�����⣬�����½��˶Ѷ���
			// �����ں���ɾ����customerSet
		}
		deleteCustomerSet(customerSet);
	}
}

void deleteCustomer(vector<int> removedIndexset, vector<int> customerNum, vector<Customer*> allCustomerInOrder, 
					vector<Car*> &originCarSet, vector<Customer*> &removedCustomer){
	// ���ڽ�removedIndexset��Ӧ��customer���ó���
	// ͬʱ������car��·�����и��ģ��Ƴ��ڵ㣩
	int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();
	for(int i=0; i<indexRemovedLen; i++){
		int carIndex;
		int currentIndex = removedIndexset[i];
		vector<int>::iterator iter;
		iter = upper_bound(customerNum.begin(), customerNum.end(), currentIndex);  // ��һ����currentIndex���Ԫ��
		carIndex = iter - customerNum.begin();
		bool mark = originCarSet[carIndex]->deleteCustomer(*allCustomerInOrder[currentIndex]);
		if(mark == false) {
			cout << "what??@@" << endl;
		}
		Customer *temp = new Customer;
		*temp = *allCustomerInOrder[currentIndex];
		removedCustomer.push_back(temp);
	}
}

void computeReducedCost(vector<Car*> originCarSet, vector<int> indexsetInRoute, vector<int> removedIndexset, 
						vector<pair<float, int> > &reducedCost, float DTpara[]){
	// indexsetInRoute: ����·���еĽڵ���
	// removedIndexSet: �ѱ��Ƴ��Ľڵ���
	int i;
	int carNum = originCarSet.end() - originCarSet.begin();
	vector<float> reducedCostInRoute(0); // ����·���еĸ����ڵ���Ƴ�����
	for(i=0; i<carNum; i++){
		vector<float> tempReducedCost = originCarSet[i]->computeReducedCost(DTpara);
		reducedCostInRoute.insert(reducedCostInRoute.end(), tempReducedCost.begin(), tempReducedCost.end());
	}
	for(i=0; i<indexsetInRoute.end()-indexsetInRoute.begin(); i++){
		int index = indexsetInRoute[i];
		reducedCost[index].first = reducedCostInRoute[i];
		reducedCost[index].second = index;
	}
	for(i=0; i<removedIndexset.end() - removedIndexset.begin(); i++){
		int index = removedIndexset[i];
		reducedCost[index].first = MAX_FLOAT;  // �Ѿ��Ƴ����Ľڵ㣬��������
		reducedCost[index].second = index;	
	}
}

void generateMatrix(vector<int> &allIndex, vector<Car*> &removedCarSet, vector<Customer*> removedCustomer, Matrix<float> &minInsertPerRoute, 
					Matrix<Customer> &minInsertPos, Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, float noiseAmount, bool noiseAdd,
					float DTpara[], bool regularization){
	// regularization: Ϊtrue��ʾ����ALNS�㷨������Ҫʩ�ӳͷ���
	//                 Ϊfalse��ʾ����SSALNS�㷨����Ҫʩ�ӳͷ���
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);
	int removedCustomerNum = removedCustomer.size();
	int carNum = removedCarSet.size();
	for(int i=0; i<carNum; i++){
		// removedCarSet[i]->getRoute().refreshArrivedTime();  // �ȸ���һ�¸���·����arrivedTime
		for(int j=0; j<removedCustomerNum; j++){
			if(i==0){
				allIndex.push_back(j);
			}
			float minValue, secondValue;
			Customer customer1, customer2;
			float penaltyPara = 0;
			if(regularization = false) {
				if(removedCarSet[i]->judgeArtificial() == false){  // ����������⳵
					switch(removedCustomer[j]->priority) {  // ��������⳵
					case 1:
						penaltyPara = -DTH1;
						break;
					case 2:
						penaltyPara = -DTL1;
						break;
					}
				} else {   // ��������⳵������Ҫ���гͷ�
					switch(removedCustomer[j]->priority) {  // ��������⳵
					case 1:
						penaltyPara = DTH2;
						break;
					case 2:
						penaltyPara = DTL2;
						break;
					}
				}
			}
			removedCarSet[i]->computeInsertCost(*removedCustomer[j], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara, regularization);
			minInsertPerRoute.setValue(i, j, minValue);
			minInsertPos.setValue(i, j, customer1);
			secondInsertPerRoute.setValue(i, j, secondValue);
			secondInsertPos.setValue(i, j, customer2);
		}
	}
}

void updateMatrix(vector<int> restCustomerIndex, Matrix<float> &minInsertPerRoute, Matrix<Customer> &minInsertPos, 
				  Matrix<float> &secondInsertPerRoute, Matrix<Customer> &secondInsertPos, int selectedCarPos, vector<Car*> &removedCarSet,
				  vector<Customer*>removedCustomer, float noiseAmount, bool noiseAdd, float DTpara[], bool regularization){
	// regularization: Ϊtrue��ʾ����ALNS�㷨������Ҫʩ�ӳͷ���
	// Ϊfalse��ʾ����SSALNS�㷨����Ҫʩ�ӳͷ���
	// ��ȡDTpara�е�Ԫ��
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);

	// �����ĸ�����
	// removedCarSet[selectedCarPos]->getRoute().refreshArrivedTime();
	for(int i=0; i<(int)restCustomerIndex.size();i++) {
		int index = restCustomerIndex[i];   // �˿��±�
		float minValue, secondValue;
		Customer customer1, customer2;
		float penaltyPara = 0;
		if(regularization = false) {
			if(removedCarSet[selectedCarPos]->judgeArtificial() == false) { // ��������鹹�ĳ���
				switch(removedCustomer[index]->priority) {  // ���ݲ�ͬ�Ĺ˿����ȼ������費ͬ�ĳͷ�ϵ���������뵽artificial vehicleʱ��
				case 1:
					penaltyPara = -DTH1;
					break;
				case 2:
					penaltyPara = -DTL1;
					break;
				}		
			} else {   // ���鹹�ĳ���
				switch(removedCustomer[index]->priority) {  // ���ݲ�ͬ�Ĺ˿����ȼ������費ͬ�ĳͷ�ϵ���������뵽artificial vehicleʱ��
				case 1:
					penaltyPara = DTH2;
					break;
				case 2:
					penaltyPara = DTL2;
					break;
				}		
			}
		}
		removedCarSet[selectedCarPos]->computeInsertCost(*removedCustomer[index], minValue, customer1, secondValue, customer2, noiseAmount, noiseAdd, penaltyPara, regularization);
		minInsertPerRoute.setValue(selectedCarPos, index, minValue);
		minInsertPos.setValue(selectedCarPos, index, customer1);
		secondInsertPerRoute.setValue(selectedCarPos, index, secondValue);
		secondInsertPos.setValue(selectedCarPos, index, customer2);
	}
}

void reallocateCarIndex(vector<Car*> &originCarSet){  // ����Ϊ�������
	int count = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){
		if(originCarSet[i]->judgeArtificial() == false) {  // ��Ϊ��ʵ�ĳ���������Ҫ���±��
			count++;
		} else{  // ��Ϊ��ٵĳ����������±��
			originCarSet[i]->changeCarIndex(count);
			count++;
		}
	}
}

void removeNullRoute(vector<Car*> &originCarSet, bool mark){    
	// ����ճ���
	// ��mark=true, ��ֻ�����������Ŀճ�
	vector<Car*>::iterator iter;
	vector<Car*>::iterator temp;
	int count = 0;
	for(iter=originCarSet.begin(); iter<originCarSet.end();){
		if ((*iter)->getRoute().getSize() == 0){
			if(mark == true) {
				if ((*iter)->judgeArtificial() == true) { // ����ǿճ�����������ĳ�
					temp = iter;
					delete(*iter);
					iter = originCarSet.erase(temp);
				} else{
					(*iter)->changeCarIndex(count++);
					++iter;				
				}
			}
			else {
				temp = iter;
				delete(*iter);
				iter = originCarSet.erase(temp);
			}
		} else {
			(*iter)->changeCarIndex(count++);
			++iter;
		}
	}
}

size_t codeForSolution(vector<Car*> originCarSet){  // ��ÿ���⣨����·��������hash����
	vector<int> customerNum;
	vector<Customer*> allCustomerInOrder;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	stringstream ss;
	string allStr = "";
	for(int i=0; i<(int)allCustomerInOrder.size(); i++){
		ss.str("");  // ÿ��ʹ��֮ǰ�����ss
		int a = allCustomerInOrder[i]->id;
		ss << a;
		allStr += ss.str();
	}
	ss.str("");
	hash<string> str_hash;
	deleteCustomerSet(allCustomerInOrder);
	return str_hash(allStr);
}

void computeMax(vector<Customer*> allCustomer, float &maxd, float &mind, float &maxquantity){
	// �������й˿�֮����������Լ��˿͵�������������
	int customerAmount = (int)allCustomer.size();
	Matrix<float> D1(customerAmount, customerAmount);
	Matrix<float> D2(customerAmount, customerAmount);
	float tempmax = -MAX_FLOAT;
	for(int i=0; i<customerAmount; i++){
		if(allCustomer[i]->quantity > tempmax){
			tempmax = allCustomer[i]->quantity;
		}
		D1.setValue(i,i, 0.0f);
		D2.setValue(i,i, MAX_FLOAT);
		for(int j=i+1; j<customerAmount; j++){
			float temp = sqrt(pow(allCustomer[i]->x - allCustomer[j]->x, 2) + pow(allCustomer[i]->y - allCustomer[j]->y, 2));
			D1.setValue(i, j, temp);
			D2.setValue(i, j, temp);
			D1.setValue(j, i, temp);
			D2.setValue(j, i, temp);
		}
	}
	int t1, t2;
	maxd = D1.getMaxValue(t1, t2);
	mind = D2.getMinValue(t1, t2);
	maxquantity = tempmax;
}

float getCost(vector<Car*> originCarSet, float DTpara[]){
	// ����originCarSet��·��
	float totalCost = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){

		float temp;
		if(originCarSet[i]->judgeArtificial() == true) {
			temp = originCarSet[i]->getRoute().getLen(DTpara, true);
		} else {
			temp = originCarSet[i]->getRoute().getLen(DTpara, false);
		}
		totalCost += temp;
	}
	return totalCost;
}

int getCustomerNum(vector<Car*> originCarSet){
	// ���·�����й˿ͽڵ����Ŀ
	int customerNum = 0;
	for(int i=0; i<(int)originCarSet.size(); i++){
		customerNum += originCarSet[i]->getRoute().getSize();
	}
	return customerNum;
}

bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2){
	// �ж�carSet1��carSet2�Ƿ����
	if(carSet1.size() != carSet2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)carSet1.size(); i++){
		vector<Customer*> cust1 = carSet1[i]->getRoute().getAllCustomer();
		vector<Customer*> cust2 = carSet2[i]->getRoute().getAllCustomer();
		if(cust1.size() != cust2.size()) {mark = false; break;}
		for(int j=0; j<(int)cust1.size(); j++) {
			if(cust1[j]->id != cust2[j]->id) {mark = false; break;}
		}
		deleteCustomerSet(cust1);
		deleteCustomerSet(cust2);
	}
	return mark;
}

bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2){
	if(c1.size() != c2.size()) {return false;}
	bool mark = true;
	for(int i=0; i<(int)c1.size(); i++) {
		if(c1[i]->id != c2[i]->id) {mark = false; break;}
	}
	return mark;

}

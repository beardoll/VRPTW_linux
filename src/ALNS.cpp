#include "PublicFunction.h"
#include "LNS_rel.h"
#include "ALNS.h"
#include<algorithm>
#include<cassert>
#include<functional>
#include<cmath>

using namespace std;

ALNS::ALNS(vector<Customer*> allCustomer, Customer depot, float capacity, int maxIter):depot(depot), capacity(capacity),
	maxIter(maxIter){
	this->allCustomer = copyCustomerSet(allCustomer);
}

void ALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	// maxd, maxt, maxquantity: ��һ��ֵ
	// ÿ��ѭ���Ƴ� y^p*|L|���˿ͣ�LΪ·����ʣ��ڵ㣬y��0-1֮��������
	ascendSort<int, int>(make_pair(1,2), make_pair(3,4));						
	int phi = 9;
	int kai = 3;
	int psi = 2;
	int carAmount = originCarSet.end()-originCarSet.begin();  // ��������
	vector<int> customerNum(0);       // ������������Ĺ˿ͽڵ���Ŀ
	vector<Customer*> allCustomerInOrder(0);
	int i,j;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);   
	// ��ȡ���е�Customer, customerNum��Ÿ�������ӵ�еĹ˿ͽڵ���Ŀ�� allCustomer������й˿ͽڵ�
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // �˿�����
	vector<pair<float, int> > R(customerAmount*customerAmount);     // ���ƾ���
	float temp1;
	vector<int> allIndex(customerAmount);  // 0~customerAmount-1
	for(i=0; i<customerAmount; i++){
		allIndex[i] = i;
		for(j=0; j<customerAmount; j++){
			if(i==j) { 
				R[i*customerAmount+j].first = MAX_FLOAT;
				R[i*customerAmount+j].second = j;
			}
			else{
				temp1 = phi*sqrt(pow(allCustomerInOrder[i]->x-allCustomerInOrder[j]->x,2) + pow(allCustomerInOrder[i]->y-allCustomerInOrder[j]->y, 2))/maxd +
					kai * abs(allCustomerInOrder[i]->arrivedTime - allCustomerInOrder[j]->arrivedTime)/maxt +
					psi * abs(allCustomerInOrder[i]->quantity - allCustomerInOrder[j]->quantity)/maxquantity;
				R[i*customerAmount+j].first = temp1;   // i��j��
				R[i*customerAmount+j].second = j;
				R[j*customerAmount+i].first = temp1;
				R[j*customerAmount+i].second = i;      // j��i��
			}
		}
	}
	int selectedIndex;           // ��ѡ�еĽڵ���allCustomer�е��±�
	vector<int> removedIndexset; // ���б��Ƴ��Ľڵ���±꼯��
	selectedIndex = int(random(0,customerAmount));         // ���ѡȡһ���ڵ�
	selectedIndex = min(selectedIndex, customerAmount-1);  // avoid reaching the right side
	removedIndexset.push_back(selectedIndex);
	vector<int> indexsetInRoute(customerAmount-1);    // ��·���еĽڵ���±꼯��

	set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
	while((int)removedIndexset.size() < q){           // Ҫ�Ƴ���һ��q���ڵ�
		vector<pair<float, int> > currentR(0);        // ��ǰҪ������������ƾ�����������ֻ��������·���еĽڵ�
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin(); // ��·���еĽڵ�ĸ���
		vector<pair<float, int> >::iterator iter1;
		for(i=0; i<indexInRouteLen; i++){
			int index = indexsetInRoute[i];
			currentR.push_back(R[selectedIndex*customerAmount + index]);
		}
		sort(currentR.begin(), currentR.end(), ascendSort<float, int>);  // �����԰�С�����������
		float y = random(0,1);  // ����0-1֮��������
		int indexsetInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();    // indexsetInRoute�ĳ���
		int removeNum = max((int)floor(pow(y,p)*indexsetInRouteLen), 1);             // �����Ƴ��Ľڵ���Ŀ
		for(i=0; i<removeNum ; i++){
			removedIndexset.push_back(currentR[i].second);
		}
		int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();  // ��ǰremovedIndexset�ĳ���
		int randint = (int)random(0,indexRemovedLen);  // ����һ��0-indexRemovedLen�������
		randint = min(randint, indexRemovedLen-1);     // avoid reaching the right side
		selectedIndex = removedIndexset[randint];
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	int i;
	vector<int> customerNum(0);       // ���������˿ͽڵ���Ŀ 
	vector<Customer*> allCustomerInOrder(0);  // ���й˿ͽڵ�
	vector<int> allIndex;             // 0~customerAmount-1
	vector<int> indexsetInRoute(0);
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();
	vector<int> removedIndexset(0); 
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	for(i=0; i<q; i++){
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // ����·���еĽڵ����
		int selectedIndex = int(random(0, indexInRouteLen));     // ��indexsetInRoute�е�����
		selectedIndex = min(selectedIndex, indexInRouteLen-1);   // avoid reaching the right side
		removedIndexset.push_back(indexsetInRoute[selectedIndex]);
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		//indexsetInRoute.clear();
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	int i;
	vector<int> customerNum(0);       // ���������˿ͽڵ���Ŀ 
	vector<Customer*> allCustomerInOrder(0);  // ���й˿ͽڵ�
	vector<int> allIndex(0);             // 0~customerAmount-1
	vector<int> indexsetInRoute(0);	  // ����·���еĽڵ��±�
	vector<int> removedIndexset(0);    // ���Ƴ��Ľڵ��±�
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomer.end() - allCustomer.begin();
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int> > reducedCost(customerAmount);  // ���ڵ���Ƴ�����
		float penaltyPara[4] = {0, 0 , 0, 0};
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, penaltyPara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort<float, int>);   // ��������
		float y = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max((float)floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		for(i=0; i<removedNum; i++){
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void ALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd){
	// ��removedCustomer���뵽removedCarSet��
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();  // ������Ŀ
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��
	float penaltyPara[4] = {0, 0, 0, 0};
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, noiseAmount, noiseAdd, penaltyPara);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����allIndex
	vector<pair<float, pair<int,int> > > minInsertPerRestCust(0);  // ����removedcustomer����С�������
	                                                             // ֻ����û�в��뵽·���еĽڵ�
	                                                             // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ����±�
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // ÿ��ʹ��֮ǰ�����
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // ֻ��������·���еĽڵ�
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort<float, pair<int, int> >);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // ��ѡ�еĹ˿ͽڵ���
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // ����ҵ��˿��в���λ��
			int selectedCarIndex = minInsertPerRestCust[0].second.second;  // ��ѡ�еĳ������
			removedCarSet[selectedCarIndex]->insertAfter(minInsertPos.getElement(selectedCarIndex, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		} else {  // û�п��в���λ�ã������¿�һ������
			int selectedCarIndex = carNum++;  // ��ѡ�еĳ������
			Car *newCar = new Car(depot, depot, capacity, selectedCarIndex);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // ��ӵ�����������
			alreadyInsertIndex.push_back(selectedCustIndex);             // ����selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();    // ����һ��
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		}
	}
}

void ALNS::regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd){
	// ��removedCustomer���뵽removedCarSet��
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();  // ������Ŀ
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��
	float penaltyPara[4] = {0, 0, 0, 0};
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, noiseAmount, noiseAdd, penaltyPara);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����allIndex
	vector<pair<float, pair<int,int> > > regretdiffPerRestCust(0);  // ����removedcustomer����С����������С�������֮��
	                                                              // ֻ����û�в��뵽·���еĽڵ�
	                                                              // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ����±�
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // ѡ�еĹ˿ͽڵ���
		int selectedCarIndex;    // ѡ�еĻ������
		regretdiffPerRestCust.clear();
		for(i=0; i<(int)restCustomerIndex.size(); i++){
			int index = restCustomerIndex[i];        // �˿ͽڵ��±�
			float minValue, secondValue1, secondValue2;
			int pos1, pos2, pos3;
			minValue = minInsertPerRoute.getMinAtCol(index, pos1);          // ��С�������
			minInsertPerRoute.setValue(pos1, index, MAX_FLOAT);
			secondValue1 = minInsertPerRoute.getMinAtCol(index, pos2);      // ��ѡ��С�������
			minInsertPerRoute.setValue(pos1, index, minValue);              // �ָ�����
			secondValue2 = secondInsertPerRoute.getMinAtCol(index, pos3);   // ��ѡ��С�������
			if(minValue == MAX_FLOAT){  
				// �������ĳ���ڵ��Ѿ�û�п��в���㣬�����Ȱ���֮
				regretdiffPerRestCust.push_back(make_pair(MAX_FLOAT, make_pair(index, pos1)));
			} else if(minValue != MAX_FLOAT && secondValue1==MAX_FLOAT && secondValue2==MAX_FLOAT){
				// ���ֻ��һ�����в���㣬��Ӧ�����Ȱ���
				// ����minValue��ֵ����С��Ӧ�����ȱ�����
				// ���diff = LARGE_FLOAT - minValue
				regretdiffPerRestCust.push_back(make_pair(LARGE_FLOAT-minValue, make_pair(index, pos1)));
			} else{
				if(secondValue1 <= secondValue2){
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue1), make_pair(index, pos1)));
				} else{
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue2), make_pair(index, pos1)));
				}
			}
		}
		sort(regretdiffPerRestCust.begin(), regretdiffPerRestCust.end(), descendSort<float, pair<int, int> >);  // Ӧ���ɴ�С��������
		if(regretdiffPerRestCust[0].first == MAX_FLOAT) {
			// ������еĽڵ㶼û�п��в���㣬�򿪱��³�
			selectedCarIndex = carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, selectedCarIndex);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // ��ӵ�����������
			alreadyInsertIndex.push_back(selectedCustIndex); // ����selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // ����һ��
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);	
		} else {
			// ���򣬲���Ҫ�����³�
			selectedCarIndex = regretdiffPerRestCust[0].second.second;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			alreadyInsertIndex.push_back(selectedCustIndex);
			removedCarSet[selectedCarIndex]->insertAfter(minInsertPos.getElement(selectedCarIndex, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin());
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarIndex, removedCarSet, removedCustomer, noiseAmount, noiseAdd, penaltyPara);
		}
	}
}

void ALNS::run(vector<Car*> &finalCarSet, float &finalCost){  // �����㷨���൱���㷨��main()����
	int i;
	// ������ʼ��
	int customerAmount = allCustomer.size();
	vector<Car*> currentCarSet(0);
	Car *initialCar = new Car(depot, depot, capacity, 0);  // ���½�һ����
	currentCarSet.push_back(initialCar);
	greedyInsert(currentCarSet, allCustomer, 0, false);  // ������ʼ·��
	float penaltyPara[4] = {0, 0, 0, 0}; 
	float currentCost = getCost(currentCarSet, penaltyPara);
	vector<Car*> globalCarSet = copyPlan(currentCarSet);
	float globalCost = currentCost;

	// ��ϣ��
	vector<size_t> hashTable(0);  
	hashTable.push_back(codeForSolution(currentCarSet));

	// ���ֻ�����ز������趨
	const int removeNum = 3;    // remove heuristic�ĸ���
	const int insertNum = 2;    // insert heuristic�ĸ���
	float removeProb[removeNum];  // ����remove heuristic�ĸ���
	float insertProb[insertNum];  // ����insert heuristic�ĸ���
	float noiseProb[2] = {0.5, 0.5};        // ����ʹ�õĸ���
	for(i=0; i<removeNum; i++){
		removeProb[i] = 1.0f/removeNum;
	}
	for(i=0; i<insertNum; i++){
		insertProb[i] = 1.0f/insertNum;
	}
	float removeWeight[removeNum];  // ����remove heuristic��Ȩ��
	float insertWeight[insertNum];  // ����insert heuristic��Ȩ��
	float noiseWeight[2];   // ������/�������� �ֱ��Ȩ��
	setOne(removeWeight, removeNum);
	setOne(insertWeight, insertNum);
	setOne(noiseWeight, 2);
	int removeFreq[removeNum];      // ����remove heuristicʹ�õ�Ƶ��
	int insertFreq[insertNum];      // ����insert heuristicʹ�õ�Ƶ��
	int noiseFreq[2];               // ����ʹ�õ�Ƶ�ʣ���һ����with noise���ڶ�����without noise
	float haha[3];
	setZero(removeFreq, removeNum);
	setZero(insertFreq, insertNum);
	setZero(noiseFreq, 2);
	int removeScore[removeNum];     // ����remove heuristic�ĵ÷�
	int insertScore[insertNum];     // ����insert heuristic�ĵ÷�
	int noiseScore[2];              // �����÷�
	setZero(removeScore, removeNum);
	setZero(insertScore, insertNum);
	setZero(noiseScore, 2);
	// ����÷��趨
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight��������

	// ������Ĳ���
	int segment = 100;   // ÿ��һ��segment����removeProb, removeWeight�Ȳ���
	float w = 0.05f;      // ��ʼ�¶��趨�йز���
	float T = w * currentCost / (float)log(2);   // ��ʼ�¶�
	int p = 6;           // ����shawRemoval�����
	int pworst = 3;      // ����worstRemoval�������
	float ksi = 0.4f;     // ÿ���Ƴ������ڵ���Ŀռ�ܽڵ����ı���
	float eta = 0.025f;   // ����ϵ��
	float maxd, mind, maxquantity;    // �ڵ�֮����������Լ��ڵ��������������
	computeMax(allCustomer, maxd, mind, maxquantity);
	float noiseAmount = eta * maxd;   // ������
	float c = 0.9998f;    // ��������
	vector<Customer*> removedCustomer(0);                // ���Ƴ��Ľڵ�
	vector<Car*> tempCarSet = copyPlan(currentCarSet);   // ��ʱ��ŵ�ǰ��
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // �µ�segment��ʼ
			// cout << "...............Segement:" << (int)floor(iter/segment)+1 << "................" << endl;
			// cout << "current best cost is:" << globalCost << endl;
			// cout << "hash table length is:" << hashTable.size() << endl;
			// cout << "shaw   removal:" <<  "(score)-" << removeScore[0] << '\t' << "(freq)-" << removeFreq[0] << endl;
			// cout << "random removal:" <<  "(score)-" << removeScore[1] << '\t' << "(freq)-" << removeFreq[1] << endl;
			// cout << "worst  removal:" <<  "(score)-" << removeScore[2] << '\t' << "(freq)-" << removeFreq[2] << endl;
			// cout << "greedy  insert:" <<  "(score)-" << insertScore[0] << '\t' << "(freq)-" << insertFreq[0] << endl;
			// cout << "regret  insert:" <<  "(score)-" << insertScore[1] << '\t' << "(freq)-" << insertFreq[1] << endl;
			// cout << "noise    addIn:" <<  "(score)-" << noiseScore[0]  << '\t' << "(freq)-" << noiseFreq[0]  << endl;
			// cout << endl;
			if(iter != 0){      // ������ǵ�һ��segment
				// ����Ȩ��
				updateWeight(removeFreq, removeWeight, removeScore, r, removeNum);
				updateWeight(insertFreq, insertWeight, insertScore, r, insertNum);
				updateWeight(noiseFreq, noiseWeight, noiseScore, r, 2);
				// ���¸���
				updateProb(removeProb, removeWeight, removeNum);
				updateProb(insertProb, insertWeight, insertNum);
				updateProb(noiseProb, noiseWeight, 2);
				// ������������
				setZero(removeFreq, removeNum);
				setZero(removeScore, removeNum);
				setZero(insertFreq, insertNum);
				setZero(insertScore, insertNum);
				setZero(noiseFreq, 2);
				setZero(noiseScore, 2);
			}
		}

		// ���������ѡȡremove heuristic��insert heuristic
		// �Ը���ѡ��remove heuristic
		float removeSelection = random(0,1);  // ����0-1֮��������
		float sumation = removeProb[0];
		int removeIndex = 0;    // remove heuristic���
		while(sumation < removeSelection){
			sumation += removeProb[++removeIndex];
		}
		removeIndex = min(removeIndex, removeNum-1);   // avoid reaching the right side

		// �Ը���ѡ��insert heurisitc
		float insertSelection = random(0,1);
		sumation = insertProb[0];
		int insertIndex = 0;
		while(sumation < insertSelection){
			sumation += insertProb[++insertIndex];
		}
		insertIndex = min(insertIndex, insertNum-1);   // avoid reaching the right side

		// �Ը���ѡ���Ƿ���������Ӱ��
		float noiseSelection = random(0,1);
		bool noiseAdd = false;
		if(noiseProb[0] > noiseSelection) {
			noiseAdd = true;
		}

		////@@@@@@@@@ dangerous!!!!!!!!! @@@@@@@@@//
		//removeIndex = 0;
		////////////////////////////////////////////

		// ��Ӧ����ʹ�ô�����һ
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;
		int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // ����Ƴ���ô��ڵ�
		int minRemoveNum = 4;  // �����Ƴ���ô��ڵ�
		int currentRemoveNum = (int)random(minRemoveNum, maxRemoveNum);  // ��ǰҪ�Ƴ��Ľڵ���Ŀ

		deleteCustomerSet(removedCustomer);  // ���removedCustomer       
		removedCustomer.resize(0);

		// ִ��remove heuristic
		switch(removeIndex) {
		case 0: 
			{
				// ���ȵõ�maxArrivedTime
				float maxArrivedTime = -MAX_FLOAT;
				for(i=0; i<(int)tempCarSet.size(); i++){
					tempCarSet[i]->getRoute().refreshArrivedTime();	
					vector<float> temp = tempCarSet[i]->getRoute().getArrivedTime();
					sort(temp.begin(), temp.end(), greater<float>());
					if(temp[0] > maxArrivedTime) {
						maxArrivedTime = temp[0];
					}
				}
				shawRemoval(tempCarSet, removedCustomer, currentRemoveNum, p, maxd, maxArrivedTime, maxquantity);
				break;
			}
		case 1:
			{
				randomRemoval(tempCarSet, removedCustomer, currentRemoveNum);
				break;
			}
		case 2:
			{
				worstRemoval(tempCarSet, removedCustomer, currentRemoveNum, pworst);
				break;
			}
		}
		// ִ��insert heuristic
		switch(insertIndex) {
		case 0:
			{
				greedyInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd);
				break;
			}
		case 1:
			{
				regretInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd);
				break;
			}
		}
		assert(getCustomerNum(tempCarSet) == customerAmount);
		// �Ƴ���·��
		removeNullRoute(tempCarSet);

		// ʹ��ģ���˻��㷨�����Ƿ���ոý�
		float newCost = getCost(tempCarSet, penaltyPara);
		float acceptProb = exp(-(newCost - currentCost)/T);
		bool accept = false;
		if(acceptProb > rand()/(RAND_MAX+1.0f)) {
			accept = true;
		}
		T = T * c;   // ����
		size_t newRouteCode = codeForSolution(tempCarSet);

		// �������ж��Ƿ���Ҫ�ӷ�
		// �ӷ�������£�
		// 1. ���õ�һ��ȫ�����Ž�ʱ
		// 2. ���õ�һ����δ�����ܹ��ģ����Ҹ��õĽ�ʱ
		// 3. ���õ�һ����δ�����ܹ��Ľ⣬��Ȼ�����ȵ�ǰ����������ⱻ������
		if(newCost < globalCost){  // ���1
			removeScore[removeIndex] += sigma1;
			insertScore[insertIndex] += sigma1;
			noiseScore[1-(int)noiseAdd] += sigma1;
			withdrawPlan(globalCarSet);
			globalCarSet = copyPlan(tempCarSet);
			globalCost = newCost;
		} else {
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){  // �ý����û�б����ܹ�
				if(newCost < currentCost){    // ����ȵ�ǰ��Ҫ�ã����2
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {
					if(accept == true) {       // ���3
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    
			// ����������ˣ������currentCarSet�� ����tempCarSet����
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // ��������գ�����µ�ǰ��
			withdrawPlan(currentCarSet);
			currentCarSet = copyPlan(tempCarSet);
		} else {    
			// ����tempCarSet�ָ�ΪcurrentCarSet
			withdrawPlan(tempCarSet);
			tempCarSet = copyPlan(currentCarSet);
		}
	}

	// �������
	finalCarSet.clear();
	finalCarSet.resize(globalCarSet.size());
	finalCarSet = copyPlan(globalCarSet);
	withdrawPlan(globalCarSet);
	withdrawPlan(tempCarSet);
	withdrawPlan(currentCarSet);
	deleteCustomerSet(allCustomer);
	hashTable.clear();
	finalCost = globalCost;
}


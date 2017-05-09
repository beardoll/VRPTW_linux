#include "SSALNS.h"
#include "LNS_rel.h"
#include "PublicFunction.h"
#include<algorithm>
#include<cassert>
#include "Matrix.h"
#include<functional>
#include "TxtRecorder.h"
#include<cmath>

using namespace std;

SSALNS::SSALNS(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter){
	// �����˿͸������ȼ�
	this->capacity = capacity;
    this->maxIter = maxIter;
	depot = originPlan[0]->getRearNode();
	vector<Customer*>::iterator custPtr;
	for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++){
		Customer* newCust = new Customer(**custPtr);
		newCust->priority = 2;
		this->waitCustomer.push_back(newCust);
	}
	vector<Car*>::iterator carPtr;
	for(carPtr = originPlan.begin(); carPtr < originPlan.end(); carPtr++){
		Customer headNode = (*carPtr)->getHeadNode();
		Customer rearNode = (*carPtr)->getRearNode();
		int carIndex = (*carPtr)->getCarIndex();
		float capacity = (*carPtr)->getCapacity();
		Car* newCar = new Car(headNode, rearNode, capacity, carIndex);
		vector<Customer*> custVec = (*carPtr)->getAllCustomer();
		for(custPtr = custVec.begin(); custPtr < custVec.end(); custPtr++) {
			(*custPtr)->priority = 1;
			newCar->insertAtRear(**custPtr);
		}
		deleteCustomerSet(custVec);
		this->originPlan.push_back(newCar);
	}
};

SSALNS::~SSALNS() {}

void SSALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	// maxd, maxt, maxquantity: ��һ��ֵ
	// ÿ��ѭ���Ƴ� y^p*|L|���˿ͣ�LΪ·����ʣ��ڵ㣬y��0-1֮��������

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
	selectedIndex = int(random(0,customerAmount));          // ���ѡȡһ���ڵ�
	selectedIndex = min(selectedIndex, customerAmount-1);   // avoid reaching the right side
	removedIndexset.push_back(selectedIndex);
	vector<int> indexsetInRoute(customerAmount-1);     // ��·���еĽڵ���±꼯��

	/////////////// check if there have customers //////////////
	if(customerAmount <= 0) {                                 //
		cout << "==============================" << endl;     //
		cout << "Currently no customers in plan" << endl;     //
		cout << "==============================" << endl;     //
	}                                                         //
	////////////////////////////////////////////////////////////

	set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
	while((int)removedIndexset.size() < q){  // Ҫ�Ƴ���һ��q���ڵ�
		vector<pair<float, int> > currentR(0);        // ��ǰҪ������������ƾ�����������ֻ��������·���еĽڵ�
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin(); // ��·���еĽڵ�ĸ���
		vector<pair<float, int> >::iterator iter1;
		for(i=0; i<indexInRouteLen; i++){
			int index = indexsetInRoute[i];
			currentR.push_back(R[selectedIndex*customerAmount + index]);
		}
		sort(currentR.begin(), currentR.end(), ascendSort<float, int>);  // �����԰�С�����������
		float y = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		int indexsetInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // indexsetInRoute�ĳ���
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

	///////////////////////// check repeated elements ///////////////////////
	sort(removedIndexset.begin(), removedIndexset.end());                  //
	if (removedIndexset.size() != 0) {                                     //
		vector<int>::iterator iter99, iter100;                             //
		iter99 = removedIndexset.begin();                                  //
		iter100 = iter99 + 1;                                              //
		for (; iter100 < removedIndexset.end(); iter99++, iter100++) {     //
			if (*iter99 == *iter100) {                                     //
				cout << endl;                                              //
				cout << "========================" << endl;                //
				cout << "Catch repeated elements!" << endl;                //
				cout << "========================" << endl;                //
				cout << endl;                                              //
				break;                                                     //
			}                                                              //
		}                                                                  //
	}                                                                      //
	/////////////////////////////////////////////////////////////////////////

	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
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

	/////////////// check if there have customers //////////////
	if(customerAmount <= 0) {                                 //
		cout << "==============================" << endl;     //
		cout << "Currently no customers in plan" << endl;     //
		cout << "==============================" << endl;     //
	}                                                         //
	////////////////////////////////////////////////////////////

	indexsetInRoute = allIndex;
	for(i=0; i<q; i++){
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // ����·���еĽڵ����
		int selectedIndex = int(random(0, indexInRouteLen));     // ��indexsetInRoute�е�����
		selectedIndex = min(selectedIndex, indexInRouteLen-1);   // avoid reaching the right side
		removedIndexset.push_back(indexsetInRoute[selectedIndex]);
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}

	///////////////////////// check repeated elements ///////////////////////
	sort(removedIndexset.begin(), removedIndexset.end());                  //
	if (removedIndexset.size() != 0) {                                     //
		vector<int>::iterator iter99, iter100;                             //
		iter99 = removedIndexset.begin();                                  //
		iter100 = iter99 + 1;                                              //
		for (; iter100 < removedIndexset.end(); iter99++, iter100++) {     //
			if (*iter99 == *iter100) {                                     //
				cout << endl;                                              //
				cout << "========================" << endl;                //
				cout << "Catch repeated elements!" << endl;                //
				cout << "========================" << endl;                //
				cout << endl;                                              //
				break;                                                     //
			}                                                              //
		}                                                                  //
	}                                                                      //
	/////////////////////////////////////////////////////////////////////////

	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float DTpara[]){
	// originCarSet: δִ��remove����ǰ�Ļ�������
	// removedCarSet: ִ��remove������Ļ�������
	// removedCustomer: ���Ƴ��Ĺ˿ͽڵ�
	// q: ������Ҫremove�Ĺ˿�����
	// p: ����remove�����������
	int i;
	vector<int> customerNum(0);       // ���������˿ͽڵ���Ŀ 
	vector<Customer*> allCustomerInOrder(0);  // ���й˿ͽڵ�
	vector<int> allIndex(0);             // 0~customerAmount-1��Ϊ���е�customer���λ��
	vector<int> indexsetInRoute(0);	  // ����·���еĽڵ��±�
	vector<int> removedIndexset(0);    // ���Ƴ��Ľڵ��±�
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // originCarSet�еĹ˿�����
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}

	//////////////// check if there have customers /////////////
	if(customerAmount <= 0) {                                 //
		cout << "==============================" << endl;     //
		cout << "Currently no customers in plan" << endl;     //
		cout << "==============================" << endl;     //
	}                                                         //
	////////////////////////////////////////////////////////////

	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int> > reducedCost(customerAmount);  // ���ڵ���Ƴ�����	
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, DTpara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort<float, int>);   // ��������
		float y = rand()/(RAND_MAX+1.0f);  // ����0-1֮��������
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max((float)floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		for(i=0; i<removedNum; i++) {
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;   // ��������������
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}

	///////////////////// check repeated elements ////////////////////////////
	sort(removedIndexset.begin(), removedIndexset.end());                   //
	if (removedIndexset.size() != 0) {                                      //
		vector<int>::iterator iter99, iter100;                              //
		iter99 = removedIndexset.begin();                                   //
		iter100 = iter99 + 1;                                               //
		for (; iter100 < removedIndexset.end(); iter99++, iter100++) {      //
			if (*iter99 == *iter100) {                                      //
				cout << endl;                                               //
				cout << "========================" << endl;                 //
				cout << "Catch repeated elements!" << endl;                 //
				cout << "========================" << endl;                 //
				cout << endl;                                               //
				break;                                                      //
			}                                                               //
		}                                                                   //
	}                                                                       //
	//////////////////////////////////////////////////////////////////////////

	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, float DTpara[]){
	// ��removedCustomer���뵽removedCarSet��
	// ����ǰ�����޷��������е�removedCustomer�����½�artificial car
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ������Ŀ

	////////////////////// check empty car ////////////////////////
	if (carNum == 0) {                                           //
		cout << endl;                                            //
		cout << "============================" << endl;          //
		cout << "why carNum is equal to 0??" << endl;            //
		cout << "============================" << endl;          //
		cout << endl;                                            //
	}                                                            //
	///////////////////////////////////////////////////////////////

	int newCarIndex = removedCarSet[carNum-1]->getCarIndex()+1;  // �³�����ʼ���
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��,1,2,3,...
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����removedCustomer
	vector<pair<float, pair<int,int> > > minInsertPerRestCust(0);  // ����removedcustomer����С�������
	                                                             // ֻ����û�в��뵽·���еĽڵ�
	                                                             // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ���λ��
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // ÿ��ʹ��֮ǰ�����
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // ֻ��������·���еĽڵ�
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort<float, pair<int,int> >);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // ��ѡ�еĹ˿ͽڵ���
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // ����ҵ��˿��в���λ��
			int selectedCarPos = minInsertPerRestCust[0].second.second;  // ��ѡ�еĳ���λ��
			removedCarSet[selectedCarPos]->insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		} else {  // û�п��в���λ�ã������¿�һ������
			int selectedCarPos = carNum++;  // ��ѡ�еĳ���λ��
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // ��ӵ�����������
			alreadyInsertIndex.push_back(selectedCustIndex); // ����selectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceҪ��������
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ����restCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // ����һ��
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		}
	}
}

void SSALNS::regretInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, float DTpara[]){
	// ��removedCustomer���뵽removedCarSet��
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ��Ҫ���뵽·���еĽڵ���Ŀ
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ������Ŀ

	////////////////////// check empty car ////////////////////////
	if(carNum == 0) {                                            //
		cout << endl;                                            //
		cout << "============================" << endl;          //
		cout << "why carNum is equal to 0??" << endl;            //
		cout << "============================" << endl;          //
		cout << endl;                                            //
	}                                                            //
	///////////////////////////////////////////////////////////////

	int newCarIndex = removedCarSet[carNum - 1]->getCarIndex();  // �³����
	int i;
	vector<int> alreadyInsertIndex(0);		   // �Ѿ����뵽·���еĽڵ��±꣬�����allIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ��ÿ��·���е���С������۾��������꣺�����������꣺�˿ͣ�
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ��ÿ��·���е���С�����������Ӧ�Ľڵ�
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ��ÿ��·���еĴ�С������۾���
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ��ÿ��·���еĴ�С�����������Ӧ�Ľڵ�
	vector<int> allIndex(0);   // ��removedCustomer���б��
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	vector<int> restCustomerIndex = allIndex;  // ʣ��û�в��뵽·���еĽڵ��±꣬�����removedCustomer
	vector<pair<float, pair<int,int> > > regretdiffPerRestCust(0);  // ����removedcustomer����С����������С�������֮��
	                                                              // ֻ����û�в��뵽·���еĽڵ�
	                                                              // ��һ�������ǽڵ��±꣬�ڶ����ڵ��ǳ����±�
	
	
	///////////////// check repeated elements //////////////////////
	sort(allIndex.begin(), allIndex.end());                       //
	if (allIndex.size() != 0) {                                   //
		vector<int>::iterator iter99, iter100;                    //
		iter99 = allIndex.begin();                                //
		iter100 = iter99 + 1;                                     //
		for (; iter100 < allIndex.end(); iter99++, iter100++) {   //
			if (*iter99 == *iter100) {                            //
				cout << endl;                                     //
				cout << "========================" << endl;       //
				cout << "Catch repeated elements!" << endl;       //
				cout << "========================" << endl;       //
				cout << endl;                                     //
				break;                                            //
			}                                                     //
		}                                                         //
	}                                                             //
	////////////////////////////////////////////////////////////////

	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // ѡ�еĹ˿ͽڵ���
		int selectedCarPos;      // ѡ�еĻ���λ��
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
			selectedCarPos= carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
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
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);	
		} else {
			// ���򣬲���Ҫ�����³�
			selectedCarPos = regretdiffPerRestCust[0].second.second;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			alreadyInsertIndex.push_back(selectedCustIndex);
			removedCarSet[selectedCarPos]->insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin());
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		}
	}
}

bool judgeFeasible(vector<Car*> carSet, int &infeasibleNum) {
	// �ж�carSet�Ƿ����
	bool mark = true;
	vector<Car*>::iterator carIter;
	infeasibleNum = 0;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {

		////////////////////// check null pointer ///////////////////////
		if ((*carIter) == NULL) {                                      //                         
			cout << endl;                                              //
			cout << "==========================" << endl;              //
			cout << "Null cars are mixed in the carSet" << endl;       //
			cout << "==========================" << endl;              //
			cout << endl;                                              //
		}                                                              //
		/////////////////////////////////////////////////////////////////

		if((*carIter)->judgeArtificial() == true) {
			// �ж�artificial�����Ƿ���priorityΪ1�Ľڵ�
			vector<Customer*> tempCust = (*carIter)->getAllCustomer();
			for(vector<Customer*>::iterator custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
				if((*custIter)->priority == 1) {
					mark = false;
					infeasibleNum++;
				}
			}
			deleteCustomerSet(tempCust);
		}
	}
	return mark;
}


void SSALNS::run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck){  // �����㷨���൱���㷨��main()����
	int i;
	int PR1NUM = getCustomerNum(originPlan);
	int PR2NUM = (int)waitCustomer.size();
	int customerAmount = PR1NUM + PR2NUM;  // �ܵĹ˿���
	int originCarNum = (int)originPlan.size();   // ��ʼӵ�еĻ�������
	vector<Customer*>::iterator custPtr;
	vector<Car*>::iterator carIter;

	// �ѵ�ǰӵ�е����й˿ͽڵ�ŵ�allCustomer��
	vector<Customer*> allCustomer;         // ���е�customer
	allCustomer.reserve(customerAmount);   // ΪallCustomerԤ���ռ�
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		vector<Customer*> temp = (*carIter)->getAllCustomer();
		vector<Customer*>::iterator tempIter;
		for(tempIter = temp.begin(); tempIter < temp.end(); tempIter++) {
			allCustomer.push_back(*tempIter);
		}
	}
	for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++) {
		Customer *temp = new Customer(**custPtr);
		allCustomer.push_back(temp);
	}

	///////////////// check if there are no customers ///////////////
	if (allCustomer.size() == 0) {                                 //
		cout << endl;                                              //
		cout << "========================" << endl;                //
		cout << "In replan, but no customers!" << endl;            //
		cout << "========================" << endl;                //
		cout << endl;                                              //
	}                                                              //
	/////////////////////////////////////////////////////////////////

	// �������DT�����η���vector DTpara��
	float DTH1, DTH2, DTL1, DTL2;
	float maxd, mind, maxquantity, distToDepot;    // �ڵ�֮������/��С�����Լ��ڵ��������������
	computeMax(allCustomer, maxd, mind, maxquantity);
	distToDepot = 0;    // �����˿ͽڵ㵽�ֿ�ľ���
	for(custPtr = allCustomer.begin(); custPtr < allCustomer.end(); custPtr++) {
		distToDepot += sqrt(pow((*custPtr)->x - depot.x, 2) + pow((*custPtr)->y - depot.y, 2));
	}
	DTL2 = 50;
	DTL1 = 2*maxd + 1;
	DTH2 = 80;
	float tempsigma1 = 2*maxd + DTH2;
	//float tempsigma2 = 2*(PR1NUM + PR2NUM + PR3NUM) * maxd + PR2NUM * DT22 + PR3NUM * DT32 - 
	//	(PR1NUM + PR2NUM + PR3NUM) * mind + PR2NUM * DT21 + PR3NUM * DT31 - DT12;
	float tempsigma2 = 2*distToDepot - DTH2 + PR2NUM * (DTL1 + DTL2) - (PR1NUM + PR2NUM + 1) * mind;
	DTH1 = max(tempsigma1, tempsigma2) + 1;

	float DTpara[4];
	DTpara[0] = DTH1;
	DTpara[1] = DTH2;
	DTpara[2] = DTL1;
	DTpara[3] = DTL2;

	// ����base solution
	vector<Car*> baseCarSet = copyPlan(originPlan);
	if (waitCustomer.size() != 0) {       // ֻ�е�waitCustomer��Ϊ��ʱ����"replan"�ļ�ֵ
		vector<Car*> tempCarSet1;
		Car *tcar = new Car(depot, depot, capacity, 100, true);
		tempCarSet1.push_back(tcar);    // artificial carset, ��Ŵ�����Ĺ˿ͽڵ�
		greedyInsert(tempCarSet1, waitCustomer, 0, false, DTpara);
		for (carIter = tempCarSet1.begin(); carIter < tempCarSet1.end(); carIter++) {
			Car *tcar = new Car(**carIter);
			baseCarSet.push_back(tcar);
		}
		withdrawPlan(tempCarSet1);
	}
	float baseCost = getCost(baseCarSet, DTpara);   // ��׼���ۣ�����õ��Ľ���������⣬��һ������
	                                                // һ����˵����������Ľ��ǲ����е�

	vector<Car*> currentCarSet(0);
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		// ����ԭ�еĳ�������¼������Լ��յ��Լ�ʣ����������׼ʱ��
		Car *newCar = new Car((*carIter)->getNullCar());
		currentCarSet.push_back(newCar);
	}
	greedyInsert(currentCarSet, allCustomer, 0, false, DTpara);  // �Ե�ǰ��ӵ�еĳ���Ϊ�����������ʼ�⣨��ȫ���¹��죩
	vector<Car*> globalCarSet = copyPlan(currentCarSet);         // ȫ�����Ž⣬��ʼ���뵱ǰ����ͬ
	float currentCost = getCost(currentCarSet, DTpara);
	float globalCost = currentCost;

	vector<size_t> hashTable(0);  // ��ϣ��
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
	setZero<int>(removeFreq, removeNum);
	setZero<int>(insertFreq, insertNum);
	setZero<int>(noiseFreq, 2);
	int removeScore[removeNum];     // ����remove heuristic�ĵ÷�
	int insertScore[insertNum];     // ����insert heuristic�ĵ÷�
	int noiseScore[2];              // �����÷�
	setZero<int>(removeScore, removeNum);
	setZero<int>(insertScore, insertNum);
	setZero<int>(noiseScore, 2);
	// ����÷��趨
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight��������

	// ������Ĳ���
	int segment = 100;   // ÿ��һ��segment����removeProb, removeWeight�Ȳ���
	float w = 0.05f;      // ��ʼ�¶��趨�йز���
	float T = w * abs(currentCost) / (float)log(2);   // ��ʼ�¶�
	int p = 6;           // ����shawRemoval�����
	int pworst = 3;      // ����worstRemoval�������    
	float ksi = 0.8f;    // ÿ���Ƴ������ڵ���Ŀռ�ܽڵ����ı���
	float eta = 0.025f;   // ����ϵ��
	float noiseAmount = eta * maxd;   // ������
	float c = 0.9998f;    // ��������
	vector<Customer*> removedCustomer(0);    // ���Ƴ��Ľڵ�
	vector<Car*> tempCarSet = copyPlan(currentCarSet);      // ��ʱ��ŵ�ǰ��

	pair<bool, int> removalSelectTrend = make_pair(false, 0);
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // �µ�segment��ʼ
			// cout << "...............Segement:" << (int)floor(iter/segment)+1 << "................" << endl;
			// cout << "base cost is: " << baseCost << endl;
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
				setZero<int>(removeFreq, removeNum);
				setZero<int>(removeScore, removeNum);
				setZero<int>(insertFreq, insertNum);
				setZero<int>(insertScore, insertNum);
				setZero<int>(noiseFreq, 2);
				setZero<int>(noiseScore, 2);
			}
		}

		// ���������ѡȡremove heuristic��insert heuristic
		// �Ը���ѡ��remove heuristic
		int removeIndex;
		float sumation;
		if(removalSelectTrend.first == false) {
			float removeSelection = random(0,1);  // ����0-1֮��������
			sumation = removeProb[0];
			removeIndex = 0;    // remove heuristic���
			while(sumation < removeSelection){
				sumation += removeProb[++removeIndex];
			}
			removeIndex = min(removeIndex, removeNum-1);  // avoid reaching the right side
		}
		else{
			removeIndex = removalSelectTrend.second;
		}
		removalSelectTrend.first = false;
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
		//removeIndex = 1;
		//insertIndex = 0;
		////////////////////////////////////////////

		// ��Ӧ����ʹ�ô�����һ
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;

		// decide the number to remove
        int currentRemoveNum;   
		int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // ����Ƴ���ô��ڵ�
		int minRemoveNum = 4;  // �����Ƴ���ô��ڵ�
		currentRemoveNum = (int)floor(random(minRemoveNum, maxRemoveNum));  // ��ǰҪ�Ƴ��Ľڵ���Ŀ 
    
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
				worstRemoval(tempCarSet, removedCustomer, currentRemoveNum, pworst, DTpara);
				break;
			}
		}
		// ִ��insert heuristic
		switch(insertIndex) {
		case 0:
			{
				greedyInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		case 1:
			{
				regretInsert(tempCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara);
				break;
			}
		}

		// ********** check what symptom is while "getCustomerNum(tempCarSet)!=customerAmount" ******** //
		// if (getCustomerNum(tempCarSet) != customerAmount) {
		// 	print_lck.lock();
		// 	cout << "The remove heuristic is:" << removeIndex << endl;
		// 	cout << "The insert heuristic is:" << insertIndex << endl;
		// 	cout << "The customer retains in the plan is:" << endl;
		// 	showAllCustomer(tempCarSet);
		// 	cout << "The customer in pool originally is:" << endl;
		// 	int count = 0;
		// 	for (custPtr = allCustomer.begin(); custPtr < allCustomer.end(); custPtr++) {
		// 		if (count % 8 == 0) {
		// 			cout << endl;
		// 		}
		// 		cout << (*custPtr)->id << '\t';
		// 		count++;
		// 	}
		// 	cout << endl;
		// 	cout << "removed customer set is: " << endl;
		// 	count = 0;
		// 	for (custPtr = removedCustomer.begin(); custPtr < removedCustomer.end(); custPtr++) {
		// 		if (count % 8 == 0) {
		// 			cout << endl;
		// 		}
		// 		cout << (*custPtr)->id << '\t';
		// 		count++;
		// 	}
		// 	cout << endl;
		// 	print_lck.unlock();
		// }
		// ******************************************************************************************** //

		assert(getCustomerNum(tempCarSet) == customerAmount);
		// �Ƴ���·��
		removeNullRoute(tempCarSet, true);

		// ʹ��ģ���˻��㷨�����Ƿ���ոý�
		bool accept = false;
		float newCost = getCost(tempCarSet, DTpara);
		float acceptProb = exp(-(newCost - currentCost)/T);
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
			ksi = 0.4f;    // reduce the interuption of the solution
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
					ksi = 0.4f;  // ����õ��˸��õĽ⣬����ٶԵ�ǰ����Ŷ�
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {      
					if(accept == true) {       // ���3
                        if(newCost > baseCost) {
						    ksi = 0.8f;   // ����õ��˸���Ľ⣬�����ӶԵ�ǰ����Ŷ�
							removalSelectTrend.first = true;   // ��ʱǿ��ʹ��random removal���ƻ���ǰ�Ľ�
							removalSelectTrend.second = 1;     // random removal
						}
						else {
							ksi = 0.6f;   // using medium interuption factor
						}
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    // ����������ˣ������currentCarSet�� ����tempCarSet����
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // ��������գ�����µ�ǰ��
			withdrawPlan(currentCarSet);
			currentCarSet = copyPlan(tempCarSet);
		} else {    // ����tempCarSet�ָ�ΪcurrentCarSet
			withdrawPlan(tempCarSet);
			tempCarSet = copyPlan(currentCarSet);
		}
	}

	withdrawPlan(finalCarSet);
	finalCarSet.reserve(originPlan.size());
	ostringstream ostr;
	ostr.str("");
    print_lck.lock();
	// unique_lock<mutex> lck(print_lck);
    int infeasibleNum;

	if(judgeFeasible(globalCarSet, infeasibleNum) == false) {
		// ��������������õĽ⣬��ά��ԭ���Ľ�
		ostr << "SSALNS: we should use the origin plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		print_lck.unlock();
		finalCarSet = copyPlan(originPlan);
	} else {
		ostr << "SSALNS: we will use the new plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		print_lck.unlock();
		for (carIter = globalCarSet.begin(); carIter < globalCarSet.end(); carIter++) {
			if ((*carIter)->judgeArtificial() == false) {
				Car *tempCar = new Car(**carIter);
				finalCarSet.push_back(tempCar);
			}
		}
	}

	//int infeasibleNum;
	//bool mark2 = judgeFeasible(finalCarSet, infeasibleNum);
	//cout << "Whether the solution is feasible? " << mark2 << endl;

	finalCost = globalCost;
	deleteCustomerSet(waitCustomer);
	deleteCustomerSet(allCustomer);
	withdrawPlan(originPlan);
	withdrawPlan(baseCarSet);
	withdrawPlan(tempCarSet);
	withdrawPlan(globalCarSet);
	hashTable.clear();
}


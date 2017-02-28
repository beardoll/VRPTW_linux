#include "SSALNS.h"
#include "LNS_rel.h"
#include "PublicFunction.h"
#include<algorithm>
#include<cassert>
#include "Matrix.h"
#include<functional>
#include<ctime>
#include "TxtRecorder.h"
#include<cmath>

using namespace std;

SSALNS::SSALNS(vector<Customer*> waitCustomer, vector<Car*> originPlan, float capacity, int maxIter){
	// ¸ø¸÷¹Ë¿Í¸³ÓèÓÅÏÈ¼¶
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
		this->originPlan.push_back(newCar);
	}
};

SSALNS::~SSALNS() {}

void SSALNS::shawRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer,
							int q, int p, float maxd, float maxt, float maxquantity){
	// originCarSet: Î´Ö´ĞĞremove²Ù×÷Ç°µÄ»õ³µ¼¯ºÏ
	// removedCarSet: Ö´ĞĞremove²Ù×÷ºóµÄ»õ³µ¼¯ºÏ
	// removedCustomer: ±»ÒÆ³ıµÄ¹Ë¿Í½Úµã
	// q: ±¾´ÎĞèÒªremoveµÄ¹Ë¿ÍÊıÁ¿
	// p: Ôö¼Óremove²Ù×÷µÄËæ»úĞÔ
	// maxd, maxt, maxquantity: ¹éÒ»»¯Öµ
	// Ã¿´ÎÑ­»·ÒÆ³ı y^p*|L|¸ö¹Ë¿Í£¬LÎªÂ·¾¶ÖĞÊ£Óà½Úµã£¬yÊÇ0-1Ö®¼äµÄËæ»úÊı

	int phi = 9;
	int kai = 3;
	int psi = 2;
	int carAmount = originCarSet.end()-originCarSet.begin();  // »õ³µÊıÁ¿
	vector<int> customerNum(0);       // ¸÷Á¾³µËù¸ºÔğµÄ¹Ë¿Í½ÚµãÊıÄ¿
	vector<Customer*> allCustomerInOrder(0);
	int i,j;
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);   
	// »ñÈ¡ËùÓĞµÄCustomer, customerNum´æ·Å¸÷Á¾³µËùÓµÓĞµÄ¹Ë¿Í½ÚµãÊıÄ¿£¬ allCustomer´æ·ÅËùÓĞ¹Ë¿Í½Úµã
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // ¹Ë¿ÍÊıÁ¿
	vector<pair<float, int> > R(customerAmount*customerAmount);     // ÏàËÆ¾ØÕó
	float temp1;
	vector<int> allIndex(customerAmount);  // 0~customerAmount-1
	for(i=0; i<(int)originCarSet.size(); i++){
		originCarSet[i]->getRoute().refreshArrivedTime();
	}
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
				R[i*customerAmount+j].first = temp1;   // iĞĞjÁĞ
				R[i*customerAmount+j].second = j;
				R[j*customerAmount+i].first = temp1;
				R[j*customerAmount+i].second = i;      // jĞĞiÁĞ
			}
		}
	}
	int selectedIndex;           // ±»Ñ¡ÖĞµÄ½ÚµãÔÚallCustomerÖĞµÄÏÂ±ê
	vector<int> removedIndexset; // ËùÓĞ±»ÒÆ³ıµÄ½ÚµãµÄÏÂ±ê¼¯ºÏ
	selectedIndex = int(random(0,customerAmount));   // Ëæ»úÑ¡È¡Ò»¸ö½Úµã
	removedIndexset.push_back(selectedIndex);
	vector<int> indexsetInRoute(customerAmount-1);     // ÔÚÂ·¾¶ÖĞµÄ½ÚµãµÄÏÂ±ê¼¯ºÏ
	set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
	while((int)removedIndexset.size() < q){  // ÒªÒÆ³ıµôÒ»¹²q¸ö½Úµã
		vector<pair<float, int> > currentR(0);        // µ±Ç°Òª½øĞĞÅÅĞòµÄÏàËÆ¾ØÕó£¨ÏòÁ¿£©£¬Ö»°üº¬ÉĞÔÚÂ·¾¶ÖĞµÄ½Úµã
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin(); // ÔÚÂ·¾¶ÖĞµÄ½ÚµãµÄ¸öÊı
		vector<pair<float, int> >::iterator iter1;
		for(i=0; i<indexInRouteLen; i++){
			int index = indexsetInRoute[i];
			currentR.push_back(R[selectedIndex*customerAmount + index]);
		}
		sort(currentR.begin(), currentR.end(), ascendSort<float, int>);  // ÏàËÆĞÔ°´Ğ¡µ½´ó½øĞĞÅÅĞò
		float y = rand()/(RAND_MAX+1.0f);  // ²úÉú0-1Ö®¼äµÄËæ»úÊı
		int indexsetInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // indexsetInRouteµÄ³¤¶È
		int removeNum = max((int)floor(pow(y,p)*indexsetInRouteLen), 1);             // ±¾´ÎÒÆ³ıµÄ½ÚµãÊıÄ¿
		for(i=0; i<removeNum ; i++){
			removedIndexset.push_back(currentR[i].second);
		}
		int indexRemovedLen = removedIndexset.end() - removedIndexset.begin();  // µ±Ç°removedIndexsetµÄ³¤¶È
		int randint = (int)random(0,indexRemovedLen);  // ²úÉúÒ»¸ö0-indexRemovedLenµÄËæ»úÊı
		selectedIndex = removedIndexset[randint];
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::randomRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q){
	// originCarSet: Î´Ö´ĞĞremove²Ù×÷Ç°µÄ»õ³µ¼¯ºÏ
	// removedCarSet: Ö´ĞĞremove²Ù×÷ºóµÄ»õ³µ¼¯ºÏ
	// removedCustomer: ±»ÒÆ³ıµÄ¹Ë¿Í½Úµã
	// q: ±¾´ÎĞèÒªremoveµÄ¹Ë¿ÍÊıÁ¿
	int i;
	vector<int> customerNum(0);       // ¸÷Á¾»õ³µ¹Ë¿Í½ÚµãÊıÄ¿ 
	vector<Customer*> allCustomerInOrder(0);  // ËùÓĞ¹Ë¿Í½Úµã
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
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();  // ÉĞÔÚÂ·¾¶ÖĞµÄ½Úµã¸öÊı
		int selectedIndex = int(random(0, indexInRouteLen));  // ÔÚindexsetInRouteÖĞµÄË÷Òı
		removedIndexset.push_back(indexsetInRoute[selectedIndex]);
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::worstRemoval(vector<Car*> &originCarSet, vector<Customer*> &removedCustomer, int q, int p, float DTpara[]){
	// originCarSet: Î´Ö´ĞĞremove²Ù×÷Ç°µÄ»õ³µ¼¯ºÏ
	// removedCarSet: Ö´ĞĞremove²Ù×÷ºóµÄ»õ³µ¼¯ºÏ
	// removedCustomer: ±»ÒÆ³ıµÄ¹Ë¿Í½Úµã
	// q: ±¾´ÎĞèÒªremoveµÄ¹Ë¿ÍÊıÁ¿
	// p: Ôö¼Óremove²Ù×÷µÄËæ»úĞÔ
	int i;
	vector<int> customerNum(0);       // ¸÷Á¾»õ³µ¹Ë¿Í½ÚµãÊıÄ¿ 
	vector<Customer*> allCustomerInOrder(0);  // ËùÓĞ¹Ë¿Í½Úµã
	vector<int> allIndex(0);             // 0~customerAmount-1£¬ÎªËùÓĞµÄcustomer±ê¼ÇÎ»ÖÃ
	vector<int> indexsetInRoute(0);	  // ÉĞÔÚÂ·¾¶ÖĞµÄ½ÚµãÏÂ±ê
	vector<int> removedIndexset(0);    // ±»ÒÆ³ıµÄ½ÚµãÏÂ±ê
	getAllCustomerInOrder(originCarSet, customerNum, allCustomerInOrder);
	int customerAmount = allCustomerInOrder.end() - allCustomerInOrder.begin();  // originCarSetÖĞµÄ¹Ë¿ÍÊıÁ¿
	for(i=0; i<customerAmount; i++){
		allIndex.push_back(i);
	}
	indexsetInRoute = allIndex;
	while((int)removedIndexset.size() < q){
		vector<pair<float, int> > reducedCost(customerAmount);  // ¸÷½ÚµãµÄÒÆ³ı´ú¼Û	
		computeReducedCost(originCarSet, indexsetInRoute, removedIndexset, reducedCost, DTpara);
		sort(reducedCost.begin(), reducedCost.end(), ascendSort<float, int>);   // µİÔöÅÅĞò
		float y = rand()/(RAND_MAX+1.0f);  // ²úÉú0-1Ö®¼äµÄËæ»úÊı
		int indexInRouteLen = indexsetInRoute.end() - indexsetInRoute.begin();
		int removedNum = static_cast<int>(max((float)floor(pow(y,p)*indexInRouteLen), 1.0f));
		assert(removedNum <= indexInRouteLen);
		//vector<int> selectedIndex = probSelection(reducedCost, removedNum);
		//for(vector<int>::iterator intIter = selectedIndex.begin(); intIter < selectedIndex.end(); intIter++){
		//	removedIndexset.push_back(reducedCost[*intIter].second);
		//}
		for(i=0; i<removedNum; i++) {
			removedIndexset.push_back(reducedCost[i].second);
		}
		sort(removedIndexset.begin(), removedIndexset.end());
		vector<int>::iterator iterINT;   // ÕûÊıÏòÁ¿µü´úÆ÷
		iterINT = set_difference(allIndex.begin(), allIndex.end(), removedIndexset.begin(), removedIndexset.end(), indexsetInRoute.begin());
		indexsetInRoute.resize(iterINT - indexsetInRoute.begin());
	}
	deleteCustomer(removedIndexset, customerNum, allCustomerInOrder, originCarSet, removedCustomer);
	deleteCustomerSet(allCustomerInOrder);
}

void SSALNS::greedyInsert(vector<Car*> &removedCarSet, vector<Customer*> removedCustomer,
							 float noiseAmount, bool noiseAdd, float DTpara[]){
	// °ÑremovedCustomer²åÈëµ½removedCarSetÖĞ
	// Èôµ±Ç°»õ³µÎŞ·¨ÈİÄÉËùÓĞµÄremovedCustomer£¬ÔòĞÂ½¨artificial car
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ĞèÒª²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÊıÄ¿
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ³µÁ¾ÊıÄ¿
	int newCarIndex = removedCarSet[carNum-1]->getCarIndex()+1;  // ĞÂ³µµÄÆğÊ¼±êºÅ
	int i;
	vector<int> alreadyInsertIndex(0);		   // ÒÑ¾­²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÏÂ±ê£¬Ïà¶ÔÓÚallIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ×îĞ¡²åÈë´ú¼Û¾ØÕó£¨ĞĞ×ø±ê£º³µÁ¾£¬ÁĞ×ø±ê£º¹Ë¿Í£©
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ×îĞ¡²åÈë´ú¼ÛËù¶ÔÓ¦µÄ½Úµã
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ´ÎĞ¡²åÈë´ú¼Û¾ØÕó
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ´ÎĞ¡²åÈë´ú¼ÛËù¶ÔÓ¦µÄ½Úµã
	vector<int> allIndex(0);   // ¶ÔremovedCustomer½øĞĞ±àºÅ,1,2,3,...
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	//if(removedCustomerNum > 56) {
	//	minInsertPerRoute.printMatrixAtCol(56);
	//}
	vector<int> restCustomerIndex = allIndex;  // Ê£ÏÂÃ»ÓĞ²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÏÂ±ê£¬Ïà¶ÔÓÚremovedCustomer
	vector<pair<float, pair<int,int> > > minInsertPerRestCust(0);  // ¸÷¸öremovedcustomerµÄ×îĞ¡²åÈë´ú¼Û
	                                                             // Ö»°üº¬Ã»ÓĞ²åÈëµ½Â·¾¶ÖĞµÄ½Úµã
	                                                             // µÚÒ»¸öÕûÊıÊÇ½ÚµãÏÂ±ê£¬µÚ¶ş¸ö½ÚµãÊÇ³µÁ¾Î»ÖÃ
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		minInsertPerRestCust.clear();  // Ã¿´ÎÊ¹ÓÃÖ®Ç°ÏÈÇå¿Õ
		for(i=0; i<(int)restCustomerIndex.size(); i++){               // Ö»¼ÆËãÉĞÔÚÂ·¾¶ÖĞµÄ½Úµã
			int index = restCustomerIndex[i];
			int pos;
			float minValue;
			minValue = minInsertPerRoute.getMinAtCol(index, pos);
			minInsertPerRestCust.push_back(make_pair(minValue, make_pair(index, pos)));
		}	
		sort(minInsertPerRestCust.begin(), minInsertPerRestCust.end(), ascendSort<float, pair<int,int> >);
		int selectedCustIndex = minInsertPerRestCust[0].second.first;  // ±»Ñ¡ÖĞµÄ¹Ë¿Í½Úµã±àºÅ
		if(minInsertPerRestCust[0].first != MAX_FLOAT){  // Èç¹ûÕÒµ½ÁË¿ÉĞĞ²åÈëÎ»ÖÃ
			int selectedCarPos = minInsertPerRestCust[0].second.second;  // ±»Ñ¡ÖĞµÄ³µÁ¾Î»ÖÃ
			removedCarSet[selectedCarPos]->insertAfter(minInsertPos.getElement(selectedCarPos, selectedCustIndex), *removedCustomer[selectedCustIndex]);
			alreadyInsertIndex.push_back(selectedCustIndex);
			vector<int>::iterator iterINT;
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceÒªÇóÏÈÅÅĞò
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ¸üĞÂrestCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);
		} else {  // Ã»ÓĞ¿ÉĞĞ²åÈëÎ»ÖÃ£¬ÔòÔÙĞÂ¿ªÒ»Á¾»õ³µ
			int selectedCarPos = carNum++;  // ±»Ñ¡ÖĞµÄ³µÁ¾Î»ÖÃ
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // Ìí¼Óµ½»õ³µ¼¯ºÏÖĞ
			alreadyInsertIndex.push_back(selectedCustIndex); // ¸üĞÂselectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());  // set_differenceÒªÇóÏÈÅÅĞò
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ¸üĞÂrestCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // Ôö¼ÓÒ»ĞĞ
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
	// °ÑremovedCustomer²åÈëµ½removedCarSetÖĞ
	int removedCustomerNum = removedCustomer.end() - removedCustomer.begin();  // ĞèÒª²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÊıÄ¿
	int carNum = removedCarSet.end() - removedCarSet.begin();    // ³µÁ¾ÊıÄ¿
	int newCarIndex = removedCarSet[carNum - 1]->getCarIndex();  // ĞÂ³µ±àºÅ
	int i;
	vector<int> alreadyInsertIndex(0);		   // ÒÑ¾­²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÏÂ±ê£¬Ïà¶ÔÓÚallIndex
	Matrix<float> minInsertPerRoute(carNum, removedCustomerNum);     // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ×îĞ¡²åÈë´ú¼Û¾ØÕó£¨ĞĞ×ø±ê£º³µÁ¾£¬ÁĞ×ø±ê£º¹Ë¿Í£©
	Matrix<Customer> minInsertPos(carNum, removedCustomerNum);       // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ×îĞ¡²åÈë´ú¼ÛËù¶ÔÓ¦µÄ½Úµã
	Matrix<float> secondInsertPerRoute(carNum, removedCustomerNum);  // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ´ÎĞ¡²åÈë´ú¼Û¾ØÕó
	Matrix<Customer> secondInsertPos(carNum, removedCustomerNum);    // ÔÚÃ¿ÌõÂ·¾¶ÖĞµÄ´ÎĞ¡²åÈë´ú¼ÛËù¶ÔÓ¦µÄ½Úµã
	vector<int> allIndex(0);   // ¶ÔremovedCustomer½øĞĞ±àºÅ
	generateMatrix(allIndex, removedCarSet, removedCustomer, minInsertPerRoute,  minInsertPos, secondInsertPerRoute, secondInsertPos, 
		noiseAmount, noiseAdd, DTpara, false);
	vector<int> restCustomerIndex = allIndex;  // Ê£ÏÂÃ»ÓĞ²åÈëµ½Â·¾¶ÖĞµÄ½ÚµãÏÂ±ê£¬Ïà¶ÔÓÚremovedCustomer
	vector<pair<float, pair<int,int> > > regretdiffPerRestCust(0);  // ¸÷¸öremovedcustomerµÄ×îĞ¡²åÈë´ú¼ÛÓë´ÎĞ¡²åÈë´ú¼ÛÖ®²î
	                                                              // Ö»°üº¬Ã»ÓĞ²åÈëµ½Â·¾¶ÖĞµÄ½Úµã
	                                                              // µÚÒ»¸öÕûÊıÊÇ½ÚµãÏÂ±ê£¬µÚ¶ş¸ö½ÚµãÊÇ³µÁ¾ÏÂ±ê
	while((int)alreadyInsertIndex.size() < removedCustomerNum){
		int selectedCustIndex;   // Ñ¡ÖĞµÄ¹Ë¿Í½Úµã±àºÅ
		int selectedCarPos;      // Ñ¡ÖĞµÄ»õ³µÎ»ÖÃ
		regretdiffPerRestCust.clear();
		for(i=0; i<(int)restCustomerIndex.size(); i++){
			int index = restCustomerIndex[i];        // ¹Ë¿Í½ÚµãÏÂ±ê
			float minValue, secondValue1, secondValue2;
			int pos1, pos2, pos3;
			minValue = minInsertPerRoute.getMinAtCol(index, pos1);          // ×îĞ¡²åÈë´ú¼Û
			minInsertPerRoute.setValue(pos1, index, MAX_FLOAT);
			secondValue1 = minInsertPerRoute.getMinAtCol(index, pos2);      // ºòÑ¡´ÎĞ¡²åÈë´ú¼Û
			minInsertPerRoute.setValue(pos1, index, minValue);              // »Ö¸´Êı¾İ
			secondValue2 = secondInsertPerRoute.getMinAtCol(index, pos3);   // ºòÑ¡´ÎĞ¡²åÈë´ú¼Û
			if(minValue == MAX_FLOAT){  
				// Èç¹û·¢ÏÖÄ³¸ö½ÚµãÒÑ¾­Ã»ÓĞ¿ÉĞĞ²åÈëµã£¬ÔòÓÅÏÈ°²ÅÅÖ®
				regretdiffPerRestCust.push_back(make_pair(MAX_FLOAT, make_pair(index, pos1)));
			} else if(minValue != MAX_FLOAT && secondValue1==MAX_FLOAT && secondValue2==MAX_FLOAT){
				// Èç¹ûÖ»ÓĞÒ»¸ö¿ÉĞĞ²åÈëµã£¬ÔòÓ¦¸ÃÓÅÏÈ°²ÅÅ
				// °´ÕÕminValueµÄÖµ£¬×îĞ¡ÕßÓ¦¸ÃÂÊÏÈ±»°²ÅÅ
				// Òò´Ëdiff = LARGE_FLOAT - minValue
				regretdiffPerRestCust.push_back(make_pair(LARGE_FLOAT-minValue, make_pair(index, pos1)));
			} else{
				if(secondValue1 <= secondValue2){
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue1), make_pair(index, pos1)));
				} else{
					regretdiffPerRestCust.push_back(make_pair(abs(minValue-secondValue2), make_pair(index, pos1)));
				}
			}
		}
		sort(regretdiffPerRestCust.begin(), regretdiffPerRestCust.end(), descendSort<float, pair<int, int> >);  // Ó¦¸ÃÓÉ´óµ½Ğ¡½øĞĞÅÅÁĞ
		if(regretdiffPerRestCust[0].first == MAX_FLOAT) {
			// Èç¹ûËùÓĞµÄ½Úµã¶¼Ã»ÓĞ¿ÉĞĞ²åÈëµã£¬Ôò¿ª±ÙĞÂ³µ
			selectedCarPos= carNum++;
			selectedCustIndex = regretdiffPerRestCust[0].second.first;
			Car *newCar = new Car(depot, depot, capacity, newCarIndex++, true);
			newCar->insertAtHead(*removedCustomer[selectedCustIndex]);
			removedCarSet.push_back(newCar);  // Ìí¼Óµ½»õ³µ¼¯ºÏÖĞ
			alreadyInsertIndex.push_back(selectedCustIndex); // ¸üĞÂselectedCustIndex
			sort(alreadyInsertIndex.begin(), alreadyInsertIndex.end());
			vector<int>::iterator iterINT;
			iterINT = set_difference(allIndex.begin(), allIndex.end(), alreadyInsertIndex.begin(), alreadyInsertIndex.end(), restCustomerIndex.begin()); // ¸üĞÂrestCustomerIndex
			restCustomerIndex.resize(iterINT-restCustomerIndex.begin());
			minInsertPerRoute.addOneRow();   // Ôö¼ÓÒ»ĞĞ
			minInsertPos.addOneRow();
			secondInsertPerRoute.addOneRow();
			secondInsertPos.addOneRow();
			updateMatrix(restCustomerIndex, minInsertPerRoute, minInsertPos, secondInsertPerRoute, secondInsertPos, 
				selectedCarPos, removedCarSet, removedCustomer, noiseAmount, noiseAdd, DTpara, false);	
		} else {
			// ·ñÔò£¬²»ĞèÒª¿ª±ÙĞÂ³µ
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
	// ÅĞ¶ÏcarSetÊÇ·ñ¿ÉĞĞ
	bool mark = true;
	vector<Car*>::iterator carIter;
	infeasibleNum = 0;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		if((*carIter)->judgeArtificial() == true) {
			// ÅĞ¶Ïartificial³µÉÏÊÇ·ñÓĞpriorityÎª1µÄ½Úµã
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


void SSALNS::run(vector<Car*> &finalCarSet, float &finalCost, mutex &print_lck){  // ÔËĞĞËã·¨£¬Ïàµ±ÓÚËã·¨µÄmain()º¯Êı
	int i;
	int PR1NUM = getCustomerNum(originPlan);
	int PR2NUM = (int)waitCustomer.size();
	int customerAmount = PR1NUM + PR2NUM;  // ×ÜµÄ¹Ë¿ÍÊı
	int originCarNum = (int)originPlan.size();   // ³õÊ¼ÓµÓĞµÄ»õ³µÊıÁ¿
	vector<Customer*>::iterator custPtr;
	vector<Car*>::iterator carIter;
	srand(time(0));

	// °Ñµ±Ç°ÓµÓĞµÄËùÓĞ¹Ë¿Í½Úµã·Åµ½allCustomerÖĞ
	vector<Customer*> allCustomer;         // ËùÓĞµÄcustomer
	allCustomer.reserve(customerAmount);   // ÎªallCustomerÔ¤Áô¿Õ¼ä
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		vector<Customer*> temp = (*carIter)->getAllCustomer();
		vector<Customer*>::iterator tempIter;
		for(tempIter = temp.begin(); tempIter < temp.end(); tempIter++) {
			allCustomer.push_back(*tempIter);
		}
	}
	for(custPtr = waitCustomer.begin(); custPtr < waitCustomer.end(); custPtr++) {
		allCustomer.push_back(*custPtr);
	}

	// ¼ÆËã±äÁ¿DT£¬ÒÀ´Î·ÅÈëvector DTparaÖĞ
	float DTH1, DTH2, DTL1, DTL2;
	float maxd, mind, maxquantity, distToDepot;    // ½ÚµãÖ®¼äµÄ×î´ó/×îĞ¡¾àÀëÒÔ¼°½ÚµãµÄ×î´ó»õÎïĞèÇóÁ¿
	computeMax(allCustomer, maxd, mind, maxquantity);
	distToDepot = 0;    // ¸÷¸ö¹Ë¿Í½Úµãµ½²Ö¿âµÄ¾àÀë
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

	// ÕâÊÇ¸ù¾İpriorityÎª1µÄ¹Ë¿Í²åÈëµ½artificial carÖĞµÄ¸öÊıÊ©¼ÓÏàÓ¦µÄ³Í·£
	float Delta = maxd*1.5;


	// ¹¹Ôìbase solution
	vector<Car*> tempCarSet1;
	Car *tcar = new Car(depot, depot, capacity, 100, true);
	tempCarSet1.push_back(tcar);    // artificial carset, ´æ·Å´ı·şÎñµÄ¹Ë¿Í½Úµã
	greedyInsert(tempCarSet1, waitCustomer, 0, false, DTpara); 
	vector<Car*> baseCarSet = copyPlan(originPlan);   
	for(carIter = tempCarSet1.begin(); carIter < tempCarSet1.end(); carIter++) {
		Car *tcar = new Car(**carIter);
		baseCarSet.push_back(tcar);
	}
	withdrawPlan(tempCarSet1);
	float baseCost = getCost(baseCarSet, DTpara);   // »ù×¼´ú¼Û£¬Èç¹ûµÃµ½µÄ½âÓÉÓÚÕâ¸ö½â£¬ÔòÒ»¶¨¿ÉĞĞ
	                                                 // Ò»°ãÀ´Ëµ±ÈÕâ¸ö½â¸ü²îµÄ½âÊÇ²»¿ÉĞĞµÄ
	vector<Car*> artificialCarSet(0);
	vector<Car*> currentCarSet(0);
	for(carIter = originPlan.begin(); carIter < originPlan.end(); carIter++) {
		// ±£ÁôÔ­ÓĞµÄ³µÁ¾£¬¼ÇÂ¼ÆäÆğµãÒÔ¼°ÖÕµãÒÔ¼°Ê£ÓàÈİÁ¿¡¢»ù×¼Ê±¼ä
		Car *newCar = new Car((*carIter)->getNullCar());
		currentCarSet.push_back(newCar);
	}
	greedyInsert(currentCarSet, allCustomer, 0, false, DTpara);  // ÒÔµ±Ç°ËùÓµÓĞµÄ³µÁ¾Îª»ù´¡£¬¹¹Ôì³õÊ¼½â£¨ÍêÈ«ÖØĞÂ¹¹Ôì£©
	vector<Car*> globalCarSet = copyPlan(currentCarSet);         // È«¾Ö×îÓÅ½â£¬³õÊ¼»¯Óëµ±Ç°½âÏàÍ¬
	float currentCost = getCost(currentCarSet, DTpara);
	float globalCost = currentCost;

	vector<size_t> hashTable(0);  // ¹şÏ£±í
	hashTable.push_back(codeForSolution(currentCarSet));

	// ÆÀ·Ö»úÖÆÏà¹Ø²ÎÊıµÄÉè¶¨
	const int removeNum = 3;    // remove heuristicµÄ¸öÊı
	const int insertNum = 2;    // insert heuristicµÄ¸öÊı
	float removeProb[removeNum];  // ¸÷¸öremove heuristicµÄ¸ÅÂÊ
	float insertProb[insertNum];  // ¸÷¸öinsert heuristicµÄ¸ÅÂÊ
	float noiseProb[2] = {0.5, 0.5};        // ÔëÉùÊ¹ÓÃµÄ¸ÅÂÊ
	for(i=0; i<removeNum; i++){
		removeProb[i] = 1.0f/removeNum;
	}
	for(i=0; i<insertNum; i++){
		insertProb[i] = 1.0f/insertNum;
	}
	float removeWeight[removeNum];  // ¸÷¸öremove heuristicµÄÈ¨ÖØ
	float insertWeight[insertNum];  // ¸÷¸öinsert heuristicµÄÈ¨ÖØ
	float noiseWeight[2];   // ¼ÓÔëÉù/²»¼ÓÔëÉù ·Ö±ğµÄÈ¨ÖØ
	setOne(removeWeight, removeNum);
	setOne(insertWeight, insertNum);
	setOne(noiseWeight, 2);
	int removeFreq[removeNum];      // ¸÷¸öremove heuristicÊ¹ÓÃµÄÆµÂÊ
	int insertFreq[insertNum];      // ¸÷¸öinsert heuristicÊ¹ÓÃµÄÆµÂÊ
	int noiseFreq[2];               // ÔëÉùÊ¹ÓÃµÄÆµÂÊ£¬µÚÒ»¸öÊÇwith noise£¬µÚ¶ş¸öÊÇwithout noise
	setZero<int>(removeFreq, removeNum);
	setZero<int>(insertFreq, insertNum);
	setZero<int>(noiseFreq, 2);
	int removeScore[removeNum];     // ¸÷¸öremove heuristicµÄµÃ·Ö
	int insertScore[insertNum];     // ¸÷¸öinsert heuristicµÄµÃ·Ö
	int noiseScore[2];              // ÔëÉùµÃ·Ö
	setZero<int>(removeScore, removeNum);
	setZero<int>(insertScore, insertNum);
	setZero<int>(noiseScore, 2);
	// ÈıÏîµÃ·ÖÉè¶¨
	int sigma1 = 33;
	int sigma2 = 9;
	int sigma3 = 13;
	float r = 0.1f;       // weight¸üĞÂËÙÂÊ

	// ÆäÓàºËĞÄ²ÎÊı
	// int maxIter = 10000; // ×ÜµÄµü´ú´ÎÊı
	int segment = 100;   // Ã¿¸ôÒ»¸ösegment¸üĞÂremoveProb, removeWeightµÈ²ÎÊı
	float w = 0.05f;      // ³õÊ¼ÎÂ¶ÈÉè¶¨ÓĞ¹Ø²ÎÊı
	float T = w * abs(currentCost) / (float)log(2);   // ³õÊ¼ÎÂ¶È
	int p = 6;           // Ôö¼ÓshawRemovalËæ»úĞÔ
	int pworst = 3;      // Ôö¼ÓworstRemovalµÄËæ»úĞÔ    
	float ksi = 0.8f;    // Ã¿´ÎÒÆ³ıµÄ×î´ó½ÚµãÊıÄ¿Õ¼×Ü½ÚµãÊıµÄ±ÈÀı
	float eta = 0.025f;   // ÔëÉùÏµÊı
	float noiseAmount = eta * maxd;   // ÔëÉùÁ¿
	float c = 0.9998f;    // ½µÎÂËÙÂÊ
	vector<Customer*> removedCustomer(0);    // ±»ÒÆ³ıµÄ½Úµã
	vector<Car*> tempCarSet = copyPlan(currentCarSet);      // ÔİÊ±´æ·Åµ±Ç°½â
    bool destroy = false;   // ¿true¿¿¿¿¿¿¿¿¿¿¿¿

	pair<bool, int> removalSelectTrend = make_pair(false, 0);
	for(int iter=0; iter<maxIter; iter++){
		if(iter%segment == 0){  // ĞÂµÄsegment¿ªÊ¼
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
			if(iter != 0){      // Èç¹û²»ÊÇµÚÒ»¸ösegment
				// ¸üĞÂÈ¨ÖØ
				updateWeight(removeFreq, removeWeight, removeScore, r, removeNum);
				updateWeight(insertFreq, insertWeight, insertScore, r, insertNum);
				updateWeight(noiseFreq, noiseWeight, noiseScore, r, 2);
				// ¸üĞÂ¸ÅÂÊ
				updateProb(removeProb, removeWeight, removeNum);
				updateProb(insertProb, insertWeight, insertNum);
				updateProb(noiseProb, noiseWeight, 2);
				// ½«¸÷±äÁ¿ÖÃÁã
				setZero<int>(removeFreq, removeNum);
				setZero<int>(removeScore, removeNum);
				setZero<int>(insertFreq, insertNum);
				setZero<int>(insertScore, insertNum);
				setZero<int>(noiseFreq, 2);
				setZero<int>(noiseScore, 2);
			}
		}

		// ²úÉúËæ»úÊıÑ¡È¡remove heuristicºÍinsert heuristic
		// ÒÔ¸ÅÂÊÑ¡Ôñremove heuristic
		int removeIndex;
		float sumation;
		if(removalSelectTrend.first == false) {
			float removeSelection = random(0,1);  // ²úÉú0-1Ö®¼äµÄËæ»úÊı
			sumation = removeProb[0];
			removeIndex = 0;    // remove heuristic±àºÅ
			while(sumation < removeSelection){
				sumation += removeProb[++removeIndex];
			}
		}
		else{
			removeIndex = removalSelectTrend.second;
		}
		removalSelectTrend.first = false;
		// ÒÔ¸ÅÂÊÑ¡Ôñinsert heurisitc
		float insertSelection = random(0,1);
		sumation = insertProb[0];
		int insertIndex = 0;
		while(sumation < insertSelection){
			sumation += insertProb[++insertIndex];
		}
		// ÒÔ¸ÅÂÊÑ¡ÔñÊÇ·ñÔö¼ÓÔëÉùÓ°Ïì
		float noiseSelection = random(0,1);
		bool noiseAdd = false;
		if(noiseProb[0] > noiseSelection) {
			noiseAdd = true;
		}

		////@@@@@@@@@ dangerous!!!!!!!!! @@@@@@@@@//
		//removeIndex = 1;
		//insertIndex = 0;
		////////////////////////////////////////////

		// ÏàÓ¦Ëã×ÓÊ¹ÓÃ´ÎÊı¼ÓÒ»
		removeFreq[removeIndex]++;
		insertFreq[insertIndex]++;
		noiseFreq[1-(int)noiseAdd]++;
        int currentRemoveNum;
        if(destroy == true) {
            currentRemoveNum = customerAmount;
            destroy = false;
        } else {    
		    int maxRemoveNum = min(100, static_cast<int>(floor(ksi*customerAmount)));  // ×î¶àÒÆ³ıÄÇÃ´¶à½Úµã
		    int minRemoveNum = 4;  // ×îÉÙÒÆ³ıÄÇÃ´¶à½Úµã
		    minRemoveNum = floor(0.4*customerAmount);
		    currentRemoveNum = (int)floor(random(minRemoveNum, maxRemoveNum));  // µ±Ç°ÒªÒÆ³ıµÄ½ÚµãÊıÄ¿ 
        }      
		deleteCustomerSet(removedCustomer);  // Çå¿ÕremovedCustomer
		removedCustomer.resize(0);

		// Ö´ĞĞremove heuristic
		switch(removeIndex) {
		case 0: 
			{
				// Ê×ÏÈµÃµ½maxArrivedTime
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
		// Ö´ĞĞinsert heuristic
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
		assert(getCustomerNum(tempCarSet) == customerAmount);
		// ÒÆ³ı¿ÕÂ·¾¶
		removeNullRoute(tempCarSet, true);

		// Ê¹ÓÃÄ£ÄâÍË»ğËã·¨¾ö¶¨ÊÇ·ñ½ÓÊÕ¸Ã½â
		bool accept = false;
		float newCost = getCost(tempCarSet, DTpara);
		float acceptProb = exp(-(newCost - currentCost)/T);
		if(acceptProb > rand()/(RAND_MAX+1.0f)) {
			accept = true;
		}
		T = T * c;   // ½µÎÂ
		size_t newRouteCode = codeForSolution(tempCarSet);

		// ½ÓÏÂÀ´ÅĞ¶ÏÊÇ·ñĞèÒª¼Ó·Ö
		// ¼Ó·ÖÇé¿öÈçÏÂ£º
		// 1. µ±µÃµ½Ò»¸öÈ«¾Ö×îÓÅ½âÊ±
		// 2. µ±µÃµ½Ò»¸öÉĞÎ´±»½ÓÊÜ¹ıµÄ£¬¶øÇÒ¸üºÃµÄ½âÊ±
		// 3. µ±µÃµ½Ò»¸öÉĞÎ´±»½ÓÊÜ¹ıµÄ½â£¬ËäÈ»Õâ¸ö½â±Èµ±Ç°½â²î£¬µ«ÊÇÕâ¸ö½â±»½ÓÊÜÁË
		if(newCost < globalCost){  // Çé¿ö1
			removeScore[removeIndex] += sigma1;
			insertScore[insertIndex] += sigma1;
			noiseScore[1-(int)noiseAdd] += sigma1;
			withdrawPlan(globalCarSet);
			globalCarSet = copyPlan(tempCarSet);
			globalCost = newCost;
		} else {
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){  // ¸Ã½â´ÓÀ´Ã»ÓĞ±»½ÓÊÜ¹ı
				if(newCost < currentCost){    // Èç¹û±Èµ±Ç°½âÒªºÃ£¬Çé¿ö2
					ksi = 0.4f;  // Èç¹ûµÃµ½ÁË¸üºÃµÄ½â£¬Ôò¼õÉÙ¶Ôµ±Ç°½âµÄÈÅ¶¯
					removeScore[removeIndex] += sigma2;
					insertScore[insertIndex] += sigma2;
					noiseScore[1-(int)noiseAdd] += sigma2;
				} else {      
					if(accept == true) {       // Çé¿ö3
                        if(newCost < baseCost) {
                            // ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
                            destroy = true;
                        } else {
						    ksi = 0.8f;   // Èç¹ûµÃµ½ÁË¸ü²îµÄ½â£¬ÔòÔö¼Ó¶Ôµ±Ç°½âµÄÈÅ¶¯
                        }
						removalSelectTrend.first = true;   // ÕâÊ±Ç¿ÖÆÊ¹ÓÃrandom removalÀ´ÆÆ»µµ±Ç°µÄ½â
						removalSelectTrend.second = 1;     // random removal
						removeScore[removeIndex] += sigma3;
						insertScore[insertIndex] += sigma3;
						noiseScore[1-(int)noiseAdd] += sigma3;						
					}
				}
			}
		}
		if(accept == true) {    // Èç¹û±»½ÓÊÜÁË£¬Ôò¸üĞÂcurrentCarSet£¬ ²¢ÇÒtempCarSet²»±ä
			vector<size_t>::iterator tempIter = find(hashTable.begin(), hashTable.end(), newRouteCode);
			if(tempIter == hashTable.end()){
				hashTable.push_back(newRouteCode); 
			}
			currentCost = newCost;     // Èç¹û±»½ÓÊÕ£¬Ôò¸üĞÂµ±Ç°½â
			withdrawPlan(currentCarSet);
			currentCarSet = copyPlan(tempCarSet);
		} else {    // ·ñÔò£¬tempCarSet»Ö¸´ÎªcurrentCarSet
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
	if(globalCost > baseCost) {
		// Èç¹ûËÑË÷²»µ½¸üºÃµÄ½â£¬ÔòÎ¬³ÖÔ­À´µÄ½â
		ostr << "SSALNS: we should use the origin plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		finalCarSet = copyPlan(originPlan);
	} else {
		ostr << "SSALNS: we will use the new plan" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		for (carIter = globalCarSet.begin(); carIter < globalCarSet.end(); carIter++) {
			if ((*carIter)->judgeArtificial() == false) {
				Car *tempCar = new Car(**carIter);
				finalCarSet.push_back(tempCar);
			}
		}
	}
    print_lck.unlock();
	//int infeasibleNum;
	//bool mark2 = judgeFeasible(finalCarSet, infeasibleNum);
	//cout << "Whether the solution is feasible? " << mark2 << endl;
	finalCost = globalCost;
	deleteCustomerSet(waitCustomer);
	withdrawPlan(originPlan);
	withdrawPlan(baseCarSet);
	withdrawPlan(tempCarSet);
	hashTable.clear();
}


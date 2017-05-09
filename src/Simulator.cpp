#include "Simulator.h"
#include "ALNS.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include "SSALNS.h"
#include<algorithm>
#include "TxtRecorder.h"
#include<thread>
#include<mutex>
#include<condition_variable>

using namespace std;

static const int CORE_NUM = 10;    // restrict the number of used core


Simulator::Simulator(int samplingRate, int timeSlotLen, int timeSlotNum, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
					 vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan, float iter_percentage, int predictMethod):samplingRate(samplingRate), timeSlotLen(timeSlotLen), 
					 timeSlotNum(timeSlotNum), slotIndex(slotIndex), iter_percentage(iter_percentage), predictMethod(predictMethod){ // ¹¹Ôìº¯Êý
	vector<Customer*>::iterator custIter;
	this->promiseCustomerSet.reserve(promiseCustomerSet.end() - promiseCustomerSet.begin());
	this->promiseCustomerSet = copyCustomerSet(promiseCustomerSet);
	
	this->waitCustomerSet.reserve(waitCustomerSet.end() - waitCustomerSet.begin());
	this->waitCustomerSet = copyCustomerSet(waitCustomerSet);

	this->dynamicCustomerSet.reserve(dynamicCustomerSet.end() - dynamicCustomerSet.begin());
	this->dynamicCustomerSet = copyCustomerSet(dynamicCustomerSet);
	
	this->currentPlan.reserve(currentPlan.size());
	this->currentPlan = copyPlan(currentPlan);
}

Simulator::~Simulator(){  // Îö¹¹º¯Êý
	//clearCarSet();
	//clearCustomerSet();
}

void clearPlanSet(vector<vector<Car*> > planSet) {
	// Çå³ýplanSet
	vector<vector<Car*> >::iterator iter;
	for(iter = planSet.begin(); iter < planSet.end(); iter++) {
		withdrawPlan((*iter));
	}
	planSet.resize(0);
}

vector<Customer*> Simulator::generateScenario(){
	// ²úÉúÇé¾°
	// ¸ù¾Ý¶¯Ì¬¹Ë¿ÍµÄËæ»úÐÅÏ¢²úÉúÆäÊ±¼ä´°
	// ×¢Òâ¶¯Ì¬¹Ë¿ÍÖ»¿ÉÄÜ³öÏÖÔÚslotIndexÖ®ºó
	int leftBound = 0;
	int rightBound = 0;
	switch(predictMethod) {
		case 0: {
			leftBound = 1;
			rightBound = 4;
			break;
		}
		case 1: {
			leftBound = 5;
			rightBound = 15;
			break;
		}
	}
	vector<Customer*> tempCustomer = copyCustomerSet(dynamicCustomerSet);
	vector<Customer*>::iterator iter = tempCustomer.begin();
	for(iter; iter<tempCustomer.end(); iter++){
		float randFloat = random(0,1);  // ²úÉúËæ»úÊýÑ¡Ôñ¹Ë¿Í¿ÉÄÜÌá³öÐèÇóµÄÊ±¼ä
		float sumation = 0;
		int count = roulette((*iter)->timeProb + slotIndex, timeSlotNum - slotIndex);  // Ê±¼ä¶Î¼ÆÊý
		float t1 = (count+slotIndex) * timeSlotLen;
		float t2 = (count+slotIndex+1) * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = timeSlotNum * timeSlotLen;  // ×î´óÔÊÐíÊ±¼ä
		(*iter)->startTime = min(tempt, maxTime - 5*(*iter)->serviceTime);
        float t3 = leftBound*(*iter)->serviceTime;
        float t4 = rightBound*(*iter)->serviceTime;  
		float timeWindowLen = random(t3, t4);       // time window length in-between [t3, t4]
		(*iter)->endTime = min((*iter)->startTime + timeWindowLen, maxTime);
	}
	return tempCustomer;
}

vector<int> getID(vector<Customer*> customerSet){ // µÃµ½customerSetµÄËùÓÐId
	vector<int> ids(0);
	ids.reserve(customerSet.end() - customerSet.begin());
	vector<Customer*>::iterator iter = customerSet.begin();
	for(iter; iter<customerSet.end(); iter++){
		ids.push_back((*iter)->id);
	}
	return ids;
}

void threadForInitial(Customer depot, float capacity, int coreId, int planNum, vector<vector<Car*> > &planSet, vector<Customer*> allCustomer, vector<int> validId, 
	Matrix<int> &transformMatrix, mutex &record_lck, float iter_percentage) {
	ALNS alg(allCustomer, depot, capacity, 10000*iter_percentage);
	vector<Car*> solution(0);
	float cost = 0;
	alg.run(solution, cost);
	vector<Car*>::iterator carIter;
	for(carIter = solution.begin(); carIter < solution.end(); carIter++) {
		(*carIter)->removeInvalidCustomer(validId);
		(*carIter)->updateTransformMatrix(transformMatrix);
	}
	deleteCustomerSet(allCustomer);
    record_lck.lock();
	planSet.push_back(solution);
	cout << "The core with id #" << coreId << " finished its task" << endl;
	record_lck.unlock();
 //    if (coreId == planNum) {   // if all threads finisb their work
	// 	bus_sign = true;
	// 	bus_cond.notify_one();
	// }
}

vector<Car*> Simulator::initialPlan(Customer depot, float capacity){     // ÀûÓÃ²ÉÑùÖÆ¶¨³õÊ¼¼Æ»®
	ostringstream ostr;
	vector<int>::iterator intIter;
	vector<Car*>::iterator carIter;
	mutex record_lck;   // lock for record plan in planSet
	mutex bus_lck;       // lock for the main thread
	condition_variable bus_cond;  // control the main thread to run
	vector<thread> thread_pool;   // a pool to capitalize thread
	bool bus_sign = false;        // avoid dead lock
	int i,j;
	int count = 0;
	sort(promiseCustomerSet.begin(), promiseCustomerSet.end());  // ¹Ë¿Í¼¯°´ÕÕid´óÐ¡½øÐÐÅÅÐò
	vector<int> validId;
	validId.push_back(0);   // µÚÒ»¸ö½ÚµãÊÇ²Ö¿â½Úµã
	vector<int> tempId = getID(promiseCustomerSet);    // ËùÓÐÔÚ¼Æ»®¿ªÊ¼Ç°ÒÑÖªµÄ¹Ë¿Íid(ÊôÓÚ±ØÐë·þÎñµÄ¹Ë¿Í)
	for(intIter = tempId.begin(); intIter < tempId.end(); intIter++) {
		validId.push_back(*intIter);
	}
	// ³õÊ¼»¯transformMatrix
	Matrix<int> transformMatrix(validId.size(), validId.size());
	for(i=0; i<validId.size(); i++) {
		for(j=0; j<validId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*> > planSet;   // ËùÓÐ²ÉÑùµÃµ½µÄ¼Æ»®
	planSet.reserve(samplingRate);
	vector<vector<Car*> >::iterator planIter = planSet.begin();
	// ¶ÔËùÓÐµÄÇé¾°ÔËÐÐALNSËã·¨£¬²¢ÇÒ°Ñ½â·ÅÈëplanSetÖÐ
	// ÔÚ´Ë¹ý³ÌÖÐ½«¸ù¾ÝvalidId¶ÔËùÓÐµÄ½â£¬½ö±£ÁôidÔÚvalidIdÖÐµÄ¹Ë¿Í½Úµã
	vector<Customer*>::iterator iter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();

	int restSampleNum = samplingRate;       // ÉÐÎ´ÅÜÍêµÄÑù±¾
	while(restSampleNum > 0) {
		int coreId = samplingRate - restSampleNum + 1; // start from 1 rather than 0
		for(int i=0; i<min(CORE_NUM,restSampleNum); i++) {
			vector<Customer*> allCustomer = copyCustomerSet(promiseCustomerSet);   // ±¾´ÎÖ´ÐÐALNSËã·¨µÄËùÓÐ¹Ë¿Í½Úµã
			vector<Customer*> currentDynamicCust = generateScenario();  // ²ÉÑù
			iter = currentDynamicCust.begin();
			for(iter; iter<currentDynamicCust.end(); iter++){
				allCustomer.push_back(*iter);
			}
			thread_pool.push_back(thread(threadForInitial, depot, capacity, coreId + i, samplingRate, ref(planSet), allCustomer, validId, ref(transformMatrix),
				ref(record_lck), ref(iter_percentage)));
		}
		for(auto& thread:thread_pool) {
			thread.join();
		}
		restSampleNum = restSampleNum - CORE_NUM;
		thread_pool.clear();
		thread_pool.resize(0);
	}

	// unique_lock<mutex> lck(bus_lck);
	// while(!bus_sign) {
	// 	bus_cond.wait(lck);
	// }

	// È»ºó¶ÔËùÓÐÇé¾°ÏÂµÄ¼Æ»®½øÐÐÆÀ·Ö£¬È¡µÃ·Ö×î¸ßÕß×÷Îª³õÊ¼Â·¾¶¼Æ»®
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int> > scoreForPlan;    // Ã¿¸ö¼Æ»®µÄµÃ·Ö
	scoreForPlan.reserve(samplingRate);
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // ÔÚ²ÉÑùµÃµ½µÄ¼Æ»®ÖÐµÄÎ»ÖÃ
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	withdrawPlan(currentPlan);
	return outputPlan;
}



bool Simulator::checkFeasible(vector<Car*> carSet){
	// ÅÐ¶ÏpromiseCustomerSetÖÐµÄ¹Ë¿ÍÊÇ·ñ¶¼ÔÚcarSetÖÐ
	vector<int> tempId = getID(promiseCustomerSet);  // µÃµ½ÁËpromiseµÄ¹Ë¿Íid
	sort(tempId.begin(), tempId.end());
	vector<Car*>::iterator carIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getRoute().getAllCustomer();
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
			if(intIter < tempId.end()) {
				// Èç¹ûÕÒµ½ÁË£¬¾ÍÉ¾µô
				tempId.erase(intIter);
			}
		}
	}
	if(tempId.size() != 0) {
		return false;
	} else {
		return true;
	}
}

void validPromise(vector<Car*>Plan, vector<Customer*> hurryCustomer, vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId){
	// ¶Ôhurry customerÈ·ÈÏpromise
	// give "accept" or "reject" response
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	vector<int> hurryCustomerId = getID(hurryCustomer);   // hurry customerµÄid
	int i;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++){
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			int tempId = (*custIter)->id;
			vector<int>::iterator tempIter = find(hurryCustomerId.begin(), hurryCustomerId.end(), tempId);
			if(tempIter < hurryCustomerId.end()) {
				// Èç¹ûtempIdÔÚhurryCustomerIdÖÐ
				newServedCustomerId.push_back(tempId);
				hurryCustomerId.erase(tempIter);  // É¾³ýµô±»·þÎñµÄ¹Ë¿ÍÊ£ÏÂÎ´±»·þÎñµÄ¹Ë¿Í
			}
		}
        deleteCustomerSet(tempCust);
	}
	sort(hurryCustomerId.begin(), hurryCustomerId.end());
	// µÃµ½·ÅÆúµÄ¹Ë¿Íid
	vector<int>::iterator intIter;
	for(intIter = hurryCustomerId.begin(); intIter < hurryCustomerId.end(); intIter++) {
		newAbandonedCustomerId.push_back(*intIter);
	}
}

void threadForReplan(float capacity, int coreId, int planNum, vector<vector<Car*>> &planSet, vector<Customer*> sampleCustomer, vector<Car*> newPlan, vector<int> validId,
	Matrix<int> &transformMatrix, mutex &record_lck, mutex &print_lck, float iter_percentage) {
	vector<Car*> tempPlan;
	float finalCost = 0;
	SSALNS SSALNS_alg3(sampleCustomer, newPlan, capacity, 10000*iter_percentage);
	SSALNS_alg3.run(tempPlan, finalCost, print_lck);
	vector<Car*>::iterator carIter;
	for(carIter = tempPlan.begin(); carIter < tempPlan.end(); carIter++) {
		(*carIter)->removeInvalidCustomer(validId);
		(*carIter)->updateTransformMatrix(transformMatrix);
	}
	deleteCustomerSet(sampleCustomer);
	{
        record_lck.lock();
		// unique_lock<mutex> lck1(record_lck);
		planSet.push_back(tempPlan);
        record_lck.unlock();
	}
    print_lck.lock();
	// unique_lock<mutex> lck2(print_lck);
	cout << "Core with id #" << coreId << " finished its task!" << endl;
	print_lck.unlock();
	// if(coreId == planNum) {    // if all thread finish their work
	// 	bus_sign = true;
	// 	bus_cond.notify_one();
	// } 
}

vector<Car*> Simulator::replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, vector<int> &delayCustomerId, float capacity) {
	// ÖØÐÂ¼Æ»®£¬ÓÃÓÚvehicle³ö·¢ÒÔºó
	// Ê×ÏÈÉ¸Ñ¡³öÒ»Ð©×Å¼±µÈ´ý»Ø¸´µÄwaitCustomerÒÔ¼°Ò»Ð©²»×Å¼±»Ø¸´µÄ
	// newServedCustomer:  (wait customerÖÐ)Í¨¹ýre-plan½ÓÊÜµ½·þÎñµÄ¹Ë¿Í
	// newAbandonedCustomer: (wait customerÖÐ)Í¨¹ýre-planÈ·ÈÏÎÞ·¨½ÓÊÜ·þÎñµÄ¹Ë¿Í
	// delayCustomer: ¶ÔÓÚpatient customer£¬Èç¹ûµ±Ç°²»ÄÜÈ·ÈÏ·þÎñ£¬Ôò¿ÉÔÚÎ´À´ÔÙÎªÆä°²ÅÅ
	ostringstream ostr;
	vector<Customer*> hurryCustomer;
	vector<Customer*> patientCustomer;
	vector<Customer*>::iterator custIter;
	mutex record_lck;    // lock for recording plan of each scenario
	mutex bus_lck;       // lock for the main thread
	mutex print_lck;     // lock for print process of SSALNS algorithm
	// condition_variable bus_cond;   // control the main thread
	vector<thread> thread_pool;    // pool for storing all threads
	// bool bus_sign = false;         // partner with "bus_cond"
	int count = 0;

	float nextMoment = (slotIndex+1) * timeSlotLen; // ÏÂÒ»¸öÊ±¼ä¶ÎµÄÖÕÖ¹Ê±¼ä£¨ÏÂÏÂ¸öÊ±¼ä¶ÎµÄ¿ªÊ¼Ê±¼ä£©
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); custIter++) {
		if((*custIter)->tolerantTime <= nextMoment) {  // ¸Ã¹Ë¿Í×Å¼±ÓÚÏÂÊ±¼ä¶ÎÇ°µÃµ½»Ø¸´
			Customer *tempCust = new Customer(**custIter);
			hurryCustomer.push_back(tempCust);     // ¼ÓÈëhurryCustomerÖÐ
		} else {
			Customer *tempCust = new Customer(**custIter);
			patientCustomer.push_back(tempCust);
		}
	}
	// È»ºó½«ÕâÐ©¹Ë¿Í¾¡¿ÉÄÜµØ·ÅÈëµ±Ç°¼Æ»®ÖÐ
	// Èô²»ÄÜ£¬Ôò·µ»Ø'No' promise
	vector<Car*> newPlan;
	vector<Car*>::iterator carIter;
	float finalCost = 0;
	const int iterForWaitCustomer = 30000*iter_percentage;   // iterations in SSALNS for wait customers
	if(hurryCustomer.size() == 0) {  
		// Èç¹ûÃ»ÓÐhurryCustomer£¬ÄÇÃ´²»ÐèÒª¶ÔÆä½øÐÐreplan
		ostr.str("");
		ostr << "There are no hurry Customer" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer, the number is " << patientCustomer.size() << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// Èç¹ûpatientCustomerÎª¿Õ£¬Ôò²»ÐèÒª¶ÔÆä½øÐÐreplan
			SSALNS SSANLS_alg2(patientCustomer, currentPlan, capacity, iterForWaitCustomer);
			SSANLS_alg2.run(newPlan, finalCost, print_lck);
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);
		} else {
			// Èç¹ûÁ½¸öcustomerSet¶¼Îª¿Õ£¬ÔòÖ±½Ó¸´ÖÆcurrentPlanÖÁnewPlan
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			newPlan = copyPlan(currentPlan);
		}
	} else {
		// ¶ÔhurryCustomer½øÐÐreplan
		ostr.str("");
		ostr << "Replan for hurry customer, the number is " <<  hurryCustomer.size() << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		SSALNS SSALNS_alg1(hurryCustomer, currentPlan, capacity, iterForWaitCustomer);
		SSALNS_alg1.run(newPlan, finalCost, print_lck);
        cout << "The plan after insert hurry customers is valid? " << checkFeasible(newPlan) << endl;
		validPromise(newPlan, hurryCustomer, newServedCustomerId, newAbandonedCustomerId);
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer, the number is " << patientCustomer.size() << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// Èç¹ûpatientCustomer²»Îª¿Õ£¬ÔòÓ¦¸Ã¶ÔÆä½øÐÐreplan
			SSALNS SSANLS_alg2(patientCustomer, newPlan, capacity, iterForWaitCustomer);
			SSANLS_alg2.run(newPlan, finalCost, print_lck);
            cout << "The plan after insert patient customers is valid? " << checkFeasible(newPlan) << endl;
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);		
		} else {
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
		}
	}

	deleteCustomerSet(hurryCustomer);
	deleteCustomerSet(patientCustomer);
	
    ostr.str("");
    ostr << "There are newly " << newServedCustomerId.size() << " customers get service" << endl;
    ostr << "There are still " << delayCustomerId.size() << " customers waiting for promise" << endl;
    TxtRecorder::addLine(ostr.str());
    cout << ostr.str();

	// Ê×ÏÈµÃµ½"OK promise"µÄ¹Ë¿ÍµÄid£¬ÓÃÓÚÇó½âÆÀ·Ö¾ØÕó
	vector<int> allServedCustomerId;    // all customers got "accept" response
	allServedCustomerId.push_back(0);   // ²Ö¿â½ÚµãÊÇÆÀ·Ö¾ØÕóÖÐµÄµÚÒ»¸ö½Úµã
	vector<int>::iterator intIter;
	vector<int> promiseCustomerId = getID(promiseCustomerSet);
	for(intIter = promiseCustomerId.begin(); intIter < promiseCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	for(intIter = newServedCustomerId.begin(); intIter < newServedCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	sort(allServedCustomerId.begin(), allServedCustomerId.end());
	// È»ºó½øÐÐ²ÉÑù£¬µ÷ÓÃSSALNSËã·¨¼ÆËã¸÷¸ö²ÉÑùÇé¾°ÏÂµÄ¼Æ»®
	// ²¢ÇÒ¼ÆËãÆÀ·Ö¾ØÕó
	// ³õÊ¼»¯transformMatrix
	Matrix<int> transformMatrix(allServedCustomerId.size(), allServedCustomerId.size());
	for(int i=0; i<allServedCustomerId.size(); i++) {
		for(int j=0; j<allServedCustomerId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*> > planSet;     // store all threads of all scenarios
	planSet.reserve(samplingRate);
	vector<vector<Car*> >::iterator planIter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	ostr << "----In replan, there will be " << dynamicCustomerSet.size() << " dynamic customers" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();

	int restSampleNum = samplingRate;
	while(restSampleNum > 0) {
		thread_pool.clear();
		thread_pool.resize(0);
		int coreId = samplingRate - restSampleNum + 1;
		for (int i = 0; i < min(CORE_NUM, restSampleNum); i++) {
			vector<Customer*> sampleCustomer = generateScenario(); // ²úÉú¶¯Ì¬µ½´ï¹Ë¿ÍµÄÇé¾°
			thread_pool.push_back(thread(threadForReplan, capacity, coreId + i, samplingRate, ref(planSet), sampleCustomer, ref(newPlan), allServedCustomerId, 
				ref(transformMatrix), ref(record_lck), ref(print_lck), iter_percentage));
		}
		for (auto& thread : thread_pool) {
			thread.join();
		}
		restSampleNum = restSampleNum - CORE_NUM;
	}

	// unique_lock<mutex> lck(bus_lck);
	// while(!bus_sign) { // if "bus_sign=false", then hang the main thread
	// 	bus_cond.wait(lck);
	// }

	// È¡ÆÀ·Ö×î¸ßµÄ¼Æ»®×÷ÎªÊä³ö
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int> > scoreForPlan;    // Ã¿¸ö¼Æ»®µÄµÃ·Ö
	scoreForPlan.reserve(samplingRate);
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // ÔÚ²ÉÑùµÃµ½µÄ¼Æ»®ÖÐµÄÎ»ÖÃ
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
    cout << "The output plan is feasible? " << checkFeasible(outputPlan) << endl;
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	deleteCustomerSet(dynamicCustomerSet);
	withdrawPlan(currentPlan);
	withdrawPlan(newPlan);
	return outputPlan;
}

vector<Car*> Simulator::no_replan() {
    ostringstream ostr;
    ostr.str("");
    ostr << "We don't apply replan this time!!" << endl;
    TxtRecorder::addLine(ostr.str());
    cout << ostr.str();
    return currentPlan;
}

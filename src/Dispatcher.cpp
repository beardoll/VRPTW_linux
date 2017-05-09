#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "PublicFunction.h"
#include "TxtRecorder.h"

const float MAX_FLOAT = numeric_limits<float>::max();

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
					   int timeSlotNum, int samplingRate, float iter_percentage, int predictMethod): depot(depot), capacity(capacity), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), 
						samplingRate(samplingRate), iter_percentage(iter_percentage), predictMethod(predictMethod){
	int custNum = staticCustomerSet.end() - staticCustomerSet.begin();
	custNum += dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // �ܹ˿���
	ServedCustomerId.reserve(custNum);     // �Ѿ�������Ĺ˿�id
	promisedCustomerId.reserve(custNum);
	waitCustomerId.reserve(custNum);
	rejectCustomerId.reserve(custNum);
	vector<Customer*>::iterator custIter = staticCustomerSet.begin();
	for(custIter; custIter < staticCustomerSet.end(); custIter++) {
		// �ڼƻ���ʼǰ�Ѿ��������Ĺ˿Ͷ�����promiseCustomer
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);
		promisedCustomerId.push_back(newCust->id);
	}
	for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);	
		dynamicCustomerId.push_back(newCust->id);
	}
	sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // ��id���е�������
	sort(promisedCustomerId.begin(), promisedCustomerId.end());
	sort(dynamicCustomerId.begin(), dynamicCustomerId.end());
}


void Dispatcher::destroy() {
	deleteCustomerSet(allCustomer);
}

void Dispatcher::carFinishedTask(int carIndex){       
	// �ճ�
	// we will delete the car with id "carIndex" in the "currentPlan"
	// and add it into "finishPlan" 
	vector<Car*>::iterator carIter;
	ostringstream ostr;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			Car *newCar = new Car(**carIter);
			delete(*carIter);
			currentPlan.erase(carIter);
			finishedPlan.push_back(newCar);
			break;
		}
	}
	if(currentPlan.size() == 0) {
		ostr.str("");
		ostr << "=============== END ================" << endl;
		ostr << "----OH!! Finished!" << endl;
		ostr << "----" << ServedCustomerId.size() << " customers get served finally" << endl;
		ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	} else {
		// �������ʣ��ĳ���������wait״̬����Ҳ�������������
		bool mark = true;
		int num = 0;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			if((*carIter)->getState() != wait) {
				mark = false;
				break;
			} else {
				num++;
			}
		}
		if(mark == true) {
			ostr.str("");
			ostr << "=============== END ================" << endl;
			ostr << "----There are " << num << " cars remains unused!" << endl;
			ostr << "----" << ServedCustomerId.size() << " customers get served finally" << endl;
			ostr << "----" << rejectCustomerId.size() << " customers get rejected finally" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();	
		}
	}
}

bool Dispatcher::checkFeasible(vector<Car*> carSet){
	// �ж�promiseCustomerSet�еĹ˿��Ƿ���carSet��
	vector<int> tempId = promisedCustomerId;  // �õ���promise�Ĺ˿�id
	sort(tempId.begin(), tempId.end());
	vector<Car*>::iterator carIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
			if(intIter < tempId.end()) {
				// ����ҵ��ˣ���ɾ��
				tempId.erase(intIter);
			}
		}
		deleteCustomerSet(tempCust);
	}
	if(tempId.size() != 0) {   // if there are promiseCustomers excluded
		return false;
	} else {
		return true;
	}
}


vector<EventElement> Dispatcher::handleNewTimeSlot(int slotIndex){ // ��ʱ��ο�ʼ
	vector<Customer*> promiseCustomerSet;
	vector<Customer*> waitCustomerSet;
	vector<Customer*> dynamicCustomerSet;
	vector<int>::iterator custIdIter;
	vector<Car*>::iterator carIter;
	vector<Car*> updatedPlan;
	for(custIdIter = promisedCustomerId.begin(); custIdIter< promisedCustomerId.end(); custIdIter++) {
		promiseCustomerSet.push_back(allCustomer[*custIdIter - 1]);  // id��1��ʼ���
	}
	for(custIdIter = dynamicCustomerId.begin(); custIdIter < dynamicCustomerId.end(); custIdIter++) {
		dynamicCustomerSet.push_back(allCustomer[*custIdIter - 1]);
	}

	vector<EventElement> newEventList;
	ostringstream ostr;
	if(slotIndex == 0) {  // ·���ƻ���Ҫ��ʼ��
		ostr.str("");
		ostr << "============ Now Initialize the routing plan ===========" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		Simulator smu(samplingRate, timeSlotLen, timeSlotNum, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, currentPlan, iter_percentage, predictMethod);
		updatedPlan = smu.initialPlan(depot, capacity);
		currentPlan = copyPlan(updatedPlan);
		withdrawPlan(updatedPlan);
		vector<Car*>::iterator carIter;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			EventElement newEvent = (*carIter)->launchCar(0);  // ����������
			newEventList.push_back(newEvent);
		}
		ostr.str("");
		ostr << "----Initialization Finished! Now there are " << currentPlan.size() << " cars dispatched!" << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	} else {
		// ��ȡÿ����δ�߹���·�����з��棬�õ��µ�·���ƻ�
		ostr.str("");
		ostr << "============ Now replan, the time slot is: " << slotIndex << "============" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		float currentTime = slotIndex * timeSlotLen;
		for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
			Customer *temp = new Customer;
			*temp = *allCustomer[*custIdIter - 1];
			waitCustomerSet.push_back(temp);
		}
		vector<Car*> futurePlan;
		for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
			// ץȡ���ӻ�û���߹��ļƻ�
			Car *tempCar = new Car((*carIter)->capturePartRoute(currentTime));
			futurePlan.push_back(tempCar);
		}
		if (currentPlan.size() != 0) {  // �л�������ʱ���Ž���replan
			Simulator smu(samplingRate, timeSlotLen, timeSlotNum, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, futurePlan, iter_percentage, predictMethod);
			vector<int> newServedCustomerId;
			vector<int> newAbandonedCustomerId;
			vector<int> delayCustomerId;
			updatedPlan = smu.replan(newServedCustomerId, newAbandonedCustomerId, delayCustomerId, capacity);
			withdrawPlan(futurePlan);
			//updatedPlan = smu.no_replan();
			vector<Customer*>::iterator custIter;

			// ����promiseCustomerId, rejectCustomerId�Լ�waitCustomerId
			vector<int>::iterator intIter;
			vector<int> tempVec;
			if(newServedCustomerId.size() != 0) {
				for (intIter = newServedCustomerId.begin(); intIter < newServedCustomerId.end(); intIter++) {
					promisedCustomerId.push_back(*intIter);
					tempVec.push_back(*intIter);
				}
				sort(promisedCustomerId.begin(), promisedCustomerId.end());
			}

			if(newAbandonedCustomerId.size() != 0) {
				for (intIter = newAbandonedCustomerId.begin(); intIter < newAbandonedCustomerId.end(); intIter++) {
					rejectCustomerId.push_back(*intIter);
					tempVec.push_back(*intIter);
				}

				sort(rejectCustomerId.begin(), rejectCustomerId.end());
			}

			//////////////////////// check if all id in tempVec also in waitCustomerId /////////////////////////////
			for(intIter = tempVec.begin(); intIter < tempVec.end(); intIter++) {                                  //
				vector<int>::iterator iter99 = find(waitCustomerId.begin(), waitCustomerId.end(), *intIter);      //
				if(iter99 == waitCustomerId.end()) {                                                              //
					// not found       												   							  //
					cout << endl;                                                                                 //
					cout << "===================================================" << endl;                        //
					cout << "tempVec not totally concluded in waitCustomerId!!!!" << endl;                        //
					cout << "===================================================" << endl;   					  //
					cout << endl;                     															  //
					break;                                                                                        //
				}                                                                                                 //
			}                                                                                                     //                                                                                     
			////////////////////////////////////////////////////////////////////////////////////////////////////////

			if(tempVec.size() != 0) {
				sort(waitCustomerId.begin(), waitCustomerId.end());
				sort(tempVec.begin(), tempVec.end());
				vector<int> tempVec2(20);

				vector<int>::iterator iterxx = set_difference(waitCustomerId.begin(), waitCustomerId.end(), tempVec.begin(), tempVec.end(), tempVec2.begin());
				tempVec2.resize(iterxx - tempVec2.begin());
				waitCustomerId = tempVec2;
			}

			// ��������future plan���嵽currentPlan��Ӧλ��֮��
			int count = 0;
			for (carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
				currentPlan[count]->replaceRoute(**carIter, currentTime);
				EventElement newEvent;
				if (currentPlan[count]->getState() == wait) {
					// �������ԭ������wait״̬������Ҫ���䷢��
					newEvent = currentPlan[count]->launchCar(currentTime);
				}
				else {
					newEvent = currentPlan[count]->getCurrentAction(currentTime);
				}
				newEventList.push_back(newEvent);
				count++;
			}
			withdrawPlan(updatedPlan);
			deleteCustomerSet(waitCustomerSet);
			ostr.str("");
			ostr << "----Replan Finished! Now there are " << currentPlan.size() << " cars working!" << endl << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
		}
		else {
			ostr.str("");
			ostr << "----no car is applicable!!!" << endl << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
		}
	}
	return newEventList;
} 

EventElement Dispatcher::handleNewCustomer(int slotIndex, const Customer& newCustomer){  
	// �����¹˿͵���
	ostringstream ostr;
	ostr.str("");
	ostr<< "----Customer with id #" << newCustomer.id << " is arriving..." << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<int>::iterator intIter = find(dynamicCustomerId.begin(), dynamicCustomerId.end(), newCustomer.id);
	dynamicCustomerId.erase(intIter);
	float minInsertCost = MAX_FLOAT;
	pair<int, Customer> insertPos;   // ��һ��int�ǻ�����ţ���currentPlan�е�λ�ã����ڶ���Customer�ǲ����ǰ��Ĺ˿�
	vector<Car*>::iterator carIter;
	float currentTime = newCustomer.startTime;       // �˿���������ʱ��������ʱ�䴰��ʼ��ʱ��
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		// ��newCustomer��ÿ��route����С�������
		Car tempCar = (*carIter)->capturePartRoute(currentTime);
		Customer customer1, customer2;
		float minValue, secondValue;
		tempCar.computeInsertCost(newCustomer, minValue, customer1, secondValue, customer2);
		if(minValue < minInsertCost) {
			int pos = carIter - currentPlan.begin();  
			minInsertCost = minValue;
			insertPos = make_pair(pos, customer1);
		}
	}
	EventElement newEvent;
	if(minInsertCost == MAX_FLOAT) {
		// û�п��в����
        bool ma = true; 
		if(newCustomer.tolerantTime < slotIndex*timeSlotLen) { 
			// �Ȳ���replan����reject
			ostr.str("");
			ostr << "He is rejected!" << endl;
            ostr << "His tolerance time is " << newCustomer.tolerantTime << endl;
            ostr << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			rejectCustomerId.push_back(newCustomer.id);
		} else {  // ���򣬽���ȴ��Ĺ˿�����
			ostr.str("");
			ostr << "He will wait for replan!" << endl << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			waitCustomerId.push_back(newCustomer.id);  
			sort(waitCustomerId.begin(), waitCustomerId.end());
		}
	} else {
		promisedCustomerId.push_back(newCustomer.id);  // ��Щ�˿�һ����õ�����
		sort(promisedCustomerId.begin(), promisedCustomerId.end());
		int selectedCarPos = insertPos.first;
		Customer selectedCustomer = insertPos.second;
		currentPlan[selectedCarPos]->insertAfter(selectedCustomer, newCustomer);
		if(currentPlan[selectedCarPos]->getState() == wait) {  // if the car stays asleep
			newEvent = currentPlan[selectedCarPos]->launchCar(currentTime);
		} else {
			newEvent = currentPlan[selectedCarPos]->getCurrentAction(currentTime);
		}
		int carIndex = currentPlan[selectedCarPos]->getCarIndex();
		ostr.str("");
		ostr << "He is arranged to car #" << carIndex << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
 	}
	return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 
	// ������������¼�
	// �ڴ˴����¹˿͵ĵ���ʱ��
	ostringstream ostr;
	vector<Car*>::iterator carIter;
	int pos;  // carIndex��Ӧ�ĳ�����currentPlan�е�λ��
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			pos = carIter - currentPlan.begin();
			break;
		}
	}
	EventElement tempEvent = currentPlan[pos]->getCurrentAction(time);
	if(currentPlan[pos]->getState() == offwork) {
		// �ճ�
		ostr.str("");
		ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() << " finished its task!" << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		carFinishedTask(tempEvent.carIndex);
	} else {
		// ����newServedCustomerId�Լ�promisedCustomerId
		int currentId = currentPlan[pos]->getCurrentNode().id;
		vector<int>::iterator intIter = find(promisedCustomerId.begin(), promisedCustomerId.end(), currentId);
		promisedCustomerId.erase(intIter);
		ServedCustomerId.push_back(currentId);
		sort(ServedCustomerId.begin(), ServedCustomerId.end());
		ostr.str("");
		ostr << "----Time " << time << ", Car #" << currentPlan[pos]->getCarIndex() << " arrives at customer #" << currentId << endl << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
	}
	return tempEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       // ���������ɷ����¼�
	ostringstream ostr;
	EventElement newEvent(-1, carArrived, -1, -1);     // ��Ч�¼�
	vector<Car*>::iterator carIter;
	int pos;  // carIndex��Ӧ�ĳ�����currentPlan�е�λ��
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex() == carIndex) {
			pos = carIter - currentPlan.begin();
			break;
		}
	}

	newEvent = currentPlan[pos]->getCurrentAction(time);
	int currentId = currentPlan[pos]->getCurrentNode().id;
	ostr.str("");
	ostr << "----Time " << time << ", car #" << carIndex << " finished service in customer #" << currentId << endl;
	ostr << "Its end time for servce is " << currentPlan[pos]->getCurrentNode().endTime << endl << endl;;
    TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	return newEvent;
}



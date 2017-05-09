#include "Timer.h"
#include "Dispatcher.h"
#include<cassert>
#include<algorithm>

bool ascendSortEvent(const EventElement &a, const EventElement &b){  // ��������
	return a.time < b.time;
}

EventElement Timer::pop(){  // ��������¼���˳�������eventList��ɾ����
	assert(eventList.size() > 0);
	vector<EventElement>::iterator iter = eventList.begin(); 
    vector<EventElement>::iterator iter2;
    EventElement newElement;
    if(iter->carIndex == -1 && eventList.size() > 1) {
    	// if there are some events happen at the same time
    	// then "newTimeSlot" is of lowest priority
    	// and other events have the same priority
        iter2 = iter + 1;
        if(iter2->time == iter->time) {
            newElement = *iter2;
            eventList.erase(iter2);
        } else {
            newElement = *iter;
            eventList.erase(iter);
        }
    } else {
        newElement = *iter;
	    eventList.erase(iter);
    }
	return newElement;
}

Timer::Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int timeSlotLen, int timeSlotNum, float capacity, Customer depot, int sampleRate, float iter_percentage, int predictMethod): 
	staticCustomerSet(staticCustomerSet), dynamicCustomerSet(dynamicCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), capacity(capacity), depot(depot), 
	sampleRate(sampleRate), iter_percentage(iter_percentage), predictMethod(predictMethod)
{   
	// ���캯�����������Ϊ���й˿ͣ��Լ���ʱ��ο�ʼֵ
	EventElement newEvent;
	int i;
	for(i=0; i<timeSlotNum; i++) {  
		// ���ӡ�ʱ��ε���¼�
		newEvent = EventElement(i*timeSlotLen, newTimeSlot, -1, -1);
		eventList.push_back(newEvent);
	}
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter< dynamicCustomerSet.end(); iter++) {  
		// ���ӡ��¹˿͵����¼���
		newEvent = EventElement((*iter)->startTime, newCustomer, -1, (*iter)->id);
		eventList.push_back(newEvent);
	}
	sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::addEventElement(EventElement &newEvent){  
	// ���¼����������¼�
	eventList.push_back(newEvent);
	sort(eventList.begin(), eventList.end(), ascendSortEvent);
}

void Timer::deleteEventElement(int carIndex){     
	// ɾ����carIndex��ص��¼�
	if(carIndex == -1) return;    // "carIndex=1" indicates the "newTimeSlot" event, which is prohibited being deleted
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); ){
		if((*iter).carIndex == carIndex) {
            EventElement *p = &(*iter);
            delete(p);
			iter = eventList.erase(iter);
			break;
		} else {
			iter++;
		}
	}
}

Customer searchCustomer(int customerId, vector<Customer*> customerSet) {
	// ����customerId��customerSet��Ѱ�ҹ˿�
	vector<Customer*>::iterator custIter;
	for(custIter = customerSet.begin(); custIter < customerSet.end(); custIter++) {
		if((*custIter)->id == customerId) {
			return **custIter;
			break;
		}
	}
}

void Timer::updateEventElement(EventElement &newEvent){  
	// �����¼�
	vector<EventElement>::iterator iter = eventList.begin();
	if(newEvent.time == -1) {
		// ������Ч�¼���������
		return;	
	} else {
		bool found = false;
		for(iter; iter<eventList.end(); iter++){
			if((*iter).carIndex == newEvent.carIndex) {
				// �жϸ����¼���λ�ã�������֮
				// ע�����ÿһ���������¼����н����һ���¼�
				found = true;
				(*iter).time = newEvent.time;
				(*iter).eventType = newEvent.eventType;
				(*iter).customerId = newEvent.customerId;
			}
		}
		if(found == false) {
			// �Ҳ�������Ϊ�����ڴ�֮ǰû�н�ʱ���¼��
			// ������������ڻ����ڲֿ�ȴ���һ��ʱ�����յ���������
			addEventElement(newEvent);
		}
	}
}

// enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
void Timer::run(vector<Car*> &finishedPlan, vector<Customer*> &rejectCustomer, float &travelDistance, float &addAveDistance) {
	Dispatcher disp(staticCustomerSet, dynamicCustomerSet, depot, capacity, timeSlotLen, timeSlotNum, sampleRate, iter_percentage, predictMethod);  // �������ĳ�ʼ��
	int slotIndex = 0;  // ��0��ʱ���
	while(eventList.size() != 0) {
		EventElement currentEvent = pop();  // ��������¼�
		switch(currentEvent.eventType) {
		case newCustomer: {   // �¹˿͵���
			EventElement newEvent = disp.handleNewCustomer(slotIndex, searchCustomer(currentEvent.customerId, dynamicCustomerSet));
			updateEventElement(newEvent);
			break;
						  }
		case carArrived: {
			EventElement newEvent = disp.handleCarArrived(currentEvent.time, currentEvent.carIndex);
			updateEventElement(newEvent);
			break;
						 }
		case finishedService: {
			EventElement newEvent = disp.handleFinishedService(currentEvent.time, currentEvent.carIndex);
			updateEventElement(newEvent);
			break;
							  }
		case newTimeSlot: {
			vector<EventElement> newEventList = disp.handleNewTimeSlot(slotIndex);
			vector<EventElement>::iterator eventIter;
			for(eventIter = newEventList.begin(); eventIter < newEventList.end(); eventIter++) {
				// ��������¼��б���
				updateEventElement(*eventIter);
			}
			slotIndex++;
			break;
						  }
		case carOffWork: {
			// do nothing now
			break;
						 }
		}
	}
    vector<Customer*> allCustomer;
    vector<Customer*>::iterator custIter;
    for(custIter = staticCustomerSet.begin(); custIter < staticCustomerSet.end(); custIter++) {
       allCustomer.push_back(*custIter);
    }
    for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
       allCustomer.push_back(*custIter);
    }
    sort(allCustomer.begin(), allCustomer.end());
    vector<int> rejectCustomerId = disp.getRejectCustomerId();
    vector<int>::iterator intIter;
    for(intIter = rejectCustomerId.begin(); intIter < rejectCustomerId.end(); intIter++) {
        rejectCustomer.push_back(allCustomer[*intIter]);
    }
    finishedPlan = disp.getFinishedPlan();
	vector<Car*>::iterator carIter;
	travelDistance = 0;
	addAveDistance = 0;
	for(carIter = finishedPlan.begin(); carIter < finishedPlan.end(); carIter++) {
		travelDistance += (*carIter)->getTravelDistance();
		addAveDistance += (*carIter)->getAddDistance();
	}
	addAveDistance /= (dynamicCustomerSet.size() - rejectCustomer.size());  // Ϊ����̬�˿����������ӵ�ƽ��·��
	disp.destroy();
}

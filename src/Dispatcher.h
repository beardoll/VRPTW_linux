#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventElement.h"
#include "Car.h"
#include "ProbInfo.h"
#include<vector>

class Dispatcher{   // ����������
public:
	Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
		int timeSlotNum, int samplingRate, float iter_percentage, int predictMethod);  // ���캯��
	~Dispatcher(){}; // ��������
	void destroy();  // ����Dispatcher
	EventElement handleNewCustomer(int slotIndex, const Customer& newCustomer);  // �����¹˿͵���
	EventElement handleCarArrived(float time, int carIndex);            // ������������¼�
	EventElement handleFinishedService(float time, int carIndex);       // ���������ɷ����¼�
	// EventElement handleDepature(float time, int carIndex);              // ������������¼�
	vector<EventElement> handleNewTimeSlot(int slotIndex); // ��ʱ��ο�ʼ 
	//Car& searchCar(int carIndex);   // ���ݻ��������������
	void carFinishedTask(int carIndex);       // �ճ� 
	bool checkFeasible(vector<Car*> carSet);
	//bool checkTimeConstraint();
    vector<int> getRejectCustomerId() {return rejectCustomerId; }
    vector<Car*> getFinishedPlan() {return finishedPlan; }
private:
	vector<Customer*> allCustomer;    // ���еĹ˿�
	vector<int> dynamicCustomerId;    // ��̬����Ĺ˿͵�id
	vector<int> ServedCustomerId;     // �Ѿ�������Ĺ˿�id
	vector<int> promisedCustomerId;   // (δ�������)�Ѿ��õ�'OK' promise�Ĺ˿�id
	vector<int> waitCustomerId;       // (δ�����������֪��)���ڵȴ�'OK' promise�Ĺ˿�id
	vector<int> rejectCustomerId;     // (δ�����������֪��)�õ���'NO' promise�Ĺ˿�id
	vector<Car*> currentPlan;         // ��ǰ�ƻ�
	vector<Car*> finishedPlan;        // ����ɼƻ�    
	Customer depot;
	float capacity;
	int timeSlotLen;   // ʱ��εĳ���
	int timeSlotNum;   // ʱ��εĸ���
	int samplingRate;    // ������
	float iter_percentage;
	int predictMethod;
};

#endif

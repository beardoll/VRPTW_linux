#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Customer.h"
#include "Car.h"
#include<vector>

class Simulator{  // ��������
public:
	Simulator(int samplingRate, int timeSlotLen, int timeSlotNum, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
		vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan); // ���캯��
	~Simulator();  // �������� 
	vector<Car*> initialPlan(Customer depot, float capacity);     // ���ò����ƶ���ʼ�ƻ�
	vector<Car*> replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, vector<int> &delayCustomerId, float capacity);
	vector<Customer*> generateScenario();  // ������̬�˿͵��龰
	bool checkFeasible(vector<Car*> carSet);
private:
	int samplingRate;   // ������
	int timeSlotLen;    // ʱ��γ���
	int timeSlotNum;    // timeSlot�ĸ���
	int slotIndex;      // ��ǰ�ǵڼ���slot
	vector<Customer*> promiseCustomerSet;     // �������Ĺ˿ͣ���һ��Ҫ�ڼƻ����У�
	vector<Customer*> waitCustomerSet;        // ����Ҫ����Ĺ˿ͣ����Բ������ڼƻ����У����Ǳ�����tolerantTime֮ǰ����ظ��Ƿ��ܷ���   
	vector<Customer*> dynamicCustomerSet;     // δ֪�Ĺ˿ͼ�
	vector<Car*> currentPlan;  
	// ��ǰ�ƻ�
	// �ڳ�ʼ����ʱ�򣬲�û�е�ǰ�ƻ�����ʱcurrentPlan������һ���ճ���������������ȡcapacity, depot��Ϣ
};

#endif

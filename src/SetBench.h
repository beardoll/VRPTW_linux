#ifndef _SETBENCH_H
#define _SETBENCH_H

#include "Customer.h"
#include<vector>

using namespace std;

class SetBench{  // �����benchmark���޸ģ����ɶ�̬�˿�����
public:
	SetBench(vector<Customer*> originCustomerSet, int timeSlotLen, int timeSlotNum, float dynamicism = 0.3);  // ���캯��
	~SetBench(){};  // ��������
	void constructProbInfo(); // ΪprobInfo��ֵ
	void construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet);   // ����˿�����
private:
	vector<Customer*> originCustomerSet; // ԭʼ�˿�����
	int timeSlotLen;  // ʱ��γ���
	int timeSlotNum;  // ʱ��θ���
	float dynamicism; // ��̬�̶�
};


#endif
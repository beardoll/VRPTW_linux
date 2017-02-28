#ifndef _EVENTELEMENT_H
#define _EVENTELEMENT_H

#include<string>
#include<iostream>
#include "Customer.h"
using namespace std;

enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
// �¼�  
// 1.�¹˿͵���
// 2.��������Ŀ�ĵ�
// 3.������ɷ���
// 4.��������
// 5.��ʱ��ε�����2-4����¼���

struct EventElement{
	float time;   // �¼�������ʱ��
	EventType eventType;  
	int carIndex;      // �¼������ĳ������
	int customerId;    // �¼������Ĺ˿ͽڵ�id
	EventElement(const float time = -1, const EventType eventType = carArrived, const int carIndex = -1, const int customerId = -1){ // ���캯��
		this->time = time;
		this->eventType = eventType;
		this->carIndex = carIndex;
		this->customerId = customerId;
	}  
	EventElement(const EventElement &item){  // ���ƹ��캯��
		this->time = item.time;
		this->eventType = item.eventType;
		this->carIndex = item.carIndex;
		this->customerId = item.customerId;
	}
	EventElement& operator= (const EventElement& item) {
		this->time = item.time;
		this->carIndex = item.carIndex;
		this->eventType = item.eventType;
		this->customerId = item.customerId;
		return *this;
	}
};

#endif
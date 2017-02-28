#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Customer.h"

using namespace std;


class Route{
public:
	Route(Customer &headNode, Customer &rearNode, float capacity);      // ���캯��
	~Route();   // ��������
	Route(const Route &L);  // ���ƹ��캯��
	Route& operator= (const Route &L);  // ���ظ�ֵ����� 
	Customer& operator[] (int k);     // �õ������еĵ�k��Ԫ��
	const Customer& operator[] (int k) const; // ��̬����
	bool isEmpty();   // �ж������Ƿ�Ϊ��
	void printRoute();   // ��ӡ·��
	void clear();     // �������

	// �����Լ�ɾ���ڵ����
	bool insertAfter(Customer &item1, Customer &item2); // ����������item1��ͬ�Ľڵ�������ڵ�item2
	bool insertAtHead(Customer &item);   // �ڱ�ͷ����item
	bool insertAtRear(Customer &item);    // �ڱ�β����item
	bool deleteNode(Customer &item);   // ��������ɾ����item��ͬ�Ľڵ�

	// ��õ��ڵ����
	Customer& currentPos();   // ���ص�ǰλ��
	Customer& nextPos() {return *(current->next);}      // ��һ���ڵ��λ��
	Customer& pastPos() {return *(current->front);}     // ǰһ���ڵ��λ��
	Customer& getHeadNode();    // �õ�ͷ���
	Customer& getRearNode();    // �õ�β�ڵ�

	// ��ȡ��������
	float getQuantity() {return this->quantity;}            // �õ�������ʹ�õ�����(װ�ع˿�)
	float getLeftQuantity() {return this->leftQuantity;}    // �õ�����ʣ���������복����ʻ�����أ�
	float getCapacity() {return this->capacity;}            // ���ر����ĳ�����
	float getLen(float DTpara[], bool artificial = false);  // �õ�·������
	int getSize();           // �õ���ǰ����Ĵ�С
	vector<float> getArrivedTime();      // �õ��������нڵ��arrivedTime
	vector<Customer*> getAllCustomer();  // �õ�·�������еĹ˿ͽڵ�

	// �޸���������
	void decreaseLeftQuantity(float amount) {leftQuantity -= amount;}   
		// ����ʣ��ĳ��������������µĹ˿ͣ�
	void setLeftQuantity(float newValue) {leftQuantity = newValue;}
	void setCurrentArrivedTime(float time) {current->arrivedTime = time;}      
		// ���õ�ǰ�ڵ�ĵ���ʱ��
	bool moveForward();      // ��ǰ��

	// �������/ɾ���ڵ����
	vector<float> computeReducedCost(float DTpara[], bool artificial = false);  // �������нڵ���Ƴ�����
	void computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2,
		float pertubation = 0.0f, bool regularization = true);  
		// ����item�ڵ���·���е���С������ۺʹ�С�������
		// ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
		// penaltyParaΪ�ͷ�ϵ����������Ҫ�ͷ���penaltyPara = 0
	bool timeWindowJudge(Customer *pre, int pos, Customer item);
		// �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
	void refreshArrivedTime();   
		// ����һ�¸����ڵ�ĵ���ʱ��

	// ·�����滻����ȡ
	void replaceRoute(const Route &route);  
		// ��route�滻��currentָ����·��
	Route& capture();  
		// ץȡcurrentָ����·��

	// ���ศ�����ʵĺ���
	vector<int> removeInvalidCustomer(vector<int> validCustomerId);
	bool checkPassRoute();
private:
	Customer *head, *current, *rear;  // ��ͷ����β�͵�ǰָ�룬��ǰָ��ָ�������ǰ��פ��
	int size;         // ����ĳ���
	float quantity;   // ��ǰ��ʹ�õ�����(��װ�صĹ˿������й�)
	float leftQuantity;   // ʣ���������복����ʻ����йأ�
	float capacity;       // �������������ﱣ��
	void copy(const Route& L);  // �������������ƹ��캯�������ء�=������ʹ��
	vector<float> arrivedTime;  // �ڸ����ڵ�ĵ���ʱ��
};


#endif
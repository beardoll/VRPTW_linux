#include "Route.h"
#include "Customer.h"
#include<iostream>
#include<cassert>
#include<vector>
#include<cmath>
#include<limits>
#include<ctime>
#include<cstdlib>
#include<algorithm>

const float MAX_FLOAT = numeric_limits<float>::max();

using namespace std;
Route::Route(Customer &headNode, Customer &rearNode, float capacity):capacity(capacity){ // ���캯��
	head = new Customer;
	*head = headNode;  // ���ƽڵ�
	rear = new Customer;
	*rear = rearNode; 
	head->front = NULL;
	head->next = rear;
	rear->front = head;
	rear->next = NULL;
	current = head;  // ��ʼ��currentָ��ָ��head�ڵ�
	size = 0;
	arrivedTime.push_back(head->arrivedTime);
	quantity = 0;
	leftQuantity = capacity;
}

Route::~Route(){ // ��������
	this->clear();
}

//=============== ����������� ================//
void Route::copy(const Route &L){
	// Ӧ����L��head�ڵ��rear�ڵ㶼����
	// ����֮�⣬ע��private�����е����ݳ�Ա��Ҫ���ƹ���
	this->size = L.size;
	this->capacity = L.capacity;
	this->quantity = L.quantity;
	this->leftQuantity = L.leftQuantity;
	this->arrivedTime = L.arrivedTime;
	Customer* originPtr = L.head;
	Customer* copyPtr = head;
	Customer* temp = NULL;
	while(originPtr!=NULL){
		// ��ͷ�ڵ�һֱ���Ƶ�β�ڵ�
		if(originPtr == L.head){  // ���ڸ��Ƶ�һ���ڵ�
			copyPtr = new Customer;
			copyPtr->front = NULL;
			head = copyPtr;
			*copyPtr = *L.head;
		} else{
			temp = new Customer;
			*temp = *originPtr;
			temp->front = copyPtr;
			copyPtr->next = temp;
			copyPtr = temp;
		}
		if(L.current == originPtr){
			// currentָ��ĸ���
			current = copyPtr;
		}
		originPtr = originPtr->next;
	}
	temp->next = NULL;
	rear = temp;
}

Customer& Route::operator[] (int k){
	assert(k>=0 && k<size);
	Customer* temp = head->next;
	for(int i=0; i<k; i++){
		temp = temp->next;
	}
	return *temp;
}

const Customer& Route::operator[] (int k) const{
	assert(k>=0 && k<size);
	Customer* temp = head->next;
	for(int i=0; i<k; i++){
		temp = temp->next;
	}
	return *temp;
}

Route::Route(const Route &L){ // ���ƹ��캯��	
	this->copy(L);
}


Route& Route::operator= (const Route &L){ 
	// ����"="�����������������
	this->clear();  // ��յ�ǰ����	
	this->copy(L);
	return *this;
}

bool Route::isEmpty(){ //�ж������Ƿ�Ϊ��
	return (size==0);
}

void Route::clear(){  // ������������head�ڵ��rear�ڵ�?
	Customer* ptr1 = head;
	Customer* ptr2;
	while(ptr1!=NULL){
		ptr2 = ptr1->next;
		delete ptr1;
		ptr1 = ptr2;
	}
	head = NULL;
	rear = NULL;
	current = NULL;
	size = 0;
}

void Route::printRoute(){ // ��ӡ����
	Customer* ptr = head;
	for(; ptr!=NULL; ptr=ptr->next) {
		cout << "id:" << ptr->id << ' ' << "type:" << ' ' << ptr->type << endl;
	}
}


//=============== �����Լ�ɾ���ڵ���� ================//
bool Route::insertAfter(Customer &item1, Customer &item2){
	// ����������item1��ͬ�Ľڵ�������ڵ�item2
	Customer* temp = new Customer;
	*temp = item2;
	Customer* ptr = head;
	while(ptr!=rear){
		if (ptr->id == item1.id){  // ����id���ж������ڵ��Ƿ���ͬ
			break;
		}
		ptr = ptr->next;
	}
	if(ptr == rear) {
		// û���ҵ�������false
		delete temp;
		return false;
	} else{
		quantity = quantity + item2.quantity;
		temp->next = ptr->next;
		ptr->next->front = temp;
		temp->front = ptr;
		ptr->next = temp;
		size++;
		refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
		return true;
	}
}

bool Route::insertAtHead(Customer &item){ 
	// �ڱ�ͷ����item
	// ֻ�е�currentָ��Ϊheadʱ����true
	if(current == head) {
		Customer *temp = new Customer;
		*temp = item;
		temp->next = head->next;
		head->next->front = temp;
		head->next = temp;
		temp->front = head;
		quantity = quantity + item.quantity;
		size++;
		refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
		return true;
	}
	else{
		return false;
	}
}

bool Route::insertAtRear(Customer &item){   
	// �ڱ�β����item
	// ֻ�е���β����current�ڵ�ʱ����true
	if(current != rear) {
		Customer *temp = new Customer;
		*temp = item;
		temp->next = rear;
		temp->front = rear->front;
		rear->front->next = temp;
		rear->front = temp;
		quantity = quantity + item.quantity;
		size++;
		refreshArrivedTime();  // ����ڵ�󣬸���arrivedTime
		return true;
	} else {
		return false;
	}
}

bool Route::deleteNode(Customer &item){
	// ɾ����������item��ͬ�Ľڵ�
	// ֻ��ɾ��currentָ�����Ľڵ�
	if(current == rear) {
		// �Ѿ�������·���е����нڵ㣬��ֹɾ��
		return false;
	}
	Customer* temp1 = current->next;
	while(temp1!=rear) {
		if(temp1->id == item.id) {  // ����ҵ���temp1ָ��ǰ�ڵ㣬temp2->next=temp1;
			break;
		}
		temp1 = temp1->next;
	}
	if(temp1 == rear) {  // û���ҵ�
		return false;
	} else {
		Customer* nextNode = temp1->next;
		Customer* frontNode = temp1->front;
		frontNode->next = nextNode;
		nextNode->front = frontNode;
		delete temp1;
		size--;
		quantity = quantity - item.quantity;
		refreshArrivedTime();  // ɾ���ڵ�󣬸���arrivedTime
		return true;
	}
}


//=============== ��õ��ڵ���� ================//
Customer& Route::currentPos(){ // ���ص�ǰλ��
	return *current;
}

Customer& Route::getHeadNode() {
	Customer* newCust = new Customer(*head);
	return *newCust; 
}

Customer& Route::getRearNode() {
	Customer* newCust = new Customer(*rear);
	return *newCust; 
}


//=============== ��ȡ�������� ================//
int Route::getSize() {
	return this->size;
}

vector<Customer*> Route::getAllCustomer(){  // �õ�·�������еĹ˿ͽڵ�
	// ���ص�customer����new�����ĶѶ�������ڴ�������ע��˴�
	vector<Customer*> customerSet(size);
	Customer* ptr = head->next;
	Customer* ptr2;
	for(int i=0; i<size; i++){
		ptr2 = new Customer;
		*ptr2 = *ptr;
		customerSet[i] = ptr2;
		ptr = ptr->next;
	}
	return customerSet;
}

float Route::getLen(float DTpara[], bool artificial){   // �õ�·������
	// ����ֵΪʵ�ʵ�·�����ȼ��ϳͷ�����
	// ��ȡDTpara
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);

	Customer *ptr1 = head;
	Customer *ptr2 = head->next;
	if(artificial == false) { // real vehicle routing scheme
		float len = 0;
		while(ptr2 != NULL){
			float temp1 = 0;
			switch(ptr1->priority){
			case 0:
				temp1 = 0.0f;
				break;
			case 1:
				temp1 = -DTH1;
				break;
			case 2:
				temp1 = -DTL1;
				break;
			}
			len = len + sqrt(pow(ptr1->x - ptr2->x, 2)+pow(ptr1->y - ptr2->y, 2));
			len += temp1;
			ptr2 = ptr2->next;
			ptr1 = ptr1->next;
		}
		return len;
	} else {
		float len = 0;
		while(ptr2 != NULL){
			float temp1 = 1.0f;
			switch(ptr1->priority){
			case 0:
				temp1 = 0.0f;
				break;
			case 1:
				temp1 = DTH2;
				break;
			case 2:
				temp1 = DTL2;
				break;
			}
			len = len + sqrt(pow(ptr1->x - ptr2->x, 2)+pow(ptr1->y - ptr2->y, 2));
			len += temp1;
			ptr2 = ptr2->next;
			ptr1 = ptr1->next;
		}
		return len;		
	}
}

vector<float> Route::getArrivedTime(){     // �õ��������нڵ��arrivedTime
	return arrivedTime;
}


//=============== �޸��������� ================//
bool Route::moveForward(){
	current = current->next;
	if(current == NULL) {  // �Ѿ��������
		return false;
	} else {
		return true;
	}
}


//=============== �������/ɾ���ڵ���� ================//
vector<float> Route::computeReducedCost(float DTpara[], bool artificial){ 
	// �õ����й˿ͽڵ���Ƴ�����
	// ֵԽС��ʾ�Ƴ������Խ�ʡ����Ĵ���
	// artificial: Ϊtrue��ʾ��һ����ٵĳ�
	// �����Ҫ�������Ƴ����ۣ��������DT��ȡΪ0����
	float DTH1, DTH2, DTL1, DTL2;
	float *DTIter = DTpara;
	DTH1 = *(DTIter++);
	DTH2 = *(DTIter++);
	DTL1 = *(DTIter++);
	DTL2 = *(DTIter++);
	vector<float> costArr(0);
	Customer *ptr1 = head;   // ǰ�ڵ�
	Customer *ptr2, *ptr3;
	for(int i=0; i<size; i++){
		ptr2 = ptr1->next;  // ��ǰ�ڵ�
		ptr3 = ptr2->next;  // ��ڵ�
		float temp =  -sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2)) - 
			sqrt(pow(ptr2->x - ptr3->x, 2) + pow(ptr2->y - ptr3->y, 2)) +
			sqrt(pow(ptr1->x - ptr3->x, 2) + pow(ptr1->y - ptr3->y, 2));
		float temp1 = 0;
		if(artificial == true) {
			switch(ptr1->priority){
			case 0:
				temp1 = 0;
				break;
			case 1:
				temp1 = -DTH2;
				break;
			case 2:
				temp1 = -DTL2;
				break;
			}
		} else {
			switch(ptr1->priority){
			case 0:
				temp1 = 0;
				break;
			case 1:
				temp1 = DTH1;
				break;
			case 2:
				temp1 = DTL1;
				break;
			}		
		}
		temp += temp1;
		costArr.push_back(temp);
		ptr1 = ptr1->next;
	}
	return costArr;
} 

bool Route::timeWindowJudge(Customer *pre, int pos, Customer item){  
	// �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
	// ��ʱ�����ǲֿ�Ĺز�ʱ��
	// pos��pre��λ��, 0��ʾ�ֿ�
	float time = arrivedTime[pos];
	Customer *ptr1, *ptr2;

	// ���������жϲ���item��᲻��Υ��item�������̽ڵ��ʱ�䴰Լ��
	if(time < pre->startTime){   // arrivedTime[pos]ֻ�ӵ���pre��arrived time��û���ж��Ƿ���ǰ����
		time = pre->startTime;
	}
	time += pre->serviceTime;
	time = time + sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2));
	if(time > item.endTime) {  // Υ����ʱ�䴰Լ��
		return false;
	} else{
		if(time < item.startTime) {
			time = item.startTime;
		}
		time = time + item.serviceTime;
		ptr2 = pre->next;
		if(ptr2 == rear){  // item��������յ㣬��ʱ������
			return true;
		} else {
			time = time + sqrt(pow(ptr2->x - item.x, 2) + pow(ptr2->y - item.y, 2));
			if(time > ptr2->endTime) {
				return false;
			} else {
				if(time < ptr2->startTime) {
					time = ptr2->startTime;
				}
				time = time + ptr2->serviceTime;
			}
		}
	}

	// Ȼ���жϻ᲻��Υ��������Ľڵ��ʱ�䴰Լ��
	bool mark = true;
	ptr1 = pre->next;
	ptr2 = ptr1->next;
	while(mark == true && ptr2 !=rear){ 
		time = time + sqrt(pow(ptr1->x - ptr2->x, 2) + pow(ptr1->y - ptr2->y, 2));	
		if(time > ptr2->endTime){
			mark = false;
			break;
		} else {
			if(time < ptr2->startTime){
				time = ptr2->startTime;
			}
			time = time + ptr2->serviceTime;
		}
		ptr1 = ptr1->next;
		ptr2 = ptr2->next;
	}
	return mark;
}

void Route::computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2,
		float pertubation, bool regularization){
	// ����item�ڵ���·���е���С������ۺʹ�С�������
	// ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
	// pertubation: �Ŷ���������
	// regularization: Ϊtrue��ʾ�������ȡ�Ǹ�����Ϊfalse��ʾ��ȡ����
	// penaltyPara: ����������ʱʩ�ӵĳͷ���
	Customer *pre = current;   // ֻ�ܲ��뵽δ�߹��Ľڵ�ǰ
	Customer *succ = pre->next;
	minValue = MAX_FLOAT;
	secondValue = MAX_FLOAT;
	customer1.id = -1;
	customer2.id = -1;
	int startPos = 0;
	Customer* temp = head;
	while(temp!= pre) {
		temp = temp->next;
		startPos++;
	}
	for(int i=startPos; i<=size; i++) {  // һ����size+1��λ�ÿ��Կ��ǲ���
		if(quantity + item.quantity <= capacity){   // ����Լ��
			if(timeWindowJudge(pre, i, item) == true) { // ����ʱ�䴰Լ��
				float temp = sqrt(pow(pre->x - item.x, 2) + pow(pre->y - item.y, 2)) +
						sqrt(pow(item.x - succ->x, 2) + pow(item.y - succ->y, 2)) -
						sqrt(pow(pre->x - succ->x, 2) + pow(pre->y - succ->y, 2));
				temp += pertubation;
				if(regularization == true) {
					temp = max(0.0f, temp);
				}
				if(temp <= minValue){  // �ҵ��˸�С�ģ�����minValue��secondValue
					secondValue = minValue;
					customer2 = customer1;
					minValue = temp;
					customer1 = *pre;
				}
			}
		}
		pre = pre->next;
		if(succ != rear){
			succ = succ->next;
		}
	}
}

void Route::refreshArrivedTime(){   
	// ����һ�¸����ڵ�ĵ���ʱ��
	// ͷ����arrivedTime + serviceTime����Ϊ��׼ʱ��
	arrivedTime.clear();
	Customer* tfront = head;
	while(tfront != current->next){
		// ��ͷ��㵽current�ڵ�֮ǰ��arrivedTime������Ҫ���¼���
		arrivedTime.push_back(tfront->arrivedTime);
		tfront = tfront->next;
	}
	tfront = current;
	Customer* tcurrent = current->next;
	float time = current->arrivedTime + current->serviceTime;
	while(tcurrent != rear){
		// current�ڵ�����arrivedTime��Ҫ���¼���
		time = time + sqrt(pow(tfront->x - tcurrent->x, 2) + pow(tfront->y - tcurrent->y, 2));
		arrivedTime.push_back(time);
		// tcurrent->arrivedTime = time;
		if(time < tcurrent->startTime){
			time = tcurrent->startTime;
		}
		time = time + tcurrent->serviceTime;
		tfront = tfront->next;
		tcurrent = tcurrent->next;
	}
}


//=============== ·�����滻����ȡ ================//
Route& Route::capture(){ 
	// ץȡcurrentָ����·��
	// currentָ�뵱ǰ�ڵ㽫��Ϊhead�ڵ�
	// ����ǰ·����capacity��leftQuantityԭ������
	Route* ptr1 = new Route(*current, *rear, capacity);
	if(current->next == rear) { // currentָ����Ѿ�û��·��
		return *ptr1;
	}
	Customer *ptr2 = current->next;
	Customer *ptr3 = NULL;
	Customer *ptr4 = NULL;
	ptr4 = ptr1->head;
	while(ptr2 != rear) {
		ptr3 = new Customer;
		*ptr3 = *ptr2;
		ptr4->next = ptr3;
		ptr3->front = ptr4;
		ptr4 = ptr3;
		ptr1->quantity = ptr1->quantity + ptr2->quantity;
		ptr2 = ptr2->next;
		ptr1->size++;
	}
	ptr3->next = ptr1->rear;
	ptr1->rear->front = ptr3;
	ptr1->setLeftQuantity(leftQuantity);
	ptr1->refreshArrivedTime();   // ����part route��arrivedTime
	return *ptr1;
}

void Route::replaceRoute(const Route &route) {  // ��route�滻��currentָ����·��
	Customer* ptr1;
	Customer *ptr2, *ptr3;
	if(current->next != rear) { // current���滹�нڵ㣬��Ҫ�����ԭ��·��
		ptr2 = current->next;
		// ���ԭ·����currentָ������Ԫ��
		// ��������rear�ڵ�����
		while(ptr2 != rear) { 
			quantity = quantity - ptr2->quantity;
			ptr3 = ptr2->next;
			delete ptr2;
			ptr2 = ptr3;
			size--;
		}
	}
	// ��route�г�head��rear��Ľڵ㶼���Ƶ�currentָ���
	ptr1 = route.head->next;
	ptr3 = current;
	while(ptr1 != route.rear){  
		quantity = quantity + ptr1->quantity;
		ptr2 = new Customer;
		*ptr2 = *ptr1;
		ptr3->next = ptr2;
		ptr2->front = ptr3;
		ptr3 = ptr2;
		ptr1 = ptr1->next;
		size++;
	}
	ptr3->next = rear;
	rear->front = ptr3;
	refreshArrivedTime();   // ������·����refreshһ��arrivedTime
	return;
}


//=============== ���ศ�����ʵĺ��� ================//
bool Route::checkPassRoute(){
	// ������߹���·���Ƿ�Υ��ʱ�䴰Լ��
	// ����ֻ��鵽����һ���ڵ��ʱ���Ƿ�С��ǰһ���ڵ��ʱ�䴰��ʼʱ��
	Customer* ptr1 = head;
	Customer* ptr2 = head->next;
	bool mark = true;
	if(current == head) {  // ���ӻ�û�Ӳֿ������������
		return true;
	} else {
		while(ptr2 != current->next) {
			// һֱ��鵽current�ڵ�
			float leastArriveTime = ptr1->startTime;  // ������һ���ڵ�����ʱ��
			if(ptr2->arrivedTime < leastArriveTime) {
				mark = false;
			}
			ptr1 = ptr1 -> next;
			ptr2 = ptr2 -> next;
		}
		return mark;
	}
}

vector<int> Route::removeInvalidCustomer(vector<int> validCustomerId){
	// ������id��validCustomerId�еĽڵ�
	// ���ر����Ľڵ���validCustomerId�е�λ��
	vector<int> posVec;
	posVec.push_back(0);   // �ֿ�ڵ�λ��
	Customer* ptr1 = head->next;
	while(ptr1 != rear) {
		int currentId = ptr1->id;
		vector<int>::iterator intIter = find(validCustomerId.begin(), validCustomerId.end(), currentId);
		if(intIter == validCustomerId.end()) {
			// ����Ҳ�����˵���ýڵ���invalid��ɾ��֮
			quantity -= ptr1->quantity;
			size--;
			ptr1->front->next = ptr1->next;
			ptr1->next->front = ptr1->front; 
		} else {
			int pos = intIter - validCustomerId.begin();
			posVec.push_back(pos);
		} 
		ptr1 = ptr1->next;
	}
	posVec.push_back(0);  // �ֿ�ڵ�λ��
	return posVec;
}
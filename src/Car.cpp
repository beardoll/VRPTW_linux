#include "Car.h"
#include "PublicFunction.h"
#include<cmath>

Car::Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial):
	carIndex(index), route(headNode, rearNode, capacity), artificial(artificial){
	state = wait;
	nearestDepatureTime = 0;
	nextArriveTime = 0;
}

Car::~Car(){  // ��Ƕ��������������ᱻ���ã������ڴ˴�delete route

}

Car::Car(Car &item):route(item.route), artificial(item.artificial){  //���ƹ��캯��
	this->state = item.state;
	this->carIndex = item.carIndex;
	this->state = item.state;
	this->artificial = item.artificial;
	this->nearestDepatureTime = item.nearestDepatureTime;
	this->nextArriveTime = item.nextArriveTime;
}


Car& Car::operator= (Car &item){ // ���ظ�ֵ����
	this->route = item.route;
	this->carIndex = item.carIndex;
	this->state = item.state;
	this->artificial = item.artificial;
	this->nearestDepatureTime = item.nearestDepatureTime;
	this->nextArriveTime = item.nextArriveTime;
	return *this;
}


//================ �õ��������� =================//
Car& Car::getNullCar(){
	float leftCapacity = route.getLeftQuantity();
	Car* newCar = new Car(getHeadNode(), getRearNode(), leftCapacity, carIndex);
	return *newCar;
}


//================ insert cost��remove cost =================//
void Car::computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2, 
		float noiseAmount, bool noiseAdd, float penaltyPara, bool regularization){
	// ����item�ڵ㵽����·������С������ۺʹ�С�������
	// noiseAmount: �������������regularization = trueʱ��Ч
	// penaltyPara: ����������ʱ�ĳͷ�������regularizationΪfalseʱ��Ч
	// ��regularizationΪtrueʱ��penaltyPara = 0
	// time: ����insertCostʱ��ʱ��
	float pertubation = penaltyPara;   // �Ŷ���
	if(noiseAdd == true) {
		// ������������
		if(regularization == false) {
			pertubation -= penaltyPara * random(0,1);
		} else {
			pertubation += -noiseAmount + 2*noiseAmount*random(0,1);
		}
	} else{  
		// ���򣬲���Ҫ�������
		pertubation += 0;
	}
	route.computeInsertCost(item, minValue, customer1, secondValue, customer2, pertubation, regularization);
}

vector<float> Car::computeReducedCost(float DTpara[]){  
	// ���㳵��Ҫ��������нڵ���Ƴ�����
	// DTpara: ��ͬ���ȼ��˿ͼ���reduce costʱ��bonus
	return route.computeReducedCost(DTpara, artificial);
}


//================ insert �� delete Customer���� =================//
bool Car::insertAtRear(Customer &item){
	return route.insertAtRear(item);
} 

bool Car::insertAtHead(Customer &item){
	return route.insertAtHead(item);
}

bool Car::insertAfter(const Customer &item1, const Customer &item2) {
	// �򵥵���item1�������item2
	Customer itema, itemb;
	itema = item1;
	itemb = item2;
	if(item1.id == 0) {
		// ˵�����뵽����ʼ�����
		// ����ʼ����Ķ����capturePartRoute()
		// ������Ҫ����һ��nextArriveTime
		// ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
		Customer itemx = route.currentPos();  // �������פ�㣨��ʵ��·;�У�פ��ָ���ǹ˿͵㣩
		float currentTime = nearestDepatureTime + sqrt(pow(item1.x - itemx.x, 2) + pow(item1.y - itemx.y, 2));
		nextArriveTime = currentTime + sqrt(pow(item1.x - item2.x, 2) + pow(item1.y - item2.y, 2));
		itema = itemx;
	}

	return route.insertAfter(itema, itemb);
}

bool Car::deleteCustomer(Customer &item) {
	return route.deleteNode(item);
}


//================ part Route���� =================//
void Car::replaceRoute(Car &newCar, float currentTime){      
	// ��newCar��·�����뵽��ǰ����·����current�ڵ�֮��
	updateState(currentTime);  // �ȸ���״̬
	Customer currentNode = route.currentPos();  // ���������
	Customer originNextNode = route.nextPos();  // ԭĿ�ĵ�
	route.replaceRoute(newCar.getRoute());      // replaceRoute�����Ļ�����״̬
	Customer changedNextNode = route.nextPos(); // ���ĺ��Ŀ�ĵ�
	if(state == depature && originNextNode.id != changedNextNode.id) {
		// ���������·;�У����Ҹ���·����Ŀ�ĵظı䣬��Ӧ���޸�nextArriveTime����
		float dist = nextArriveTime - nearestDepatureTime;
		// x,y�ǻ�����ǰλ��
		float x = (currentTime - nearestDepatureTime) / dist * (originNextNode.x - currentNode.x) + currentNode.x;
		float y = (currentTime - nearestDepatureTime) / dist * (originNextNode.y - currentNode.y) + currentNode.y;
		nextArriveTime = currentTime + sqrt(pow(x - changedNextNode.x, 2) + pow(y - changedNextNode.y, 2));
	}
}

Car& Car::capturePartRoute(float time){   
	// ץȡroute��currentָ��֮���·�������ҷ���һ����
	// timeΪץȡ��ʱ��
	updateState(time);    // �Ƚ�״̬����
	Customer currentNode = route.currentPos();  // �Ӹõ����
	Customer nextNode = route.nextPos();        // ��һվĿ�ĵ�
	Customer *startNode = new Customer;         // ���ӵĳ�����
	startNode->id = 0;   
	startNode->type = 'D';
	startNode->priority = 0;
	switch(state) {
	case depature: {
		// ������·;�У���������ĳ�ʼ��
		// �õ����λ��λ�ڳ������Ŀ�ĵ������ϵ�ĳһ��
		// �õ��arrivedTime�趨Ϊ��ǰʱ�䣬������ʱ��Ϊ0���Ͳֿ�һ��
		float dist = nextArriveTime - nearestDepatureTime;
		startNode->x = (time - nearestDepatureTime) / dist * (nextNode.x - currentNode.x) + currentNode.x;
		startNode->y = (time - nearestDepatureTime) / dist * (nextNode.y - currentNode.y) + currentNode.y;
		startNode->arrivedTime = time;
		startNode->serviceTime = 0;
		startNode->startTime = time;
		break;
				   }
	case wait: {
		// ���Ӵ��ڵȴ�״̬��ֱ��ȡ��ǰ�ڵ���Ϊ��ʼ��
		// ������ʱ���Գ���������serviceTimeΪ0
		startNode->x = currentNode.x;
		startNode->y = currentNode.y;
		startNode->arrivedTime = time;
		startNode->serviceTime = 0;
		startNode->startTime = time;
		break;
				   }
	case serving: {
		// ���ӵ�ǰ�ڷ���˿ͣ���ʼ��Ϊ��ǰ�����
		// ������ʱ���趨���������ʱ���ȥ��ǰʱ��
		// ע�⵱�����ڵȴ���Ϊ�˿ͷ���ʱ������Ҳ��״̬�趨Ϊserving
		// ��������˿͵������������nearestDepatureTime��������ǿ�������֮
		Customer currentPos = route.currentPos();
		startNode->x = currentNode.x;
		startNode->y = currentNode.y;
		startNode->arrivedTime = time;
		startNode->serviceTime = nearestDepatureTime - time;  
		// ����ʱ���Ѿ���ȥ��һ���֣�ע��˿͵����Ӧ��ȷ��arrivedTime
		// time - baseTime��ʾ�Ѿ��������ʱ��
		startNode->startTime = time;
		break;
				  }
	case offwork: {  
		// �ճ��˵ĳ����ǲ����õ�
		// ��ʱ����һ���ճ�������startNodeû���κ�����
		break;		
				  }
	}
	float leftQuantity = route.getLeftQuantity();  // ����ʣ������
	float capacity = route.getCapacity();
	Customer depot = route.getRearNode();          // �κ�һ�������յ㶼��depot
	Car* newCar = new Car(*startNode, depot, leftQuantity, carIndex, false);
	Route tempRoute = route.capture();                         // ץȡcurrentָ����·��
	vector<Customer*> tempCust = tempRoute.getAllCustomer();   // ���currentָ�������й˿�
	vector<Customer*>::iterator custIter;
	for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
		newCar->insertAtRear(**custIter);
	}
	deleteCustomerSet(tempCust);
	return *newCar;
}


//================ state��� =================//
void Car::updateState(float time){
	// ���»���״̬
	switch(state){
	case depature: {
		// ԭ״̬�ǳ���������һ״̬�ǻ���ִ�з���
		// ��������»�����nearestDepartureTime
		if(time == nextArriveTime) {
			// ����ǰʱ��������״̬�ı��ʱ�䣬��״̬�ı�
			route.moveForward();   // ִ�з��񣬸��ĵ�ǰפ��
			Customer currentPos = route.currentPos();  // ��ǰפ��
			route.currentPos().arrivedTime = time;     // ���µ�ǰ�˿͵ĵ���ʱ��
			if(currentPos.id == 0) {
				state = offwork;   // ����ֿ⣬�ճ�
			} else {
				Customer nextPos = route.nextPos();
				if(time < currentPos.startTime) {
					time = currentPos.startTime;
				}
				nearestDepatureTime = time + currentPos.serviceTime;
				if(nearestDepatureTime == time) {  
					// ��������Ȳ���Ҫ�ȴ���Ҳ����Ҫ�������������
					state = depature;
					nextArriveTime = nearestDepatureTime + sqrt(pow(currentPos.x - nextPos.x, 2) + pow(currentPos.y - nextPos.y, 2));
				} else {
					route.decreaseLeftQuantity(currentPos.quantity);
					state = serving;
				}
			}
		}
		break;
					}
	case serving: {
		// ԭ״̬��ִ�з�������һ״̬�ǻ�������
		Customer currentPos = route.currentPos();
		Customer nextPos = route.nextPos();
		if(time == nearestDepatureTime) {
			// ���Խ���״̬ת��
			state = depature;
			nextArriveTime = nearestDepatureTime + sqrt(pow(currentPos.x - nextPos.x, 2) + pow(currentPos.y - nextPos.y, 2));
		}
		break;
				  }
	case wait: {
		// do nothing now
		// �����������˻���֮����ܽ���״̬ת��
		break;
			   }
	case offwork: {
		// do nothing now
		// �ճ�֮����Ӧû�к�������
		break;
				  }
	}
}

EventElement Car::getCurrentAction(float time){        
	// ��û�����ǰʱ�̵Ķ���
	EventElement event;
	event.carIndex = carIndex;
	updateState(time);   // �ȸ���״̬
	Customer currentPos = route.currentPos();
	switch(state){
	case depature: {
		Customer nextPos = route.nextPos();
		event.time = nextArriveTime;
		event.eventType = carArrived;
		event.customerId = nextPos.id;
		break;
				   }
	case wait: {
		// do nothing	
		// ������Ч�¼���˵��������û������
		break;
			   }
	case serving: {
		Customer currentPos = route.currentPos();
		event.time = nearestDepatureTime;
		event.customerId = currentPos.id; 
		event.eventType = finishedService;
		break;
				  }
	case offwork: {
		// �ճ�
		event.time = time;
		event.customerId = 0;
		event.eventType = carOffWork;
		break;
				  }
	}
	return event;
}

EventElement Car::launchCar(float currentTime){         
	// ��������������������wait״̬ʱ��Ч
	EventElement event;
	if(state == wait && route.getSize() != 0) {
		// �������й˿͵�ʱ�Ż�����
		state = depature;
		Customer currentPos = route.currentPos();  // ��ǰפ��
		Customer nextPos = route.nextPos();        // ��һĿ�ĵ�
		nearestDepatureTime = currentTime;
		float time = currentTime + sqrt(pow(currentPos.x - nextPos.x, 2) + pow(currentPos.y - nextPos.y, 2));
		nextArriveTime = time;
		event.time = time;
		event.eventType = carArrived;
		event.carIndex = carIndex;
		event.customerId = nextPos.id;
	}
	return event;
}


//================ assessment��� =================//
void Car::removeInvalidCustomer(vector<int> validCustomerId){
	// �Ƴ�·���е���Ч�˿�
	// ��¼���������Ĺ˿���removeCustomerId�е�λ�ã�posVec
	posVec = route.removeInvalidCustomer(validCustomerId);
}


void Car::updateTransformMatrix(Matrix<int> &transformMatrix){
	// ��transformMatrix���и���
	// transformMatrix����λ�ö�Ӧ�˿���validCustomerId��ָ��
	assert(posVec.size() != 0);  // Ϊ0��ʾû�н���removeInvalidCustomer�Ĳ���
	vector<int>::iterator intIter;
	for(intIter = posVec.begin(); intIter < posVec.end() - 1; intIter++) {
		int frontPos, backPos;
		frontPos = *(intIter);
		backPos = *(intIter+1);
		int temp = transformMatrix.getElement(frontPos, backPos);
		transformMatrix.setValue(frontPos, backPos, temp+1);
	}
}

int Car::computeScore(Matrix<int> transformMatrix){
	// ���㵱ǰ������·����transformMatrixָ���µĵ÷�
	// transformMatrix����λ�ö�Ӧ�˿���validCustomerId��ָ��
	assert(posVec.size() != 0);  // Ϊ0��ʾû�н���removeInvalidCustomer�Ĳ���
	vector<int>::iterator intIter;
	int score = 0;
	for(intIter = posVec.begin(); intIter < posVec.end() - 1; intIter++) {
		int frontPos, backPos;
		frontPos = *(intIter);
		backPos = *(intIter+1);
		score += transformMatrix.getElement(frontPos, backPos);
	}
	return score;
}

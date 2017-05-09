#include "LoadBench.h"
#include "LoadFile.h"
#include "tinystr.h"
#include "tinyxml.h"
#include<iostream>

static const int timeSlotNum = 6;

void loadXmlToVector(vector<Customer*> &customerSet, TiXmlElement *nodeElem) {
	int tempINT;
	float tempFLOAT;
	for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement()) { // ������ȡnode�ڵ����Ϣ
		Customer *customer = new Customer;
		TiXmlHandle node(nodeElem);  // nodeElem��ָ��Ľڵ�
		TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx�ڵ�
		TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy�ڵ�
		TiXmlElement *quantity = node.FirstChild("quantity").Element();
		TiXmlElement *startTime = node.FirstChild("startTime").Element();
		TiXmlElement *endTime = node.FirstChild("endTime").Element();
		TiXmlElement *arrivedTime = node.FirstChild("arrivedTime").Element();
		TiXmlElement *tolerantTime = node.FirstChild("tolerantTime").Element();
		TiXmlElement *serviceTime = node.FirstChild("serviceTime").Element();
		nodeElem->QueryIntAttribute("id", &tempINT);  //��id�ŵ�tempINT�У�����ֵ����
		customer->id = tempINT;
		nodeElem->QueryIntAttribute("property", &tempINT);  //��property�ŵ�tempINT�У�����ֵ����
		customer->prop = tempINT;
		customer->type = 'P';
		tempFLOAT = (float)atof(xElem->GetText());    // charתfloat
		customer->x = tempFLOAT;
		tempFLOAT = (float)atof(yElem->GetText());
		customer->y = tempFLOAT;
		tempFLOAT = (float)atof(quantity->GetText());    // charתfloat
		customer->quantity = tempFLOAT;
		tempFLOAT = (float)atof(startTime->GetText());
		customer->startTime = tempFLOAT;
		tempFLOAT = (float)atof(endTime->GetText());    // charתfloat
		customer->endTime = tempFLOAT;
		tempFLOAT = (float)atof(arrivedTime->GetText());
		customer->arrivedTime = tempFLOAT;
		tempFLOAT = (float)atof(tolerantTime->GetText());
		customer->tolerantTime = tempFLOAT;
		tempFLOAT = (float)atof(serviceTime->GetText());
		customer->serviceTime = tempFLOAT;
		tempFLOAT = (float)atof(serviceTime->GetText());
		customer->serviceTime = tempFLOAT;
		customer->priority = 0;
		TiXmlElement *ProbInfo = node.FirstChild("ProbInfo").Element();
		TiXmlHandle node2(ProbInfo);
		TiXmlElement *timeProb = node2.FirstChild("timeProb").Element();
		int count = 0;
		for (timeProb; timeProb; timeProb = timeProb->NextSiblingElement()) {
			tempFLOAT = (float)atof(timeProb->GetText());
			customer->timeProb[count++] = tempFLOAT;
		}

		customerSet.push_back(customer);
	}
}

bool LoadBench::load(vector<Customer*> &staticCustomer, vector<Customer*> &dynamicCustomer, Customer &depot, float &capacity) {
	// ��ȡxml������allCustomers, depot, capacity��
	TiXmlDocument doc(path.c_str());     // ����XML�ļ�
	if (!doc.LoadFile()) return false;    // ����޷���ȡ�ļ����򷵻�
	TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
	TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
	pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
	TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�

	TiXmlElement* StaticCustomerElem = hRoot.FirstChild("StaticCustomer").FirstChild("Node").Element();  // cx�ڵ�
	loadXmlToVector(staticCustomer, StaticCustomerElem);

	TiXmlElement* dynamicCustomerElem = hRoot.FirstChild("DynamicCustomer").FirstChild("Node").Element();
	loadXmlToVector(dynamicCustomer, dynamicCustomerElem);

	float tempFLOAT;
	int tempINT;
	TiXmlElement* Depot = hRoot.FirstChild("Depot").Element();
	TiXmlHandle node(Depot);  // nodeElem��ָ��Ľڵ�
	Depot->QueryIntAttribute("id", &tempINT);  //��id�ŵ�temp1�У�����ֵ����
	depot.id = tempINT;
	TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx�ڵ�
	TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy�ڵ�
	tempFLOAT = (float)atof(xElem->GetText());    // charתfloat
	depot.type = 'D';
	depot.x = tempFLOAT;
	tempFLOAT = (float)atof(yElem->GetText());
	depot.y = tempFLOAT;
	depot.priority = 0;
	depot.startTime = 0;
	depot.serviceTime = 0;
	depot.arrivedTime = 0;
	depot.prop = 0;

	TiXmlElement* Vehicle = hRoot.FirstChild("Vehicle").Element();
	TiXmlHandle node2(Vehicle);  // nodeElem��ָ��Ľڵ�
	TiXmlElement* Capacity = node2.FirstChild("Capacity").Element();  // cx�ڵ�
	tempFLOAT = (float)atof(Capacity->GetText());
	capacity = tempFLOAT;
	return true;
}

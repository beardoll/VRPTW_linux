#include "LoadBench.h"
#include "LoadFile.h"
#include "tinystr.h"
#include "tinyxml.h"
#include<iostream>

static const int timeSlotNum = 6;

void loadXmlToVector(vector<Customer*> &customerSet, TiXmlElement *nodeElem) {
	int tempINT;
	float tempFLOAT;
	for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement()) { // 挨个读取node节点的信息
		Customer *customer = new Customer;
		TiXmlHandle node(nodeElem);  // nodeElem所指向的节点
		TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx节点
		TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy节点
		TiXmlElement *quantity = node.FirstChild("quantity").Element();
		TiXmlElement *startTime = node.FirstChild("startTime").Element();
		TiXmlElement *endTime = node.FirstChild("endTime").Element();
		TiXmlElement *arrivedTime = node.FirstChild("arrivedTime").Element();
		TiXmlElement *tolerantTime = node.FirstChild("tolerantTime").Element();
		TiXmlElement *serviceTime = node.FirstChild("serviceTime").Element();
		nodeElem->QueryIntAttribute("id", &tempINT);  //把id放到tempINT中，属性值读法
		customer->id = tempINT;
		nodeElem->QueryIntAttribute("property", &tempINT);  //把property放到tempINT中，属性值读法
		customer->prop = tempINT;
		customer->type = 'P';
		tempFLOAT = (float)atof(xElem->GetText());    // char转float
		customer->x = tempFLOAT;
		tempFLOAT = (float)atof(yElem->GetText());
		customer->y = tempFLOAT;
		tempFLOAT = (float)atof(quantity->GetText());    // char转float
		customer->quantity = tempFLOAT;
		tempFLOAT = (float)atof(startTime->GetText());
		customer->startTime = tempFLOAT;
		tempFLOAT = (float)atof(endTime->GetText());    // char转float
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
	// 读取xml内容于allCustomers, depot, capacity中
	TiXmlDocument doc(path.c_str());     // 读入XML文件
	if (!doc.LoadFile()) return false;    // 如果无法读取文件，则返回
	TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
	TiXmlElement* pElem;            // 指向元素的指针
	pElem = hDoc.FirstChildElement().Element(); //指向根节点
	TiXmlHandle hRoot(pElem);       // hRoot是根节点

	TiXmlElement* StaticCustomerElem = hRoot.FirstChild("StaticCustomer").FirstChild("Node").Element();  // cx节点
	loadXmlToVector(staticCustomer, StaticCustomerElem);

	TiXmlElement* dynamicCustomerElem = hRoot.FirstChild("DynamicCustomer").FirstChild("Node").Element();
	loadXmlToVector(dynamicCustomer, dynamicCustomerElem);

	float tempFLOAT;
	int tempINT;
	TiXmlElement* Depot = hRoot.FirstChild("Depot").Element();
	TiXmlHandle node(Depot);  // nodeElem所指向的节点
	Depot->QueryIntAttribute("id", &tempINT);  //把id放到temp1中，属性值读法
	depot.id = tempINT;
	TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx节点
	TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy节点
	tempFLOAT = (float)atof(xElem->GetText());    // char转float
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
	TiXmlHandle node2(Vehicle);  // nodeElem所指向的节点
	TiXmlElement* Capacity = node2.FirstChild("Capacity").Element();  // cx节点
	tempFLOAT = (float)atof(Capacity->GetText());
	capacity = tempFLOAT;
	return true;
}

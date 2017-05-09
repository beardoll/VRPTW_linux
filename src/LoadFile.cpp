#include "LoadFile.h"
#include "tinystr.h"
#include "tinyxml.h"
#include<iostream>

bool LoadFile::getData(vector<Customer*> &allCustomer, Customer &depot, float &capacity){
	// ��ȡxml������allCustomers, depot, capacity��
	string file = filePath + fileName;   // �ļ�·��+�ļ���
	TiXmlDocument doc(file.c_str());     // ����XML�ļ�
	if(!doc.LoadFile()) return false;    // ����޷���ȡ�ļ����򷵻�
	TiXmlHandle hDoc(&doc);         // hDoc��&docָ��Ķ���
	TiXmlElement* pElem;            // ָ��Ԫ�ص�ָ��
	pElem = hDoc.FirstChildElement().Element(); //ָ����ڵ�
	TiXmlHandle hRoot(pElem);       // hRoot�Ǹ��ڵ�
	Customer* customer;
	int tempINT;
	float tempFLOAT;

	// ��ȡx,y�����Ƿ���network->nodes->node�ڵ���
	TiXmlElement* nodeElem = hRoot.FirstChild("network").FirstChild("nodes").FirstChild("node").Element(); //��ǰָ����node�ڵ�
	for(nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement()) { // ������ȡnode�ڵ����Ϣ
		customer = new Customer;
		TiXmlHandle node(nodeElem);  // nodeElem��ָ��Ľڵ�
		TiXmlElement* xElem = node.FirstChild("cx").Element();  // cx�ڵ�
		TiXmlElement* yElem = node.FirstChild("cy").Element();  // cy�ڵ�
		nodeElem->QueryIntAttribute("id", &tempINT);  //��id�ŵ�temp1�У�����ֵ����
		if(tempINT == 0){  // depot�ڵ�
			depot.id = tempINT;
			depot.prop = 0;
			tempFLOAT = (float)atof(xElem->GetText());    // charתfloat
			depot.x = tempFLOAT;
			tempFLOAT = (float)atof(yElem->GetText());
			depot.y = tempFLOAT;
			depot.type = 'D';
			depot.serviceTime = 0;
			depot.arrivedTime = 0;
			depot.startTime = 0;
			depot.priority = 0;
		} else {  // ȡ����
			customer = new Customer;
			customer->id = tempINT;  
			nodeElem->QueryIntAttribute("property", 0);  //�������еĹ˿Ͷ���static��
			customer->prop = tempINT;
			tempFLOAT = (float)atof(xElem->GetText());    // charתfloat
			customer->x = tempFLOAT;
			tempFLOAT = (float)atof(yElem->GetText());
			customer->y = tempFLOAT;
			customer->type = 'P';
			customer->priority = 0;
			allCustomer.push_back(customer);
		}
	}

	 // ��ȡ������Ϣ
	TiXmlElement* requestElem = hRoot.FirstChild("requests").FirstChild("request").Element(); // ָ����request�ڵ�
	int count = 0;
	for(requestElem; requestElem; requestElem = requestElem->NextSiblingElement()) {
		customer = allCustomer[count];     // ��ǰ�˿ͽڵ㣬ע�ⲻ�ܸ�ֵ��һ���µĶ��󣬷������ø��ƹ��캯��
		TiXmlHandle request(requestElem);  // ָ��ָ��Ķ���
		TiXmlElement* startTimeElem = request.FirstChild("tw").FirstChild("start").Element(); // start time
		TiXmlElement* endTimeElem = request.FirstChild("tw").FirstChild("end").Element();     // end time
		TiXmlElement* quantityElem = request.FirstChild("quantity").Element();                // quantity
		TiXmlElement* serviceTimeElem = request.FirstChild("service_time").Element();         // service time
		// �ֱ��ȡ��������
		tempFLOAT = (float)atof(startTimeElem->GetText());
		customer->startTime = tempFLOAT;  
		tempFLOAT = (float)atof(endTimeElem->GetText());
		customer->endTime = tempFLOAT;
		tempFLOAT = (float)atof(quantityElem->GetText());
		customer->quantity = tempFLOAT;
		tempFLOAT = (float)atof(serviceTimeElem->GetText());
		customer->serviceTime = tempFLOAT;
		count++;
	}

	// ��ȡcapacity
	TiXmlElement* capacityElem = hRoot.FirstChild("fleet").FirstChild("vehicle_profile").FirstChild("capacity").Element();
	tempFLOAT = (float)atof(capacityElem->GetText());
	capacity = tempFLOAT;
	return true;
}

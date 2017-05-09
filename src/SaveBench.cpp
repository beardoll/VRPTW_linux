#include "SaveBench.h"

#include "tinystr.h"
#include "tinyxml.h"
#include "PublicFunction.h"
#include<iostream>
#include<sstream>
#include<vector>

SaveBench::SaveBench(vector<Customer*> staticCustomer, vector<Customer*> dynamicCustomer, Customer depot, float capacity, int timeSlotNum) :
	dynamicCustomer(dynamicCustomer), staticCustomer(staticCustomer), depot(depot), capacity(capacity), timeSlotNum(timeSlotNum) {

}


void saveCustomer(vector<Customer*> customer, TiXmlElement *root, int timeSlotNum) {
	vector<Customer*>::iterator custIter;
	TiXmlElement *node;

	for (custIter = customer.begin(); custIter < customer.end(); custIter++) {
		node = new TiXmlElement("Node");
		node->SetAttribute("id", (*custIter)->id);
		node->SetAttribute("type", "P");
		node->SetAttribute("property", (*custIter)->prop);
		TiXmlElement *cx = new TiXmlElement("cx");
		TiXmlElement *cy = new TiXmlElement("cy");
		loadData(cx, (*custIter)->x);
		loadData(cy, (*custIter)->y);
		node->LinkEndChild(cx);
		node->LinkEndChild(cy);
		TiXmlElement *quantity = new TiXmlElement("quantity");
		TiXmlElement *startTime = new TiXmlElement("startTime");
		TiXmlElement *endTime = new TiXmlElement("endTime");
		TiXmlElement *arrivedTime = new TiXmlElement("arrivedTime");
		TiXmlElement *tolerantTime = new TiXmlElement("tolerantTime");
		TiXmlElement *serviceTime= new TiXmlElement("serviceTime");
		loadData(quantity, (*custIter)->quantity);
		loadData(startTime, (*custIter)->startTime);
		loadData(endTime, (*custIter)->endTime);
		loadData(arrivedTime, 0);
		loadData(tolerantTime, (*custIter)->tolerantTime);
		loadData(serviceTime, (*custIter)->serviceTime);
		TiXmlElement *ProbInfo = new TiXmlElement("ProbInfo");
		for (int i = 0; i < timeSlotNum; i++) {
			TiXmlElement *timeProb = new TiXmlElement("timeProb");
			loadData(timeProb, (*custIter)->timeProb[i]);
			ProbInfo->LinkEndChild(timeProb);
		}

		node->LinkEndChild(quantity);
		node->LinkEndChild(startTime);
		node->LinkEndChild(endTime);
		node->LinkEndChild(arrivedTime);
		node->LinkEndChild(tolerantTime);
		node->LinkEndChild(serviceTime);
		node->LinkEndChild(ProbInfo);
		root->LinkEndChild(node);
	}
}

void SaveBench::save(string path) {         // Ö÷º¯Êý
	TiXmlDocument doc;
	TiXmlComment *comment;
	TiXmlDeclaration *dec1 = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(dec1);

	TiXmlElement *root = new TiXmlElement("Result");
	doc.LinkEndChild(root);

	vector<Customer*>::iterator custIter;
	

	TiXmlElement *StaticCustomer = new TiXmlElement("StaticCustomer");
	saveCustomer(staticCustomer, StaticCustomer, timeSlotNum);
	root->LinkEndChild(StaticCustomer);

	TiXmlElement *DynamicCustomer = new TiXmlElement("DynamicCustomer");
	saveCustomer(dynamicCustomer, DynamicCustomer, timeSlotNum);
	root -> LinkEndChild(DynamicCustomer);

	TiXmlElement *Depot = new TiXmlElement("Depot");
	Depot->SetAttribute("id", depot.id);
	Depot->SetAttribute("type", "D");
	TiXmlElement *cx = new TiXmlElement("cx");
	TiXmlElement *cy = new TiXmlElement("cy");
	loadData(cx, depot.x);
	loadData(cy, depot.y);
	Depot->LinkEndChild(cx);
	Depot->LinkEndChild(cy);
	root->LinkEndChild(Depot);

	TiXmlElement *Vehicle = new TiXmlElement("Vehicle");
	TiXmlElement *Capacity = new TiXmlElement("Capacity");
	loadData(Capacity, capacity);
	Vehicle->LinkEndChild(Capacity);
	root->LinkEndChild(Vehicle);

	doc.SaveFile(path.c_str());
}

#include "OutputFile.h"
#include "PublicFunction.h"
#include<sstream>

OutputFile::OutputFile(vector<Car*> carSet, vector<Customer*> rejectCustomer, vector<Customer*> dynamicCustomer, string fileName, Customer depot, float result, float extra):
	carSet(carSet), rejectCustomer(rejectCustomer), dynamicCustomer(dynamicCustomer), fileName(fileName), depot(depot), result(result), extra(extra){   // 构造函数

}



void OutputFile::exportData(){         // 主函数
	TiXmlDocument doc;
	TiXmlComment *comment;
	TiXmlDeclaration *dec1 = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(dec1);

	TiXmlElement *root = new TiXmlElement("Result");
	doc.LinkEndChild(root);

	// 添加注释
	comment = new TiXmlComment();
	string s = "ALNS results for dataset" + fileName;
	comment->SetValue(s.c_str());
	root->LinkEndChild(comment);

	TiXmlElement *routeset = new TiXmlElement("RouteSet");
	root->LinkEndChild(routeset);
	TiXmlElement *route;
	vector<Car*>::iterator iter1;
	TiXmlElement *node;
	ostringstream buffer;
	string ss;
	for(iter1 = carSet.begin(); iter1 < carSet.end(); iter1++){
		// 写入Route节点
		route = new TiXmlElement("Route");
		route->SetAttribute("index", (*iter1)->getCarIndex());
		vector<Customer*> tempcust= (*iter1)->getRoute().getAllCustomer();
		vector<Customer*>::iterator iter2 = tempcust.begin();
		Customer *newdepot1 = new Customer(depot);
		Customer *newdepot2 = new Customer(depot);
		tempcust.insert(iter2, newdepot1);
		tempcust.push_back(newdepot2);
		for(iter2 = tempcust.begin(); iter2 < tempcust.end(); iter2++){
			// 写入node节点
			node = new TiXmlElement("Node");
			node->SetAttribute("id", (*iter2)->id);
			switch((*iter2)->type){   // 属性值是string类型而非ASCII码类型
			case 'D':
				node->SetAttribute("type", "D");
				break;
			case 'P':
				node->SetAttribute("type", "P");
				break;
			}
			TiXmlElement *cx = new TiXmlElement("cx");
			TiXmlElement *cy = new TiXmlElement("cy");
			loadData(cx, (*iter2)->x);
			loadData(cy, (*iter2)->y);
			node->LinkEndChild(cx);
			node->LinkEndChild(cy);
			if ((*iter2)->type == 'P') {
				node->SetAttribute("property", (*iter2)->prop);
				TiXmlElement *quantity = new TiXmlElement("quantity");
				TiXmlElement *startTime = new TiXmlElement("startTime");
				TiXmlElement *endTime = new TiXmlElement("endTime");
				TiXmlElement *arrivedTime = new TiXmlElement("arrivedTime");
				TiXmlElement *tolerantTime = new TiXmlElement("tolerantTime");
				loadData(quantity, (*iter2)->quantity);
				loadData(startTime, (*iter2)->startTime);
				loadData(endTime, (*iter2)->endTime);
				loadData(arrivedTime, (*iter2)->arrivedTime);
				loadData(tolerantTime, (*iter2)->tolerantTime);
				node->LinkEndChild(quantity);
				node->LinkEndChild(startTime);
				node->LinkEndChild(endTime);
				node->LinkEndChild(arrivedTime);
				node->LinkEndChild(tolerantTime);
			}
			route->LinkEndChild(node);
		}
		routeset->LinkEndChild(route);
	}

	// routeLen
	buffer.str("");
	TiXmlElement *routelen = new TiXmlElement("RouteLen");
	buffer << result;
	ss = buffer.str();
	routelen->LinkEndChild(new TiXmlText(ss.c_str()));
	root->LinkEndChild(routelen);

	// extra data (addAveDistance)
	TiXmlElement *add_ave_distance = new TiXmlElement("AddAveDistance");
	buffer.str("");
	buffer << extra;
	ss = buffer.str();
	add_ave_distance->LinkEndChild(new TiXmlText(ss.c_str()));
	root->LinkEndChild(add_ave_distance);	

	TiXmlElement *RejectCustomer = new TiXmlElement("RejectCustomer");
	root->LinkEndChild(RejectCustomer);
	vector<Customer*>::iterator custIter;
	for (custIter = rejectCustomer.begin(); custIter < rejectCustomer.end(); custIter++) {
		node = new TiXmlElement("Node");
		node->SetAttribute("id", (*custIter)->id);
		node->SetAttribute("property", 1);  // 被拒绝的肯定是动态顾客
		switch ((*custIter)->type) {   // 属性值是string类型而非ASCII码类型
		case 'D':
			node->SetAttribute("type", "D");
			break;
		case 'P':
			node->SetAttribute("type", "P");
			break;
		}
		TiXmlElement *cx = new TiXmlElement("cx");
		TiXmlElement *cy = new TiXmlElement("cy");
		TiXmlElement *quantity = new TiXmlElement("quantity");
		TiXmlElement *startTime = new TiXmlElement("startTime");
		TiXmlElement *endTime = new TiXmlElement("endTime");
		TiXmlElement *arrivedTime = new TiXmlElement("arrivedTime");
		TiXmlElement *tolerantTime = new TiXmlElement("tolerantTime");
		loadData(cx, (*custIter)->x);
		loadData(cy, (*custIter)->y);
		loadData(quantity, (*custIter)->quantity);
		loadData(startTime, (*custIter)->startTime);
		loadData(endTime, (*custIter)->endTime);
		loadData(arrivedTime, (*custIter)->arrivedTime);
		loadData(tolerantTime, (*custIter)->tolerantTime);
		node->LinkEndChild(cx);
		node->LinkEndChild(cy);
		node->LinkEndChild(quantity);
		node->LinkEndChild(startTime);
		node->LinkEndChild(endTime);
		node->LinkEndChild(arrivedTime);
		node->LinkEndChild(tolerantTime);
		RejectCustomer->LinkEndChild(node);
	}
	TiXmlElement *DynamicCustomer = new TiXmlElement("DynamicCustomer");
	root->LinkEndChild(DynamicCustomer);
	for (custIter = dynamicCustomer.begin(); custIter < dynamicCustomer.end(); custIter++) {
		node = new TiXmlElement("Node");
		node->SetAttribute("id", (*custIter)->id);
		DynamicCustomer->LinkEndChild(node);
	}
	string file = fileName;
	doc.SaveFile(file.c_str());
}

#ifndef _OUTPUTFILE_H
#define _OUTPUTFILE_H
#include<vector>
#include<string>
#include "Car.h"
#include "tinystr.h"
#include "tinyxml.h"

class OutputFile{  // �����н�������xml�ļ���
public:
	OutputFile(vector<Car*> carSet, vector<Customer*> rejectCustomer, vector<Customer*> dynamicCustomer, string fileName, Customer depot, float result, float extra = 0.0f);   // ���캯��
	~OutputFile(){};           // ��������
	void exportData();         // ������
private:
	vector<Car*> carSet;
	vector<Customer*> rejectCustomer;
	vector<Customer*> dynamicCustomer;
	string fileName;
	Customer depot;
	float result;
	float extra;
};

#endif

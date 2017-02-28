#ifndef _OUTPUTFILE_H
#define _OUTPUTFILE_H
#include<vector>
#include<string>
#include "Car.h"
#include "tinystr.h"
#include "tinyxml.h"

class OutputFile{  // �����н�������xml�ļ���
public:
	OutputFile(vector<Car*> carSet, string filePath, string fileName, Customer depot, float result);   // ���캯��
	~OutputFile(){};           // ��������
	void exportData();         // ������
private:
	vector<Car*> carSet;
	string fileName;
	string filePath;
	Customer depot;
	float result;
};

#endif
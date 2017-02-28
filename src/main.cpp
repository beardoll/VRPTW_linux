#include<iostream>
#include<set>
#include<algorithm> 
#include<map>
#include "LoadFile.h"
#include "SetBench.h"
#include "Timer.h"
#include "TxtRecorder.h"
#include "Car.h"
#include "ALNS.h"
#include<sstream>
#include<string>
#include<fstream>

using namespace std;

static const string FILE_PATH = "./solomon-1987-rc1/";
static const string FILE_NAME = "RC103_100.xml";
static const string FILE_PATH2 = "./ALNS_Result/";
static const string FILE_NAME2 = FILE_NAME;
static const int NUM_OF_CUSTOMER = 100;
static const int timeSlotLen = 40;
static const int timeSlotNum = 6;


int main(){
	LoadFile lf(FILE_PATH, FILE_NAME, NUM_OF_CUSTOMER);
	vector<Customer*> allCustomer(0);
    Customer depot;
	float capacity = 0;
	bool mark = lf.getData(allCustomer, depot, capacity);
    

	// ALNS alg1(allCustomer, depot, capacity);

	// vector<Car*> finalCarSet;
	// float finalCost = 0;
	// alg1.run(finalCarSet, finalCost);

	SetBench sb(allCustomer, timeSlotLen, timeSlotNum);
	vector<Customer*> staticCustomer, dynamicCustomer;
	sb.construct(staticCustomer, dynamicCustomer);
	Timer timer(staticCustomer, dynamicCustomer, timeSlotLen, timeSlotNum, capacity, depot);
	timer.run();
	TxtRecorder::closeFile();
	//vector<Customer*> waitCustomerSet;
	//vector<Car*> currentPlan;
	//Simulator sm(8, timeSlotLen, timeSlotNum, 0, staticCustomer, waitCustomerSet,
	//	dynamicCustomer, currentPlan);
	//sm.initialPlan(depot, capacity);
	
	// vector<Car*> initialPlan = sm.initialPlan();
	//vector<Car*> finalCarSet;
	//OutputFile of(finalCarSet, FILE_PATH2, FILE_NAME2, depot,finalCost);
	//of.exportData();
	return 0;
}

#ifndef _Customer_H
#define _Customer_H
#include <vector>

// 顾客节点类

struct Customer{
	int id;
	char type;     // 类型，“D”表示仓库，“L”表示送货点，“B”表示取货点
	float x;       // x坐标
	float y;       // y坐标  
	float startTime;   // 时间窗开始时间
	float endTime;     // 时间窗结束时间
	float quantity;    // 货物需求量 
	float serviceTime; // 服务时间
	float arrivedTime; // 货车到达时间
	float tolerantTime;   // 可忍受最晚得到答复的时间（仅限于动态BHs）
	int priority;      // 顾客优先级，1表示第一优先级，2表示第二优先级，依次类推
	Customer *next;    // 指向下一个node节点的指针
	Customer *front;   // 指向前一个node节点的指针
	float timeProb[6]; // 在各个timeSlot下的分布概率
	int prop ;    // 顾客性质（0表示静态顾客，1表示动态顾客） 
	bool operator< (Customer &item){
	    return this->id < item.id;
	}
	//Customer operator= (Customer &item){
	//	id = item.id;
	//	type = item.type;
	//	x = item.x;
	//	y = item.y;
	//	startTime = item.startTime;
	//	endTime = item.endTime;
	//	quantity = item.quantity;
	//	serviceTime = item.serviceTime;
	//	arrivedTime = item.arrivedTime;
	//	next = item.next;
	//	front = item.front;
	//	return *this;
	//}
};

#endif

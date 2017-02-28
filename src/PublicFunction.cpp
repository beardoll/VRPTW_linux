#include "PublicFunction.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>
#include<sstream>
#include<iostream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>
#include "Matrix.h"

float random(float start, float end){
	// ����start��end֮��������
	return (float)(start+(end-start)*rand()/(RAND_MAX+1.0));
}

vector<float> randomVec(int num){  // ����num������������ǵĺ�Ϊ1
	float rest = 1;  // ��ʼ����Ϊ1
	vector<float> output;
	if(num == 1) {
		output.push_back(1);
	} else {
		for(int i=0; i<num - 1; i++) {
			float temp = random(0, rest); // ���������
			output.push_back(temp);
			rest -= temp;
		}
		output.push_back(rest);
	}
	return output;
}

vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
	// ����m����ͬ�ģ���ΧΪ[lb, ub)�������
	// restData, ���˷���ֵ��ʣ�����ֵ
	restData.resize(0);
	for(int i=0; i<ub-lb; i++) {
		restData.push_back(i+lb);
	}
	int total = ub-lb;
	vector<int> outputData(0);
	for(int j=0; j<m; j++) {
		vector<int>::iterator iter = restData.begin();
		int num = rand() % total; // 0-total-1
		iter += num;
		int temp = *(iter);
		outputData.push_back(temp);
		restData.erase(iter);
		total--;
	}
	return outputData;
}

int roulette(vector<float> probability) {
	// �����㷨
	// ���ѡ��һ������k (from 0 to |probability|)��
	vector<float>::iterator floatIter;
	float sumation1 = accumulate(probability.begin(), probability.end(), 0.0f); // ���
	for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
		*floatIter /= sumation1;  // ��һ��
	}
	int totalNum = probability.end() - probability.begin();  // ����Ŀ
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	floatIter = probability.begin();
	while((sumation < randFloat) && (floatIter < probability.end())) {
		k++;
		sumation += *(floatIter++);
	}
	k = max(k-1, 0); // randFloat = 0 ʱ k-1 < 0
	return k;
}

int roulette(float *probability, int num) {
	// �����㷨
	// ���ѡ��һ������k (from 0 to |probability|)��
	// һ����num�����ʷֲ�
	int i;
	float sumation1 = 0; // ���
	for(i=0; i<num; i++) {
		sumation1 += probability[i];
	}
	for(i=0; i<num; i++) {
		probability[i] /= sumation1;  // ��һ��
	}
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	while((sumation < randFloat) && (k < num)) {
		k++;
		sumation += probability[k];
	}
	k = max(k-1, 0); // randFloat = 0 ʱ k-1 < 0
	return k;
}
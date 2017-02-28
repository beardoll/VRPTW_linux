#ifndef _MATRIX_H
#define _MATRIX_H
#include<cassert>
#include<limits>
#include<iostream>
using namespace std;


template <class T>
class Matrix{
public:
	Matrix(int rows=2, int cols=2);    // ���캯��
	~Matrix();   // ��������
	Matrix(const Matrix<T> &M);   // ���ƹ��캯��
	Matrix<T>& operator= (const Matrix<T> &M);  // ���صȺ������
	void resize(int newrows, int newcols);  // ���¶�������С 

	// �в���
	T getMaxAtRow(int row, int &pos);  // �õ���row�е����ֵ��������λ��pos�Լ�ֵ
	T getMinAtRow(int row, int &pos);  // �õ���row�е���Сֵ��������λ��pos�Լ�ֵ
	Matrix<T> getElemAtRows(int row1, int row2);   // �õ���row1-row2��Ԫ��
	void setMatrixAtRow(int row, T* elements);     // ���õ�row��Ԫ��
	void addOneRow();  // ����������һ��
	void printMatrixAtRow(int row);

	// �в���
	T getMaxAtCol(int col, int &pos);  // �õ���col�е����ֵ��������λ��pos�Լ�ֵ
	T getMinAtCol(int col, int &pos);  // �õ���col�е���Сֵ��������λ��pos�Լ�ֵ
	Matrix<T> getElemAtCols(int col1, int col2);   // �õ���col1-col2��Ԫ��
	void setMatrixAtCol(int col, T* elements);     // ���õ�col��Ԫ��
	void printMatrixAtCol(int col);

	// �������
	void setMatrix(const T* values);  // ���þ���Ԫ��
	T getMaxValue(int &i, int &j); // �õ���ǰ��������Ԫ�ؼ���λ�ã�����һ����
	T getMinValue(int &i, int &j); // �õ���ǰ�������СԪ�ؼ���λ�ã�����һ����
	Matrix<T> getElemAtRowsByCols(int row1, int row2, int col1, int col2);  // �õ�row1-row2, col1-col2���Ӿ���
	void setValue(int i, int j, const T& newValue); // �޸�Ԫ��(i,j)ΪnewValue
	T getElement(int i, int j);  // �õ�Ԫ��(i,j)
	void printMatrix();
private:
	int rows, cols;    // ����Ĵ�С
	T *elements; // ����Ԫ��
}; // ������


//=================== �������� ===================//
template <class T>
Matrix<T>::Matrix(int rows, int cols):rows(rows), cols(cols){   // ���캯��
	assert(rows>=0 && cols>=0);
	elements = new T[rows*cols];
}

template <class T>
Matrix<T>::~Matrix(){    // ��������
	delete [] elements;
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> &M){    // ���ƹ��캯��
	rows = M.rows;
	cols = M.cols;
	elements = new T[rows*cols];
	for(int i=0; i<rows*cols; i++){
		elements[i] = M.elements[i];
	}
}

template <class T>
Matrix<T>& Matrix<T>:: operator=(const Matrix<T> &M){   // ���ء�=�������
	if (rows!=M.rows || cols!=M.cols){
		delete [] elements;
		rows = M.rows;
		cols = M.cols;
		elements = new T[rows*cols];
	}
	for(int i=0; i<rows*cols; i++){
		elements[i] = M.elements[i];
	}
	return *this;
}

template <class T>
void Matrix<T>::resize(int newrows, int newcols){  // ���¶�������С
	assert(newrows>0 && newcols>0);
	delete [] elements;
	elements = new T[newrows * newcols];
	rows = newrows;
	cols = newcols;
}


//=================== �в��� ===================//
template <class T>
T Matrix<T>::getMaxAtRow(int row, int &pos){ // �õ���row�е����ֵ��������λ��pos�Լ�ֵ
	assert(row>=0 && row<rows);
	pos = 0;
	T maxValue = -numeric_limits<T>::max();  // �������ֵΪ����С
	for(int i=row*cols; i<(row+1)*cols; i++) {
		if(elements[i] > maxValue) {
			maxValue = elements[i];
			pos = i - row*cols;
		}
	}
	return maxValue;
}  

template <class T>
T Matrix<T>::getMinAtRow(int row, int &pos){ // �õ���row�е���Сֵ��������λ��pos�Լ�ֵ
	assert(row>=0 && row<rows);
	pos = 0;
	T minValue = numeric_limits<T>::max();  // ������СֵΪ�����
	for(int i=row*cols; i<(row+1)*cols; i++) {
		if(elements[i] < minValue) {
			minValue = elements[i];
			pos = i - row*cols;
		}
	}
	return minValue;
}  

template <class T> 
Matrix<T> Matrix<T>::getElemAtRows(int row1, int row2){   // �õ���row1-row2��Ԫ��
	assert(row1>=0 && row1<rows && row2>=0 && row2<rows && row1<=row2);
	Matrix<T> tempMat(row2-row1+1, cols);
	for(int i=row1; i<=row2; i++){
		for(int j=0; j<=cols; j++){
			tempMat.setValue(i-row1, j, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
void Matrix<T>::setMatrixAtRow(int row, T* elements){     // ���õ�row��Ԫ��
	for(int j=0; j<cols; j++){
		this->setValue(row, j, elements[j]);
	}
}

template <class T>
void Matrix<T>::printMatrixAtRow(int row){
	assert(row>=0 && row < rows);
	int startPoint = row * cols;
	int endPoint = (row+1) * cols;
	for(int i=startPoint; i<endPoint; i++) {
		cout << *elements[i] << endl;
	}
}

template <class T>
void Matrix<T>::addOneRow(){  
	// ����������һ��
	// ͬʱ����ԭ��������
	T* newElements = new T[rows*cols + cols];
	for(int i=0; i<rows*cols; i++){
		newElements[i] = elements[i];
	}
	delete [] elements;
	elements = newElements;
	rows++;
}


//=================== �в��� ===================//
template <class T>
T Matrix<T>::getMaxAtCol(int col, int &pos){ // �õ���col�е����ֵ��������λ��pos�Լ�ֵ
	assert(col>=0 && cols<cols);
	T maxValue = -numeric_limits<T>::max();  // �������ֵΪ����С
	pos = 0;
	int tpos = 0;
	for(int i=0; i<rows; i++){
		tpos = i*cols+col;    // ��λ��ǰԪ��
		if(elements[tpos] > maxValue) {
			maxValue = elements[tpos];
			pos = i;
		}
	}
	return maxValue;
}

template <class T>
T Matrix<T>::getMinAtCol(int col, int &pos){ // �õ���col�е���Сֵ��������λ��pos�Լ�ֵ
	assert(col>=0 && col<cols);
	pos = 0;
	T minValue = numeric_limits<T>::max();  // ������СֵΪ�����
	int tpos;
	for(int i=0; i<rows; i++){
		tpos = i*cols+col;    // ��λ��ǰԪ��
		if(elements[tpos] < minValue) {
			minValue = elements[tpos];
			pos = i;
		}
	}
	return minValue;
}

template <class T>
Matrix<T> Matrix<T>::getElemAtCols(int col1, int col2){  // �õ���cols1-col2��Ԫ��
	assert(col1>=0 && col1<cols && col2>=0 && col2<cols && col1<=col2);
	Matrix<T> tempMat(rows, col2-col1+1);
	for(int i=0; i<rows; i++){
		for(int j=col1; j<=col2; j++){
			tempMat.setValue(i, j-col1, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
void Matrix<T>::setMatrixAtCol(int col, T* elements){     // ���õ�col��Ԫ��
	for(int i=0; i<rows; i++){
		this->setValue(i, col, elements[i]);
	}
}

template <class T>
void Matrix<T>::printMatrixAtCol(int col){
	assert(col>=0 && col < cols);
	for(int i=0; i<rows; i++) {
		cout << getElement(i, col) << endl;
	}
}


//=================== ������� ===================//
template <class T>
void Matrix<T>:: setMatrix(const T* values){
	for(int i=0; i<rows*cols; i++){
		elements[i] = values[i];
	}
}

template <class T>
void Matrix<T>::setValue(int i, int j, const T& value){
	assert(i>=0 && i<rows && j>=0 && j<cols);
	elements[i*cols+j] = value;

}

template <class T>
T Matrix<T>::getMaxValue(int &i, int &j){ // �õ���ǰ��������Ԫ�ؼ���λ�ã�����һ����
	T maxValue = -numeric_limits<T>::max();  // �������ֵΪ����С
	i = 0;
	j = 0;
	int pos;
	for(int k=0; k<rows; k++) {
		for(int l=0; l<cols; l++){
			pos = k*cols+l;
			if(elements[pos] > maxValue) {
				maxValue = elements[pos];
				i = k;
				j = l;
			}
		}
	}
	return maxValue;
}

template <class T>
T Matrix<T>::getMinValue(int &i, int &j){ // �õ���ǰ�������С����λ�ã�����һ����
	T minValue = numeric_limits<T>::max();  // ������СֵΪ�����
	i = 0;
	j = 0;
	int pos;
	for(int k=0; k<rows; k++) {
		for(int l=0; l<cols; l++){
			pos = k*cols+l;
			if(elements[pos] < minValue) {
				minValue = elements[pos];
				i = k;
				j = l;
			}
		}
	}
	return minValue;
}

template <class T>
Matrix<T> Matrix<T>::getElemAtRowsByCols(int row1, int row2, int col1, int col2){  // �õ�row1-row2, col1-col2���Ӿ���
	assert(row1>=0 && row1<rows && row2>=0 && row2<rows && row1<=row2);
	assert(col1>=0 && col1<cols && col2>=0 && col2<cols && col1<=col2);
	Matrix<T> tempMat(row2-row1+1, col2-col1+1);
	for(int i=row1; i<=row2; i++){
		for(int j=col1; j<=col2; j++){
			tempMat.setValue(i-row1, j-col1, elements[i*cols+j]);
		}
	}
	return tempMat;
}

template <class T>
T Matrix<T>::getElement(int i, int j){  // ���������()
	assert(i>=0 && i<rows && j>=0 && j<cols);
	T value = elements[i*cols+j];
	return value;
}

template <class T>
void Matrix<T>::printMatrix(){
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			cout << getElement(i,j) << '\t';
		}
		cout << endl;
	}
	cout << endl;
}

#endif
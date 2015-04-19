#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{
public:
	int sizeX;
	int sizeY;

	float ** grid;

	Matrix(int x, int y);

	bool SetGridDim(int x, int y);

	friend Matrix operator*(Matrix &A, Matrix &B);

	void Print();
};

#endif //MATRIX_H
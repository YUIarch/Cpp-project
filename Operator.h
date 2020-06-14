#ifndef Operator_h
#define Operator_h
#include<vector>
#include <iostream>
using namespace std;

typedef vector<double> Vector;
typedef vector<vector<double>> Matrix;

Vector operator+(const Vector& V, const Vector& W)
{
	int d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
	return U;
}

Vector operator-(const Vector& V, const Vector& W)
{
	int d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] - W[j];
	return U;
}

Vector operator+=(Vector& V, const Vector& W)
{
	int d = V.size();
	for (int j = 0; j < d; j++) V[j] += W[j];
	return V;
}

Vector operator*(const double& a, const Vector& V)
{
	int d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = a * V[j];
	return U;
}

ostream& operator<<(ostream& out, Vector& V)
{
	for (Vector::iterator itr = V.begin(); itr != V.end(); itr++)
		out << *itr << "   ";
	out << endl;
	return out;
}

ostream& operator<<(ostream& out, Matrix& V)
{
	for (Matrix::iterator itr = V.begin(); itr != V.end(); itr++)
		out << *itr << "   ";
	out << endl;
	return out;
}

Vector operator^(const Vector& V, const double& a)
{
	double sum = 0.0;
	int d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = pow(V[j], a);
	return U;
}

Vector operator/(const Vector& V, const double& a)
{
	int d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] / a;
	return U;
}


#endif


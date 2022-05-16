#pragma once

#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Sparse>

class actorContour
{

public:
	actorContour();
	~actorContour();

public:

	bool exec(std::vector<std::vector<float>>* Image, std::vector<std::vector<float>>* Phi);
	void updatec();
	static float deltah(float x);
	void updatematrix();
	void changePhi();

	float Rx(int i, int j);
	float Ry(int i, int j);

	int largeidx(int i, int j);
	int componentx(int s);
	int componenty(int s);


public:
	static const float space_step;
	static const float mu;
	static const float nu;
	static const float lambda1;
	static const float lambda2;
	static const float delta;
	static const float deltat;
	static const float epsilon;
	static const float pi;

	float c1;
	float c2;

	int image_width;
	int image_height;
	int mat_size;

	bool isChanged;


public:

	std::vector<std::vector<float>>* image;
	std::vector<std::vector<float>>* phi;

	Eigen::SparseMatrix<float>* sparse_mat;
	Eigen::VectorXf* arr;


};


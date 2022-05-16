#include "actorContour.h"

const float actorContour::space_step = 1;
const float actorContour::mu = 2000;
const float actorContour::nu = 0.001;
const float actorContour::lambda1 = 1;
const float actorContour::lambda2 = 1;
const float actorContour::delta = 1;
const float actorContour::deltat = 0.1;
const float actorContour::epsilon = 0.0001;
const float actorContour::pi = 3.1415926535;

actorContour::actorContour() {
	c1 = 0;
	c2 = 0;
	image_height = 0;
	image_width = 0;
	mat_size = 0;

	isChanged = false;

	sparse_mat = nullptr;
	arr = nullptr;
}

actorContour::~actorContour() {

	delete sparse_mat;
	sparse_mat = nullptr;
	delete arr;
	arr = nullptr;
	image = nullptr;
	phi = nullptr;

}

float actorContour::deltah(float x) {
	/*
	if (x >= delta || x <= -delta) {
		return 0.0f;
	}

	float a = 1 / delta;
	float b = x / (delta * delta);
	if (x >= 0) {
		return a - b;
	}
	else {
		return a + b;
	}

	printf("error3 occured!\n");
	exit(3);
	return 0.0f;
	*/
	
	float z = pi * sqrt(x * x + delta * delta);
	return 1 / z;

}


bool actorContour::exec(std::vector<std::vector<float>>* Image, std::vector<std::vector<float>>* Phi) {

	isChanged = false;
	image = Image;
	phi = Phi;
	image_height = (*image)[0].size();
	image_width = image->size();
	updatec();
	mat_size = image_height * image_width;

	if ( sparse_mat == nullptr || arr->rows() != mat_size ) {
		delete sparse_mat;
		delete arr;
		
		sparse_mat = new Eigen::SparseMatrix<float>(mat_size, mat_size);
		arr = new Eigen::VectorXf(mat_size);
	}

	updatematrix();
	changePhi();
	/*
	std::cout << std::endl;
	for (auto i = 0; i < image_width; i++) {
		for (auto j = 0; j < image_height; j++) {
			std::cout << (*phi)[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/
	return isChanged;
}


void actorContour::updatec() {
	
	int interior_num = 0;
	float interior_val = 0;
	int exterior_num = 0;
	float exterior_val = 0;
	for (auto i = 0; i < image_width; i++) {
		for (auto j = 0; j < image_height; j++) {
			if ((*phi)[i][j] > 0) {
				interior_num++;
				interior_val += (*image)[i][j];
			}
			else if ((*phi)[i][j] < 0) {
				exterior_num++;
				exterior_val += (*image)[i][j];
			}
		}
	}

	if (interior_num > 0) {
		c1 = interior_val / interior_num;
	}
	if (exterior_num > 0) {
		c2 = exterior_val / exterior_num;
	}

	return;
}


float actorContour::Rx(int i, int j) {
	if (i < 0 || i >= image_width || j < 0 || j >= image_height) {
		printf("error4 occured!\n");
		exit(4);
	}

	float x1, x2, x3;
	x2 = (*phi)[i][j];
	if (i + 1 >= image_width) {
		x1 = x2;
	}
	else {
		x1 = (*phi)[i + 1][j];
	}

	if (i - 1 < 0) {
		x3 = x1;
	}
	else {
		x3 = (*phi)[i - 1][j];
	}

	float y1 = (x1 - x2) * (x1 - x2);
	float y2 = ((x1 - x3) * (x1 - x3)) / 4;

	float ans = sqrt(y1 + y2);

	if (ans > 0) {
		return ans;
	}
	else {
		return epsilon;
	}
}

float actorContour::Ry(int i, int j) {
	if (i < 0 || i >= image_width || j < 0 || j >= image_height) {
		printf("error5 occured!\n");
		exit(5);
	}

	float x1, x2, x3;
	x2 = (*phi)[i][j];
	if (j + 1 >= image_height) {
		x1 = x2;
	}
	else {
		x1 = (*phi)[i][j + 1];
	}

	if (j - 1 < 0) {
		x3 = x1;
	}
	else {
		x3 = (*phi)[i][j - 1];
	}

	float y1 = (x1 - x2) * (x1 - x2);
	float y2 = ((x1 - x3) * (x1 - x3)) / 4;

	float ans = sqrt(y1 + y2);

	if (ans > 0) {
		return ans;
	}
	else {
		return epsilon;
	}
}

int actorContour::largeidx(int i, int j) {

	if (i > image_width || i<-1 || j>image_height || j < -1) {
		printf("error6 occured!\n");
		exit(6);
	}

	if (i == image_width) {
		return (i - 1) * image_height + j;
	}
	if (i == -1) {
		return j;
	}
	if (j == image_height) {
		return i * image_height + j - 1;
	}
	if (j == -1) {
		return i * image_height;
	}

	return i * image_height + j;

}

int actorContour::componenty(int s) {

	return s % image_height;

}

int actorContour::componentx(int s) {

	int t = s - componenty(s);
	return t / image_height;

}


void actorContour::updatematrix() {


	std::vector<Eigen::Triplet<float>> trip_vec(0);
	for (auto i = 0; i < image_width; i++) {
		for (auto j = 0; j < image_height; j++) {

			int idx = largeidx(i, j);
			float deltaij = deltah((*phi)[i][j]);
			if (deltaij == 0) {
				(*arr)(idx) = (*phi)[i][j];
				trip_vec.emplace_back(idx, idx, 1);
			}
			else {

				float rxij = Rx(i, j);
				float ryij = Ry(i, j);
				float rxi1j, ryij1;
				if (j - 1 < 0) {
					ryij1 = ryij;
				}
				else {
					ryij1 = Ry(i, j - 1);
				}

				if (i - 1 < 0) {
					rxi1j = rxij;
				}
				else {
					rxi1j = Rx(i - 1, j);
				}

				float c0 = deltaij * mu * (1 / space_step);
				float cij = 1 / deltat + c0 * (1 / rxij + 1 / rxi1j + 1 / ryij + 1 / ryij1);
				float cia1j = -c0 * (1 / rxij);
				float cim1j = -c0 * (1 / rxi1j);
				float cija1 = -c0 * (1 / ryij);
				float cijm1 = -c0 * (1 / ryij1);
				(*arr)(idx) = (*phi)[i][j] / deltat - deltaij * (nu + lambda1 * ((*image)[i][j] - c1) * ((*image)[i][j] - c1) - lambda2 * ((*image)[i][j] - c2) * ((*image)[i][j] - c2));
				
				int idxia1 = largeidx(i + 1, j);
				int idxim1 = largeidx(i - 1, j);
				int idxja1 = largeidx(i, j + 1);
				int idxjm1 = largeidx(i, j - 1);

				trip_vec.emplace_back(idx, idx, cij);
				trip_vec.emplace_back(idx, idxia1, cia1j);
				trip_vec.emplace_back(idx, idxim1, cim1j);
				trip_vec.emplace_back(idx, idxja1, cija1);
				trip_vec.emplace_back(idx, idxjm1, cijm1);
			
			}
			

		}
	}

	sparse_mat->setFromTriplets(trip_vec.begin(), trip_vec.end());
	sparse_mat->makeCompressed();
	return;
}


void actorContour::changePhi() {
	
	Eigen::SparseLU<Eigen::SparseMatrix<float>> qr(*sparse_mat);
	Eigen::VectorXf newphi = qr.solve(*arr);

	for (auto idx = 0; idx < mat_size; idx++) {
		int i = componentx(idx);
		int j = componenty(idx);
		if (((*phi)[i][j] == 0 && newphi(idx) != 0) || ((*phi)[i][j] != 0 && newphi(idx) == 0) ||
			((*phi)[i][j] < 0 && newphi(idx) > 0) || ((*phi)[i][j] > 0 && newphi(idx) < 0)) {
			isChanged = true;
		}
		(*phi)[i][j] = newphi(idx);
	}

	return;
}
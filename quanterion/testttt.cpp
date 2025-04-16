#include "quat.hpp"
#include <iostream>
#include <cmath>

int main() {
//	Quat< double > a(0, 1, 2, 3), b(0, 10, 2, 3);
//	bool c = a != b;
//	std::cout << c << "\n";
//	std::cout << M_PI;
//	Quat< double > actual(2.5, false, { 0.15, 0.5, 0.35 });
//
//	const Quat< float > a(2.5, true, { 0.15, 0.5, 0.35 });
//	Quat< float > actual = a;
//	for (int i = 0; i < 4; i++)
//	{
//		std::cout << actual.data()[i] << " ";
//	}
	const Quat< float > a(0.8, 0.5, 0.4, 0.6);
	matrix_t< float > actual = a.rotation_matrix();

	return 0;
}
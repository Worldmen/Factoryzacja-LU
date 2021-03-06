#include "matrix.h"
#include "sparseMatrix.h"
#include "timer.h"
#include <fstream>

void A();
void B();
void C();
void D();
void E();

int main()
{
	//A();
	//B();
	//C();
	D();
	//E();
	return 0;
}

void A() {
	int index = 165407;
	int e = (index % 1000) / 100;
	int a1 = 5 + e, a2 = -1, a3 = -1;
	int N = 900 + ((index % 100) / 10) * 10 + index % 10;
	int f = (index % 10000) / 1000;
	
	sparse::Matrix A = sparse::Matrix(N);
	A.matrixGenerator(a1, a2, a3);
	sparse::Matrix b = sparse::Matrix(N, 1);
	b.vectorBGenerator(f);

}

void B() {	//implementacja metod iteracyjnych
	Timer timer = Timer();
	std::chrono::duration<double> countedTime;

	int index = 165407;
	int e = (index % 1000) / 100;
	int a1 = 5 + e, a2 = -1, a3 = -1;
	int N = 900 + ((index % 100) / 10) * 10 + index % 10;
	int f = (index % 10000) / 1000;

	sparse::Matrix A = sparse::Matrix(N);
	A.matrixGenerator(a1, a2, a3);
	sparse::Matrix b = sparse::Matrix(N, 1);
	b.vectorBGenerator(f);


	std::ofstream logB;
	logB.open("B.txt");


	sparse::Matrix D = A.matrixD();
	sparse::Matrix L = A.matrixL();
	sparse::Matrix U = A.matrixU();

	// Jackobi x(k+1) = -D^(-1) * ((L + U)*x(k)) + (D^(-1))*b

	sparse::Matrix reversedD = D.reverseD();
	sparse::Matrix x1 = sparse::Matrix(N, 1);
	x1.vectorXGenerator();
	sparse::Matrix residuum1 = (A*x1) - b;
	sparse::Matrix reverseD_M_b = reversedD * b;
	sparse::Matrix L_P_U = L + U;

	timer.startTimer();

	int spr1 = 0;
	while (residuum1.norm() > 0.000000001) {
		x1 = -(reversedD * (L_P_U * x1)) + reverseD_M_b;
		//Matrix pomMatrix = -(L_P_U * x1) + b;
		//x1 = x1.forwardSubstitution(D, pomMatrix);
		residuum1 = (A*x1) - b;
		spr1++;
	}

	countedTime = timer.getCountedTime();
	logB << "Metoda Jackobiego: Czas - " << countedTime.count() << "sec" << " Ilosc iteracji: " << spr1 << std::endl;

	// Gauss-Seidel x(k+1) = -(D + L)^(-1) * ((U)*x(k)) + (D + L)^(-1))*b
	// x2 = ((D_P_L)^-1)*(U*x2) +  ((D_P_L)^-1) * b;
	// D_P_L * x2 = -(U*x2) + b

	sparse::Matrix x2 = sparse::Matrix(N, 1);
	x2.vectorXGenerator();
	sparse::Matrix residuum2 = (A*x2) - b;
	sparse::Matrix D_P_L = -(D + L);

	timer.startTimer();

	int spr2 = 0;
	while (residuum2.norm() > 0.000000001) {
		sparse::Matrix pomMatrix = ((U*x2)) - b;
		x2 = x2.forwardSubstitution(D_P_L, pomMatrix);
		residuum2 = (A * x2) - b;
		spr2++;
	}

	countedTime = timer.getCountedTime();
	logB << "Metoda Gaussa-Seidela: Czas - " << countedTime.count() << "sec" << " Ilosc iteracji: " << spr2 << std::endl;

	logB.close();

}


void C() { //100 iter pokazac ze nie zbiega. -> Nie zbiega poniweaz A nie jest diagonalnie dominujace dla tego przypadku.
	int index = 165407;
	int e = (index % 1000) / 100;
	int a1 = 3, a2 = -1, a3 = -1;
	int N = 900 + ((index % 100) / 10) * 10 + index % 10;
	int f = (index % 10000) / 1000;

	sparse::Matrix A = sparse::Matrix(N);
	A.matrixGenerator(a1, a2, a3);
	sparse::Matrix b = sparse::Matrix(N, 1);
	b.vectorBGenerator(f);

	std::ofstream logC;
	logC.open("C.txt");

	sparse::Matrix D = A.matrixD();
	sparse::Matrix L = A.matrixL();
	sparse::Matrix U = A.matrixU();
	sparse::Matrix reversedD = D.reverseD();

	// Jackobi
	sparse::Matrix x1 = sparse::Matrix(N, 1);
	x1.vectorXGenerator();
	sparse::Matrix residuum1 = (A*x1) - b;
	sparse::Matrix reverseD_M_b = reversedD * b;
	sparse::Matrix L_P_U = L + U;

	logC << "Metoda Jackobiego: " << std::endl;

	double normResidum1 = residuum1.norm();
	int spr1 = 0;
	while (normResidum1 > 0.000000001) {
		x1 = -(reversedD * (L_P_U * x1)) + reverseD_M_b;
		residuum1 = (A*x1) - b;
		normResidum1 = residuum1.norm();
		spr1++;
		logC << spr1 << "\t" << normResidum1 << std::endl;
		if (spr1 == 100)
			break;
	}

	// Gauss-Seidel
	sparse::Matrix x2 = sparse::Matrix(N, 1);
	x2.vectorXGenerator();
	sparse::Matrix residuum2 = (A*x2) - b;
	sparse::Matrix D_P_L = -(D + L);

	logC << std::endl << "Metoda Gaussa-Seidela: " << std::endl;

	double normResidum2 = residuum2.norm();
	int spr2 = 0;
	while (normResidum2 > 0.000000001) {
		sparse::Matrix pomMatrix = ((U*x2)) - b;
		x2 = x2.forwardSubstitution(D_P_L, pomMatrix);
		residuum2 = (A * x2) - b;
		normResidum2 = residuum2.norm();
		spr2++;
		logC << spr2 << "\t" << normResidum2 << std::endl;
		if (spr2 == 100)
			break;
	}

	logC.close();
}


void D() {	//faktoryzacja LU -> L_*U_*x3 = b, y = U_*x3
	Timer timer = Timer();
	std::chrono::duration<double> countedTime;
	int index = 165407;
	int e = (index % 1000) / 100;
	int a1 = 3, a2 = -1, a3 = -1;
	int N = 900 + ((index % 100) / 10) * 10 + index % 10;
	int f = (index % 10000) / 1000;

	//vect y: L_*y=b  -> y = y.forwardSubstitution(L_,b)
	//vext x3: U_*x3=y -> x3 = x3.backSubstitution(U_,y)

	std::ofstream logD;
	logD.open("D.txt");
	{
		normal::Matrix nA = normal::Matrix(N);
		nA.matrixGenerator(a1, a2, a3);
		normal::Matrix nb = normal::Matrix(N, 1);
		nb.vectorBGenerator(f);

		normal::Matrix L_ = normal::Matrix(N);
		normal::Matrix U_ = normal::Matrix(N);
		normal::Matrix x3 = normal::Matrix(N, 1);
		normal::Matrix y = normal::Matrix(N, 1);
		normal::Matrix residuum = normal::Matrix(N, 1);

		timer.startTimer();

		nA.doolittle_fLU(L_, U_);
		y = y.forwardSubstitution(L_, nb);
		x3 = x3.backSubstitution(U_, y);

		residuum = (nA * x3) - nb;
		double normResiduum = residuum.norm();
	
		countedTime = timer.getCountedTime();
		logD << "faktoryzacjaLU czas: " << countedTime.count() << "sec" << "  \tnorma residuum: " << normResiduum << std::endl;
	}
	logD.close();
}

void E() {
	Timer timer = Timer();
	std::chrono::duration<double> countedTime;
	int index = 165407;
	int e = (index % 1000) / 100;
	int a1 = 5 + e, a2 = -1, a3 = -1, N = 100;
	int f = (index % 10000) / 1000;
	int tab[10] = { 100, 500, 1000, 2000, 3000, 4000, 5000 };
	// 100, 500, 1000, 2000, 3000, 4000, 5000
	// 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000
	// 16000, 17000, 18000, 19000, 20000, 30000, 40000, 50000, 60000, 70000
	// 80000, 90000, 100000, 200000, 300000, 400000, 500000, 600000
	// 700000, 800000, 900000, 1000000

	std::ofstream logE;
	logE.open("E.txt");

	for (int i = 0; i < 7; i++) {
		N = tab[i];
		logE << std::endl << "Czas wykonania dla N = " << N << " wynosi:" << std::endl;

		sparse::Matrix A = sparse::Matrix(N);
		A.matrixGenerator(a1, a2, a3);
		sparse::Matrix b = sparse::Matrix(N, 1);
		b.vectorBGenerator(f);

		sparse::Matrix D = A.matrixD();
		sparse::Matrix L = A.matrixL();
		sparse::Matrix U = A.matrixU();

		// Jackobi x(k+1) = -D^(-1) * ((L + U)*x(k)) + (D^(-1))*b
		{
			sparse::Matrix reversedD = D.reverseD();
			sparse::Matrix x1 = sparse::Matrix(N, 1);
			x1.vectorXGenerator();
			sparse::Matrix residuum1 = (A*x1) - b;
			sparse::Matrix reverseD_M_b = reversedD * b;
			sparse::Matrix L_P_U = L + U;

			timer.startTimer();

			int spr1 = 0;
			while (residuum1.norm() > 0.000000001) {
				x1 = -(reversedD * (L_P_U * x1)) + reverseD_M_b;
				residuum1 = (A*x1) - b;
				spr1++;
			}

			countedTime = timer.getCountedTime();
			logE << "\tCzas metody Jackobiego: " << countedTime.count() << "sec" << std::endl;
		}
		// Gauss-Seidel x(k+1) = -(D + L)^(-1) * ((U)*x(k)) + (D + L)^(-1))*b
		{
			sparse::Matrix x2 = sparse::Matrix(N, 1);
			x2.vectorXGenerator();
			sparse::Matrix residuum2 = (A*x2) - b;
			sparse::Matrix D_P_L = -(D + L);

			timer.startTimer();

			int spr2 = 0;
			while (residuum2.norm() > 0.000000001) {
				sparse::Matrix pomMatrix = ((U*x2)) - b;
				x2 = x2.forwardSubstitution(D_P_L, pomMatrix);
				residuum2 = (A * x2) - b;
				spr2++;
			}

			countedTime = timer.getCountedTime();
			logE << "\tCzas metody Gaussa-Seidela: " << countedTime.count() << "sec" << std::endl;
		}
		// faktoryzacjaLU

		normal::Matrix nA = normal::Matrix(N);
		nA.matrixGenerator(a1, a2, a3);
		normal::Matrix nb = normal::Matrix(N, 1);
		nb.vectorBGenerator(f);
		{
			normal::Matrix L_ = normal::Matrix(N);
			normal::Matrix U_ = normal::Matrix(N);
			normal::Matrix x3 = normal::Matrix(N, 1);
			normal::Matrix y = normal::Matrix(N, 1);
			normal::Matrix residuum = normal::Matrix(N, 1);

			timer.startTimer();

			nA.doolittle_fLU(L_, U_);
			y = y.forwardSubstitution(L_, nb);
			x3 = x3.backSubstitution(U_, y);

			countedTime = timer.getCountedTime();
			logE << "\tCzas metody faktoryzacjaLU normal: " << countedTime.count() << "sec" << std::endl;
		}
	}
}
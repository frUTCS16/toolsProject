//**************************************************************
//
// this program is for calculating the solution to the first 
// order differntial equation of the form y'=f(y,t)
// in this project we solve y'=y, y0=0
// the analytical solusion is y=e^t
// WRITTEN BY FARZAN MEMARIAN, FALL 2016
//
//************************************************************** 

#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_errno.h>
#include <grvy.h>
using namespace std;
using namespace GRVY;
#include "headers.h"

int DEBUG = 1;
int main(int argc, char *argv[) {
	// in main, the user is asked to decide which problem to choose. 
	// then for each problem, the corresponding functions are called
	
	// reading input using GRVY
	GRVY_Input_CLASS iparse;     // input parsing object
	int problem, debug, verification;
	double h, method;

	
	bool j = true;
	while (j == true){
		cout << "enter the problem number you are interested in, either 1 or 2"<< endl;
		int i=0;
		cin >> i;
		if (i==1) 
		{
			// here we choose some of the parameters for the first problem
			double y0=1; // value of y at t_0=0
			double h=0.01; // step size
			double nEuler = 100; // number of iterations
			std::vector<double> y(nEuler);
			myEuler(y, h, y0, nEuler); // This is the function I wrote for forward euler
			analyticalEuler(h, y0, nEuler); // Analytical solution of y'=y

			// odeSolver makes use of gsl ode solver to solve the y'=y
			odeSolver();
			j = false;
		}
		else if(i==2)
		{
			// this is for the second problem
			odeSolver2();
			j = false;
		}
		else
		{
			cout << "enter either 1 or 2" << endl;
		}
	}
	return 0;
}




std::vector<double>  myEuler(vector<double> & y, double h, double y0, double nEuler)
{
	y[0]=y0;
	for (int t=0; t<nEuler; t++){
		y[t+1] = y[t] + h * y[t];
	}

	// write debug output to screen
	if (DEBUG == 1){
		cout << "these are values of y obtained from myEuler" << endl;
		for (int i=0; i<nEuler; i++){
			cout << y[i] << endl;
		}
	}

	// write output to a file
	ofstream myfile;
	myfile.open("prob1MyEuler.txt");
	for(int i=0; i<nEuler; i++){
		myfile  << i  << "   " << y0 + i*h  << "   " << y[i] << endl;
	}
	myfile.close();
	return y;
}


void analyticalEuler(double h, double y0, double nEuler)
{
	// this functin calculateds the analytical solution of the
	// first order ODE y'=y(t)
	double y=0; // the dependent variable
	double t=0; // the independent variable

	ofstream myfile;
	myfile.open("analyticalEuler.txt");

	if (DEBUG == 1){
		cout << "these are values of y obtained from analytical solution" << endl;
	}
	for (int i=0; i<nEuler; i++){
		t=i*h;
		y = exp(t);

		if (DEBUG == 1){
			cout << i<< "   " << t << "   "  << y << endl;
		}
		myfile  << i << "   " << t << "   "  << y << endl;
	}
	myfile.close();
}



// THIS IS THE GSL VERSION
int func (double t, const double y[], double f[], void *params)
{
	(void)(t); 
	f[0] = y[0];
	return GSL_SUCCESS;
}

int jac (double t, const double y[], double *dfdy, double dfdt[], void *params)
{
	(void)(t);
	// Jacobian matrix, df/dy 
	gsl_matrix_view dfdy_mat 
		= gsl_matrix_view_array (dfdy, 0, 0);
	gsl_matrix * m = &dfdy_mat.matrix; 
	gsl_matrix_set (m, 0, 0, 1.0);
	dfdt[0] = 1.0;
	return GSL_SUCCESS;
}

void odeSolver()
{
	// this function solves the ODE using gsl library
	gsl_odeiv2_system sys = {func, jac, 1};

	gsl_odeiv2_driver * d = 
		gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk2,
				1e-6, 1e-6, 0.0);

	int i;
	double t = 0.0, t1 = 1.0;
	double y[1] = { 1.0 };

	ofstream myfile;
	myfile.open("gslSolverProb1.txt");
	for (i = 1; i <= 1000; i++)
	{
		double ti = i * t1 / 1000.0;
		int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

		if (status != GSL_SUCCESS)
		{
			printf ("error, return value=%d\n", status);
			break;
		}
		if (DEBUG == 1){
		printf ("%.5e %.5e \n", t, y[0]);
		}
		myfile  << t << y[0] << endl;
	}
	myfile.close();
	gsl_odeiv2_driver_free (d);
}



int func2 (double t, const double y[], double f[],
		void *params)
{
	(void)(t); /* avoid unused parameter warning */
	int w = 5;
	int T = 5;
	f[0] = y[3];
	f[1] = y[4];
	f[2] = y[5];
	f[3] = w*y[4]-y[3]/T;
	f[4] = -w*y[3] - y[4]/T;
	f[5] = -y[5]/T;
	return GSL_SUCCESS;
}

int jac2 (double t, const double y[], double *dfdy, 
		double dfdt[], void *params)
{
	(void)(t); /* avoid unused parameter warning */
	gsl_matrix_view dfdy_mat 
		= gsl_matrix_view_array (dfdy, 6, 6);
	gsl_matrix * m = &dfdy_mat.matrix; 
	gsl_matrix_set (m, 0, 0, 0.0);
	gsl_matrix_set (m, 0, 1, 0.0);
	gsl_matrix_set (m, 0, 2, 0.0);
	gsl_matrix_set (m, 0, 3, 1.0);
	gsl_matrix_set (m, 0, 4, 0.0);
	gsl_matrix_set (m, 0, 5, 0.0);
	gsl_matrix_set (m, 1, 0, 0.0);
	gsl_matrix_set (m, 1, 1, 0.0);
	gsl_matrix_set (m, 1, 2, 0.0);
	gsl_matrix_set (m, 1, 3, 0.0);
	gsl_matrix_set (m, 1, 4, 1.0);
	gsl_matrix_set (m, 1, 5, 0.0);
	gsl_matrix_set (m, 2, 0, 0.0);
	gsl_matrix_set (m, 2, 1, 0.0);
	gsl_matrix_set (m, 2, 2, 0.0);
	gsl_matrix_set (m, 2, 3, 0.0);
	gsl_matrix_set (m, 2, 4, 0.0);
	gsl_matrix_set (m, 2, 5, 1.0);
	gsl_matrix_set (m, 3, 0, 0.0);
	gsl_matrix_set (m, 3, 1, 0.0);
	gsl_matrix_set (m, 3, 2, 0.0);
	gsl_matrix_set (m, 3, 3, -0.2);
	gsl_matrix_set (m, 3, 4, 5.0);
	gsl_matrix_set (m, 3, 5, 0.0);
	gsl_matrix_set (m, 4, 0, 0.0);
	gsl_matrix_set (m, 4, 1, 0.0);
	gsl_matrix_set (m, 4, 2, 0.0);
	gsl_matrix_set (m, 4, 3, -5.0);
	gsl_matrix_set (m, 4, 4, -0.2);
	gsl_matrix_set (m, 4, 5, 0.0);
	gsl_matrix_set (m, 5, 0, 0.0);
	gsl_matrix_set (m, 5, 1, 0.0);
	gsl_matrix_set (m, 5, 2, 0.0);
	gsl_matrix_set (m, 5, 3, 0.0);
	gsl_matrix_set (m, 5, 4, 0.0);
	gsl_matrix_set (m, 5, 5, -0.2);
	dfdt[0] = 0.0;
	dfdt[1] = 0.0;
	dfdt[2] = 0.0;
	dfdt[3] = 0.0;
	dfdt[4] = 0.0;
	dfdt[5] = 0.0;

	return GSL_SUCCESS;
}

void odeSolver2()
{
	gsl_odeiv2_system sys = {func2, jac2, 6};

	gsl_odeiv2_driver * d = 
		gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk4,
				1e-6, 1e-6, 0.0);
	int i;
	double t = 0.0, t1 = 30;
	double y[6] = {0.0, 0.0, 0.0, 20.0, 0.0, 2.0};

	FILE * pFile;
	pFile = fopen ("prob2.txt","w");

	for (i = 1; i <= 1000; i++)
	{
		double ti = i * t1 / 1000.0;
		int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

		if (status != GSL_SUCCESS)
		{
			printf ("error, return value=%d\n", status);
			break;
		}

		//myfile  << t << y[0] << y[1] << y[2] << y[3] << y[4] << y[5] << endl;
		fprintf (pFile, "%.5e     %.5e     %.5e     %.5e     %.5e     %.5e     %.5e\n", t, y[0], y[1], y[2], y[3], y[4], y[5]);
		//printf ("%.5e %.5e %.5e %.5e\n", t, y[0], y[1], y[2]);
	}

	fclose(pFile);
	gsl_odeiv2_driver_free (d);
}


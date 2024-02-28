#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <vector>

using namespace std;

#ifndef POLYNOM
#define POLYNOM

vector<vector<double>> A_inverse(double h);

class Polynom
{
  private:
    int i;
    int j;
    vector<double> x;
    vector<double> y;

  public:
    vector<vector<double>> G;
    vector<vector<double>> F_ij;
    double value(double px, double py);
    Polynom(int i, int j, const vector<double> &x, const vector<double> &y,
            const vector<vector<double>> &F_ij);
};

#endif

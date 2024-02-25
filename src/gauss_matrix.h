#include <iostream>
#include <string>
#include <vector>

#pragma once

class GaussMatrix
{
private:
  int N;
  std::vector<std::vector<double>> values;
  std::vector<double> m; // for keeping the values returned by A actions
  std::vector<std::vector<double>> n;
  static constexpr double eps = 1.19209e-07;

  static std::vector<double> numbersFromLine(std::string &line, int N);

  // Action A: find multiplier for row i, needed to subtract this row from row k
  void actionA(int i, int k);

  // Action B: find j-th element of row i multiplied by m
  void actionB(int i, int j, int k);

  // Action C: subtract j-th element of row i from row k
  void actionC(int j, int k);

  void pivoting(int i);

public:
  void readMatrixFromFile(std::string filename);
  void writeMatrixToFile(std::string filename);

  // Initalise the matrix of size N x (N+1) from A and Y
  void init(int N, std::vector<std::vector<double>> &A, std::vector<double> &Y);
  int getN();

  // Concurrent Gaussian elimination (with sequential pivoting)
  void concurrentGauss();

  void backSubstitution();

  double &operator()(int row, int col); // Access the individual elements
  friend std::ostream &operator<<(std::ostream &out,
                                  const GaussMatrix &M); // Print the matrix
};
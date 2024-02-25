#include <fstream>
#include <iomanip>
#include <math.h>
#include <thread>

#include "gauss_matrix.h"

std::vector<double> GaussMatrix::numbersFromLine(std::string &line, int N)
{
  std::vector<double> result;

  std::string token;
  size_t start = 0;
  size_t end = line.find(' ');
  for (int i = 0; i < N; i++)
  {
    token = line.substr(start, end - start);
    result.push_back(std::stod(token));
    start = end + 1;
    end = line.find(' ', start);
  }

  return result;
}

/////////////// actions ///////////////
// Action A: find multiplier for row i, needed to subtract this row from row k
void GaussMatrix::actionA(int i, int k) { m[k] = values[k][i] / values[i][i]; }

// Action B: find j-th element of row i multiplied by m
void GaussMatrix::actionB(int i, int j, int k)
{
  n[k][j] = values[i][j] * m[k];
}

// Action C: subtract j-th element of row i from row k
void GaussMatrix::actionC(int j, int k) { values[k][j] -= n[k][j]; }
/////////////// actions ///////////////

void GaussMatrix::pivoting(int i)
{
  if (std::abs(values[i][i]) < eps)
  {
    int j = i + 1;
    while (j < N && std::abs(values[j][i]) < eps)
      j++;

    if (j == N)
      throw std::invalid_argument("Could not perform pivoting.");

    for (int k = i; k <= N; k++)
    {
      std::swap(values[i][k], values[j][k]);
    }
  }
}

void GaussMatrix::readMatrixFromFile(std::string filename)
{
  std::ifstream file(filename);

  std::string line;
  std::getline(file, line);
  int N = std::stoi(line);

  std::vector<std::vector<double>> A(N, std::vector<double>(N));
  for (int i = 0; i < N; i++)
  {
    std::getline(file, line);
    std::vector<double> row = numbersFromLine(line, N);
    for (int j = 0; j < N; j++)
    {
      A[i][j] = row[j];
    }
  }

  std::getline(file, line);
  std::vector<double> Y = numbersFromLine(line, N);

  file.close();

  init(N, A, Y);
}

void GaussMatrix::writeMatrixToFile(std::string filename)
{
  std::ofstream file(filename);

  file << (*this);

  file.close();
}

// Initalise the matrix of size Nx(N+1) from M and Y
void GaussMatrix::init(int N, std::vector<std::vector<double>> &A,
                       std::vector<double> &Y)
{
  this->N = N;
  values.resize(N);
  m.resize(N);
  n.resize(N);
  for (int i = 0; i < N; i++)
  {
    values[i].resize(N + 1);
    n[i].resize(N + 1);
    for (int j = 0; j < N; j++)
    {
      values[i][j] = A[i][j];
    }
    values[i][N] = Y[i];
  }
}

int GaussMatrix::getN() { return N; }

void GaussMatrix::concurrentGauss()
{
  std::vector<std::thread> threads;
  for (int i = 0; i < N - 1; i++)
  {
    pivoting(i); // Perform pivoting if necessary

    // F_A,i
    threads.resize(0);
    for (int j = i + 1; j < N; j++)
    {
      std::thread threadA(&GaussMatrix::actionA, this, i, j);
      threads.push_back(std::move(threadA));
    }
    for (auto &t : threads)
    {
      t.join();
    }

    // F_B,i
    threads.resize(0);
    for (int k = i + 1; k < N; k++)
    {
      for (int j = i; j <= N; j++)
      {
        std::thread threadB(&GaussMatrix::actionB, this, i, j, k);
        threads.push_back(std::move(threadB));
      }
    }
    for (auto &t : threads)
    {
      t.join();
    }

    // F_C,i
    threads.resize(0);
    for (int k = i + 1; k < N; k++)
    {
      for (int j = i; j <= N; j++)
      {
        std::thread threadC(&GaussMatrix::actionC, this, j, k);
        threads.push_back(std::move(threadC));
      }
    }
    for (auto &t : threads)
    {
      t.join();
    }
  }
}

void GaussMatrix::backSubstitution()
{
  for (int i = N - 1; i >= 0; i--)
  {
    for (int j = N - 1; j > i; j--)
    {
      values[i][N] -= values[i][j] * values[j][N];
      values[i][j] = 0;
    }
    values[i][N] /= values[i][i];
    values[i][i] = 1;
  }
}

double &GaussMatrix::operator()(int row, int col) { return values[row][col]; }

// Print the matrix
std::ostream &operator<<(std::ostream &out, const GaussMatrix &M)
{
  out << M.N << std::endl;
  out << std::setprecision(16);
  for (int i = 0; i < M.N; i++)
  {
    for (int j = 0; j < M.N; j++)
    {
      if (std::abs(M.values[i][j]) < M.eps)
      {
        out << 0 << " ";
      }
      else
      {
        out << M.values[i][j] << " ";
      }
    }
    out << std::endl;
  }
  for (int i = 0; i < M.N; i++)
  {
    if (std::abs(M.values[i][M.N]) < M.eps)
    {
      out << 0 << " ";
    }
    else
    {
      out << M.values[i][M.N] << " ";
    }
  }
  return out;
}
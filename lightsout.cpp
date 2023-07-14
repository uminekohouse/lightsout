#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> get_laplacian_matrix(int N) {
  vector<vector<int>> A(N * N, vector<int>(N * N, false));
  for (int i = 0; i < N * N; ++i) {
    A[i][i] = 1;
    if (i % N != 0)
      A[i][i - 1] = 1;
    if ((i + 1) % N != 0)
      A[i][i + 1] = 1;
    if (i + N < N * N)
      A[i][i + N] = 1;
    if (i - N >= 0)
      A[i][i - N] = 1;
  }
  return A;
}

tuple<vector<vector<int>>, vector<vector<int>>>
get_stair_matrix_inverse_matrix(vector<vector<int>> A) {
  int m = A.size();
  vector<vector<int>> B(m, vector<int>(m, 0));
  for (int i = 0; i < m; ++i)
    B[i][i] = 1;

  for (int i = 0; i < m; ++i) {
    if (!A[i][i]) {
      for (int j = i + 1; j < m; ++j) {
        if (A[j][i]) {
          swap(A[i], A[j]);
          swap(B[i], B[j]);
          break;
        }
      }
    }
    for (int j = i + 1; j < m; ++j) {
      if (A[j][i]) {
        for (int k = 0; k < m; ++k) {
          A[j][k] ^= A[i][k];
          B[j][k] ^= B[i][k];
        }
      }
    }

    for (int i = m - 1; i >= 0; --i) {
      bool ok = false;
      if (!A[i][i])
        continue;
      for (int j = i - 1; j >= 0; --j) {
        if (A[j][i]) {
          for (int k = 0; k < m; ++k) {
            A[j][k] ^= A[i][k];
            B[j][k] ^= B[i][k];
          }
        }
      }
    }

    return make_tuple(A, B);
  }
}

vector<int> matrix_times_vector(vector<vector<int>> A, vector<int> b) {
  int m = A.size();
  vector<int> res(m, 0);
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < m; ++j) {
      res[i] ^= A[i][j] * b[j];
    }
  }
  return res;
}

vector<vector<int>> matrix_times_matrix(vector<vector<int>> A,
                                        vector<vector<int>> B) {

  vector<vector<int>> C(A.size(), vector<int>(A.size(), 0));
  for (int i = 0; i < A.size(); ++i) {
    for (int j = 0; j < A.size(); ++j) {
      for (int k = 0; k < A.size(); ++k) {
        C[i][j] ^= B[i][k] * A[k][j];
      }
    }
  }
  return C;
}

int get_corank(vector<vector<int>> A) {
  for (int i = 0; i < A.size(); ++i) {
    bool ok = true;
    for (int j = 0; j < A.size(); ++j)
      if (A[i][j])
        ok = false;
    if (ok)
      return A.size() - i;
  }
  return 0;
}

int get_1st_coefficient(vector<vector<int>> B, int corank) {
  int n = B.size();
  int cnt = 0;
  for (int i = 0; i < n; ++i) {
    vector<int> x(n, 0);
    x[i] = 1;
    x = matrix_times_vector(B, x);
    bool ok = true;
    for (int j = n - corank; j < n; ++j)
      if (x[j])
        ok = false;
    if (ok)
      cnt++;
  }
  return cnt;
}

int get_2nd_coefficient(vector<vector<int>> B, int corank) {
  int n = B.size();
  int cnt = 0;
  for (int i = 0; i < n - 1; ++i) {
    for (int j = i + 1; j < n; ++j) {
      vector<int> x(n, 0);
      x[i] = 1;
      x[j] = 1;
      x = matrix_times_vector(B, x);
      bool ok = true;
      for (int k = n - corank; k < n; ++k)
        if (x[k])
          ok = false;
      if (ok)
        cnt++;
    }
  }
  return cnt;
}

void write_csv(int i, int A_1, int A_2, int corank) {
  ofstream ofs;
  string output_csv_file_path = "tmp.csv";
  ofs.open(output_csv_file_path, ios::app);
  ofs << i << "," << A_1 << "," << A_2 << "," << i * i << "," << corank << endl;
  ofs.close();
}

int main() {
  for (int i = 1; i <= 1000; ++i) {
    vector<vector<int>> A = get_laplacian_matrix(i);
    vector<vector<int>> B;
    tie(A, B) = get_stair_matrix_inverse_matrix(A);
    int corank = get_corank(A);
    int A_1 = get_1st_coefficient(B, corank);
    int A_2 = get_2nd_coefficient(B, corank);
    write_csv(i, A_1, A_2, corank);
    cout << "n" << i << "corank" << corank << "A_1" << A_1 << "A_2" << A_2;
  }
}

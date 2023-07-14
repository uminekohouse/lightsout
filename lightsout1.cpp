#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> get_laplacian_matrix(int H, int W) {
  int N = H * W;
  vector<vector<int>> A(N, vector<int>(N, false));
  for (int i = 0; i < N; ++i) {
    A[i][i] = 1;
    if (i % H != 0)
      A[i][i - 1] = 1;
    if ((i + 1) % H != 0)
      A[i][i + 1] = 1;
    if (i + W < N)
      A[i][i + W] = 1;
    if (i - W >= 0)
      A[i][i - W] = 1;
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

void solve_all_ones(int H, int W, vector<vector<int>> inverse_matrix) {
  int N = H * W;
  vector<int> all_ones(N, 1);
  vector<int> solve_matrix = matrix_times_vector(inverse_matrix, all_ones);
  vector<vector<int>> tmp(H, vector<int>(W));
  for (int i = 0; i < H; ++i) {
    for (int j = 0; j < W; ++j) {
      tmp[i][j] = solve_matrix[i * H + j];
    }
  }
  for (auto vec : tmp) {
    for (auto val : vec) {
      cout << val;
    }
    cout << endl;
  }
}

void write_csv(int i, int A_1, int A_2, int corank) {
  ofstream ofs;
  string output_csv_file_path = "tmp.csv";
  ofs.open(output_csv_file_path, ios::app);
  ofs << i << "," << A_1 << "," << A_2 << "," << i * i << "," << corank << endl;
  ofs.close();
}

int main() {
  for (int i = 1; i <= 3; ++i) {
    for (int j = 1; j <= 3; ++j) {
      vector<vector<int>> A = get_laplacian_matrix(i, j);
      vector<vector<int>> B;
      tie(A, B) = get_stair_matrix_inverse_matrix(A);
      int corank = get_corank(A);
      /*
      set<vector<vector<int>> solves;
      solves = solve_all_ones(i, j, B);
      */
      cout << "H" << i << "W" << j << endl;
      cout << "corank" << corank << endl;
      solve_all_ones(i, j, B);
      cout << endl;
    }
  }
}

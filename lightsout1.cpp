#include <fstream>
#include <iostream>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void print_vector(vector<int> vec) {
  for (auto v : vec)
    cout << v;
  cout << endl;
}

vector<vector<int>> vector_to_matrix(int H, int W, vector<int> vec) {
  vector<vector<int>> res(H, vector<int>(W));
  for (int i = 0; i < H; ++i) {
    for (int j = 0; j < W; ++j) {
      res[i][j] = vec[i * W + j];
    }
  }
  return res;
}

void print_matrix(vector<vector<int>> matrix) {
  for (int i = 0; i < matrix.size(); ++i) {
    for (int j = 0; j < matrix[i].size(); ++j) {
      if (j)
        cout << " ";
      cout << matrix[i][j];
    }
    cout << endl;
  }
}

vector<vector<int>> get_laplacian_matrix(int H, int W) {
  int N = H * W;
  vector<vector<int>> A(N, vector<int>(N, false));
  for (int i = 0; i < N; ++i) {
    A[i][i] = 1;
    if (i % W != 0)
      A[i][i - 1] = 1;
    if ((i + 1) % W != 0)
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
  int N = A.size();
  vector<vector<int>> B(N, vector<int>(N, 0));
  for (int i = 0; i < N; ++i)
    B[i][i] = 1;

  for (int i = 0; i < N; ++i) {
    if (!A[i][i]) {
      for (int j = i + 1; j < N; ++j) {
        if (A[j][i]) {
          swap(A[i], A[j]);
          swap(B[i], B[j]);
          break;
        }
      }
    }
    for (int j = i + 1; j < N; ++j) {
      if (A[j][i]) {
        for (int k = 0; k < N; ++k) {
          A[j][k] ^= A[i][k];
          B[j][k] ^= B[i][k];
        }
      }
    }
  }

  for (int i = N - 1; i >= 0; --i) {
    bool ok = false;
    if (!A[i][i])
      continue;
    for (int j = i - 1; j >= 0; --j) {
      if (A[j][i]) {
        for (int k = 0; k < N; ++k) {
          A[j][k] ^= A[i][k];
          B[j][k] ^= B[i][k];
        }
      }
    }
  }

  return make_tuple(A, B);
}

vector<int> matrix_times_vector(vector<vector<int>> A, vector<int> b) {
  int N = A.size();
  vector<int> res(N, 0);
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      res[i] ^= A[i][j] * b[j];
    }
  }
  return res;
}

vector<vector<int>> matrix_times_matrix(vector<vector<int>> A,
                                        vector<vector<int>> B) {

  int N = A.size();
  vector<vector<int>> C(N, vector<int>(N, 0));
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < N; ++k) {
        C[i][j] ^= B[i][k] * A[k][j];
      }
    }
  }
  return C;
}

int get_corank(vector<vector<int>> A) {
  int N = A.size();
  for (int i = 0; i < N; ++i) {
    bool ok = true;
    for (int j = 0; j < N; ++j)
      if (A[i][j])
        ok = false;
    if (ok)
      return N - i;
  }
  return 0;
}

set<vector<int>> get_kernel_vectors(vector<vector<int>> A, int corank) {
  int N = A.size();
  set<vector<int>> res;
  vector<int> tmp(N, 0);
  res.insert(tmp);

  set<vector<int>> kernel_base_vectors;
  for (int i = N - corank; i < N; ++i) {
    vector<int> v(N, 0);
    for (int j = 0; j < i; ++j) {
      if (A[j][i])
        v[j] = 1;
    }
    v[i] = 1;
    kernel_base_vectors.insert(v);
  }

  for (auto v : kernel_base_vectors) {
    set<vector<int>> st_tmp;
    for (auto s : res) {
      vector<int> tmp(N, 0);
      for (int i = 0; i < N; ++i) {
        tmp[i] = s[i] ^ v[i];
      }
      st_tmp.insert(tmp);
      st_tmp.insert(s);
    }
    res = st_tmp;
  }
  return res;
}

set<vector<int>> solve_all_ones(int H, int W,
                                vector<vector<int>> inverse_matrix,
                                vector<vector<int>> stair_matrix, int corank) {
  int N = H * W;
  vector<int> all_ones(N, 1);
  vector<int> solve_vector = matrix_times_vector(inverse_matrix, all_ones);
  set<vector<int>> solve_vector_set;
  set<vector<int>> kernek_vectors = get_kernel_vectors(stair_matrix, corank);
  for (auto vec : kernek_vectors) {
    vector<int> tmp(N);
    for (int i = 0; i < N; ++i)
      tmp[i] = vec[i] ^ solve_vector[i];
    solve_vector_set.insert(tmp);
  }
  return solve_vector_set;
}

void write_csv(int i, int A_1, int A_2, int corank) {
  ofstream ofs;
  string output_csv_file_path = "tmp.csv";
  ofs.open(output_csv_file_path, ios::app);
  ofs << i << "," << A_1 << "," << A_2 << "," << i * i << "," << corank << endl;
  ofs.close();
}

int main() {
  int H = 4;
  int W = 4;
  cout << H * W << endl;
  for (int i = 1; i <= H; ++i) {
    for (int j = 1; j <= W; ++j) {
      vector<vector<int>> A = get_laplacian_matrix(i, j);
      vector<vector<int>> B;
      tie(A, B) = get_stair_matrix_inverse_matrix(A);
      int corank = get_corank(A);
      set<vector<int>> solves;
      solves = solve_all_ones(i, j, B, A, corank);
      cout << "H " << i << " W" << j << " solvesize " << solves.size() << endl;
      cout << "corank" << corank << endl;
      cout << endl;
      cout << "solve_matrix_list" << endl;
      for (auto vec : solves) {
        print_matrix(vector_to_matrix(i, j, vec));
        cout << endl;
      }
      /*
      for (auto vec : get_kernel_vectors(A, corank)) {
        print_matrix(vector_to_matrix(i, j, vec));
        cout << endl;
      }
      */
    }
  }
}

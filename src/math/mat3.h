#ifndef MAT3_H
#define MAT3_H

#include <cmath>
#include <ostream>
#include <iomanip>

struct Mat3 {
  // row-major: m[row][col]
  float m[3][3];

  static Mat3 identity() {
    Mat3 r{};
    r.m[0][0]=1; r.m[0][1]=0; r.m[0][2]=0;
    r.m[1][0]=0; r.m[1][1]=1; r.m[1][2]=0;
    r.m[2][0]=0; r.m[2][1]=0; r.m[2][2]=1;
    return r;
  }
};

inline Mat3 transpose(const Mat3& a) {
  Mat3 r{};
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      r.m[i][j] = a.m[j][i];
  return r;
}

inline std::ostream& operator<<(std::ostream& os, const Mat3& M) {
  std::ios oldState(nullptr);
  oldState.copyfmt(os);

  os << std::fixed << std::setprecision(4);
  for (int r = 0; r < 3; ++r) {
    os << "[ ";
    for (int c = 0; c < 3; ++c) {
      os << std::setw(9) << M.m[r][c] << ' ';
    }
    os << "]";
    if (r < 2) os << '\n';
  }

  os.copyfmt(oldState);
  return os;
}


#endif

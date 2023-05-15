#ifndef CMathUtil_H
#define CMathUtil_H

#include <string>
#include <cmath>
#include <cassert>

namespace CMathUtil {

inline bool isInteger(double r) {
  return std::abs(r - static_cast<double>(static_cast<int>(r))) < 1E-3;
}

inline bool isLong(double r) {
  return std::abs(r - static_cast<double>(static_cast<long>(r))) < 1E-3;
}

inline bool realEq(double r1, double r2, double tol=1E-6) {
  return (std::abs(r1 - r2) < tol);
}

#if 0
inline bool realEq(double r1, double r2, double tol=1E-6) {
  if (r1 == r2 || std::abs(r1 - r2) < tol)
    return true;

  return (std::abs((r1 - r2)/(std::abs(r2) > std::abs(r1) ? r2 : r1)) <= tol);
}
#endif

inline bool isZero(double r) {
  return realEq(r, 0.0);
}

//------

// sign of value
template<typename T>
int sign(T v) {
  return (T(0) < v) - (v < T(0));
}

// average of two reals
inline double avg(double x1, double x2) {
  return (x1 + x2)/2.0;
}

// map x in low->high to 0->1
inline double norm(double x, double low, double high) {
  if (high != low)
    return (x - low)/(high - low);
  else
    return low;
}

// map x in 0->1 to low->high
inline double lerp(double x, double low, double high) {
  return low + (high - low)*x;
}

// map value in range low1->high2 to low2->high2
inline double map(double value, double low1, double high1, double low2, double high2) {
  return lerp(norm(value, low1, high1), low2, high2);
}

inline double mapSqr(double value, double low1, double high1, double low2, double high2) {
  assert(value >= 0 && low1 >= 0 && high1 >= 0);

  auto n = norm(std::sqrt(value), std::sqrt(low1), std::sqrt(high1));

  return lerp(n, low2, high2);
}

// clamp value to range
template<typename T>
inline T clamp(const T &val, const T &low, const T &high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

//------

inline double Deg2Rad(double d) { return M_PI*d/180.0; }
inline double Rad2Deg(double r) { return 180.0*r/M_PI; }

inline double normalizeAngle(double a, bool isEnd=false) {
  while (a < 0.0) a += 360.0;

  if (! isEnd) {
    while (a >= 360.0) a -= 360.0;
  }
  else {
    while (a > 360.0) a -= 360.0;
  }

  return a;
}

//------

inline std::string scaledNumberString(double r, int ndp=3) {
  // (1,000) k, (1,000,000) M, (1,000,000,000) G, (1,000,000,000,000) T
  // (0.001) m, (0.000 001) µ, (0.000 000 001) n, (0.000 000 000 001) p

  if (r == 0.0) return "0"; // TODO: epsilon

  if (r < 0) return "-" + scaledNumberString(-r);

  int n = int(std::log10(r));

  auto realToString = [&](double r1) {
    auto s = std::to_string(r1);

    auto len = s.size();

    auto p = s.find('.');

    if (ndp > 0) {
      if (p == std::string::npos) {
        s += ".";

        p = s.find('.');
      }

      while (int(len - p) <= ndp) {
        s += "0";

        ++len;
      }

      return s.substr(0, p + size_t(ndp) + 1);
    }
    else {
      if (p == std::string::npos)
        return s;

      return s.substr(0, p);
    }
  };

  if (n >= 0) {
    if      (n <  3) return realToString(r                )      ;
    else if (n <  6) return realToString(r/         1000.0) + "k";
    else if (n <  9) return realToString(r/      1000000.0) + "M";
    else if (n < 12) return realToString(r/   1000000000.0) + "G";
    else             return realToString(r/1000000000000.0) + "T";
  }
  else {
    if      (n >  -3) return realToString(r                )      ;
    else if (n >  -6) return realToString(r*         1000.0) + "m";
    else if (n >  -9) return realToString(r*      1000000.0) + "µ";
    else if (n > -12) return realToString(r*   1000000000.0) + "n";
    else              return realToString(r*1000000000000.0) + "p";
  }
}

}

#endif

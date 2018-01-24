#ifndef CQBucketer_H
#define CQBucketer_H

#include <QVariant>

class CQBucketer {
 public:
  enum class Type {
    STRING,
    INTEGER_RANGE,
    REAL_RANGE
  };

 public:
  CQBucketer() { }

  //---

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  //---

  // fixed bucket delta

  double delta() const { return delta_; }
  void setDelta(double r) { delta_ = r; }

  //---

  // auto bucket delta

  double min() const { return min_; needsCalc_ = true; }
  void setMin(double r) { min_ = r; }

  double max() const { return max_; needsCalc_ = true; }
  void setMax(double r) { max_ = r; }

  int numAuto() const { return numAuto_; needsCalc_ = true; }
  void setNumAuto(int i) { numAuto_ = i; }

  //---

  int bucket(const QVariant &var) {
    if      (type() == Type::STRING) {
      QString str = var.toString();

      return stringBucket(str);
    }
    else if (type() == Type::INTEGER_RANGE) {
      int i;

      if (var.type() == QVariant::Int)
        i = var.toInt();
      else {
        bool ok;

        i = var.toString().toInt(&ok);

        if (! ok)
          return INT_MIN; // optional ?
      }

      int idelta = delta();

      if (idelta <= 0)
        return INT_MIN; // optional ?

      int n = i/idelta;

      return n;
    }
    else if (type() == Type::REAL_RANGE) {
      double r = 0.0;

      if (var.type() == QVariant::Double) {
        r = var.toReal();
      }
      else {
        bool ok;

        r = var.toString().toDouble(&ok);

        if (! ok)
          return INT_MIN; // optional ?
      }

      if (delta() <= 0)
        return INT_MIN; // optional ?

      int n = r/delta();

      return n;
    }
    else {
      return INT_MIN; // optional ?
    }
  }

  //----

  QString bucketName(int bucket) {
    if      (type() == Type::STRING) {
      auto p = indString_.find(bucket);

      if (p == indString_.end())
        return "";

      return (*p).second;
    }
    else if (type() == Type::INTEGER_RANGE) {
      int idelta = delta();

      int imin = bucket*idelta;
      int imax = imin + idelta;

      return QString("%1-%2").arg(imin).arg(imax);
    }
    else if (type() == Type::REAL_RANGE) {
      double rmin = bucket*delta();
      double rmax = rmin + delta();

      return QString("%1-%2").arg(rmin).arg(rmax);
    }
    else
      return QString("%1").arg(bucket);
  }

  //----

  void autoCalc() {
    if (needsCalc_) {
      double length = max_ - min_;

      double length1 = length/numAuto_;

      // Calculate nearest Power of Ten to Length
      int power = (length1 > 0 ? roundNearest(log10(length1)) : 1);

      calcDelta_ = 0.1;

      if      (power < 0) {
        for (int i = 0; i < -power; i++)
          calcDelta_ /= 10.0;
      }
      else if (power > 0) {
        for (int i = 0; i <  power; i++)
          calcDelta_ *= 10.0;
      }

      // round min value to increment
      if (length1 > 0) {
        calcDelta_ = calcDelta_*roundNearest(length1/calcDelta_);

        calcMin_ = calcDelta_*roundDown(min_/calcDelta_);
      }
      else {
        calcMin_ = min_;
      }

      needsCalc_ = false;
    }
  }

  //----

 private:
  int stringBucket(const QString &str) {
    auto p = stringInd_.find(str);

    if (p == stringInd_.end()) {
      int ind = stringInd_.size();

      p = stringInd_.insert(p, StringInd::value_type(str, ind));

      indString_[ind] = str;
    }

    return p->second;
  }

  int roundNearest(double x) {
    double x1;

    if (x <= 0.0)
      x1 = (x - 0.499999);
    else
      x1 = (x + 0.500001);

    if (x1 < INT_MIN || x1 > INT_MAX)
      errno = ERANGE;

    return int(x1);
  }

  int roundDown(double x) {
    double x1;

    if (x >= 0.0)
      x1 = (x       + 1E-6);
    else
      x1 = (x - 1.0 + 1E-6);

    if (x1 < INT_MIN || x1 > INT_MAX)
      errno = ERANGE;

    return int(x1);
  }

 private:
  using StringInd = std::map<QString,int>;
  using IndString = std::map<int,QString>;

  Type         type_      { Type::STRING }; // data type
  double       min_       { 0.0 };          // min value
  double       max_       { 0.0 };          // max value
  double       delta_     { 1.0 };          // delta value
  int          numAuto_   { 10 };           // num auto
  double       calcMin_   { 0.0 };          // calculated min value
  double       calcMax_   { 1.0 };          // calculated max value
  double       calcDelta_ { 1.0 };          // calculated delta value
  mutable bool needsCalc_ { true };         // needs auto calc
  StringInd    stringInd_;                  // string to ind map
  IndString    indString_;                  // ind to string map
};

#endif

#ifndef CQBucketer_H
#define CQBucketer_H

#include <QVariant>
#include <cmath>

class CQBucketer {
 public:
  enum class Type {
    STRING,
    INTEGER_RANGE,
    REAL_RANGE,
    REAL_AUTO
  };

 public:
  CQBucketer() { }

  //---

  // get/set value type
  const Type &type() const { return type_; }

  void setType(const Type &t) {
    if (type_ != t) {
      // use auto delete for fixed delta if switching from auto to fixed
      if (type_ == Type::REAL_AUTO && t == Type::REAL_RANGE)
        setRDelta(calcDelta());

      type_ = t;

      needsCalc_ = true;
    }
  }

  //---

  // fixed bucket delta

  double rstart() const { return rstart_; }
  void setRStart(double r) { rstart_ = r; }

  double rdelta() const { return rdelta_; }
  void setRDelta(double r) { rdelta_ = r; }

  double istart() const { return int(rstart_); }
  void setIStart(int i) { rstart_ = i; }

  double idelta() const { return int(rdelta_); }
  void setIDelta(int i) { rdelta_ = i; }

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b) { integral_ = b; }

  //---

  // auto bucket delta

  double rmin() const { return rmin_; }
  void setRMin(double r) { rmin_ = r; needsCalc_ = true; }

  double rmax() const { return rmax_; }
  void setRMax(double r) { rmax_ = r; needsCalc_ = true; }

  double imin() const { return int(rmin_); }
  void setIMin(int i) { rmin_ = i; needsCalc_ = true; }

  double imax() const { return int(rmax_); }
  void setIMax(int i) { rmax_ = i; needsCalc_ = true; }

  int numAuto() const { return numAuto_; }
  void setNumAuto(int i) { numAuto_ = i; needsCalc_ = true; }

  //---

  double calcDelta() const { return calcDelta_; }

  //---

  // get bucket for generic value
  int bucket(const QVariant &var) const {
    int n = INT_MIN; // optional ?

    bool ok;

    if      (type() == Type::STRING) {
      QString str = var.toString();

      return stringBucket(str);
    }
    else if (type() == Type::INTEGER_RANGE) {
      int i = varInt(var, ok);

      if (ok)
        return intBucket(i);
    }
    else if (type() == Type::REAL_RANGE) {
      double r = varReal(var, ok);

      if (ok)
        return realBucket(r);
    }
    else if (type() == Type::REAL_AUTO) {
      double r = varReal(var, ok);

      if (ok)
        return autoRealBucket(r);
    }

    return n;
  }

  bool bucketValues(int bucket, double &min, double &max) const {
    if      (type() == Type::INTEGER_RANGE)
      bucketIValues(bucket, min, max);
    else if (type() == Type::REAL_RANGE)
      bucketRValues(bucket, min, max);
    else if (type() == Type::REAL_AUTO)
      autoBucketValues(bucket, min, max);
    else
      return false;

    return true;
  }

  bool bucketIValues(int bucket, double &min, double &max) const {
    int idelta = this->idelta();

    if (idelta > 0) {
      min = bucket*idelta + calcIStart();
      max = min + idelta;
    }

    return true;
  }

  bool bucketRValues(int bucket, double &min, double &max) const {
    double rdelta = this->rdelta();

    if (rdelta > 0.0) {
      min = bucket*rdelta + calcRStart();
      max = min + rdelta;
    }

    return true;
  }

  bool autoBucketValues(int bucket, double &min, double &max) const {
    double rdelta = calcDelta_;

    if (rdelta > 0.0) {
      min = bucket*rdelta + calcMin_;
      max = min + rdelta;
    }

    return true;
  }

  //----

  static int varInt(const QVariant &var, bool &ok) {
    ok = true;

    if (var.type() == QVariant::Int)
      return var.toInt();

    return var.toString().toInt(&ok);
  }

  static double varReal(const QVariant &var, bool &ok) {
    ok = true;

    if (var.type() == QVariant::Double)
      return var.toReal();

    return var.toString().toDouble(&ok);
  }

  //----

  QString bucketName(int bucket) const {
    if      (type() == Type::STRING) {
      auto p = indString_.find(bucket);

      if (p == indString_.end())
        return "";

      return (*p).second;
    }
    else if (type() == Type::INTEGER_RANGE) {
      int idelta = this->rdelta();

      int imin = bucket*idelta;
      int imax = imin + idelta;

      return QString("%1-%2").arg(imin).arg(imax);
    }
    else if (type() == Type::REAL_RANGE) {
      double rmin = bucket*this->rdelta();
      double rmax = rmin + this->rdelta();

      return QString("%1-%2").arg(rmin).arg(rmax);
    }
    else
      return QString("%1").arg(bucket);
  }

  //----

  void autoCalc() const {
    if (needsCalc_) {
      double length = rmax() - rmin();

      double length1 = length/numAuto();

      // Calculate nearest Power of Ten to Length
      int power = (length1 > 0 ? roundNearest(log10(length1)) : 1);

      // prefer integral values
      if (isIntegral()) {
        if (power < 0) {
          length1 = 1.0;
          power   = 0;
        }
      }

      calcDelta_ = 1;
    //calcDelta_ = 0.1;

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
        calcDelta_ = calcDelta_*roundNearestF(length1/calcDelta_);

        calcMin_ = calcDelta_*roundDownF(rmin()/calcDelta_);
      }
      else {
        calcMin_ = rmin();
      }

      needsCalc_ = false;
    }
  }

  //----

  int stringBucket(const QString &str) const {
    auto p = stringInd_.find(str);

    if (p == stringInd_.end()) {
      int ind = stringInd_.size();

      p = stringInd_.insert(p, StringInd::value_type(str, ind));

      indString_[ind] = str;
    }

    return p->second;
  }

  int intBucket(int i) const {
    int n = INT_MIN; // optional ?

    int idelta = this->idelta();
    int istart = this->calcIStart();

    if (idelta > 0)
      n = (i - istart)/idelta;

    return n;
  }

  int realBucket(double r) const {
    int n = INT_MIN; // optional ?

    double rdelta = this->rdelta();
    double rstart = this->calcRStart();

    if (rdelta > 0)
      n = std::floor((r - rstart)/rdelta);

    return n;
  }

  int autoRealBucket(double r) const {
    autoCalc();

    int n = INT_MIN; // optional ?

    double rdelta = calcDelta_;

    if (rdelta > 0)
      n = std::floor((r - calcMin_)/rdelta);

    return n;
  }

  //----

 private:
  double calcRStart() const {
    double rstart = std::min(rmin(), this->rstart());

    if (rdelta() > 0.0)
      rstart = rdelta()*roundDownF(rstart/rdelta());

    return rstart;
  }

  int calcIStart() const {
    int istart = std::min(imin(), this->istart());

    if (idelta() > 0)
      istart = idelta()*roundDownF(istart/idelta());

    return istart;
  }

  int roundNearest(double x) const {
    double x1;

    if (x <= 0.0)
      x1 = (x - 0.499999);
    else
      x1 = (x + 0.500001);

    if (x1 < INT_MIN || x1 > INT_MAX)
      errno = ERANGE;

    return int(x1);
  }

  int roundDown(double x) const {
    double x1;

    if (x >= 0.0)
      x1 = (x       + 1E-6);
    else
      x1 = (x - 1.0 + 1E-6);

    if (x1 < INT_MIN || x1 > INT_MAX)
      errno = ERANGE;

    return int(x1);
  }

  double roundNearestF(double x) const {
    double x1;

    if (x <= 0.0)
      x1 = (x - 0.499999);
    else
      x1 = (x + 0.500001);

    return std::trunc(x1);
  }

  double roundDownF(double x) const {
    double x1;

    if (x >= 0.0)
      x1 = (x       + 1E-6);
    else
      x1 = (x - 1.0 + 1E-6);

    return std::trunc(x1);
  }

 private:
  using StringInd = std::map<QString,int>;
  using IndString = std::map<int,QString>;

  // data
  Type   type_ { Type::STRING }; // data type
  double rmin_ { 0.0 };          // actual min value
  double rmax_ { 1.0 };          // actual max value
  bool   integral_ { false };    // prefer integral buckets

  // manual
  double rstart_  { 0.0 }; // manual bucktet start value
  double rdelta_  { 1.0 }; // manual bucktet delta value

  // auto bucket number of values
  int numAuto_ { 10 }; // num auto

  // cached data
  mutable bool      needsCalc_ { true }; // needs auto calc
  mutable double    calcMin_   { 0.0 };  // calculated min value
  mutable double    calcMax_   { 1.0 };  // calculated max value
  mutable double    calcDelta_ { 1.0 };  // calculated delta value
  mutable StringInd stringInd_;          // string to ind map
  mutable IndString indString_;          // ind to string map
};

#endif

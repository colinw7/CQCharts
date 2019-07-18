#ifndef CQBucketer_H
#define CQBucketer_H

#include <QVariant>
#include <cmath>
#include <boost/optional.hpp>

/*!
 * \brief bucketer for model
 */
class CQBucketer {
 public:
  enum class Type {
    STRING,
    INTEGER_RANGE,
    REAL_RANGE,
    REAL_AUTO
  };

  using OptReal = boost::optional<double>;

  enum class NameFormat {
    DASH,
    ARROW,
    BRACKETED
  };

 public:
  CQBucketer();

  //---

  // get/set value type
  const Type &type() const { return type_; }
  void setType(const Type &t);

  //---

  void reset();

  //---

  // fixed bucket delta

  //! get/set real start point (of range or one of the buckets)
  double rstart() const { return rstart_.value_or(rmin()); }
  void setRStart(double r) { rstart_ = r; }

  //! get/set real bucket delta size
  double rdelta() const { return rdelta_; }
  void setRDelta(double r) { rdelta_ = r; }

  //! get/set integer start point (of range or one of the buckets)
  int istart() const { return int(rstart()); }
  void setIStart(int i) { rstart_ = i; }

  //! get/set integer bucket delta size
  int idelta() const { return int(rdelta_); }
  void setIDelta(int i) { rdelta_ = i; }

  //---

  //! get/set is integral range
  bool isIntegral() const { return integral_; }
  void setIntegral(bool b) { if (b != integral_) { integral_ = b; needsCalc_ = true; } }

  //---

  // auto bucket delta

  //! get/set real minimum
  double rmin() const { return rmin_; }
  void setRMin(double r) { if (r != rmin_) { rmin_ = r; needsCalc_ = true; } }

  //! get/set real maximum
  double rmax() const { return rmax_; }
  void setRMax(double r) { if (r != rmax_) { rmax_ = r; needsCalc_ = true; } }

  //! get/set integer minimum
  int imin() const { return int(rmin_); }
  void setIMin(int i) { if (i != rmin_) { rmin_ = i; needsCalc_ = true; } }

  //! get/set integer maximum
  int imax() const { return int(rmax_); }
  void setIMax(int i) { if (i != rmax_) { rmax_ = i; needsCalc_ = true; } }

  //! get/set number of automatic buckets
  int numAuto() const { return numAuto_; }
  void setNumAuto(int i) { if (i != numAuto_) { numAuto_ = i; needsCalc_ = true; } }

  //---

  double calcMin  () const { autoCalc(); return calcMin_  ; }
  double calcMax  () const { autoCalc(); return calcMax_  ; }
  double calcDelta() const { autoCalc(); return calcDelta_; }
  int    calcN    () const { autoCalc(); return calcN_    ; }

  //---

  //! get bucket for generic value
  int bucket(const QVariant &var) const;

  bool bucketValues(int bucket, double &min, double &max) const;

  bool bucketIValues(int bucket, int &min, int &max) const;
  bool bucketRValues(int bucket, double &min, double &max) const;

  bool autoBucketValues(int bucket, double &min, double &max) const;

  //----

  static int    varInt (const QVariant &var, bool &ok);
  static double varReal(const QVariant &var, bool &ok);

  //----

  void bucketMinMax(int bucket, QVariant &min, QVariant &max) const;

  //---

  QString bucketName(int bucket, NameFormat format=NameFormat::DASH) const;

  static QString bucketName(int    imin, int    imax, NameFormat format=NameFormat::DASH);
  static QString bucketName(double rmin, double rmax, NameFormat format=NameFormat::DASH);

  //----

  void autoCalc() const;

  //----

  int stringBucket(const QString &str) const;
  QString bucketString(int bucket) const;

  int intBucket(int i) const;

  int realBucket(double r) const;

  int autoRealBucket(double r) const;

  //----

 private:
  double calcRStart() const;
  int    calcIStart() const;

 private:
  using StringInd = std::map<QString,int>;
  using IndString = std::map<int,QString>;

  // data
  Type   type_     { Type::STRING }; //!< data type
  double rmin_     { 0.0 };          //!< actual min value
  double rmax_     { 1.0 };          //!< actual max value
  bool   integral_ { false };        //!< prefer integral buckets

  // manual
  OptReal rstart_;          //!< manual bucket start value
  double  rdelta_  { 1.0 }; //!< manual bucket delta value

  // auto bucket number of values
  int numAuto_ { 10 }; //!< num auto

  // cached data
  mutable bool      needsCalc_ { true }; //!< needs auto calc
  mutable double    calcMin_   { 0.0 };  //!< calculated min value
  mutable double    calcMax_   { 0.0 };  //!< calculated max value
  mutable double    calcDelta_ { 1.0 };  //!< calculated delta value
  mutable int       calcN_     { 0 };    //!< calculated delta value
  mutable StringInd stringInd_;          //!< string to ind map
  mutable IndString indString_;          //!< ind to string map
};

#endif

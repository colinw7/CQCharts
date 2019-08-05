#ifndef CQChartsValueInd_H
#define CQChartsValueInd_H

#include <CQBaseModelTypes.h>
#include <CQChartsUtil.h>

/*!
 * \brief Class to store set of values and return unique id for each unique value
 * \ingroup Charts
 */
class CQChartsValueInd {
 public:
  using Type = CQBaseModelType;

 public:
  CQChartsValueInd() { }

  void clear() {
    type_ = Type::NONE;

    valueSet_.clear();
    setValue_.clear();
    nameSet_ .clear();
    setName_ .clear();
  }

  int calcId(const QVariant &var, CQBaseModelType type);

  int calcId(int i) {
    assert(type_ == Type::NONE || type_ == Type::INTEGER);

    type_ = Type::INTEGER;

    //---

    return i;
  }

  int calcId(double r) {
    assert(type_ == Type::NONE || type_ == Type::REAL);

    type_ = Type::REAL;

    //---

    auto p = valueSet_.find(r);

    if (p == valueSet_.end()) {
      int id = valueSet_.size() + 1;

      p = valueSet_.insert(p, ValueSet::value_type(r, id));

      setValue_[id] = r;
    }

    return (*p).second;
  }

  int calcId(const QString &s) {
    assert(type_ == Type::NONE || type_ == Type::STRING);

    type_ = Type::STRING;

    //---

    auto p = nameSet_.find(s);

    if (p == nameSet_.end()) {
      int id = nameSet_.size() + 1;

      p = nameSet_.insert(p, NameSet::value_type(s, id));

      setName_[id] = s;
    }

    return (*p).second;
  }

  QString idName(int id) const {
    if      (type_ == Type::REAL) {
      auto p = setValue_.find(id);

      if (p == setValue_.end())
        return "";

      return QString("%1").arg((*p).second);
    }
    else if (type_ == Type::STRING) {
      auto p = setName_.find(id);

      if (p == setName_.end())
        return "";

      return (*p).second;
    }
    else
      return QString("%1").arg(id);
  }

 private:
  using ValueSet = std::map<double,int,CQChartsUtil::RealCmp>;
  using SetValue = std::map<int,double>;
  using NameSet  = std::map<QString,int>;
  using SetName  = std::map<int,QString>;

  Type     type_     { Type::NONE }; //!< value type
  ValueSet valueSet_;                //!< value set map
  SetValue setValue_;                //!< set value map
  NameSet  nameSet_;                 //!< name set map
  SetName  setName_;                 //!< set name map
};

#endif

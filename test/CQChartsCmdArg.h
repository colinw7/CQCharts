#ifndef CQChartsCmdArg_H
#define CQChartsCmdArg_H

#include <QString>
#include <vector>

class CQChartsCmdArg {
 public:
  enum class Type {
    None,
    Boolean,
    Integer,
    Real,
    String,
    SBool,
    Enum,
    Color,
    Font,
    LineDash,
    Length,
    Position,
    Rect,
    Polygon,
    Align,
    Sides,
    Column,
    Row
  };

  using NameValue  = std::pair<QString,int>;
  using NameValues = std::vector<NameValue>;

 public:
  CQChartsCmdArg(int ind, const QString &name, Type type, const QString &argDesc="",
                 const QString &desc="") :
   ind_(ind), name_(name), type_(type), argDesc_(argDesc), desc_(desc) {
    if (name_.left(1) == "-") {
      isOpt_ = true;

      name_ = name_.mid(1);
    }
  }

  const QString &name() const { return name_; }

  bool isOpt() const { return isOpt_; }

  Type type() const { return type_; }

  const QString &argDesc() const { return argDesc_; }

  const QString &desc() const { return desc_; }

  bool isRequired() const { return required_; }
  CQChartsCmdArg &setRequired(bool b=true) { required_ = b; return *this; }

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

  CQChartsCmdArg &addNameValue(const QString &name, int value) {
    nameValues_.push_back(NameValue(name, value));
    return *this;
  }

  const NameValues &nameValues() const { return nameValues_; }

 private:
  int        ind_      { -1 };         // command ind
  QString    name_;                    // arg name
  bool       isOpt_    { false };      // is option
  Type       type_     { Type::None }; // value type
  QString    argDesc_;                 // short description
  QString    desc_;                    // long description
  bool       required_ { false };      // is required
  int        groupInd_ { -1 };         // cmd group ind
  NameValues nameValues_;              // enum name values
};

#endif

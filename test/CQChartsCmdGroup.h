#ifndef CQChartsCmdGroup_H
#define CQChartsCmdGroup_H

/*!
 * \brief Charts Tcl Command Argument Group
 * \ingroup Charts
 */
class CQChartsCmdGroup {
 public:
  enum class Type {
    None,
    OneOpt,
    OneReq
  };

 public:
  CQChartsCmdGroup(int ind, Type type) :
   ind_(ind), type_(type) {
  }

  int ind() const { return ind_; }

  bool isRequired() const { return (type_ == Type::OneReq); }

 private:
  int  ind_ { -1 };
  Type type_ { Type::None };
};

#endif

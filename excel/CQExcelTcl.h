#ifndef CQExcelTcl_H
#define CQExcelTcl_H

#include <CQTclUtil.h>

namespace CQExcel {

class Model;

class Tcl : public CQTcl {
 public:
  Tcl(Model *model);

  Model *model() const { return model_; }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

 private:
  void handleRead(const char *name) override;

 private:
  static int sumCmd    (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int averageCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int minCmd    (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int maxCmd    (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int columnCmd (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int rowCmd    (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int cellCmd   (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int evalCmd   (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int concatCmd (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int sumUpCmd  (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);
  static int colorCmd  (ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv);

  bool processCmd(const QString &name, int objc, const Tcl_Obj **objv);

  void argValues(int objc, const Tcl_Obj **objv, QVariantList &values) const;

 private:
  Model* model_  { nullptr };
  int    row_    { 0 };
  int    column_ { 0 };
};

//---

}

#endif

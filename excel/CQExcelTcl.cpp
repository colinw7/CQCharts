#include <CQExcelTcl.h>
#include <CQExcelModel.h>

namespace CQExcel {

Tcl::
Tcl(Model *model) :
 model_(model)
{
  setObjectName("excelTcl");

  createExprCommand("sum"    , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::sumCmd    ), this);
  createExprCommand("average", reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::averageCmd), this);
  createExprCommand("min"    , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::minCmd    ), this);
  createExprCommand("max"    , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::maxCmd    ), this);
  createExprCommand("cell"   , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::cellCmd   ), this);
  createExprCommand("column" , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::columnCmd ), this);
  createExprCommand("row"    , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::rowCmd    ), this);
  createExprCommand("eval"   , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::evalCmd   ), this);
  createExprCommand("concat" , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::concatCmd ), this);
  createExprCommand("sumup"  , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::sumUpCmd  ), this);
  createExprCommand("color"  , reinterpret_cast<CQTcl::ObjCmdProc>(&Tcl::colorCmd  ), this);

  traceVar("row");
  traceVar("column");
}

int
Tcl::
sumCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  return (tcl->processCmd("sum", objc, objv) ? TCL_OK : TCL_ERROR);
}

int
Tcl::
averageCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  return (tcl->processCmd("average", objc, objv) ? TCL_OK : TCL_ERROR);
}

int
Tcl::
minCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  return (tcl->processCmd("min", objc, objv) ? TCL_OK : TCL_ERROR);
}

int
Tcl::
maxCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  return (tcl->processCmd("max", objc, objv) ? TCL_OK : TCL_ERROR);
}

int
Tcl::
columnCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  tcl->setResult(QVariant(tcl->column()));

  return TCL_OK;
}

int
Tcl::
rowCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  tcl->setResult(QVariant(tcl->row()));

  return TCL_OK;
}

int
Tcl::
cellCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  if (values.size() != 1)
    return TCL_ERROR;

  auto *model = tcl->model();

  auto cellStr = values[0].toString();

  int row, col;

  if (! model->decodeCellName(cellStr, row, col))
    return TCL_ERROR;

  auto ind = model->index(row, col, QModelIndex());

  auto var = model->data(ind, Qt::DisplayRole);

  tcl->setResult(var);

  return TCL_OK;
}

int
Tcl::
evalCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  if (values.length() != 1)
    return TCL_ERROR;

  QVariant res;

  if (! tcl->evalExpr(values[0].toString(), res))
    return TCL_ERROR;

  tcl->setResult(res);

  return TCL_OK;
}

int
Tcl::
concatCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  QString res;

  for (const auto &value : values)
    res += value.toString();

  tcl->setResult(res);

  return TCL_OK;
}

int
Tcl::
sumUpCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  QString res;

  if (values.size() != 1)
    return TCL_ERROR;

  auto *model = tcl->model();

  int col;

  if (! model->decodeColumnName(values[0].toString(), col))
    return TCL_ERROR;

  int row = tcl->row();

  auto sum = 0.0;

  for (int r = row; r >= 0; --r) {
    auto ind = model->index(r, col, QModelIndex());

    auto var = model->data(ind, Qt::DisplayRole);

    bool ok;
    auto v = var.toDouble(&ok);

    if (ok)
      sum += v;
  }

  tcl->setResult(sum);

  return TCL_OK;
}

int
Tcl::
colorCmd(ClientData clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *tcl = static_cast<Tcl *>(clientData);

  QVariantList values;

  tcl->argValues(objc, objv, values);

  QString res;

  if (values.size() != 3)
    return TCL_ERROR;

  bool ok;
  auto r = values[0].toDouble(&ok);
  auto g = values[1].toDouble(&ok);
  auto b = values[2].toDouble(&ok);

  tcl->setResult(QVariant(QColor(r, g, b)));

  return TCL_OK;
}

bool
Tcl::
processCmd(const QString &name, int objc, const Tcl_Obj **objv)
{
  QVariantList values;

  argValues(objc, objv, values);

  auto *model = this->model();

  if (name == "sum" || name == "average" || name == "min" || name == "max") {
    std::vector<double> rvalues;

    for (const auto &value : values) {
      auto str = value.toString();

      int row1, col1, row2, col2;

      if (model->decodeCellRange(str, row1, col1, row2, col2)) {
        for (int r = row1; r <= row2; ++r) {
          for (int c = col1; c <= col2; ++c) {
            auto ind = model->index(r, c, QModelIndex());

            auto var = model->data(ind, Qt::DisplayRole);

            bool ok;
            double r = var.toDouble(&ok);

            if (ok)
              rvalues.push_back(r);
          }
        }
      }
      else {
        bool ok;
        double r = str.toDouble(&ok);

        if (ok)
          rvalues.push_back(r);
      }
    }

    if (rvalues.empty()) {
      setResult(0.0);
      return true;
    }

    double res = 0.0;

    if      (name == "sum" || name == "average") {
      for (const auto &r : rvalues)
        res += r;

      if (name == "average")
        res /= double(rvalues.size());
    }
    else if (name == "min") {
      res = rvalues[0];

      for (const auto &r : rvalues)
        res = std::min(res, r);
    }
    else if (name == "max") {
      res = rvalues[0];

      for (const auto &r : rvalues)
        res = std::max(res, r);
    }

    setResult(res);
  }
  else
    assert(false);

  return true;
}

void
Tcl::
argValues(int objc, const Tcl_Obj **objv, QVariantList &values) const
{
  for (int i = 1; i < objc; ++i) {
    const auto *obj = objv[i];

    values.push_back(CQTclUtil::variantFromObj(interp(), obj));
  }
}

void
Tcl::
handleRead(const char *name)
{
  auto *model = this->model();

  if      (strcmp(name, "row") == 0)
    createVar(name, row());
  else if (strcmp(name, "column") == 0)
    createVar(name, column());
  else {
    int row, col;

    if      (model->decodeCellName(QString(name), row, col)) {
      auto ind = model->index(row, col, QModelIndex());

      auto var = model->data(ind, Qt::DisplayRole);

      createVar(name, var);
    }
    else if (model->decodeColumnName(QString(name), col)) {
      int row = this->row();

      auto ind = model->index(row, col, QModelIndex());

      auto var = model->data(ind, Qt::DisplayRole);

      createVar(name, var);
    }
  }
}

}

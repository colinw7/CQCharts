#include <CQExcelModel.h>
#include <CQExcelTcl.h>

#include <cassert>

namespace CQExcel {

Model::
Model(QObject *parent, int nc, int nr) :
 CQDataModel(parent, nc, nr)
{
  init(nc, nr);
}

Model::
Model(int nc, int nr) :
 CQDataModel(nc, nr)
{
  init(nc, nr);
}

void
Model::
init(int nc, int nr)
{
  setObjectName("excelModel");

  tcl_ = new Tcl(this);

  //---

  for (int c = 0; c < nc; ++c) {
    auto name = columnName(c);

    setHeaderData(c, Qt::Horizontal, name, Qt::DisplayRole);
  }

  for (int r = 0; r < nr; ++r) {
    auto name = rowName(r);

    setHeaderData(r, Qt::Vertical, name, Qt::DisplayRole);
  }

  updateTraceVars();
}

void
Model::
addRow(int n)
{
  int nr = rowCount();

  CQDataModel::addRow(n);

  auto name = rowName(nr);

  setHeaderData(nr, Qt::Vertical, name, Qt::DisplayRole);

  updateTraceVars();
}

void
Model::
addColumn(int n)
{
  int nc = columnCount();

  CQDataModel::addColumn(n);

  auto name = columnName(nc);

  setHeaderData(nc, Qt::Horizontal, name, Qt::DisplayRole);

  updateTraceVars();
}

QVariant
Model::
headerData(int section, Qt::Orientation orientation, int role) const
{
  return CQDataModel::headerData(section, orientation, role);
}

QVariant
Model::
data(const QModelIndex &index, int role) const
{
  assert(index.model() == this);

  if      (role == Qt::BackgroundRole || role == Qt::ForegroundRole || role == Qt::FontRole) {
    if (hasCellStyle(index)) {
      const auto &style = this->cellStyle(index);

      if      (role == Qt::BackgroundRole) {
        return style.brush;
      }
      else if (role == Qt::ForegroundRole) {
        return style.pen;
      }
      else if (role == Qt::FontRole) {
        return style.font;
      }
    }
  }
  else if (role == int(CQBaseModelRole::Style)) {
    const auto &style = this->cellStyle(index);

    return style.toString();
  }
  else if (role == int(CQBaseModelRole::Export)) {
    if (hasCellExpression(index))
      return "";

    return CQDataModel::data(index, Qt::DisplayRole);
  }
  else if (role == int(Qt::ToolTipRole)) {
    auto str = CQDataModel::data(index, Qt::DisplayRole).toString();

    if (hasCellExpression(index))
      str += " {" + cellExpression(index) + "}";

    return QVariant(str);
  }

  return CQDataModel::data(index, role);
}

bool
Model::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  assert(index.model() == this);

  bool rc = true;

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
    auto text = value.toString();

    if (text.left(1) == "=")
      setCellExpression(index, text);
    else {
      clearCellExpression(index);

      rc = CQDataModel::setData(index, value, role);
    }
  }
  else if (role == int(CQBaseModelRole::Style)) {
    auto text = value.toString();

    auto style = Style::fromString(text);

    setCellStyle(index, style);
  }
  else {
    rc = CQDataModel::setData(index, value, role);
  }

  //---

  // update associated calculated cells
  // TODO: only those affected by this cell
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    for (const auto &pr : expr_) {
      int r = pr.first;

      for (const auto &pc : pr.second) {
        int c = pc.first;

        if (pc.second.length()) {
          auto ind = this->index(r, c, QModelIndex());

          updateCellExpression(ind, pc.second);
        }
      }
    }
  }

  return rc;
}

//---

bool
Model::
hasCellExpression(const QModelIndex &ind) const
{
  assert(ind.model() == this);

  auto pr = expr_.find(ind.row());
  if (pr == expr_.end()) return false;

  auto pc = (*pr).second.find(ind.column());
  if (pc == (*pr).second.end()) return false;

  return (*pc).second.length();
}

QString
Model::
cellExpression(const QModelIndex &ind) const
{
  assert(ind.model() == this);

  auto pr = expr_.find(ind.row());
  if (pr == expr_.end()) return "";

  auto pc = (*pr).second.find(ind.column());
  if (pc == (*pr).second.end()) return "";

  return (*pc).second;
}

void
Model::
setCellExpression(const QModelIndex &ind, const QString &expr)
{
  assert(ind.model() == this);

  expr_[ind.row()][ind.column()] = expr;

  //---

  updateCellExpression(ind, expr);
}

void
Model::
clearCellExpression(const QModelIndex &ind)
{
  assert(ind.model() == this);

  expr_[ind.row()][ind.column()] = "";
}

void
Model::
updateCellExpression(const QModelIndex &ind, const QString &text)
{
  assert(ind.model() == this);

  auto *tcl = this->tcl();

  tcl->setRow   (ind.row());
  tcl->setColumn(ind.column());

  auto expr = text.mid(1);

  QVariant res;

  if (! tcl->evalExpr(expr, res, /*showError*/true))
    return;

  auto ind1 = index(ind.row(), ind.column(), QModelIndex());

  CQDataModel::setData(ind1, res, Qt::DisplayRole);
}

void
Model::
getCellExpressions(CellExpr &cellExpr) const
{
  for (const auto &pr : expr_) {
    int row = pr.first;

    for (const auto &pc : pr.second) {
      int         column = pc.first;
      const auto &expr   = pc.second;

      if (expr.length())
        cellExpr[index(row, column, QModelIndex())] = expr;
    }
  }
}

//---

bool
Model::
hasCellStyle(const QModelIndex &ind) const
{
  assert(ind.model() == this);

  auto p = cellStyle_.find(ind);

  return (p != cellStyle_.end());
}

const Model::Style &
Model::
cellStyle(const QModelIndex &ind) const
{
  assert(ind.model() == this);

  auto p = cellStyle_.find(ind);
  assert(p != cellStyle_.end());

  return (*p).second;
}

void
Model::
setCellStyle(const QModelIndex &ind, const Style &style)
{
  assert(ind.model() == this);

  cellStyle_[ind] = style;
}

void
Model::
clearCellStyle(const QModelIndex &ind)
{
  assert(ind.model() == this);

  auto p = cellStyle_.find(ind);
  if (p == cellStyle_.end()) return;

  cellStyle_.erase(p);
}

void
Model::
getCellStyles(CellStyle &cellStyle) const
{
  for (const auto &ps : cellStyle_) {
    const auto &ind   = ps.first;
    const auto &style = ps.second;

    cellStyle[ind] = style;
  }
}

//---

QString
Model::
rowName(int r) const
{
  auto p = rowNames_.find(r);

  if (p != rowNames_.end())
    return (*p).second;

  //---

  auto *th = const_cast<Model *>(this);

  auto name = QString::number(r + 1);

  th->rowNames_[r] = name;

  return name;
}

QString
Model::
columnName(int c) const
{
  auto p = columnNames_.find(c);

  if (p != columnNames_.end())
    return (*p).second;

  //---

  static QString columnChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::vector<int> chars;

  int c1 = c;

  while (c1 >= 26) {
    int ic = c1 % 26;

    chars.push_back(ic);

    c1 /= 26;

    --c1;
  }

  chars.push_back(c1);

  QString name;

  for (const auto &c : chars)
    name += columnChars.mid(c, 1);

  auto *th = const_cast<Model *>(this);

  th->columnNames_[c] = name;

  return name;
}

QString
Model::
cellName(int row, int column) const
{
  return QString("%1%2").arg(columnName(column)).arg(rowName(row));
}

bool
Model::
decodeCellRange(const QString &str, int &row1, int &col1, int &row2, int &col2) const
{
  auto p = str.indexOf(":");
  if (p < 0) return false;

  auto lhs = str.mid(0, p);
  auto rhs = str.mid(p + 1);

  if (! decodeCellName(lhs, row1, col1))
    return false;

  if (! decodeCellName(rhs, row2, col2))
    return false;

  return true;
}

bool
Model::
decodeCellName(const QString &name, int &row, int &col) const
{
  row = 0;
  col = 0;

  int pos = 0;
  int len = name.length();

  while (pos < len && name[pos].isLetter())
    ++pos;

  auto lhs = name.mid(0, pos);
  auto rhs = name.mid(pos);

  if (lhs.length() == 0)
    return false;

  bool ok;

  row = rhs.toInt(&ok) - 1;
  if (! ok) return false;

  if (! decodeColumnName(lhs, col))
    return false;

  return true;
}

bool
Model::
decodeColumnName(const QString &name, int &col) const
{
  static QString columnChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  col = 0;

  for (int i = 0; i < name.length(); ++i) {
    int pos = columnChars.indexOf(name.mid(i, 1));
    if (pos < 0) return false;

    col = 26*col + pos;
  }

  return true;
}

void
Model::
setIndicesData(const QModelIndexList &indices, IndicesData &indicesData)
{
  for (const auto &ind : indices) {
    assert(ind.model() == this);

    indicesData.rowCols[ind.row   ()].insert(ind.column());
    indicesData.colRows[ind.column()].insert(ind.row   ());
  }
}

void
Model::
updateTraceVars()
{
  auto *tcl = this->tcl();

  int nr = rowCount();
  int nc = columnCount();

  for (int ir = 0; ir < nr; ++ir) {
    for (int ic = 0; ic < nc; ++ic) {
      auto cellName = this->cellName(ir, ic);

      auto p = traceNames_.find(cellName);

      if (p == traceNames_.end()) {
        tcl->traceVar(cellName);

        traceNames_.insert(cellName);
      }
    }
  }

  for (int ic = 0; ic < nc; ++ic) {
    auto name = columnName(ic);

    auto p = traceNames_.find(name);

    if (p == traceNames_.end()) {
      tcl->traceVar(name);

      traceNames_.insert(name);
    }
  }
}

}

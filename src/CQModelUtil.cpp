#include <CQModelUtil.h>
#include <CQModelVisitor.h>
#include <QSortFilterProxyModel>

namespace CQModelUtil {

bool
isHierarchical(const QAbstractItemModel *model)
{
  if (! model)
    return false;

  QModelIndex parent;

  int nr = model->rowCount(parent);

  nr = std::min(nr, 100); // limit number of rows checked

  for (int row = 0; row < nr; ++row) {
    QModelIndex index1 = model->index(row, 0, parent);

    if (model->hasChildren(index1))
      return true;
  }

  return false;
}

//------

QVariant
modelHeaderValue(const QAbstractItemModel *model, int c, Qt::Orientation orientation,
                 int role, bool &ok)
{
  QVariant var = model->headerData(c, orientation, role);

  ok = var.isValid();

  return var;
}

QVariant
modelHeaderValue(const QAbstractItemModel *model, int c, Qt::Orientation orientation,
                 CQBaseModelRole role, bool &ok)
{
  return modelHeaderValue(model, c, orientation, (int) role, ok);
}

QVariant
modelHeaderValue(const QAbstractItemModel *model, int c, int role, bool &ok)
{
  return modelHeaderValue(model, c, Qt::Horizontal, role, ok);
}

QVariant
modelHeaderValue(const QAbstractItemModel *model, int c, CQBaseModelRole role, bool &ok)
{
  return modelHeaderValue(model, c, (int) role, ok);
}

QString
modelHeaderString(const QAbstractItemModel *model, int c, bool &ok)
{
  QVariant var = modelHeaderValue(model, c, Qt::DisplayRole, ok);

  return var.toString();
}

//------

QVariant
modelValue(const QAbstractItemModel *model, int r, int c, const QModelIndex &parent, bool &ok)
{
  QModelIndex ind = model->index(r, c, parent);

  return modelValue(model, ind, ok);
}

QVariant
modelValue(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok)
{
  QVariant var;

  ok = ind.isValid();

  if (ok) {
    var = model->data(ind, role);

    ok = var.isValid();
  }

  return var;
}

QVariant
modelValue(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok)
{
  QVariant var = modelValue(model, ind, Qt::EditRole, ok);

  if (! ok)
    var = modelValue(model, ind, Qt::DisplayRole, ok);

  return var;
}

double
modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok)
{
  QVariant var = modelValue(model, ind, ok);

  double r = 0.0;

  if (ok)
    r = var.toDouble(&ok);

  return r;
}

long
modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok)
{
  QVariant var = modelValue(model, ind, ok);

  long i = 0;

  if (ok)
    i = var.toInt(&ok);

  return i;
}

QString
modelString(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok)
{
  QVariant var = modelValue(model, ind, ok);

  QString s;

  if (ok)
    s = var.toString();

  return s;
}

//------

bool
columnValueType(const QAbstractItemModel *model, int c, CQBaseModelType &type)
{
  type = CQBaseModelType::STRING;

  QVariant var = model->headerData(c, Qt::Horizontal, (int) CQBaseModelRole::Type);
  if (! var.isValid()) return false;

  bool ok;
  type = (CQBaseModelType) var.toInt(&ok);
  if (! ok) return false;

  return true;
}

CQBaseModelType
calcColumnType(const QAbstractItemModel *model, int icolumn, int maxRows)
{
  //CQPerfTrace trace("CQUtil::calcColumnType");

  // determine column type from values

  // process model data
  class ColumnTypeVisitor : public CQModelVisitor {
   public:
    ColumnTypeVisitor(int column) :
     column_(column) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      QModelIndex ind = model->index(data.row, column_, data.parent);

      // if column can be integral, check if value is valid integer
      if (isInt_) {
        bool ok;

        (void) modelInteger(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isInt_ = false;
      }

      // if column can be real, check if value is valid real
      if (isReal_) {
        bool ok;

        (void) modelReal(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isReal_ = false;
      }

      // not value real or integer so assume string and we are done
      return State::TERMINATE;
    }

    CQBaseModelType columnType() {
      if      (isInt_ ) return CQBaseModelType::INTEGER;
      else if (isReal_) return CQBaseModelType::REAL;
      else              return CQBaseModelType::STRING;
    }

   private:
    int  column_ { -1 };   // column to check
    bool isInt_  { true }; // could be integeral
    bool isReal_ { true }; // could be real
  };

  // determine column value type by looking at model values
  ColumnTypeVisitor columnTypeVisitor(icolumn);

  if (maxRows > 0)
    columnTypeVisitor.setNumRows(maxRows);

  CQModelVisit::exec(model, columnTypeVisitor);

  return columnTypeVisitor.columnType();
}

//------

QAbstractItemModel *
getBaseModel(QAbstractItemModel *model)
{
  QAbstractItemModel *sourceModel = model;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(sourceModel);

  while (proxyModel) {
    sourceModel = proxyModel->sourceModel();

    proxyModel = qobject_cast<QSortFilterProxyModel *>(sourceModel);
  }

  return sourceModel;
}

//------

const QStringList &
roleNames()
{
  static QStringList names;

  if (names.empty())
    names << "display" << "edit" << "user" << "font" << "size_hint" <<
             "tool_tip" << "background" << "foreground" << "text_alignment" <<
             "text_color" << "decoration" <<
             "type" << "base_type" << "type_values" << "min" << "max" << "sorted" <<
             "sort_order" << "title" << "key" << "raw_value" << "intermediate_value" <<
             "cached_value" << "output_value" << "group" << "format" << "data_min" << "data_max";

  return names;
};

int
nameToRole(const QString &name)
{
  if      (name == "display"       ) return Qt::DisplayRole;
  else if (name == "edit"          ) return Qt::EditRole;
  else if (name == "user"          ) return Qt::UserRole;
  else if (name == "font"          ) return Qt::FontRole;
  else if (name == "size_hint"     ) return Qt::SizeHintRole;
  else if (name == "tool_tip"      ) return Qt::ToolTipRole;
  else if (name == "background"    ) return Qt::BackgroundRole;
  else if (name == "foreground"    ) return Qt::ForegroundRole;
  else if (name == "text_alignment") return Qt::TextAlignmentRole;
  else if (name == "text_color"    ) return Qt::TextColorRole;
  else if (name == "decoration"    ) return Qt::DecorationRole;

  else if (name == "type"              ) return (int) CQBaseModelRole::Type;
  else if (name == "base_type"         ) return (int) CQBaseModelRole::BaseType;
  else if (name == "type_values"       ) return (int) CQBaseModelRole::TypeValues;
  else if (name == "min"               ) return (int) CQBaseModelRole::Min;
  else if (name == "max"               ) return (int) CQBaseModelRole::Max;
  else if (name == "sorted"            ) return (int) CQBaseModelRole::Sorted;
  else if (name == "sort_order"        ) return (int) CQBaseModelRole::SortOrder;
  else if (name == "title"             ) return (int) CQBaseModelRole::Title;
  else if (name == "key"               ) return (int) CQBaseModelRole::Key;
  else if (name == "raw_value"         ) return (int) CQBaseModelRole::RawValue;
  else if (name == "intermediate_value") return (int) CQBaseModelRole::IntermediateValue;
  else if (name == "cached_value"      ) return (int) CQBaseModelRole::CachedValue;
  else if (name == "output_value"      ) return (int) CQBaseModelRole::OutputValue;
  else if (name == "group"             ) return (int) CQBaseModelRole::Group;
  else if (name == "format"            ) return (int) CQBaseModelRole::Format;
  else if (name == "data_min"          ) return (int) CQBaseModelRole::DataMin;
  else if (name == "data_max"          ) return (int) CQBaseModelRole::DataMax;

  bool ok;

  int role = name.toInt(&ok);

  if (ok)
    return role;

  return -1;
}

}

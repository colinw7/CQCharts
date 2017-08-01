#include <CQCsvModel.h>
#include <CCsv.h>

CQCsvModel::
CQCsvModel()
{
}

bool
CQCsvModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // parse file into array of fields
  CCsv csv(filename_.toStdString());

  csv.setSkipComments(false);

  CCsv::FieldsArray fieldsArray;

  csv.getFields(fieldsArray);

  //---

  // add fields to model
  numColumns_ = 0;

  bool columnHeaders = hasColumnHeaders();

  for (const auto &fields : fieldsArray) {
    if (columnHeaders) {
      for (const auto &f : fields)
        header_.push_back(f.c_str());

      columnHeaders = false;

      numColumns_ = std::max(numColumns_, int(header_.size()));

      continue;
    }

    Cells cells;

    for (const auto &f : fields)
      cells.push_back(f.c_str());

    numColumns_ = std::max(numColumns_, int(cells.size()));

    data_.push_back(cells);
  }

  return true;
}

int
CQCsvModel::
columnCount(const QModelIndex &) const
{
  return numColumns_;
}

QVariant
CQCsvModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal) {
    if (section < 0 || section >= numColumns_)
      return QVariant();

    if (role == Qt::DisplayRole) {
      if (section < int(header_.size()))
        return QVariant(header_[section]);
    }
    else if (role == Qt::EditRole) {
      return QVariant();
    }
  }

  return QVariant();
}

QVariant
CQCsvModel::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    const Cells &cells = data_[index.row()];

    if (index.column() < int(cells.size()))
      return QString(cells[index.column()]);
    else
      return QVariant();
  }
  else
    return QVariant();
}

QModelIndex
CQCsvModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column, nullptr);
}

QModelIndex
CQCsvModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return QModelIndex();
}

int
CQCsvModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return data_.size();
}

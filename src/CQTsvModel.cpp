#include <CQTsvModel.h>
#include <CTsv.h>

CQTsvModel::
CQTsvModel()
{
}

bool
CQTsvModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // parse file into array of fields
  CTsv tsv(filename_.toStdString());

  tsv.setCommentHeader  (isCommentHeader());
  tsv.setFirstLineHeader(isFirstLineHeader());

  if (! tsv.load())
    return false;

  const CTsv::Fields &header = tsv.header();

  const CTsv::Data &data = tsv.data();

  //---

  // add fields to model
  numColumns_ = 0;

  if (! header.empty()) {
    for (const auto &f : header)
      header_.push_back(f.c_str());

    numColumns_ = std::max(numColumns_, int(header_.size()));
  }

  for (const auto &fields : data) {
    Cells cells;

    for (const auto &f : fields)
      cells.push_back(f.c_str());

    numColumns_ = std::max(numColumns_, int(cells.size()));

    data_.push_back(cells);
  }

  return true;
}

int
CQTsvModel::
columnCount(const QModelIndex &) const
{
  return numColumns_;
}

QVariant
CQTsvModel::
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
CQTsvModel::
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
  else {
    return QVariant();
  }
}

QModelIndex
CQTsvModel::
index(int row, int column, const QModelIndex &) const
{
  return createIndex(row, column, nullptr);
}

QModelIndex
CQTsvModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  return QModelIndex();
}

int
CQTsvModel::
rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return data_.size();
}

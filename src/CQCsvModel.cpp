#include <CQCsvModel.h>
#include <CCsv.h>

CQCsvModel::
CQCsvModel()
{
  // default read only
  setReadOnly(true);
}

bool
CQCsvModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // parse file into array of fields
  CCsv csv(filename_.toStdString());

  csv.setCommentHeader  (isCommentHeader());
  csv.setFirstLineHeader(isFirstLineHeader());

  if (! csv.load())
    return false;

  // header data and rows
  const CCsv::Fields &header = csv.header();
  const CCsv::Data   &data   = csv.data();

  //---

  int numColumns = 0;

  // add header to model
  if (! header.empty()) {
    for (const auto &f : header)
      header_.push_back(f.c_str());

    numColumns = std::max(numColumns, int(header_.size()));
  }

  //---

  for (const auto &fields : data)
    numColumns = std::max(numColumns, int(fields.size()));

  while (int(header_.size()) < numColumns)
    header_.push_back("");

  //---

  bool hasFilter = filter_.length();

  if (hasFilter)
    initFilter();

  // add fields to model
  for (const auto &fields : data) {
    if (hasFilter && ! acceptsRow(fields))
      continue;

    //---

    Cells cells;

    for (const auto &f : fields) {
      cells.push_back(f.c_str());
    }

    data_.push_back(cells);
  }

  //---

  genColumnTypes();

  return true;
}

void
CQCsvModel::
initFilter()
{
  filterDatas_.clear();

  QStringList patterns = filter_.split(",");

  for (int i = 0; i < patterns.size(); ++i) {
    FilterData filterData;

    QStringList fields = patterns[i].split(":");

    if (fields.length() == 2) {
      QString name  = fields[0];
      QString value = fields[1];

      filterData.column = -1;

      for (std::size_t j = 0; j < header_.size(); ++j) {
        if (header_[j] == name) {
          filterData.column = j;
          break;
        }
      }

      if (filterData.column == -1) {
        bool ok;

        filterData.column = name.toInt(&ok);

        if (! ok)
          filterData.column = -1;
      }

      filterData.regexp = QRegExp(value, Qt::CaseSensitive, QRegExp::Wildcard);
    }
    else {
      filterData.column = 0;
      filterData.regexp = QRegExp(patterns[i], Qt::CaseSensitive, QRegExp::Wildcard);
    }

    filterData.valid = (filterData.column >= 0 && filterData.column < int(header_.size()));

    filterDatas_.push_back(filterData);
  }
}

bool
CQCsvModel::
acceptsRow(const std::vector<std::string> &cells) const
{
  for (std::size_t i = 0; i < filterDatas_.size(); ++i) {
    const FilterData &filterData = filterDatas_[i];

    if (! filterData.valid)
      continue;

    const std::string &field = cells[filterData.column];

    if (! filterData.regexp.exactMatch(field.c_str()))
      return false;
  }

  return true;
}

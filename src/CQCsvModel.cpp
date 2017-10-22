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

  const CCsv::Fields &header = csv.header();

  const CCsv::Data &data = csv.data();

  //---

  int numColumns = 0;

  // add header to model
  if (! header.empty()) {
    for (const auto &f : header)
      header_.push_back(f.c_str());

    numColumns = std::max(numColumns, int(header_.size()));
  }

  // add fields to model
  for (const auto &fields : data) {
    Cells cells;

    for (const auto &f : fields) {
      cells.push_back(f.c_str());
    }

    numColumns = std::max(numColumns, int(cells.size()));

    data_.push_back(cells);
  }

  //---

  while (int(header_.size()) < numColumns)
    header_.push_back("");

  //---

  genColumnTypes();

  return true;
}

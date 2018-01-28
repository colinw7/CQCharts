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
    int i = 0;

    for (const auto &f : header) {
      if (i == 0 && isFirstColumnHeader())
        vheader_.push_back(f.c_str());
      else
        hheader_.push_back(f.c_str());

      ++i;
    }

    numColumns = std::max(numColumns, int(hheader_.size()));
  }

  //---

  // expand horizontal header to max number of columns
  for (const auto &fields : data) {
    int numFields = fields.size();

    if (isFirstColumnHeader())
      --numFields;

    numColumns = std::max(numColumns, numFields);
  }

  while (int(hheader_.size()) < numColumns)
    hheader_.push_back("");

  //---

  // add fields to model
  for (const auto &fields : data) {
    Cells   cells;
    QString vheader;

    int i = 0;

    for (const auto &f : fields) {
      if (i == 0 && isFirstColumnHeader())
        vheader = f.c_str();
      else
        cells.push_back(f.c_str());

      ++i;
    }

    if (acceptsRow(cells)) {
      if (isFirstColumnHeader())
        vheader_.push_back(vheader);

      data_.push_back(cells);
    }
  }

  //---

  // expand vertical header to max number of rows
  int numRows = data_.size();

  while (int(vheader_.size()) < numRows)
    vheader_.push_back("");

  //---

  genColumnTypes();

  return true;
}

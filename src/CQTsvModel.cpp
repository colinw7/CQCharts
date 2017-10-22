#include <CQTsvModel.h>
#include <CTsv.h>

CQTsvModel::
CQTsvModel()
{
  setReadOnly(true);
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

    for (const auto &f : fields)
      cells.push_back(f.c_str());

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

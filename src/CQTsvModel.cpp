#include <CQTsvModel.h>
#include <CTsv.h>

CQTsvModel::
CQTsvModel()
{
  setObjectName("tsvModel");

  // default read only
  setReadOnly(true);

  setDataType(CQBaseModelDataType::TSV);
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

  // header data and rows
  const CTsv::Fields &header = tsv.header();
  const CTsv::Data   &data   = tsv.data();

  //---

  int numColumns = 0;

  // add header to model
  if (! header.empty()) {
    int i = 0;

    for (const auto &f : header) {
      if (i == 0 && isFirstColumnHeader()) {
        //vheader_.push_back(f.c_str());
      }
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
    Cells cells;
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

  resetColumnTypes();

  return true;
}

void
CQTsvModel::
save(std::ostream &os)
{
  save(this, os);
}

void
CQTsvModel::
save(QAbstractItemModel *model, std::ostream &os)
{
  int nc = model->columnCount();
  int nr = model->rowCount();

  //---

  bool hasHHeader = false;

  if (isFirstLineHeader()) {
    for (int c = 0; c < nc; ++c) {
      QVariant var = model->headerData(c, Qt::Horizontal);

      if (var.isValid()) {
        hasHHeader = true;
        break;
      }
    }
  }

  bool hasVHeader = false;

  if (isFirstColumnHeader()) {
    for (int r = 0; r < nr; ++r) {
      QVariant var = model->headerData(r, Qt::Vertical);

      if (var.isValid()) {
        hasVHeader = true;
        break;
      }
    }
  }

  //---

  // output horizontal header on first line if enabled and model has horizontal header data
  if (isFirstLineHeader() && hasHHeader) {
    bool output = false;

    // if vertical header then add empty cell
    if (isFirstColumnHeader() && hasVHeader)
      output = true;

    for (int c = 0; c < nc; ++c) {
      QVariant var = model->headerData(c, Qt::Horizontal);

      if (output)
        os << "\t";

      os << encodeVariant(var);

      output = true;
    }

    os << "\n";
  }

  // output rows
  for (int r = 0; r < nr; ++r) {
    bool output = false;

    // output vertical header value if enabled and model has vertical header data
    if (isFirstColumnHeader() && hasVHeader) {
      QVariant var = model->headerData(r, Qt::Vertical);

      os << encodeVariant(var);

      output = true;
    }

    //---

    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      QVariant var = model->data(ind);

      if (output)
        os << "\t";

      os << encodeVariant(var);

      output = true;
    }

    os << "\n";
  }
}

std::string
CQTsvModel::
encodeVariant(const QVariant &var) const
{
  std::string str;

  if      (var.type() == QVariant::Double) {
    double r = var.value<double>();

    str = std::to_string(r);
  }
  else if (var.type() == QVariant::Int) {
    int i = var.value<int>();

    str = std::to_string(i);
  }
  else {
    QString qstr = var.toString();

    str = encodeString(qstr).toStdString();
  }

  return str;
}

QString
CQTsvModel::
encodeString(const QString &str)
{
  // TOD0: handle tab in string
  return str;
}

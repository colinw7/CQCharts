#include <CQJsonModel.h>
#include <CJson.h>

CQJsonModel::
CQJsonModel()
{
  setDataType(CQBaseModelDataType::JSON);
}

CQJsonModel::
~CQJsonModel()
{
  delete json_;
}

bool
CQJsonModel::
load(const QString &filename)
{
  filename_ = filename;

  //---

  // parse file into array of fields
  json_ = new CJson;

  if (! json_->loadFile(filename.toStdString(), jsonValue_)) {
    std::cerr << "Parse failed" << std::endl;
    return false;
  }

  //---

  if (isRootHierarchical(hierName_, hierColumns_))
    setHierarchical(true);

  //---

  resetColumnTypes();

  return true;
}

void
CQJsonModel::
save(std::ostream &os)
{
  save(this, os);
}

void
CQJsonModel::
save(QAbstractItemModel *model, std::ostream &os)
{
  int nc = model->columnCount();
  int nr = model->rowCount();

  //---

  QStringList strs;

  for (int c = 0; c < nc; ++c) {
    auto var = model->headerData(c, Qt::Horizontal);

    strs << var.toString();
  }

  os << "[\n";

  for (int r = 0; r < nr; ++r) {
    os << "  {\n";

    for (int c = 0; c < nc; ++c) {
      auto ind = model->index(r, c);

      auto var = model->data(ind);

      os << "    \"" << strs[c].toStdString() << "\": ";

      if      (var.type() == QVariant::Int)
        os << var.toInt();
      else if (var.type() == QVariant::Double)
        os << var.toDouble();
      else
        os << "\"" << var.toString().toStdString() << "\"";

      if (c != nc - 1)
        os << ",";

      os << "\n";
    }

    os << "  }";

    if (r != nr - 1)
      os << ",";

    os << "\n";
  }

  os << "]\n";
}

bool
CQJsonModel::
isRootHierarchical(QString &hierName, QStringList &hierColumns) const
{
  // need object with single array child
  if (jsonValue_->isObject()) {
    ColumnMap columnMap;

    CJson::Object *obj = jsonValue_->cast<CJson::Object>();

    // get name of single array child
    std::string   arrayName;
    CJson::Array *array { nullptr };

    int i = 0;

    for (const auto &nv : obj->nameValueMap()) {
      if      (nv.second->isArray()) {
        if (array)
          return false;

        arrayName = nv.first;
        array     = nv.second->cast<CJson::Array>();
      }
      else if (nv.second->isObject()) {
        return false;
      }
      else {
        columnMap[i++] = nv.first.c_str();
      }
    }

    if (! array)
      return false;

    // check if array is hierarchical
    if (! isArrayHierarchical(arrayName, array, columnMap))
      return false;

    hierName = arrayName.c_str();

    for (const auto &ic : columnMap)
      hierColumns.push_back(ic.second);

    return true;
  }
  else {
    return false;
  }
}

bool
CQJsonModel::
isArrayHierarchical(const std::string &name, CJson::Array *array, ColumnMap &columnMap) const
{
  for (uint i = 0; i < array->size(); ++i) {
    CJson::ValueP value = array->at(i);

    if (! value->isObject())
      return false;

    CJson::Object *obj = value->cast<CJson::Object>();

    if (! isObjHierarchical(name, obj, columnMap))
      return false;
  }

  return true;
}

bool
CQJsonModel::
isObjHierarchical(const std::string &name, CJson::Object *obj, ColumnMap &columnMap) const
{
  // check name of single array child
  CJson::Array *array { nullptr };

  int i = 0;

  for (const auto &nv : obj->nameValueMap()) {
    if      (nv.second->isArray()) {
      if (array)
        return false;

      if (nv.first != name)
        return false;

      array = nv.second->cast<CJson::Array>();
    }
    else if (nv.second->isObject()) {
      return false;
    }
    else {
      columnMap[i++] = nv.first.c_str();
    }
  }

  if (array) {
    if (! isArrayHierarchical(name, array, columnMap))
      return false;
  }

  return true;
}

bool
CQJsonModel::
applyMatch(const QString &match)
{
  CJson::Values values;

  if (! json_->matchValues(jsonValue_, match.toStdString(), values))
    return false;

  if (values.size() == 1) {
    jsonValue_ = values[0];
  }
  else {
    jsonMatch_  = match;
    jsonValues_ = values;
  }

  //---

  resetColumnTypes();

  return true;
}

int
CQJsonModel::
columnCount(const QModelIndex &index) const
{
  if (jsonMatch_ != "") {
    if (! jsonValues_.empty())
      return jsonValues_[0]->numValues();
    else
      return 0;
  }

  if (! jsonValue_)
    return 0;

  //---

  if (isHierarchical()) {
    return hierColumns_.length();
  }
  else {
    CJson::Value *value = static_cast<CJson::Value *>(index.internalPointer());

    if (! value)
      value = jsonValue_.get();

    if      (value->isObject()) {
      return 2;
    }
    else if (value->isArray()) {
      return 2;
    }
    else {
      return 1;
    }
  }
}

int
CQJsonModel::
rowCount(const QModelIndex &parent) const
{
  if (jsonMatch_ != "") {
    if (! parent.isValid())
      return jsonValues_.size();

    return 0;
  }

  //---

  if (! jsonValue_)
    return 0;

  //---

  if (isHierarchical()) {
    if (parent.isValid()) {
      CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());
      assert(parentValue);

      CJson::Object *parentObj = parentValue->cast<CJson::Object>();
      assert(parentObj);

      CJson::ValueP value;

      // leaf node
      if (! parentObj->getNamedValue(hierName_.toStdString(), value))
        return 0;

      CJson::Array *childArray = value->cast<CJson::Array>();

      return childArray->size();
    }
    else {
      return 1;
    }
  }
  else {
    CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());

    if (! parentValue)
      parentValue = jsonValue_.get();

    if      (parentValue->isObject()) {
      CJson::Object *obj = parentValue->cast<CJson::Object>();

      return obj->nameValueMap().size();
    }
    else if (parentValue->isArray()) {
      return parentValue->numValues();
    }
    else
      return 0;
  }
}

QVariant
CQJsonModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return CQBaseModel::headerData(section, orientation, role);

  if      (role == Qt::DisplayRole) {
    QString str;

    if (headerString(section, str))
      return QVariant(str);
  }
  else if (role == Qt::ToolTipRole) {
    QString str;

    if (headerString(section, str)) {
      auto type = columnType(section);

      str += ":" + typeName(type);

      return QVariant(str);
    }
  }
  else if (role == Qt::EditRole) {
    return QVariant();
  }
  else {
    return CQBaseModel::headerData(section, orientation, role);
  }

  return QVariant();
}

bool
CQJsonModel::
headerString(int section, QString &str) const
{
  if (section < 0)
    return false;

  if (jsonMatch_ != "") {
    CJson::ValueP value = jsonValues_[0];

    if (section < int(value->numValues())) {
      str = value->indexKey(section).c_str();
      return true;
    }

    return false;
  }

  if (isHierarchical()) {
    str = hierColumns_[section];
    return true;
  }
  else {
    CJson::ValueP parentValue = jsonValue_;

    if      (parentValue->isObject()) {
      if (section == 0) { str = "Name" ; return true; }
      if (section == 1) { str = "Value"; return true; }
    }
    else if (parentValue->isArray()) {
      if (parentValue->indexValue(0)->isComposite()) {
        CJson::ValueP value1 = parentValue->indexValue(0);

        if (section >= 0 && section < int(value1->numValues())) {
          str = value1->indexKey(section).c_str();
          return true;
        }
      }
    }
  }

  return false;
}

bool
CQJsonModel::
setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return CQBaseModel::setHeaderData(section, orientation, value, role);
}

QString
CQJsonModel::
parentName(CJson::Value *value) const
{
  CJson::Value *parent = value->parent();

  if (! parent)
    return "";

  if      (parent->isObject()) {
    CJson::Object *obj = parent->cast<CJson::Object>();

    for (size_t i = 0; i < obj->nameValueMap().size(); ++i) {
      std::string   name1;
      CJson::ValueP value1;

      if (obj->indexNameValue(i, name1, value1) && value1.get() == value)
        return name1.c_str();
    }
  }
  else if (parent->isArray()) {
    CJson::Array *array = parent->cast<CJson::Array>();

    for (size_t i = 0; i < array->size(); ++i) {
      if (array->at(i).get() == value)
        return QString("%1").arg(i);
    }
  }

  return "";
}

QVariant
CQJsonModel::
data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
    if (jsonMatch_ != "") {
      if (! index.isValid())
        return QVariant();

      CJson::ValueP value = jsonValues_[index.row()];

      if (index.column() >= 0 && index.column() < int(value->numValues()))
        return value->indexValue(index.column())->to_string().c_str();

      return QVariant();
    }

    //---

    if (isHierarchical()) {
      if (! index.isValid())
        return QVariant();

      CJson::Value *value = static_cast<CJson::Value *>(index.internalPointer());
      assert(value);

      CJson::Object *valueObj = value->cast<CJson::Object>();
      assert(valueObj);

      CJson::ValueP columnValue;

      std::string name = hierColumns_[index.column()].toStdString();

      if (! valueObj->getNamedValue(name, columnValue))
        return QVariant();

      return columnValue->to_string().c_str();
    }
    else {
      CJson::Value *value = static_cast<CJson::Value *>(index.internalPointer());

      if (! value)
        value = jsonValue_.get();

      if (index.column() == 0)
        return parentName(value);

      if (index.column() != 1)
        return QVariant();

      if      (value->isObject()) {
        if (isFlat()) {
          CJson::Object *obj = value->cast<CJson::Object>();
          return obj->to_string().c_str();
        }

        return "";
      }
      else if (value->isArray()) {
        if (isFlat()) {
          CJson::Array *array = value->cast<CJson::Array>();
          return array->to_string().c_str();
        }

        return "";
      }
      else {
        return value->to_string().c_str();
      }
    }
  }

  return QVariant();
}

bool
CQJsonModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  return CQBaseModel::setData(index, value, role);
}

// get child of parent
QModelIndex
CQJsonModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (isHierarchical()) {
    // at root
    if (! parent.isValid())
      return createIndex(0, column, jsonValue_.get());

    CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());
    assert(parentValue);

    CJson::Object *parentObj = parentValue->cast<CJson::Object>();
    assert(parentObj);

    // parent must be non-root
    CJson::ValueP value;

    // if hierarchical then return child object
    if (parentObj->getNamedValue(hierName_.toStdString(), value)) {
      CJson::Array *childArray = value->cast<CJson::Array>();

      CJson::ValueP childValue = childArray->at(row);

      return createIndex(row, column, childValue.get());
    }
    else {
      return QModelIndex();
    }
  }
  else {
    // get parent row
    CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());

    if (! parentValue)
      parentValue = jsonValue_.get();

    //if (! parentValue)
    //  return createIndex(row, column, jsonValue_);

    if      (parentValue->isObject()) {
      CJson::Object *obj = parentValue->cast<CJson::Object>();

      if (row < 0 || row >= int(obj->nameValueMap().size()))
        return QModelIndex();

      std::string   name;
      CJson::ValueP value;

      if (! obj->indexNameValue(row, name, value))
        return QModelIndex();

      return createIndex(row, column, value.get());
    }
    else if (parentValue->isArray()) {
      CJson::Array *array = parentValue->cast<CJson::Array>();

      if (row < 0 || row >= int(array->size()))
        return QModelIndex();

      return createIndex(row, column, array->at(row).get());
    }
    else
      return QModelIndex();
  }
}

// get parent of child
QModelIndex
CQJsonModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  if (isHierarchical()) {
    CJson::Value *childValue = static_cast<CJson::Value *>(index.internalPointer());
    assert(childValue);

    CJson::Object *childObj = childValue->cast<CJson::Object>();
    assert(childObj);

    if (! childObj->parent())
      return QModelIndex();

    // get parent object
    CJson::Array *parentArray = childObj->parent()->cast<CJson::Array>();
    assert(parentArray);

    CJson::Object *parentObj = parentArray->parent()->cast<CJson::Object>();
    assert(parentObj);

    if (! parentObj->parent())
      return createIndex(0, 0, jsonValue_.get());

    // get parent, parent object
    CJson::Array *parentParentArray = parentObj->parent()->cast<CJson::Array>();
    assert(parentParentArray);

    CJson::Object *parentParentObj = parentParentArray->parent()->cast<CJson::Object>();
    assert(parentParentObj);

    // find index of parentObj in parentParentArray
    for (uint i = 0; i < parentParentArray->size(); ++i) {
      if (parentParentArray->at(i).get() == parentObj)
        return createIndex(i, 0, parentObj);
    }

    // always non-leaf parent
    return createIndex(0, 0, parentObj);
  }
  else {
    CJson::Value *childValue = static_cast<CJson::Value *>(index.internalPointer());

    if (! childValue)
      return QModelIndex();

    CJson::Value *parentValue = childValue->parent();

    if (! parentValue)
      return QModelIndex();

    if (parentValue == jsonValue_.get())
      return createIndex(0, 0, jsonValue_.get());

    CJson::Value *parentParentValue = parentValue->parent();

    if (! parentParentValue)
      return createIndex(0, 0, jsonValue_.get());

    if      (parentParentValue->isObject()) {
      CJson::Object *obj = parentParentValue->cast<CJson::Object>();

      for (size_t i = 0; i < obj->nameValueMap().size(); ++i) {
        std::string   name;
        CJson::ValueP value;

        if (obj->indexNameValue(i, name, value) && value.get() == parentValue)
          return createIndex(i, 0, parentValue);
      }
    }
    else if (parentParentValue->isArray()) {
      CJson::Array *array = parentParentValue->cast<CJson::Array>();

      for (size_t i = 0; i < array->size(); ++i) {
        if (array->at(i).get() == parentValue)
          return createIndex(i, 0, parentValue);
      }
    }

    return QModelIndex();
  }
}

Qt::ItemFlags
CQJsonModel::
flags(const QModelIndex &index) const
{
  if (! index.isValid())
    return 0;

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (! isReadOnly())
    flags |= Qt::ItemIsEditable;

  return flags;
}

#include <CQJsonModel.h>
#include <CJson.h>

CQJsonModel::
CQJsonModel()
{
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
    hier_ = true;

  return true;
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
    CJson::Value *value = array->at(i);

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
  CJson::Array::Values values;

  if (! json_->matchValues(jsonValue_, match.toStdString(), values))
    return false;

  if (values.size() == 1) {
    jsonValue_ = values[0];
  }
  else {
    jsonMatch_  = match;
    jsonValues_ = values;
  }

  return true;
}

int
CQJsonModel::
columnCount(const QModelIndex &) const
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

  if      (hier_) {
    return hierColumns_.length();
  }
  else if (jsonValue_->isObject()) {
    //CJson::Object *obj = jsonValue_->cast<CJson::Object>();

    return 2;
  }
  else if (jsonValue_->isArray()) {
    if (jsonValue_->indexValue(0)->isComposite())
      return jsonValue_->indexValue(0)->numValues();
    else
      return 1;
  }
  else
    return 1;
}

int
CQJsonModel::
rowCount(const QModelIndex &parent) const
{
  if (jsonMatch_ != "") {
    return jsonValues_.size();
  }

  if (! jsonValue_)
    return 0;

  //---

  if (hier_) {
    if (parent.isValid()) {
      CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());

      CJson::Object *parentObj = parentValue->cast<CJson::Object>();

      CJson::Value *value;

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
    if (parent.isValid())
      return 0;

    if      (jsonValue_->isObject()) {
      CJson::Object *obj = jsonValue_->cast<CJson::Object>();

      return obj->nameValueMap().size();
    }
    else if (jsonValue_->isArray()) {
      return jsonValue_->numValues();
    }
    else
      return 1;
  }
}

QVariant
CQJsonModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (jsonMatch_ != "") {
      CJson::Value *value = jsonValues_[0];

      if (section < 0 || section >= int(value->numValues()))
        return value->indexKey(section).c_str();

      return QVariant();
    }

    //---

    if      (hier_) {
      return hierColumns_[section];
    }
    else if (jsonValue_->isObject()) {
      if (section == 0) return "Name";
      if (section == 1) return "Value";

      return QVariant();
    }
    else if (jsonValue_->isArray()) {
      if (jsonValue_->indexValue(0)->isComposite()) {
        CJson::Value *value1 = jsonValue_->indexValue(0);

        if (section >= 0 && section < int(value1->numValues()))
          return value1->indexKey(section).c_str();

        return QVariant();
      }
      else
        return QVariant();
    }
    else {
      return QVariant();
    }
  }
  else
    return QVariant();
}

QVariant
CQJsonModel::
data(const QModelIndex &index, int role) const
{
  if (! index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (jsonMatch_ != "") {
      CJson::Value *value = jsonValues_[index.row()];

      if (index.column() >= 0 && index.column() < int(value->numValues()))
        return value->indexValue(index.column())->to_string().c_str();

      return QVariant();
    }

    //---

    if      (hier_) {
      CJson::Value *value = static_cast<CJson::Value *>(index.internalPointer());

      if (value) {
        CJson::Object *valueObj = value->cast<CJson::Object>();

        CJson::Value *columnValue = nullptr;

        if (! valueObj->getNamedValue(hierColumns_[index.column()].toStdString(), columnValue))
          return QVariant();

        return columnValue->to_string().c_str();
      }
      else {
        return QVariant();
      }
    }
    else if (jsonValue_->isObject()) {
      CJson::Object *obj = jsonValue_->cast<CJson::Object>();

      std::string   name;
      CJson::Value *value { nullptr };

      if (obj->indexNameValue(index.row(), name, value)) {
        if (index.column() == 0) return name.c_str();
        if (index.column() == 1) return value->to_string().c_str();
      }
    }
    else if (jsonValue_->isArray()) {
      CJson::Value *value = jsonValue_->indexValue(index.row());

      if (value->isComposite()) {
        if (index.column() >= 0 && index.column() < int(value->numValues()))
          return value->indexValue(index.column())->to_string().c_str();
      }
      else {
        if (index.column() == 0)
          return jsonValue_->to_string().c_str();
        else
          return QVariant();
      }
    }
    else {
      return jsonValue_->to_string().c_str();
    }

    return QVariant();
  }

  return QVariant();
}

QModelIndex
CQJsonModel::
index(int row, int column, const QModelIndex &parent) const
{
  if (! parent.isValid())
    return createIndex(row, column, jsonValue_);

  if (hier_) {
    CJson::Value *parentValue = static_cast<CJson::Value *>(parent.internalPointer());

    CJson::Object *parentObj = parentValue->cast<CJson::Object>();

    CJson::Value *value;

    if (! parentObj->getNamedValue(hierName_.toStdString(), value))
      return QModelIndex();

    CJson::Array *childArray = value->cast<CJson::Array>();

    CJson::Value *childValue = childArray->at(row);

    return createIndex(row, column, childValue);
  }
  else
    return createIndex(row, column, jsonValue_);
}

QModelIndex
CQJsonModel::
parent(const QModelIndex &index) const
{
  if (! index.isValid())
    return QModelIndex();

  if (hier_) {
    CJson::Value *childValue = static_cast<CJson::Value *>(index.internalPointer());

    if (childValue->parent()) {
      CJson::Array  *parentArray = childValue->parent()->cast<CJson::Array>();
      CJson::Object *parentObj   = parentArray->parent()->cast<CJson::Object>();

      for (uint i = 0; i < parentArray->size(); ++i) {
        if (parentArray->at(i) == childValue)
          return createIndex(i, 0, parentObj);
      }

      return QModelIndex();
    }
    else
      return QModelIndex();
  }
  else
    return QModelIndex();
}

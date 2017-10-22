#include <CJson.h>
#include <CStrParse.h>
#include <CUtf8.h>
#include <set>

namespace {
  inline int hexCharValue(char c) {
    if (isdigit(c)) return (c - '0');

    return (tolower(c) - 'a' + 10);
  }
}

//------

CJson::
CJson()
{
}

//------

double
CJson::
stod(const std::string &str, bool &ok)
{
  char *p;

  double r = strtod(str.c_str(), &p);

  while (*p && isspace(*p))
    ++p;

  ok = (*p == '\0');

  return r;
}

long
CJson::
stol(const std::string &str, bool &ok)
{
  char *p;

  int i = strtol(str.c_str(), &p, 10);

  while (*p && isspace(*p))
    ++p;

  ok = (*p == '\0');

  return i;
}

// read string at file pos
bool
CJson::
readString(CStrParse &parse, std::string &str1)
{
  if (! parse.isChar('\"'))
    return false;

  parse.skipChar();

  while (! parse.eof()) {
    if      (parse.isChar('\\') && ! parse.neof(1)) {
      parse.skipChar();

      char c = parse.readChar();

      switch (c) {
        case '\"': str1 += '\"'; break;
        case '\\': str1 += '\\'; break;
        case '/' : str1 += '/' ; break;
        case 'b' : str1 += '\b'; break;
        case 'f' : str1 += '\f'; break;
        case 'n' : str1 += '\n'; break;
        case 'r' : str1 += '\r'; break;
        case 't' : str1 += '\t'; break;
        case 'u' : {
          // 4 hexadecimal digits
          long i = 0;

          for (int j = 0; j < 4; ++j) {
            if (! parse.isXDigit())
              return false;

            char c = parse.readChar();

            i = (i << 4) | (hexCharValue(c) & 0xF);
          }

          CUtf8::append(str1, i);

          break;
        }
        default: {
          if (isStrict())
            return false;

          str1 += c;

          break;
        }
      }
    }
    else if (parse.isChar('\"'))
      break;
    else
      str1 += parse.readChar();
  }

  if (parse.eof() || ! parse.isChar('\"'))
    return false;

  parse.skipChar();

  return true;
}

// read numner at file pos
bool
CJson::
readNumber(CStrParse &parse, std::string &str1)
{
  if (parse.eof())
    return false;

  if (parse.isChar('-'))
    str1 += parse.readChar();

  // TODO: octal, hexadecimal
  if      (parse.isChar('0'))
    str1 += parse.readChar();
  else if (parse.isDigit()) {
    while (parse.isDigit())
      str1 += parse.readChar();
  }
  else
    return false;

  if (parse.isChar('.')) {
    str1 += parse.readChar();

    if (isStrict()) {
      if (! parse.isDigit())
        return false;
    }

    while (parse.isDigit())
      str1 += parse.readChar();
  }

  // [Ee][+-][0-9][0-9]*
  if (parse.isOneOf("eE")) {
    str1 += parse.readChar();

    if (parse.isOneOf("+-"))
      str1 += parse.readChar();

    if (! parse.isDigit())
      return false;

    while (parse.isDigit())
      str1 += parse.readChar();
  }

  return true;
}

// read object at file pos
bool
CJson::
readObject(CStrParse &parse, Object *&obj)
{
  if (! parse.isChar('{'))
    return false;

  bool open = false;

  parse.skipChar();

  obj = createObject();

  while (! parse.eof()) {
    parse.skipSpace();

    if (parse.isChar('}'))
      break;

    std::string name;

    if (! readString(parse, name)) {
      delete obj;
      return false;
    }

    parse.skipSpace();

    if (! parse.isChar(':')) {
      delete obj;
      return false;
    }

    parse.skipChar();

    parse.skipSpace();

    Value *value;

    if (! readValue(parse, value)) {
      delete obj;
      return false;
    }

    value->setParent(obj);

    parse.skipSpace();

    obj->setNamedValue(name, value);

    open = false;

    if (! parse.isChar(','))
      break;

    parse.skipChar();

    open = true;
  }

  if (open) {
    delete obj;
    return false;
  }

  if (! parse.isChar('}')) {
    delete obj;
    return false;
  }

  parse.skipChar();

  return true;
}

// read array at file pos
bool
CJson::
readArray(CStrParse &parse, Array *&array)
{
  if (! parse.isChar('['))
    return false;

  bool open = false;

  parse.skipChar();

  array = createArray();

  while (! parse.eof()) {
    parse.skipSpace();

    if (parse.isChar(']'))
      break;

    Value *value;

    if (! readValue(parse, value)) {
      delete array;
      return false;
    }

    value->setParent(array);

    array->addValue(value);

    parse.skipSpace();

    open = false;

    if (! parse.isChar(','))
      break;

    parse.skipChar();

    open = true;
  }

  if (open) {
    delete array;
    return false;
  }

  if (! parse.isChar(']')) {
    delete array;
    return false;
  }

  parse.skipChar();

  return true;
}

// read value at file pos
bool
CJson::
readValue(CStrParse &parse, Value *&value)
{
  if (parse.eof())
    return false;

  char c = parse.getCharAt();

  if      (c == '\"') {
    std::string str1;

    if (! readString(parse, str1))
      return false;

    value = createString(str1);
  }
  else if (c == '-' || isdigit(c)) {
    std::string str1;

    if (! readNumber(parse, str1))
      return false;

    bool ok;

    double n = CJson::stod(str1, ok);

    value = createNumber(n);
  }
  else if (c == '{') {
    Object *obj;

    if (! readObject(parse, obj))
      return false;

    value = obj;
  }
  else if (c == '[') {
    Array *array;

    if (! readArray(parse, array))
      return false;

    value = array;
  }
  else if (parse.isString("true")) {
    parse.skipChars("true");

    value = createTrue();
  }
  else if (parse.isString("false")) {
    parse.skipChars("false");

    value = createFalse();
  }
  else if (parse.isString("null")) {
    parse.skipChars("null");

    value = createNull();
  }
  else
    return false;

  return true;
}

bool
CJson::
readLine(FILE *fp, std::string &line)
{
  line = "";

  if (feof(fp)) return false;

  char c = fgetc(fp);

  while (! feof(fp) && c != '\n') {
    line += c;

    c = fgetc(fp);
  }

  return true;
}

// load file and return root value
bool
CJson::
loadFile(const std::string &filename, Value *&value)
{
  value = nullptr;

  FILE *fp = fopen(filename.c_str(), "r");
  if (! fp) return false;

  std::string lines;

  while (! feof(fp)) {
    std::string line;

    if (readLine(fp, line))
      lines += line;
  }

  fclose(fp);

  //---

  return loadString(lines, value);
}

bool
CJson::
loadString(const std::string &lines, Value *&value)
{
  std::vector<std::string> strs;

  CStrParse parse(lines);

  parse.skipSpace();

  if      (parse.isChar('{')) { // object
    Object *obj;

    if (! readObject(parse, obj))
      return false;

    value = obj;
  }
  else if (parse.isChar('[')) { // array
    Array *array;

    if (! readArray(parse, array))
      return false;

    value = array;
  }
  else {
    Value *value1;

    if (! readValue(parse, value1))
      return false;

    value = value1;
  }

  parse.skipSpace();

  if (! parse.eof())
    return false;

  return true;
}

//------

bool
CJson::
matchObject(Value *value, const std::string &match, Value* &value1)
{
  if (isDebug())
    std::cerr << "matchObject \'" << match << "\'" << std::endl;

  if (! value->isObject()) {
    if (! isQuiet())
      std::cerr << value->typeName() << " is not an object" << std::endl;
    return false;
  }

  Object *obj = value->cast<Object>();

  if (match == "?" || match == "?keys") {
    std::vector<std::string> names;

    obj->getNames(names);

    Array *array = createArray();

    for (const auto &n : names) {
      String *str = createString(n);

      array->addValue(str);
    }

    value1 = array;
  }
  else if (match == "?type") {
    String *str = createString(obj->typeName());

    value1 = str;
  }
  else if (match == "?values") {
    std::vector<Value *> values;

    obj->getValues(values);

    Array *array = createArray();

    for (const auto &v : values)
      array->addValue(v);

   value1 = array;
  }
  else {
    if (! obj->getNamedValue(match, value1)) {
      if (! isQuiet())
        std::cerr << "no value \'" << match << "\'" << std::endl;
      return false;
    }
  }

  return true;
}

bool
CJson::
matchArray(Value *value, const std::string &lhs, const std::string &rhs, Array::Values &values)
{
  if (isDebug())
    std::cerr << "matchArray \'" << lhs << "\' \'" << rhs << "\'" << std::endl;

  if (! value->isArray()) {
    if (! isQuiet())
      std::cerr << value->typeName() << " is not an array" << std::endl;
    return false;
  }

  Array *array = value->cast<Array>();

  if (lhs[0] != '[' || lhs[lhs.size() - 1] != ']')
    return false;

  std::string range = lhs.substr(1, lhs.size() - 2);

  if (range == "?size") {
    Number *n = createNumber(array->size());

    values.push_back(n);

    return true;
  }

  auto p = range.find(',');

  if (p != std::string::npos) {
    std::string match1 = range;

    std::string lhs1 = match1.substr(0, p);
    std::string rhs1 = match1.substr(p + 1);

    bool ok1, ok2;

    int i1 = CJson::stol(lhs1, ok1);
    int i2 = CJson::stol(rhs1, ok2);

    if (! ok1 || ! ok2) {
      if (! isQuiet())
        std::cerr << "Invalid array indices '" << lhs1 << "', '" << rhs1 << "'" << std::endl;
      return false;
    }

    for (int i = i1; i <= i2 && i < int(array->size()); ++i) {
      Value *value1 = array->at(i);

      if (rhs1 != "")
        matchValues(value1, i, rhs1, values);
      else
        values.push_back(value1);
    }
  }
  else if (range != "") {
    bool ok;

    int i1 = CJson::stol(range, ok);

    if (! ok) {
      if (! isQuiet())
        std::cerr << "Invalid array index '" << lhs << "'" << std::endl;
      return false;
    }

    int i = 0;

    for (const auto &v : array->values()) {
      if (i == i1) {
        if (rhs != "")
          matchValues(v, i, rhs, values);
        else
          values.push_back(v);
      }

      ++i;
    }
  }
  else {
    int i = 0;

    for (const auto &v : array->values()) {
      if (rhs != "")
        matchValues(v, i, rhs, values);
      else
        values.push_back(v);

      ++i;
    }
  }

  return true;
}

bool
CJson::
matchList(Value *value, int ind, const std::string &lhs,
          const std::string &rhs, Array::Values &values)
{
  if (isDebug())
    std::cerr << "matchList \'" << lhs << "\' \'" << rhs << "\'" << std::endl;

  if (lhs[0] != '{' || lhs[lhs.size() - 1] != '}')
    return false;

  std::string names = lhs.substr(1, lhs.size() - 2);

  std::vector<std::string> fields;

  auto p = names.find(",");

  while (p != std::string::npos) {
    std::string name = names.substr(0, p);

    names = names.substr(p + 1);

    fields.push_back(name);

    p = names.find(",");
  }

  fields.push_back(names);

  Array *array = createArray();

  for (const auto &f : fields) {
    Array::Values values1;

    std::string match = (rhs != "" ? f + "/" + rhs : f);

    matchValues(value, ind, match, values1);

    for (const auto &v1 : values1)
      array->addValue(v1);
  }

  values.push_back(array);

  return true;
}

bool
CJson::
matchValues(Value *value, const std::string &match, Array::Values &values)
{
  return matchValues(value, 0, match, values);
}

bool
CJson::
matchValues(Value *value, int ind, const std::string &match, Array::Values &values)
{
  std::string match1 = match;

  auto p = match.find("...");

  if (p != std::string::npos) {
    std::string lhs = match1.substr(0, p);
    std::string rhs = match1.substr(p + 3);

    return matchHier(value, ind, lhs, rhs, values);
  }

  if (match1 != "" && match1[0] != '{') {
    auto p = match1.find("/");

    while (p != std::string::npos) {
      std::string lhs = match1.substr(0, p);
      std::string rhs = match1.substr(p + 1);

      if (lhs == "")
        return false;

      if      (lhs[0] == '[') {
        return matchArray(value, lhs, rhs, values);
      }
      else if (lhs[0] == '{') {
        return matchList(value, ind, lhs, rhs, values);
      }
      else {
        Value *value1 = 0;

        if (! matchObject(value, lhs, value1))
          return false;

        value = value1;
      }

      match1 = rhs;

      p = match1.find("/");
    }
  }

  if (match1 == "")
    return true;

  if      (match1[0] == '[') {
    return matchArray(value, match1, "", values);
  }
  else if (match1[0] == '{') {
    return matchList(value, ind, match1, "", values);
  }
  else if (match1[0] == '#') {
    int base = 0;

    if (match1.size() > 1) {
      bool ok;

      base = CJson::stol(match1.substr(1), ok);
    }

    Number *n = createNumber(base + ind);

    values.push_back(n);
  }
  else {
    Value *value1 = 0;

    if (! matchObject(value, match1, value1))
      return false;

    value = value1;

    if (value)
      values.push_back(value);
  }

  return true;
}

bool
CJson::
matchHier(Value *value, int ind, const std::string &name, const std::string &hname,
          Array::Values &values)
{
  typedef std::vector<std::string> Keys;

  Keys keys;

  std::string hname1 = hname;

  auto p = hname1.find("...");

  if (p != std::string::npos) {
    std::string lhs1 = hname1.substr(0, p);
    std::string rhs1 = hname1.substr(p + 3);

    hname1 = lhs1;

    auto p1 = rhs1.find(",");

    while (p1 != std::string::npos) {
      std::string lhs2 = rhs1.substr(0, p1);
      std::string rhs2 = rhs1.substr(p1 + 1);

      keys.push_back(lhs2);

      rhs1 = rhs2;

      p1 = rhs1.find(",");
    }

    keys.push_back(rhs1);
  }

  Array::Values ivalues;

  return matchHier1(value, ind, name, hname1, keys, ivalues, values);
}

bool
CJson::
matchHier1(Value *value, int /*ind*/, const std::string &lhs, const std::string &rhs,
           const std::vector<std::string> &keys, Array::Values &ivalues, Array::Values &values)
{
  if (! value->isObject()) {
    if (! isQuiet())
      std::cerr << value->typeName() << " is not an object" << std::endl;
    return false;
  }

  Object *obj = value->cast<Object>();

  // name
  Value *lvalue = 0;

  if (obj->getNamedValue(lhs, lvalue))
    ivalues.push_back(lvalue);

  // hier object
  Value *rvalue = 0;

  if (obj->getNamedValue(rhs, rvalue)) {
    if (! rvalue->isArray()) {
      if (! isQuiet())
        std::cerr << rvalue->typeName() << " is not an object" << std::endl;
      return false;
    }

    Array *array = rvalue->cast<Array>();

    int i = 0;

    for (auto &v : array->values()) {
      Array::Values ivalues1 = ivalues;

      matchHier1(v, i, lhs, rhs, keys, ivalues1, values);

      ++i;
    }
  }
  else {
    Array::Values kvalues;

    for (const auto &k : keys) {
      Value *kvalue;

      if (obj->getNamedValue(k, kvalue))
        kvalues.push_back(kvalue);
    }

    String *str = hierValuesToKey(ivalues, kvalues);

    values.push_back(str);
  }

  return true;
}

CJson::String *
CJson::
hierValuesToKey(const Array::Values &values, const Array::Values &kvalues)
{
  std::string str;

  std::string vstr;

  for (const auto &v : values) {
    if (vstr != "")
      vstr += "/";

    if      (v->isString())
      vstr += v->cast<String>()->value();
    else if (v->isNumber())
      vstr += std::to_string(v->cast<Number>()->value());
    else
      vstr += "??";
  }

  if (! kvalues.empty()) {
    std::string kstr;

    for (const auto &k : kvalues) {
      if (kstr != "")
        kstr += ",";

      if      (k->isString())
        kstr += k->cast<String>()->value();
      else if (k->isNumber())
        kstr += std::to_string(k->cast<Number>()->value());
      else
        kstr += "??";
    }

    str = "\"" + vstr + "\"\t" + kstr;
  }
  else
    str = "\"" + vstr + "\"";

  return createString(str);
}

CJson::String *
CJson::
createString(const std::string &str)
{
  return new String(this, str);
}

CJson::Number *
CJson::
createNumber(double r)
{
  return new Number(this, r);
}

CJson::True *
CJson::
createTrue()
{
  return new True(this);
}

CJson::False *
CJson::
createFalse()
{
  return new False(this);
}

CJson::Null *
CJson::
createNull()
{
  return new Null(this);
}

CJson::Object *
CJson::
createObject()
{
  return new Object(this);
}

CJson::Array *
CJson::
createArray()
{
  return new Array(this);
}

//------

bool
CJson::String::
toReal(double &r) const
{
  bool ok;

  r = CJson::stod(str_, ok);

  return ok;
}

void
CJson::String::
print(std::ostream &os) const
{
  os << "\"" << str_ << "\"";
}

void
CJson::String::
printReal(std::ostream &os) const
{
  double r;

  if (toReal(r))
    os << r;
  else
    print(os);
}

void
CJson::String::
printShort(std::ostream &os) const
{
  os << str_;
}

//------

void
CJson::Number::
print(std::ostream &os) const
{
  os << value_;
}

//------

void
CJson::True::
print(std::ostream &os) const
{
  os << "\"true\"";
}

//------

void
CJson::False::
print(std::ostream &os) const
{
  os << "\"false\"";
}

//------

void
CJson::Null::
print(std::ostream &os) const
{
  os << "\"null\"";
}

//------

std::string
CJson::Object::
to_string() const
{
  bool first = true;

  std::string str;

  str += "{";

  for (const auto &nv : nameValueArray_) {
    if (! first)
      str += ",";

    str += "\"" + nv.first + "\":";

    str += nv.second->to_string();

    first = false;
  }

  str += "}";

  return str;
}

std::string
CJson::Object::
hierTypeName() const
{
  std::string typeName;

  typeName += "{";

  std::vector<std::string> types;

  for (const auto &nv : nameValueArray_) {
    std::string hierTypeName = nv.second->hierTypeName();

    if (types.empty() || types.back() != hierTypeName)
      types.push_back(hierTypeName);
  }

  int n = 0;

  for (const auto &t : types) {
    if (n > 0)
      typeName += ",";

    typeName += t;

    ++n;
  }

  typeName += "}";

  return typeName;
}

bool
CJson::Object::
isComposite() const
{
  for (const auto &nv : nameValueArray_) {
    if (nv.second->isComposite())
      return true;
  }

  return false;
}

int
CJson::Object::
numComposite() const
{
  int n = 0;

  for (const auto &nv : nameValueArray_) {
    if (nv.second->isComposite())
      ++n;
  }

  return n;
}

void
CJson::Object::
print(std::ostream &os) const
{
  bool first = true;

  if (! json_->isPrintFlat())
    os << "{";

  for (const auto &nv : nameValueArray_) {
    if (! json_->isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    os << "\"" << nv.first << "\":";

    nv.second->print(os);

    first = false;
  }

  if (! json_->isPrintFlat())
    os << "}";
}

void
CJson::Object::
printReal(std::ostream &os) const
{
  bool first = true;

  if (! json_->isPrintFlat())
    os << "{";

  for (const auto &nv : nameValueArray_) {
    if (! json_->isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    os << "\"" << nv.first << "\":";

    nv.second->printReal(os);

    first = false;
  }

  if (! json_->isPrintFlat())
    os << "}";
}

//------

std::string
CJson::Array::
to_string() const
{
  bool first = true;

  std::string str;

  str += "[";

  for (const auto &v : values_) {
    if (! first)
      str += ",";

    str += v->to_string();

    first = false;
  }

  str += "]";

  return str;
}

void
CJson::Array::
printReal(std::ostream &os) const
{
  bool first = true;

  if (! json_->isPrintFlat())
    os << "[";

  for (const auto &v : values_) {
    if (! json_->isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    v->printReal(os);

    first = false;
  }

  if (! json_->isPrintFlat())
    os << "]";
}

//------

std::string
CJson::Array::
hierTypeName() const
{
  std::string typeName;

  typeName += "[";

  std::vector<std::string> types;

  for (const auto &v : values_) {
    std::string hierTypeName = v->hierTypeName();

    if (types.empty() || types.back() != hierTypeName)
      types.push_back(hierTypeName);
  }

  int n = 0;

  for (const auto &t : types) {
    if (n > 0)
      typeName += ",";

    typeName += t;

    ++n;
  }

  typeName += "]";

  return typeName;
}

void
CJson::Array::
print(std::ostream &os) const
{
  bool first = true;

  if (! json_->isPrintFlat())
    os << "[";

  for (const auto &v : values_) {
    if (! json_->isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    v->print(os);

    first = false;
  }

  if (! json_->isPrintFlat())
    os << "]";
}

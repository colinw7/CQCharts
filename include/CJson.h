#ifndef CJson_H
#define CJson_H

#include <cstdio>
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include <boost/optional.hpp>

class CStrParse;

//------

class CJson {
 public:
  enum class ValueType {
    VALUE_NONE,
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_TRUE,
    VALUE_FALSE,
    VALUE_NULL,
    VALUE_OBJECT,
    VALUE_ARRAY
  };

  typedef boost::optional<std::string> OptString;

  //---

  // Json Value base class
  class Value;

  typedef std::shared_ptr<Value> ValueP;

  class Value {
   public:
    Value(CJson *json, ValueType type) : json_(json), type_(type) { }

    virtual ~Value() { }

    //---

    Value *parent() const { return parent_; }
    void setParent(Value *p) { parent_ = p; }

    //---

    ValueType type() const { return type_; }

    bool isString() const { return type_ == ValueType::VALUE_STRING; }
    bool isNumber() const { return type_ == ValueType::VALUE_NUMBER; }
    bool isTrue  () const { return type_ == ValueType::VALUE_TRUE  ; }
    bool isFalse () const { return type_ == ValueType::VALUE_FALSE ; }
    bool isNull  () const { return type_ == ValueType::VALUE_NULL  ; }
    bool isObject() const { return type_ == ValueType::VALUE_OBJECT; }
    bool isArray () const { return type_ == ValueType::VALUE_ARRAY ; }

    bool isComposite() const { return isObject() || isArray(); }

    //---

    virtual uint numValues() const { return 1; }

    virtual std::string indexKey(uint i) { assert(i == 0); return ""; }

    virtual ValueP indexValue(uint i) { assert(i == 0); return ValueP(); }

    //---

    template<typename T>
    T *cast() {
      T *t = dynamic_cast<T *>(this);
      assert(t);

      return t;
    }

    template<typename T>
    const T *cast() const {
      const T *t = dynamic_cast<const T *>(this);
      assert(t);

      return t;
    }

    //---

    virtual const char *typeName() const { return "value"; }

    virtual std::string hierTypeName() const { return typeName(); }

    //---

    virtual std::string to_string() const = 0;

    //---

    virtual void print(std::ostream &os=std::cout) const = 0;

    virtual void printReal(std::ostream &os=std::cout) const { print(os); }

    virtual void printShort(std::ostream &os=std::cout) const { print(os); }

    friend std::ostream &operator<<(std::ostream &os, const Value &v) {
      v.print(os);

      return os;
    }

   protected:
    CJson*    json_   { nullptr };
    Value*    parent_ { nullptr };
    ValueType type_   { ValueType::VALUE_NONE };
  };

  typedef std::vector<ValueP> Values;

  //---

  // Json String
  class String : public Value {
   public:
    String(CJson *json, const std::string &str) :
     Value(json, ValueType::VALUE_STRING), str_(str) {
    }

    //---

    const std::string &value() const { return str_; }

    bool toReal(double &r) const;

    //---

    const char *typeName() const override { return "string"; }

    //---

    std::string to_string() const override { return str_; }

    //---

    void print(std::ostream &os=std::cout) const override;

    void printReal(std::ostream &os=std::cout) const override;

    void printShort(std::ostream &os=std::cout) const override;

   private:
    std::string str_;
  };

  //---

  // Json Number
  class Number : public Value {
   public:
    Number(CJson *json, double value=0.0) :
     Value(json, ValueType::VALUE_NUMBER), value_(value) {
    }

    //---

    double value() const { return value_; }

    //---

    const char *typeName() const override { return "number"; }

    //---

    std::string to_string() const override { return std::to_string(value_); }

    //---

    void print(std::ostream &os=std::cout) const override;

   private:
    double value_ { 0.0 };
  };

  //---

  // Json True
  class True : public Value {
   public:
    True(CJson *json) :
     Value(json, ValueType::VALUE_TRUE) {
    }

    //---

    bool value() const { return true; }

    //---

    const char *typeName() const override { return "true"; }

    //---

    std::string to_string() const override { return "true"; }

    //---

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  // Json False
  class False : public Value {
   public:
    False(CJson *json) :
     Value(json, ValueType::VALUE_FALSE) {
    }

    //---

    bool value() const { return false; }

    //---

    const char *typeName() const override { return "false"; }

    //---

    std::string to_string() const override { return "false"; }

    //---

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  // Json Null
  class Null : public Value {
   public:
    Null(CJson *json) :
     Value(json, ValueType::VALUE_NULL) {
    }

    //---

    void *value() const { return nullptr; }

    //---

    const char *typeName() const override { return "null"; }

    //---

    std::string to_string() const override { return "null"; }

    //---

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  // Json Object (name/value map)
  class Object : public Value {
   public:
    typedef std::map<std::string,ValueP>  NameValueMap;
    typedef std::pair<std::string,ValueP> NameValue;
    typedef std::vector<NameValue>        NameValueArray;

   public:
    Object(CJson *json) :
     Value(json, ValueType::VALUE_OBJECT) {
    }

   ~Object() { }

    //---

    const NameValueMap &nameValueMap() const { return nameValueMap_; }

    const NameValueArray &nameValueArray() const { return nameValueArray_; }

    void getNames(std::vector<std::string> &names) {
      for (const auto &nv : nameValueArray_)
        names.push_back(nv.first);
    }

    void getValues(Values &values) {
      for (const auto &nv : nameValueArray_)
        values.push_back(nv.second);
    }

    bool hasName(const std::string &name) const {
      auto p = nameValueMap_.find(name);

      return (p != nameValueMap_.end());
    }

    void setNamedValue(const std::string &name, const ValueP &value) {
      auto p = nameValueMap_.find(name);

      if (p == nameValueMap_.end())
        p = nameValueMap_.insert(p, NameValueMap::value_type(name, value));

      nameValueArray_.emplace_back(name, value);
    }

    bool getNamedValue(const std::string &name, ValueP &value) const {
      auto p = nameValueMap_.find(name);

      if (p == nameValueMap_.end())
        return false;

      value = (*p).second;

      return true;
    }

    template<typename T>
    bool getNamedValueT(const std::string &name, T *&t) const {
      ValueP value;

      if (! getNamedValue(name, value))
        return false;

      t = dynamic_cast<T *>(value.get());

      if (! t)
        return false;

      return true;
    }

    bool indexNameValue(uint i, std::string &name, ValueP &value) const {
      if (i >= nameValueArray_.size())
        return false;

      const NameValue &nameValue = nameValueArray_[i];

      name  = nameValue.first;
      value = nameValue.second;

      return true;
    }

    //---

    const char *typeName() const override { return "object"; }

    std::string hierTypeName() const override;

    //---

    uint numValues() const override { return nameValueArray_.size(); }

    std::string indexKey(uint i) override {
      assert(i < numValues());

      return nameValueArray_[i].first;
    }

    ValueP indexValue(uint i) override {
      assert(i < numValues());

      return nameValueArray_[i].second;
    }

    //---

    std::string to_string() const override;

    //---

    bool isComposite() const;

    int numComposite() const;

    //---

    void print(std::ostream &os=std::cout) const override;

    void printReal(std::ostream &os=std::cout) const override;

   private:
    NameValueMap   nameValueMap_;
    NameValueArray nameValueArray_;
  };

  //---

  // Json Array
  class Array : public Value {
   public:
    Array(CJson *json) :
     Value(json, ValueType::VALUE_ARRAY) {
    }

   ~Array() { }

    //---

    const Values &values() const { return values_; }

    void addValue(const ValueP &value) {
      values_.push_back(value);
    }

    uint size() const { return values_.size(); }

    ValueP at(uint i) const { return values_[i]; }

    template<typename T>
    T *atT(uint i) const {
      T *t = dynamic_cast<T *>(values_[i].get());

      return t;
    }

    //---

    const char *typeName() const override { return "array"; }

    std::string hierTypeName() const override;

    //---

    uint numValues() const override { return size(); }

    std::string indexKey(uint) override { return ""; }

    ValueP indexValue(uint i) override {
      assert(i < numValues());

      return values_[i];
    }

    //---

    std::string to_string() const override;

    //---

    void print(std::ostream &os=std::cout) const override;

    void printReal(std::ostream &os=std::cout) const override;

   private:
    Values values_;
  };

  //------

  CJson();

 ~CJson();

  //---

  void setStrict(bool b) { strict_ = b; }
  bool isStrict() { return strict_; }

  //---

  void setDebug(bool b) { debug_ = b; }
  bool isDebug() { return debug_; }

  //---

  void setQuiet(bool b) { quiet_ = b; }
  bool isQuiet() { return quiet_; }

  //---

  void setPrintFlat(bool b) { printFlat_ = b; }
  bool isPrintFlat() { return printFlat_; }

  //---

  void setStringToReal(bool b) { stringToReal_ = b; }
  bool isStringToReal() { return stringToReal_; }

  //---

  // load file and return root value
  bool loadFile(const std::string &filename, ValueP &value);

  // load file and return typed root value
  template<typename T>
  bool loadFileT(const std::string &filename, T *&value) {
    ValueP value1;

    if (! loadFile(filename.c_str(), value1))
      return false;

    value = dynamic_cast<T *>(value1.get());

    if (! value)
      return false;

    return true;
  }

  // load string and return root value
  bool loadString(const std::string &filename, ValueP &value);

  //---

  template<typename FUNC>
  void processNodes(const ValueP value, const FUNC &f) {
    return processNameNodes(OptString(), value, 0, f);
  }

  template<typename FUNC>
  void processNameNodes(const OptString &name, const ValueP value, int depth, const FUNC &f) {
    if (! f(name, value, depth))
      return;

    switch (value->type()) {
      case ValueType::VALUE_OBJECT: {
        auto obj = value->cast<Object>();

        for (const auto &nv : obj->nameValueArray())
          processNameNodes(OptString(nv.first), nv.second, depth + 1, f);

        break;
      }
      case ValueType::VALUE_ARRAY: {
        auto array = value->cast<Array>();

        for (const auto &v : array->values())
          processNameNodes(OptString(), v, depth + 1, f);

        break;
      }
      default:
        break;
    }
  }

  //---

  template<typename T>
  bool getValues(const Object *obj, const std::string &name, std::vector<T> &values) {
    auto f = [&](const T &value) { values.push_back(value); };

    return processValues<T,decltype(f)>(obj, name, f);
  }

  //---

  /* match values:
   *  fields are separated by slash '/'
   *  values can be grouped using braces {<match>,<match>,...}
   *  arrays are added using square brackets with optional index range [<start>:<end>]
   *  list of object keys can be returned using ? or ?keys
   *  list of object values can be returned using ?values
   *  object type can be returned using ?type
   *  array index can be added using #
   *
   *  e.g. "head/[1:3]/{name1,name2}/?
   */
  bool matchValues(const ValueP &value, const std::string &match, Values &values);

  bool matchValues(const ValueP &value, int i, const std::string &match, Values &values);

  //---

 private:
  template<typename Tag, typename T>
  struct TypeMap {
    typedef CJson::Null Type;
  };

  template<typename Tag>
  struct TypeMap<Tag, std::string> {
    typedef CJson::String Type;
  };

  template<typename Tag>
  struct TypeMap<Tag, double> {
    typedef CJson::Number Type;
  };

  //---

  static double stod(const std::string &str, bool &ok);
  static long   stol(const std::string &str, bool &ok);

  //---

  // read string at file pos
  bool readString(CStrParse &parse, std::string &str1);

  // read number at file pos
  bool readNumber(CStrParse &parse, std::string &str1);

  // read object at file pos
  bool readObject(CStrParse &parse, Object *&obj);

  // read array at file pos
  bool readArray(CStrParse &parse, Array *&array);

  // read value at file pos
  bool readValue(CStrParse &parse, ValueP &value);

  bool readLine(FILE *fp, std::string &line);

  template<typename T, typename FUNC>
  bool processValues(const Object *obj, const std::string &name, const FUNC &f) {
    auto p = name.find('/');

    if (p != std::string::npos) {
      std::string lhs = name.substr(0, p);
      std::string rhs = name.substr(p + 1);

      Array *nodes;

      if (! obj->getNamedValueT<Array>(lhs, nodes))
        return false;

      for (uint i = 0; i < nodes->size(); ++i) {
        Object *node = nodes->atT<Object>(i);
        if (! node) continue;

        if (! processValues<T,FUNC>(node, rhs, f))
          continue;
      }
    }
    else {
      typedef typename TypeMap<void,T>::Type Type;

      Type *v;

      if (! obj->getNamedValueT<Type>(name, v))
        return false;

      f(v->value());
    }

    return true;
  }

  //------

  bool matchObject(const ValueP &value, const std::string &match, ValueP &value1);

  bool matchArray(const ValueP &value, const std::string &lhs, const std::string &rhs,
                  Values &values);
  bool matchList(const ValueP &value, int ind, const std::string &lhs, const std::string &rhs,
                 Values &values);

  bool matchHier(const ValueP &value, int ind, const std::string &lhs, const std::string &rhs,
                 Values &values);
  bool matchHier1(const ValueP &value, int ind, const std::string &lhs, const std::string &rhs,
                  const std::vector<std::string> &keys, Values &ivalues, Values &values);

  String *hierValuesToKey(const Values &values, const Values &kvalues);

  //------

  String* createString(const std::string &str);
  Number* createNumber(double r);
  True*   createTrue();
  False*  createFalse();
  Null*   createNull();
  Object* createObject();
  Array*  createArray();

  //------

 private:
  bool strict_       { false };
  bool debug_        { false };
  bool quiet_        { false };
  bool printFlat_    { false };
  bool stringToReal_ { false };
};

#endif

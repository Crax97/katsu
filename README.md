example:

file `test.h`
```c++
// This file was generated automatically using the katsu code analyzer or whatever they are called
// https://github.com/crax97/katsu 

#include "object/descriptor.h"
#include "object/object.h"

namespace outer::inner {
class MyClass_descriptor : public descriptor{
  public:
  MyClass_descriptor() : descriptor("MyClass", nullptr) {
    field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::inta)>()(m_fields, "inta", &MyClass::inta);
    field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::floata)>()(m_fields, "floata", &MyClass::floata);
    field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::stringa)>()(m_fields, "stringa", &MyClass::stringa);

    }
};
}

descriptor* outer::inner::MyClass::get_descriptor() {
  static outer::inner::MyClass_descriptor desc;
  return &desc;
}
// This file was generated automatically using the katsu code analyzer or whatever they are called
// https://github.com/crax97/katsu 

#include "object/descriptor.h"
#include "object/object.h"

namespace outer {
class MyOtherClass_descriptor : public descriptor{
  public:
  MyOtherClass_descriptor() : descriptor("MyOtherClass", nullptr) {
    field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::arturo)>()(m_fields, "arturo", &MyOtherClass::arturo);
    field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::ignazio)>()(m_fields, "ignazio", &MyOtherClass::ignazio);
    field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::pippo)>()(m_fields, "pippo", &MyOtherClass::pippo);

    }
};
}

descriptor* outer::MyOtherClass::get_descriptor() {
  static outer::MyOtherClass_descriptor desc;
  return &desc;
}
```
with the templates in the `sample_templates` folder:

running `./katsu ./test.h -- -ogen` generates the file `gen/test.gen.g`:
```c++
// This file was generated automatically using the katsu code analyzer or whatever they are called
// https://github.com/crax97/katsu 

#include "object/descriptor.h"
#include "object/object.h"

namespace outer::inner {
class MyClass_descriptor : public descriptor{
public:
MyClass_descriptor() : descriptor("MyClass", nullptr) {
field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::inta)>()(m_fields, "inta", &MyClass::inta);
field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::floata)>()(m_fields, "floata", &MyClass::floata);
field_adder<outer::inner::MyClass,decltype(outer::inner::MyClass::stringa)>()(m_fields, "stringa", &MyClass::stringa);

}
};
}

descriptor* outer::inner::MyClass::get_descriptor() {
static outer::inner::MyClass_descriptor desc;
return &desc;
}
namespace outer {
class MyOtherClass_descriptor : public descriptor{
public:
MyOtherClass_descriptor() : descriptor("MyOtherClass", nullptr) {
field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::arturo)>()(m_fields, "arturo", &MyOtherClass::arturo);
field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::ignazio)>()(m_fields, "ignazio", &MyOtherClass::ignazio);
field_adder<outer::MyOtherClass,decltype(outer::MyOtherClass::pippo)>()(m_fields, "pippo", &MyOtherClass::pippo);

}
};
}

descriptor* outer::MyOtherClass::get_descriptor() {
static outer::MyOtherClass_descriptor desc;
return &desc;
}

```
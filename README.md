example:

file `simpleobj.h`
```c++
//
// Created by crax on 11/7/21.
//

#pragma once

#include "macros.h"

class REFLECT() test {
    int REFLECT() x;
    float REFLECT() y;
    double REFLECT() z;
    
    void REFLECT() do_thong(const std::string& asd) {
    
    }
public:
    std::string REFLECT() pippo;

    int REFLECT() do_thing (int x, int y) {
        return x + y;
    }
}

namespace outer {
    namespace inner {
        class REFLECT() Foo {
            int num1;
            int REFLECT() num2;
            int num3;
        
            int REFLECT() do_thing_with_num1(int x) const {
                return num1 + x;
            }
        };
    }

    class REFLECT() Bar {
        int REFLECT() asd;
        int REFLECT() dsa;
        float REFLECT() answer = 42.0f;
        
        float REFLECT() get_answer() const {
            return answer;
        }
        void REFLECT() say_something() const {
            // print "Hello World";
        }
    };
}


```
with the templates in the `sample_templates` folder (which are just the templates used in my `cake` game engine):

running `./katsu ../tests/simpleobj.h -- -ogen -t../sample_templates/ -I../tests/` generates the file `gen/simpleobj.gen.g`:
```c++
// This file was generated automatically using the katsu code analyzer or whatever they are called
// https://github.com/crax97/katsu 

#include "object/fields.h"
#include "object/macros.h"

namespace  {
    CORE_BEGIN_DESCRIPTOR(test)
            CORE_REFLECT_MEMBER(x)
            CORE_REFLECT_MEMBER(y)
            CORE_REFLECT_MEMBER(z)
            CORE_REFLECT_MEMBER(pippo)

            CORE_REFLECT_METHOD(do_thong)
            CORE_REFLECT_METHOD(do_thing)

    CORE_END_DESCRIPTOR()
}

CORE_IMPLEMENT_FUNCTIONS(::test)
namespace outer::inner {
    CORE_BEGIN_DESCRIPTOR(Foo)
            CORE_REFLECT_MEMBER(num2)

            CORE_REFLECT_METHOD(do_thing_with_num1)

    CORE_END_DESCRIPTOR()
}

CORE_IMPLEMENT_FUNCTIONS(outer::inner::Foo)
namespace outer::inner {
    CORE_BEGIN_DESCRIPTOR(Bar)
            CORE_REFLECT_MEMBER(asd)
            CORE_REFLECT_MEMBER(dsa)
            CORE_REFLECT_MEMBER(answer)

            CORE_REFLECT_METHOD(get_answer)
            CORE_REFLECT_METHOD(say_something)

    CORE_END_DESCRIPTOR()
}

CORE_IMPLEMENT_FUNCTIONS(outer::inner::Bar)
```
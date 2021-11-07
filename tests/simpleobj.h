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

    int REFLECT() do_thing (int x, int y) const {
       return x + y;
    }
};

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

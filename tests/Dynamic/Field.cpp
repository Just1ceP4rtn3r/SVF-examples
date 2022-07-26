#include <iostream>

using namespace std;

struct S1
{
    /* data */
    int f;
};

struct S2
{

    int a;
    int b;
    short c;
    double key_var;
    struct S1 e;

} field_test;

int main()
{

    field_test.a = 1;
    field_test.b = 2;
    field_test.c = 2;
    field_test.key_var = 2.0;
    field_test.e = S1();
    int x = field_test.a;
}

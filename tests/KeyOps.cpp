#include <iostream>
#include <vector>

#define test 0;

using namespace std;

struct S1
{
    /* data */
    int f;
    static int static_var;
};

struct Link
{
    /* data */
    int f;
    struct Link *next;
};

class Father
{
public:
    int a[10];
    vector<int> b;
    struct Link *header;
    double d;
    struct S1 *e;
    Father()
    {
        cout << "Father construct\n";
    }

    void func()
    {
        this->e = new S1();
        this->e->f = 0;
    }

    void func2()
    {
        this->e->f = 2;
    }
};

int main()
{
    int x = test;

    Father *fa = new Father();
    fa->func();
    fa->func2();

    fa->a[0] = 10;
    fa->b.push_back(10);
    fa->a[1] = fa->b[0];

    fa->header = new Link();
    Link *node1 = new Link();
    Link *node2 = new Link();
    node1->next = node2;
    fa->header->next = node1;

    fa->header->next = fa->header->next->next;

    return 0;
}
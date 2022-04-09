#include <iostream>

using namespace std;

class A {
    public:
        A() { cout << __PRETTY_FUNCTION__ << endl; }
        A(const A &a) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~A() { cout << __PRETTY_FUNCTION__ << endl; }
        virtual void print() const { cout << __PRETTY_FUNCTION__ << endl; }
};

class B : virtual public A {
    public:
        B() { cout << __PRETTY_FUNCTION__ << endl; }
        B(const B &a) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~B() { cout << __PRETTY_FUNCTION__ << endl; }
        virtual void print() const { cout << __PRETTY_FUNCTION__ << endl; }
};

class C : virtual public A {
    public:
        C() { cout << __PRETTY_FUNCTION__ << endl; }
        C(const C &a) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~C() { cout << __PRETTY_FUNCTION__ << endl; }
        virtual void print() const { cout << __PRETTY_FUNCTION__ << endl; }
};

class D : virtual public B, virtual public C {
    public:
        D() { cout << __PRETTY_FUNCTION__ << endl; }
        D(const D &a) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~D() { cout << __PRETTY_FUNCTION__ << endl; }
        virtual void print() const { cout << __PRETTY_FUNCTION__ << endl; }
};

int main(int argc, char *argv[]) {
    // A a;
    // B b;
    // C c;
    D d;

    // d.B::A::print(); Doesn't work. Ambiguous
    
    // ((C) d).A::print(); Works. Creates and destroys a C.

    // d.print();

    d.C::A::print();

    return 0;
}

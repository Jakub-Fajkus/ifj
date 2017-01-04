//test trying to math with function #3
class Main{
    static void run(){
        int a;
        a = A.a + A.b + B.b + B.a;

    }
}
class A {
    static int a;
    static int b(){
        return 0;
    }
}

class B {
    static int a(){
        return 0;
    }
    static int b;
}

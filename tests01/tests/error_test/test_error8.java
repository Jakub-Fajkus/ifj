//void cannot return value #4
class Trol {
    static void trol (int a){
        Trol.trol(a);
        return a;
    }
}

class Main {
    static void run () {

        Trol.trol(1);
    }
}

//reading uninitialized variable #8
class Main {
    static void run() {
        int a = test();
    }

    static int test() {
        int b;
        return b;
    }
}

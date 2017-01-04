//missing return value in non-void function BOLVANSKY & AUGUSTIN #4
class Main {
    static void run() {
         int a = add(5, 10);
    }

    static int add(int a, int b){
        return;
    }

}

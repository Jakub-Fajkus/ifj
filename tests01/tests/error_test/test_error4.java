//too many arguments #4
class Main {
    static void run(){
        String a = "asdf";
        String b = "asdfsdaf";
        Main.abc(a);
        Main.abc(b);
        Main.abc(a,b);
    }
    
    static void abc(String str){
        ifj16.print(str);
    }
}

//wrong data type in function call #4

class Main
{
    static void run()
    {
         int a = 42;
         type(a);
    }

    static void type(String a)
    {
    	a = "abc";
    }
}

//uninitialized variable (section 4.3) #8
class Main
{
static void run()
    {
    int a;
    int b = 5;
    b = b + a;
    }
}
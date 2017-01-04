//using uninitialized variable #8
class Main
{
    static void run()
    {
        Second.variable();
    }
}

class Second
{
	static int variable()
	{
		int v;
        return v;
    }
}

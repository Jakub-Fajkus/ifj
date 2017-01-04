//wrong construction - function being called in initialization of static variable #2
class Main
{
	static void run()
	{
		ifj16.print("kebab");
	}
}

class Game
{
	static int number () {
		return 1;
	}
	static int a = 42 + number();
}

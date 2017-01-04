//variable named as keyword #2
class Main
{
	static void run()
	{
		int continue;
		ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
		int a = 5;
		int vysl;
		if (a < 0) {
			ifj16.print("Faktorial nelze spocitat!\n");
		}
		else {
			vysl = 1;
			while (a > 0) {
			vysl = vysl * a;
			a = a - 1;
		}
		ifj16.print("Vysledek je: " + vysl + "\n");
		}
	}
	
}

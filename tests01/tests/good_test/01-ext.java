class Main
{
	static void run()
	{
		int cnt = 10;
		while(cnt > 0)
			cnt = dec(cnt);
	}
	
	static int dec(int ciselko)
	{
		ifj16.print(ciselko+"\n");
		return ciselko-1;
	}
}

class no_error
{
	private:
		int one, two;

	public:
		no_error(int seed)
		{
			one = seed - 5;
			if(seed > 8 && seed < 15)
				two = 10;
			else
			{
				two = seed;
				one = two % 18;
			}
		}
		int result(int three)
		{
			return one + two - three;
		}
};

int main()
{
	no_error instance;
	int result;
	instance = no_error(13);

	result = .result(5);
}

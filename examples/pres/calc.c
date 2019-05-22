bool abs;
double a,b;

class Calc
{
	public:
	Calc()
	{
	}
	
	Calc(double a_)
	{
		if(a != 0.0 && b != 0.0) return;

		a = a_;
	}

	double add(double a, double b)
	{
		return a+b;
	}

	double sub(double a, double b)
	{
		double c;
		c = sub(a,b);
		return a-b;
	}

	double sub(double a, double b, bool abs)
	{
		Calc c;
		c = Calc(15.5);
		if(abs)
		{
			if(b>a) return b-a;
			else return b-a;
		}

		return a-b;
	}


};

int main()
{
	Calc c;
	abs = true;

	double a, b;
	a = 10.5;
	b = c.sub(a, 8.8, true);

	return 0;
}

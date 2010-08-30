net Unnamed
{
	node n2
	{
		TYPE = CPT;
		HEADER =
		{
			ID = n2;
			NAME = "n2";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (0, 1);
			PROBABILITIES = (0.3, 0.7);
		};
	};
	
	node n1
	{
		TYPE = CPT;
		HEADER =
		{
			ID = n1;
			NAME = "n1";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (0, 1);
			PROBABILITIES = (0.2, 0.8);
		};
	};

};

net Unnamed
{
	node Age
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Age;
			NAME = "Age";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (Adolescent, Adult, Senior);
			PROBABILITIES = (0.2, 0.6, 0.2);
		};
	};

	node Mileage
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Mileage;
			NAME = "Mileage";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (FiveThou, TwentyThou, FiftyThou, Domino);
			PROBABILITIES = (0.1, 0.4, 0.4, 0.1);
		};
	};

	node SocioEcon
	{
		TYPE = CPT;
		HEADER =
		{
			ID = SocioEcon;
			NAME = "SocioEcon";
		};
		PARENTS = (Age);
		DEFINITION =
		{
			NAMESTATES = (Prole, Middle, UpperMiddle, Wealthy);
			PROBABILITIES = (0.4, 0.4, 0.19, 0.01, 0.4, 0.4, 0.19, 0.01, 0.5, 0.2, 0.29, 0.01);
		};
	};

	node GoodStudent
	{
		TYPE = CPT;
		HEADER =
		{
			ID = GoodStudent;
			NAME = "GoodStudent";
		};
		PARENTS = (Age, SocioEcon);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (0.1, 0.9, 0, 1, 0, 1, 0.2, 0.8, 0, 1, 0, 1, 0.5, 0.5, 0, 1, 0, 1, 0.4, 0.6, 0, 1, 0, 1);
		};
	};

	node OtherCar
	{
		TYPE = CPT;
		HEADER =
		{
			ID = OtherCar;
			NAME = "OtherCar";
		};
		PARENTS = (SocioEcon);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (0.5, 0.5, 0.8, 0.2, 0.9, 0.1, 0.95, 0.05);
		};
	};

	node RiskAversion
	{
		TYPE = CPT;
		HEADER =
		{
			ID = RiskAversion;
			NAME = "RiskAversion";
		};
		PARENTS = (Age, SocioEcon);
		DEFINITION =
		{
			NAMESTATES = (Psychopath, Adventurous, Normal, Cautious);
			PROBABILITIES = (0.02, 0.58, 0.3, 0.1, 0.02, 0.38, 0.5, 0.1, 0.02, 0.48, 0.4, 0.1, 0.02, 0.58, 0.3, 0.1, 0.015, 0.285, 0.5, 0.2, 0.015, 0.185, 0.6, 0.2, 0.015, 0.285, 0.5, 0.2, 0.015, 0.285, 0.4, 0.3, 0.01, 0.09, 0.4, 0.5, 0.01, 0.04, 0.35, 0.6, 0.01, 0.09, 0.4, 0.5, 0.01, 0.09, 0.4, 0.5);
		};
	};

	node HomeBase
	{
		TYPE = CPT;
		HEADER =
		{
			ID = HomeBase;
			NAME = "HomeBase";
		};
		PARENTS = (SocioEcon, RiskAversion);
		DEFINITION =
		{
			NAMESTATES = (Secure, City, Suburb, Rural);
			PROBABILITIES = (1e-06, 0.8, 0.049999, 0.15, 0.15, 0.8, 0.04, 0.01, 0.35, 0.6, 0.04, 0.01, 0.489999, 0.5, 1e-06, 0.01, 1e-06, 0.8, 0.05, 0.149999, 0.01, 0.25, 0.6, 0.14, 0.2, 0.4, 0.3, 0.1, 0.95, 1e-06, 1e-06, 0.049998, 1e-06, 0.8, 0.05, 0.149999, 0.299999, 1e-06, 0.6, 0.1, 0.5, 1e-06, 0.4, 0.099999, 0.85, 1e-06, 0.001, 0.148999, 1e-06, 0.8, 0.05, 0.149999, 0.95, 1e-06, 0.024445, 0.025554, 0.999997, 1e-06, 1e-06, 1e-06, 0.999997, 1e-06, 1e-06, 1e-06);
		};
	};

	node VehicleYear
	{
		TYPE = CPT;
		HEADER =
		{
			ID = VehicleYear;
			NAME = "VehicleYear";
		};
		PARENTS = (RiskAversion, SocioEcon);
		DEFINITION =
		{
			NAMESTATES = (Current, Older);
			PROBABILITIES = (0.15, 0.85, 0.15, 0.85, 0.15, 0.85, 0.15, 0.85, 0.3, 0.7, 0.3, 0.7, 0.3, 0.7, 0.3, 0.7, 0.8, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0.2, 0.9, 0.1, 0.9, 0.1, 0.9, 0.1, 0.9, 0.1);
		};
	};

	node AntiTheft
	{
		TYPE = CPT;
		HEADER =
		{
			ID = AntiTheft;
			NAME = "AntiTheft";
		};
		PARENTS = (RiskAversion, SocioEcon);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (1e-06, 0.999999, 1e-06, 0.999999, 0.05, 0.95, 0.5, 0.5, 1e-06, 0.999999, 1e-06, 0.999999, 0.2, 0.8, 0.5, 0.5, 0.1, 0.9, 0.3, 0.7, 0.9, 0.1, 0.8, 0.2, 0.95, 0.05, 0.999999, 1e-06, 0.999999, 1e-06, 0.999999, 1e-06);
		};
	};

	node SeniorTrain
	{
		TYPE = CPT;
		HEADER =
		{
			ID = SeniorTrain;
			NAME = "SeniorTrain";
		};
		PARENTS = (Age, RiskAversion);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1e-06, 0.999999, 1e-06, 0.999999, 0.3, 0.7, 0.9, 0.1);
		};
	};

	node MakeModel
	{
		TYPE = CPT;
		HEADER =
		{
			ID = MakeModel;
			NAME = "MakeModel";
		};
		PARENTS = (SocioEcon, RiskAversion);
		DEFINITION =
		{
			NAMESTATES = (SportsCar, Economy, FamilySedan, Luxury, SuperLuxury);
			PROBABILITIES = (0.1, 0.7, 0.2, 0, 0, 0.1, 0.7, 0.2, 0, 0, 0.1, 0.7, 0.2, 0, 0, 0.1, 0.7, 0.2, 0, 0, 0.15, 0.2, 0.65, 0, 0, 0.15, 0.2, 0.65, 0, 0, 0.15, 0.2, 0.65, 0, 0, 0.15, 0.2, 0.65, 0, 0, 0.2, 0.05, 0.3, 0.45, 0, 0.2, 0.05, 0.3, 0.45, 0, 0.2, 0.05, 0.3, 0.45, 0, 0.2, 0.05, 0.3, 0.45, 0, 0.3, 0.01, 0.09, 0.4, 0.2, 0.3, 0.01, 0.09, 0.4, 0.2, 0.3, 0.01, 0.09, 0.4, 0.2, 0.3, 0.01, 0.09, 0.4, 0.2);
		};
	};

	node CarValue
	{
		TYPE = CPT;
		HEADER =
		{
			ID = CarValue;
			NAME = "CarValue";
		};
		PARENTS = (Mileage, MakeModel, VehicleYear);
		DEFINITION =
		{
			NAMESTATES = (FiveThou, TenThou, TwentyThou, FiftyThou, Million);
			PROBABILITIES = (0, 0.1, 0.8, 0.09, 0.01, 0, 0.1, 0.8, 0.09, 0.01, 0, 0.1, 0.8, 0.09, 0.01, 0, 0.1, 0.8, 0.09, 0.01, 0.03, 0.3, 0.6, 0.06, 0.01, 0.16, 0.5, 0.3, 0.03, 0.01, 0.4, 0.47, 0.1, 0.02, 0.01, 0.9, 0.06, 0.02, 0.01, 0.01, 0.1, 0.8, 0.1, 0, 0, 0.1, 0.8, 0.1, 0, 0, 0.1, 0.8, 0.1, 0, 0, 0.1, 0.8, 0.1, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0.7, 0.2999, 0.0001, 0, 0, 0.99, 0.009999, 1e-06, 0, 0, 0.999998, 1e-06, 1e-06, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0.1, 0.9, 0, 0, 0.2, 0.3, 0.5, 0, 0, 0.5, 0.3, 0.2, 0, 0, 0.7, 0.2, 0.1, 0, 0, 0.99, 0.009999, 1e-06, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0.01, 0.09, 0.2, 0.7, 0, 0.05, 0.15, 0.3, 0.5, 0, 0.1, 0.3, 0.3, 0.3, 0, 0.2, 0.2, 0.3, 0.3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1e-06, 1e-06, 1e-06, 1e-06, 0.999996, 1e-06, 1e-06, 1e-06, 1e-06, 0.999996, 1e-06, 1e-06, 1e-06, 1e-06, 0.999996, 1e-06, 1e-06, 1e-06, 1e-06, 0.999996);
		};
	};

	node Airbag
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Airbag;
			NAME = "Airbag";
		};
		PARENTS = (MakeModel, VehicleYear);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (1, 0, 0.1, 0.9, 1, 0, 0.05, 0.95, 1, 0, 0.2, 0.8, 1, 0, 0.6, 0.4, 1, 0, 0.1, 0.9);
		};
	};

	node Antilock
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Antilock;
			NAME = "Antilock";
		};
		PARENTS = (MakeModel, VehicleYear);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (0.9, 0.1, 0.1, 0.9, 0.001, 0.999, 0, 1, 0.4, 0.6, 0, 1, 0.99, 0.01, 0.3, 0.7, 0.99, 0.01, 0.15, 0.85);
		};
	};

	node RuggedAuto
	{
		TYPE = CPT;
		HEADER =
		{
			ID = RuggedAuto;
			NAME = "RuggedAuto";
		};
		PARENTS = (MakeModel, VehicleYear);
		DEFINITION =
		{
			NAMESTATES = (EggShell, Football, Tank);
			PROBABILITIES = (0.95, 0.04, 0.01, 0.95, 0.04, 0.01, 0.5, 0.5, 0, 0.9, 0.1, 0, 0.2, 0.6, 0.2, 0.05, 0.55, 0.4, 0.1, 0.6, 0.3, 0.1, 0.6, 0.3, 0.05, 0.55, 0.4, 0.05, 0.55, 0.4);
		};
	};

	node DrivingSkill
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DrivingSkill;
			NAME = "DrivingSkill";
		};
		PARENTS = (SeniorTrain, Age);
		DEFINITION =
		{
			NAMESTATES = (SubStandard, Normal, Expert);
			PROBABILITIES = (0.5, 0.45, 0.05, 0.5, 0.45, 0.05, 0.3, 0.6, 0.1, 0.3, 0.6, 0.1, 0.1, 0.6, 0.3, 0.4, 0.5, 0.1);
		};
	};

	node Theft
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Theft;
			NAME = "Theft";
		};
		PARENTS = (AntiTheft, CarValue, HomeBase);
		DEFINITION =
		{
			NAMESTATES = (True, False);
			PROBABILITIES = (1e-06, 0.999999, 2e-06, 0.999998, 3e-06, 0.999997, 2e-06, 0.999998, 1e-06, 0.999999, 0.0005, 0.9995, 0.002, 0.998, 0.005, 0.995, 0.005, 0.995, 1e-06, 0.999999, 1e-05, 0.99999, 0.0001, 0.9999, 0.0003, 0.9997, 0.0003, 0.9997, 1e-06, 0.999999, 1e-05, 0.99999, 2e-05, 0.99998, 5e-05, 0.99995, 5e-05, 0.99995, 1e-06, 0.999999, 1e-06, 0.999999, 2e-06, 0.999998, 3e-06, 0.999997, 2e-06, 0.999998, 1e-06, 0.999999, 0.001, 0.999, 0.005, 0.995, 0.01, 0.99, 0.01, 0.99, 1e-06, 0.999999, 1e-05, 0.99999, 0.0002, 0.9998, 0.0005, 0.9995, 0.0005, 0.9995, 1e-06, 0.999999, 1e-05, 0.99999, 0.0001, 0.9999, 0.0002, 0.9998, 0.0002, 0.9998, 1e-06, 0.999999);
		};
	};

	node Cushioning
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Cushioning;
			NAME = "Cushioning";
		};
		PARENTS = (Airbag, RuggedAuto);
		DEFINITION =
		{
			NAMESTATES = (Poor, Fair, Good, Excellent);
			PROBABILITIES = (0.5, 0.3, 0.2, 0, 0.7, 0.3, 0, 0, 0, 0.1, 0.6, 0.3, 0.1, 0.6, 0.3, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3);
		};
	};

	node DrivHist
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DrivHist;
			NAME = "DrivHist";
		};
		PARENTS = (DrivingSkill, RiskAversion);
		DEFINITION =
		{
			NAMESTATES = (Zero, One, Many);
			PROBABILITIES = (0.001, 0.004, 0.995, 0.002, 0.008, 0.99, 0.03, 0.15, 0.82, 0.3, 0.3, 0.4, 0.1, 0.3, 0.6, 0.5, 0.3, 0.2, 0.9, 0.07, 0.03, 0.95, 0.04, 0.01, 0.3, 0.3, 0.4, 0.6, 0.3, 0.1, 0.99, 0.009999, 1e-06, 0.999998, 1e-06, 1e-06);
		};
	};

	node DrivQuality
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DrivQuality;
			NAME = "DrivQuality";
		};
		PARENTS = (RiskAversion, DrivingSkill);
		DEFINITION =
		{
			NAMESTATES = (Poor, Normal, Excellent);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.5, 0.2, 0.3, 0.3, 0.4, 0.3, 0, 1, 0, 0, 0.8, 0.2, 0.3, 0.2, 0.5, 0.01, 0.01, 0.98, 0, 0, 1, 0, 0, 1);
		};
	};

	node Accident
	{
		TYPE = CPT;
		HEADER =
		{
			ID = Accident;
			NAME = "Accident";
		};
		PARENTS = (Mileage, Antilock, DrivQuality);
		DEFINITION =
		{
			NAMESTATES = (None, Mild, Moderate, Severe);
			PROBABILITIES = (0.7, 0.2, 0.07, 0.03, 0.99, 0.007, 0.002, 0.001, 0.999, 0.0007, 0.0002, 0.0001, 0.4, 0.3, 0.2, 0.1, 0.98, 0.01, 0.005, 0.005, 0.995, 0.003, 0.001, 0.001, 0.3, 0.3, 0.2, 0.2, 0.97, 0.02, 0.007, 0.003, 0.99, 0.007, 0.002, 0.001, 0.2, 0.2, 0.3, 0.3, 0.95, 0.03, 0.01, 0.01, 0.985, 0.01, 0.003, 0.002, 0.6, 0.2, 0.1, 0.1, 0.98, 0.01, 0.005, 0.005, 0.995, 0.003, 0.001, 0.001, 0.3, 0.2, 0.2, 0.3, 0.96, 0.02, 0.015, 0.005, 0.99, 0.007, 0.002, 0.001, 0.2, 0.2, 0.2, 0.4, 0.95, 0.03, 0.015, 0.005, 0.98, 0.01, 0.005, 0.005, 0.1, 0.1, 0.3, 0.5, 0.94, 0.03, 0.02, 0.01, 0.98, 0.01, 0.007, 0.003);
		};
	};

	node ThisCarDam
	{
		TYPE = CPT;
		HEADER =
		{
			ID = ThisCarDam;
			NAME = "ThisCarDam";
		};
		PARENTS = (RuggedAuto, Accident);
		DEFINITION =
		{
			NAMESTATES = (None, Mild, Moderate, Severe);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.001, 0.9, 0.098, 0.001, 0.2, 0.75, 0.049999, 1e-06, 0.7, 0.29, 0.009999, 1e-06, 1e-06, 0.000999, 0.7, 0.299, 0.001, 0.099, 0.8, 0.1, 0.05, 0.6, 0.3, 0.05, 1e-06, 9e-06, 9e-05, 0.9999, 1e-06, 0.000999, 0.009, 0.99, 0.05, 0.2, 0.2, 0.55);
		};
	};

	node ThisCarCost
	{
		TYPE = CPT;
		HEADER =
		{
			ID = ThisCarCost;
			NAME = "ThisCarCost";
		};
		PARENTS = (CarValue, Theft, ThisCarDam);
		DEFINITION =
		{
			NAMESTATES = (Thousand, TenThou, HundredThou, Million);
			PROBABILITIES = (0.2, 0.8, 0, 0, 1, 0, 0, 0, 0.05, 0.95, 0, 0, 1, 0, 0, 0, 0.04, 0.01, 0.95, 0, 1, 0, 0, 0, 0.04, 0.01, 0.95, 0, 1, 0, 0, 0, 0.04, 0.01, 0.2, 0.75, 1, 0, 0, 0, 0.15, 0.85, 0, 0, 0.95, 0.05, 0, 0, 0.03, 0.97, 0, 0, 0.95, 0.05, 0, 0, 0.03, 0.02, 0.95, 0, 0.99, 0.01, 0, 0, 0.03, 0.02, 0.95, 0, 0.99, 0.01, 0, 0, 0.02, 0.03, 0.25, 0.7, 0.98, 0.01, 0.01, 0, 0.05, 0.95, 0, 0, 0.25, 0.75, 0, 0, 0.01, 0.99, 0, 0, 0.15, 0.85, 0, 0, 0.001, 0.001, 0.998, 0, 0.01, 0.01, 0.98, 0, 0.001, 0.001, 0.998, 0, 0.005, 0.005, 0.99, 0, 0.001, 0.001, 0.018, 0.98, 0.003, 0.003, 0.044, 0.95, 0.03, 0.97, 0, 0, 0.05, 0.95, 0, 0, 1e-06, 0.999999, 0, 0, 0.01, 0.99, 0, 0, 1e-06, 1e-06, 0.999998, 0, 0.005, 0.005, 0.99, 0, 1e-06, 1e-06, 0.999998, 0, 0.001, 0.001, 0.998, 0, 1e-06, 1e-06, 0.009998, 0.99, 1e-06, 1e-06, 0.029998, 0.97);
		};
	};

	node MedCost
	{
		TYPE = CPT;
		HEADER =
		{
			ID = MedCost;
			NAME = "MedCost";
		};
		PARENTS = (Age, Accident, Cushioning);
		DEFINITION =
		{
			NAMESTATES = (Thousand, TenThou, HundredThou, Million);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.96, 0.03, 0.009, 0.001, 0.98, 0.019, 0.0009, 0.0001, 0.99, 0.0099, 9e-05, 1e-05, 0.999, 0.00099, 9e-06, 1e-06, 0.96, 0.03, 0.009, 0.001, 0.98, 0.019, 0.0009, 0.0001, 0.99, 0.0099, 9e-05, 1e-05, 0.999, 0.00099, 9e-06, 1e-06, 0.9, 0.07, 0.02, 0.01, 0.95, 0.04, 0.007, 0.003, 0.97, 0.025, 0.003, 0.002, 0.99, 0.007, 0.002, 0.001, 0.5, 0.2, 0.2, 0.1, 0.8, 0.15, 0.03, 0.02, 0.95, 0.02, 0.02, 0.01, 0.99, 0.007, 0.002, 0.001, 0.5, 0.2, 0.2, 0.1, 0.8, 0.15, 0.03, 0.02, 0.95, 0.02, 0.02, 0.01, 0.99, 0.007, 0.002, 0.001, 0.3, 0.3, 0.2, 0.2, 0.5, 0.2, 0.2, 0.1, 0.9, 0.07, 0.02, 0.01, 0.95, 0.03, 0.01, 0.01, 0.3, 0.3, 0.2, 0.2, 0.5, 0.2, 0.2, 0.1, 0.9, 0.07, 0.02, 0.01, 0.95, 0.03, 0.01, 0.01, 0.3, 0.3, 0.2, 0.2, 0.5, 0.2, 0.2, 0.1, 0.9, 0.07, 0.02, 0.01, 0.95, 0.03, 0.01, 0.01, 0.2, 0.2, 0.3, 0.3, 0.3, 0.3, 0.2, 0.2, 0.6, 0.3, 0.07, 0.03, 0.9, 0.05, 0.03, 0.02);
		};
	};

	node ILiCost
	{
		TYPE = CPT;
		HEADER =
		{
			ID = ILiCost;
			NAME = "ILiCost";
		};
		PARENTS = (Accident);
		DEFINITION =
		{
			NAMESTATES = (Thousand, TenThou, HundredThou, Million);
			PROBABILITIES = (1, 0, 0, 0, 0.999, 0.000998, 1e-06, 1e-06, 0.9, 0.05, 0.03, 0.02, 0.8, 0.1, 0.06, 0.04);
		};
	};

	node OtherCarCost
	{
		TYPE = CPT;
		HEADER =
		{
			ID = OtherCarCost;
			NAME = "OtherCarCost";
		};
		PARENTS = (RuggedAuto, Accident);
		DEFINITION =
		{
			NAMESTATES = (Thousand, TenThou, HundredThou, Million);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.99, 0.005, 0.00499, 1e-05, 0.98, 0.01, 0.009985, 5e-05, 0.95, 0.03, 0.01998, 2e-05, 0.6, 0.2, 0.19998, 2e-05, 0.5, 0.2, 0.29997, 3e-05, 0.4, 0.3, 0.29996, 4e-05, 0.2, 0.4, 0.39996, 4e-05, 0.1, 0.5, 0.39994, 6e-05, 0.005, 0.55, 0.4449, 0.0001);
		};
	};

	node PropCost
	{
		TYPE = CPT;
		HEADER =
		{
			ID = PropCost;
			NAME = "PropCost";
		};
		PARENTS = (ThisCarCost, OtherCarCost);
		DEFINITION =
		{
			NAMESTATES = (Thousand, TenThou, HundredThou, Million);
			PROBABILITIES = (0.7, 0.3, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 1, 0, 0.95, 0.05, 0, 0, 0.6, 0.4, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 1, 0, 0, 0.98, 0.02, 0, 0, 0.8, 0.2, 0, 0, 0.6, 0.4, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

};
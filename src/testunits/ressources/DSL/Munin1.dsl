net Unnamed
{
	node R_MYAS_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYAS_APB_MUDENS;
			NAME = "R_MYAS_APB_MUDENS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0);
		};
	};

	node R_LNLLP_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_MALOSS;
			NAME = "R_LNLLP_APB_MALOSS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0);
		};
	};

	node DIFFN_SEV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_SEV;
			NAME = "DIFFN_SEV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (0.78, 0.1, 0.07, 0.05);
		};
	};

	node R_LNLW_MED_TIME
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MED_TIME;
			NAME = "R_LNLW_MED_TIME";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (ACUTE, SUBACUTE, CHRONIC, OLD);
			PROBABILITIES = (0.05, 0.33, 0.6, 0.02);
		};
	};

	node R_LNLT1_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_DE_REGEN;
			NAME = "R_LNLT1_APB_DE_REGEN";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0);
		};
	};

	node R_MYDY_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYDY_APB_DENERV;
			NAME = "R_MYDY_APB_DENERV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0);
		};
	};

	node R_LNLW_MED_SEV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MED_SEV;
			NAME = "R_LNLW_MED_SEV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (0.73, 0.16, 0.07, 0.03, 0.01);
		};
	};

	node R_LNLBE_MED_TIME
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MED_TIME;
			NAME = "R_LNLBE_MED_TIME";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (ACUTE, SUBACUTE, CHRONIC, OLD);
			PROBABILITIES = (0.05, 0.6, 0.3, 0.05);
		};
	};

	node R_MYDY_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYDY_APB_DE_REGEN;
			NAME = "R_MYDY_APB_DE_REGEN";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0);
		};
	};

	node R_LNLLP_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_MUDENS;
			NAME = "R_LNLLP_APB_MUDENS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0);
		};
	};

	node R_MYOP_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_APB_MUDENS;
			NAME = "R_MYOP_APB_MUDENS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0);
		};
	};

	node R_LNLBE_MED_SEV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MED_SEV;
			NAME = "R_LNLBE_MED_SEV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (0.981, 0.01, 0.005, 0.003, 0.001);
		};
	};

	node R_LNLT1_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_NEUR_ACT;
			NAME = "R_LNLT1_APB_NEUR_ACT";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0);
		};
	};

	node DIFFN_PATHO
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_PATHO;
			NAME = "DIFFN_PATHO";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (DEMY, BLOCK, AXONAL, V_E_REIN, E_REIN);
			PROBABILITIES = (0.086, 0.01, 0.9, 0.002, 0.002);
		};
	};

	node DIFFN_TIME
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_TIME;
			NAME = "DIFFN_TIME";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (ACUTE, SUBACUTE, CHRONIC, OLD);
			PROBABILITIES = (0.01, 0.25, 0.65, 0.09);
		};
	};

	node R_LNLT1_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_MUSIZE;
			NAME = "R_LNLT1_APB_MUSIZE";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0);
		};
	};

	node R_MYOP_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_APB_DENERV;
			NAME = "R_MYOP_APB_DENERV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0);
		};
	};

	node R_LNLW_MED_PATHO
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MED_PATHO;
			NAME = "R_LNLW_MED_PATHO";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (DEMY, BLOCK, AXONAL, V_E_REIN, E_REIN);
			PROBABILITIES = (0.8, 0.12, 0.07, 0.005, 0.005);
		};
	};

	node R_MYOP_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_APB_MUSIZE;
			NAME = "R_MYOP_APB_MUSIZE";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0);
		};
	};

	node R_LNLLP_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_DENERV;
			NAME = "R_LNLLP_APB_DENERV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0);
		};
	};

	node R_LNLLP_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_MUSIZE;
			NAME = "R_LNLLP_APB_MUSIZE";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0);
		};
	};

	node R_MYOP_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_APB_DE_REGEN;
			NAME = "R_MYOP_APB_DE_REGEN";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0);
		};
	};

	node DIFFN_TYPE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_TYPE;
			NAME = "DIFFN_TYPE";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (MOTOR, MIXED, SENS);
			PROBABILITIES = (0.06, 0.935, 0.005);
		};
	};

	node R_MYAS_APB_NMT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYAS_APB_NMT;
			NAME = "R_MYAS_APB_NMT";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MOD_PRE, SEV_PRE, MLD_POST, MOD_POST, SEV_POST, MIXED);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0);
		};
	};

	node R_LNLLP_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_DE_REGEN;
			NAME = "R_LNLLP_APB_DE_REGEN";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0);
		};
	};

	node R_LNLBE_MED_PATHO
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MED_PATHO;
			NAME = "R_LNLBE_MED_PATHO";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (DEMY, BLOCK, AXONAL, V_E_REIN, E_REIN);
			PROBABILITIES = (0.6, 0.19, 0.2, 0.005, 0.005);
		};
	};

	node R_LNLT1_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_MUDENS;
			NAME = "R_LNLT1_APB_MUDENS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0);
		};
	};

	node R_APB_VOL_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_VOL_ACT;
			NAME = "R_APB_VOL_ACT";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, REDUCED, V_RED, ABSENT);
			PROBABILITIES = (1, 0, 0, 0);
		};
	};

	node DIFFN_DISTR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_DISTR;
			NAME = "DIFFN_DISTR";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (DIST, PROX, RANDOM);
			PROBABILITIES = (0.93, 0.02, 0.05);
		};
	};

	node R_LNLT1_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_MALOSS;
			NAME = "R_LNLT1_APB_MALOSS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0);
		};
	};

	node R_MYDY_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYDY_APB_MUSIZE;
			NAME = "R_MYDY_APB_MUSIZE";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0);
		};
	};

	node R_LNLLP_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLLP_APB_NEUR_ACT;
			NAME = "R_LNLLP_APB_NEUR_ACT";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0);
		};
	};

	node R_MYDY_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYDY_APB_MUDENS;
			NAME = "R_MYDY_APB_MUDENS";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0);
		};
	};

	node R_LNLT1_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_APB_DENERV;
			NAME = "R_LNLT1_APB_DENERV";
		};
		PARENTS = ();
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0);
		};
	};

	node R_LNLW_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_DE_REGEN;
			NAME = "R_LNLW_APB_DE_REGEN";
		};
		PARENTS = (R_LNLW_MED_TIME, R_LNLW_MED_SEV, R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.8, 0.2, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1);
		};
	};

	node R_LNLW_MEDD2_BLOCK_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MEDD2_BLOCK_WD;
			NAME = "R_LNLW_MEDD2_BLOCK_WD";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.3, 0.6, 0.1, 0, 0, 0, 0, 0.5, 0.5, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.1, 0.5, 0.3, 0.1, 0, 0, 0, 0.2, 0.6, 0.2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.05, 0.2, 0.55, 0.2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_LNLT1_LP_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_DENERV;
			NAME = "R_LNLT1_LP_APB_DENERV";
		};
		PARENTS = (R_LNLT1_APB_DENERV, R_LNLLP_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_LNLW_MEDD2_SALOSS_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MEDD2_SALOSS_WD;
			NAME = "R_LNLW_MEDD2_SALOSS_WD";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0.2, 0.5, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0.2, 0.5, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.4, 0.6, 0, 0.1, 0.4, 0.4, 0.1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_LNLBE_MED_BLOCK
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MED_BLOCK;
			NAME = "R_LNLBE_MED_BLOCK";
		};
		PARENTS = (R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.6, 0.4, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.4, 0.5, 0.1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 1, 0.2, 0.2, 0.2, 0.2, 0.2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_MED_RD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_RD_EW;
			NAME = "R_MED_RD_EW";
		};
		PARENTS = (R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0);
		};
	};

	node R_LNLT1_LP_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_DE_REGEN;
			NAME = "R_LNLT1_LP_APB_DE_REGEN";
		};
		PARENTS = (R_LNLLP_APB_DE_REGEN, R_LNLT1_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node DIFFN_SENS_SEV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_SENS_SEV;
			NAME = "DIFFN_SENS_SEV";
		};
		PARENTS = (DIFFN_TYPE, DIFFN_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_LNLBE_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_MUSIZE;
			NAME = "R_LNLBE_APB_MUSIZE";
		};
		PARENTS = (R_LNLBE_MED_TIME, R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.7, 0.25, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.7, 0.25, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_LNLBE_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_NEUR_ACT;
			NAME = "R_LNLBE_APB_NEUR_ACT";
		};
		PARENTS = (R_LNLBE_MED_TIME, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
		};
	};

	node R_MED_RD_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_RD_WA;
			NAME = "R_MED_RD_WA";
		};
		PARENTS = (R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0);
		};
	};

	node R_LNLW_MEDD2_LD_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MEDD2_LD_WD;
			NAME = "R_LNLW_MEDD2_LD_WD";
		};
		PARENTS = (R_LNLW_MED_SEV, R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0.25, 0.5, 0.25, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.05, 0.3, 0.5, 0.15, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.25, 0.25, 0.25, 0.25, 0, 0.5, 0.5, 0, 1, 0, 0, 0, 1, 0, 0, 0);
		};
	};

	node R_MEDD2_LD_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_LD_WD;
			NAME = "R_MEDD2_LD_WD";
		};
		PARENTS = (R_LNLW_MEDD2_LD_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_LNLW_MED_BLOCK
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MED_BLOCK;
			NAME = "R_LNLW_MED_BLOCK";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.6, 0.4, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.4, 0.5, 0.1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.2, 0.2, 0.2, 0.2, 0.2, 0, 0, 0, 0, 1, 0.2, 0.2, 0.2, 0.2, 0.2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_LNLW_MEDD2_DISP_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MEDD2_DISP_WD;
			NAME = "R_LNLW_MEDD2_DISP_WD";
		};
		PARENTS = (R_LNLW_MED_SEV, R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0.1, 0.5, 0.4, 0.3, 0.5, 0.2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_LNLBE_MEDD2_LD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_LD_EW;
			NAME = "R_LNLBE_MEDD2_LD_EW";
		};
		PARENTS = (R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0.25, 0.5, 0.25, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.05, 0.3, 0.5, 0.15, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.25, 0.25, 0.25, 0.25, 0, 0.5, 0.5, 0, 1, 0, 0, 0, 1, 0, 0, 0);
		};
	};

	node R_MYOP_MYDY_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_MYDY_APB_MUDENS;
			NAME = "R_MYOP_MYDY_APB_MUDENS";
		};
		PARENTS = (R_MYDY_APB_MUDENS, R_MYOP_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_MYOP_MYDY_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_MYDY_APB_DE_REGEN;
			NAME = "R_MYOP_MYDY_APB_DE_REGEN";
		};
		PARENTS = (R_MYOP_APB_DE_REGEN, R_MYDY_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node DIFFN_MOT_SEV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_MOT_SEV;
			NAME = "DIFFN_MOT_SEV";
		};
		PARENTS = (DIFFN_TYPE, DIFFN_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0);
		};
	};

	node R_LNLW_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_MALOSS;
			NAME = "R_LNLW_APB_MALOSS";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0.4, 0.3, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.1, 0.9, 0.25, 0.25, 0.25, 0.25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_LNLW_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_NEUR_ACT;
			NAME = "R_LNLW_APB_NEUR_ACT";
		};
		PARENTS = (R_LNLW_MED_TIME, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
		};
	};

	node R_LNLBE_MEDD2_DISP_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_DISP_EW;
			NAME = "R_LNLBE_MEDD2_DISP_EW";
		};
		PARENTS = (R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0.1, 0.5, 0.4, 0.3, 0.5, 0.2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_LNLBE_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_MUDENS;
			NAME = "R_LNLBE_APB_MUDENS";
		};
		PARENTS = (R_LNLBE_MED_TIME, R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.9, 0.1, 0, 0.9, 0.1, 0, 0.6, 0.4, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.7, 0.3, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.1, 0.6, 0.3, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.15, 0.7, 0.15, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.6, 0.4, 0, 0.6, 0.4, 0, 0.5, 0.4, 0.1, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.6, 0.4, 0, 0.6, 0.4, 0, 0.3, 0.6, 0.1, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3);
		};
	};

	node R_MED_LD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_LD_EW;
			NAME = "R_MED_LD_EW";
		};
		PARENTS = (R_LNLBE_MED_PATHO, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0.25, 0.5, 0.25, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.05, 0.3, 0.5, 0.15, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25);
		};
	};

	node R_LNLBE_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_MALOSS;
			NAME = "R_LNLBE_APB_MALOSS";
		};
		PARENTS = (R_LNLBE_MED_PATHO, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0.4, 0.3, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.1, 0.9, 0.25, 0.25, 0.25, 0.25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_MEDD2_LD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_LD_EW;
			NAME = "R_MEDD2_LD_EW";
		};
		PARENTS = (R_LNLBE_MEDD2_LD_EW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_LNLT1_LP_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_NEUR_ACT;
			NAME = "R_LNLT1_LP_APB_NEUR_ACT";
		};
		PARENTS = (R_LNLLP_APB_NEUR_ACT, R_LNLT1_APB_NEUR_ACT);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MED_LD_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_LD_WA;
			NAME = "R_MED_LD_WA";
		};
		PARENTS = (R_LNLW_MED_SEV, R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0.25, 0.5, 0.25, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0.05, 0.3, 0.5, 0.15, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25);
		};
	};

	node R_MUSCLE_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MUSCLE_APB_MUDENS;
			NAME = "R_MUSCLE_APB_MUDENS";
		};
		PARENTS = (R_MYAS_APB_MUDENS, R_MYOP_MYDY_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_LNLBE_MEDD2_DIFSLOW_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_DIFSLOW_WD;
			NAME = "R_LNLBE_MEDD2_DIFSLOW_WD";
		};
		PARENTS = (R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0);
		};
	};

	node R_LNLW_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_MUSIZE;
			NAME = "R_LNLW_APB_MUSIZE";
		};
		PARENTS = (R_LNLW_MED_TIME, R_LNLW_MED_SEV, R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.95, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.7, 0.25, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.7, 0.25, 0.05, 0, 0, 0, 0, 0.8, 0.2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.2, 0.7, 0.1, 0, 0, 0, 0.25, 0.7, 0.05, 0, 0, 0, 0.1, 0.6, 0.3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_LNLBE_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_DENERV;
			NAME = "R_LNLBE_APB_DENERV";
		};
		PARENTS = (R_LNLBE_MED_TIME, R_LNLBE_MED_SEV, R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.9, 0.1, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.5, 0.4, 0.1, 0, 0.5, 0.5, 0, 0, 0.6, 0.3, 0.1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0, 0.4, 0.4, 0.2, 0.3, 0.4, 0.3, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0, 0.4, 0.4, 0.2, 0.3, 0.4, 0.3, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.6, 0.25, 0.05, 0.5, 0.5, 0, 0, 0.45, 0.45, 0.1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05);
		};
	};

	node R_LNLBE_MEDD2_BLOCK_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_BLOCK_EW;
			NAME = "R_LNLBE_MEDD2_BLOCK_EW";
		};
		PARENTS = (R_LNLBE_MED_PATHO, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.3, 0.6, 0.1, 0, 0, 0, 0, 0.5, 0.5, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.1, 0.5, 0.3, 0.1, 0, 0, 0, 0.2, 0.6, 0.2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.05, 0.2, 0.55, 0.2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_MED_RDLDCV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_RDLDCV_EW;
			NAME = "R_MED_RDLDCV_EW";
		};
		PARENTS = (R_MED_LD_EW, R_MED_RD_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S27, M_S15, M_S07);
			PROBABILITIES = (0.90449, 0.0953095, 0.00020002, 0, 0, 0, 0.0115, 0.0333, 0.1509, 0.7319, 0.0724, 0, 0, 9.999e-05, 0.00109989, 0.146185, 0.807019, 0.0455954, 0.132, 0.6039, 0.2641, 0, 0, 0, 0.00340034, 0.0122012, 0.0690069, 0.719672, 0.19532, 0.00040004, 0, 0, 0.0002, 0.0581, 0.795, 0.1467, 0.0139, 0.1839, 0.8022, 0, 0, 0, 0.0011, 0.0045, 0.0299, 0.5742, 0.3876, 0.0027, 0, 0, 0.0001, 0.0228, 0.6344, 0.3427, 0.00120012, 0.00670067, 0.0544054, 0.860086, 0.0776078, 0, 0.0001, 0.0002, 0.0018, 0.0914, 0.6093, 0.2972, 0, 0, 0, 0.0014, 0.1063, 0.8923);
		};
	};

	node R_LNLW_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_MUDENS;
			NAME = "R_LNLW_APB_MUDENS";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_TIME, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.9, 0.1, 0, 0.9, 0.1, 0, 0.6, 0.4, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.7, 0.3, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.1, 0.6, 0.3, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.15, 0.7, 0.15, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.6, 0.4, 0, 0.6, 0.4, 0, 0.5, 0.4, 0.1, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.6, 0.4, 0, 0.6, 0.4, 0, 0.3, 0.6, 0.1, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3);
		};
	};

	node DIFFN_S_SEV_DIST
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_S_SEV_DIST;
			NAME = "DIFFN_S_SEV_DIST";
		};
		PARENTS = (DIFFN_SENS_SEV, DIFFN_DISTR);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0.25, 0.45, 0.25, 0.05, 0, 0, 1, 0, 0.5, 0.5, 0, 0, 0.1, 0.4, 0.4, 0.1, 0, 0, 0, 1, 0, 0.5, 0.5, 0, 0.05, 0.15, 0.4, 0.4);
		};
	};

	node R_LNLBE_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_APB_DE_REGEN;
			NAME = "R_LNLBE_APB_DE_REGEN";
		};
		PARENTS = (R_LNLBE_MED_PATHO, R_LNLBE_MED_TIME, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.8, 0.2, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1);
		};
	};

	node R_LNLBE_MEDD2_SALOSS_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_SALOSS_EW;
			NAME = "R_LNLBE_MEDD2_SALOSS_EW";
		};
		PARENTS = (R_LNLBE_MED_PATHO, R_LNLBE_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0.2, 0.5, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0.2, 0.5, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.4, 0.6, 0, 0.1, 0.4, 0.4, 0.1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_LNLT1_LP_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_MUSIZE;
			NAME = "R_LNLT1_LP_APB_MUSIZE";
		};
		PARENTS = (R_LNLT1_APB_MUSIZE, R_LNLLP_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MYOP_MYDY_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_MYDY_APB_MUSIZE;
			NAME = "R_MYOP_MYDY_APB_MUSIZE";
		};
		PARENTS = (R_MYOP_APB_MUSIZE, R_MYDY_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0.9983, 0.0017, 0, 0, 0, 0, 0.9857, 0.0143, 0, 0, 0, 0, 0.3673, 0.6298, 0.0029, 0, 0, 0, 0.0115012, 0.861686, 0.124912, 0.00190019, 0, 0, 0, 0.1596, 0.7368, 0.1016, 0.002, 0, 0.9983, 0.0017, 0, 0, 0, 0, 0.8667, 0.1329, 0.0004, 0, 0, 0, 0.0139014, 0.963696, 0.0224022, 0, 0, 0, 0.0003, 0.3514, 0.6035, 0.0443, 0.0005, 0, 0, 0.0105, 0.5726, 0.3806, 0.0359, 0.0004, 0, 0, 0.0792, 0.4758, 0.4066, 0.0384, 0.9857, 0.0143, 0, 0, 0, 0, 0.0139014, 0.963696, 0.0224022, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.0406041, 0.927793, 0.0316032, 0, 0, 0, 0, 0.0319, 0.9362, 0.0319, 0, 0, 0, 0, 0.0329, 0.9671, 0.3673, 0.6298, 0.0029, 0, 0, 0, 0.0003, 0.3514, 0.6035, 0.0443, 0.0005, 0, 0, 0, 0.0406041, 0.927793, 0.0316032, 0, 0, 0, 0.00039996, 0.109889, 0.779822, 0.109889, 0, 0, 0, 0.00030003, 0.123412, 0.876288, 0, 0, 0, 0, 0.0028, 0.9972, 0.0115012, 0.861686, 0.124912, 0.00190019, 0, 0, 0, 0.0105, 0.5726, 0.3806, 0.0359, 0.0004, 0, 0, 0, 0.0319, 0.9362, 0.0319, 0, 0, 0, 0.00030003, 0.123412, 0.876288, 0, 0, 0, 0, 0.0028, 0.9972, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0.1596, 0.7368, 0.1016, 0.002, 0, 0, 0, 0.0792, 0.4758, 0.4066, 0.0384, 0, 0, 0, 0, 0.0329, 0.9671, 0, 0, 0, 0, 0.0028, 0.9972, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MYOP_MYDY_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MYOP_MYDY_APB_DENERV;
			NAME = "R_MYOP_MYDY_APB_DENERV";
		};
		PARENTS = (R_MYDY_APB_DENERV, R_MYOP_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_MEDD2_DISP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MEDD2_DISP;
			NAME = "R_DIFFN_MEDD2_DISP";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_S_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0.1, 0.5, 0.4, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node DIFFN_M_SEV_DIST
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_M_SEV_DIST;
			NAME = "DIFFN_M_SEV_DIST";
		};
		PARENTS = (DIFFN_MOT_SEV, DIFFN_DISTR);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0.25, 0.45, 0.25, 0.05, 0, 0, 1, 0, 0.5, 0.5, 0, 0, 0.1, 0.4, 0.4, 0.1, 0, 0, 0, 1, 0, 0.5, 0.5, 0, 0.05, 0.15, 0.4, 0.4);
		};
	};

	node R_LNLT1_LP_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_MUDENS;
			NAME = "R_LNLT1_LP_APB_MUDENS";
		};
		PARENTS = (R_LNLT1_APB_MUDENS, R_LNLLP_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_LNLW_MEDD2_RD_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_MEDD2_RD_WD;
			NAME = "R_LNLW_MEDD2_RD_WD";
		};
		PARENTS = (R_LNLW_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0);
		};
	};

	node R_LNLT1_LP_BE_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_DE_REGEN;
			NAME = "R_LNLT1_LP_BE_APB_DE_REGEN";
		};
		PARENTS = (R_LNLT1_LP_APB_DE_REGEN, R_LNLBE_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node R_MEDD2_RD_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_RD_WD;
			NAME = "R_MEDD2_RD_WD";
		};
		PARENTS = (R_LNLW_MEDD2_RD_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_LNLW_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLW_APB_DENERV;
			NAME = "R_LNLW_APB_DENERV";
		};
		PARENTS = (R_LNLW_MED_PATHO, R_LNLW_MED_TIME, R_LNLW_MED_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.9, 0.1, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.5, 0.4, 0.1, 0, 0.5, 0.5, 0, 0, 0.6, 0.3, 0.1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0, 0.4, 0.4, 0.2, 0.3, 0.4, 0.3, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0, 0.4, 0.4, 0.2, 0.3, 0.4, 0.3, 0, 0, 0, 0, 1, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.6, 0.25, 0.05, 0.5, 0.5, 0, 0, 0.45, 0.45, 0.1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05);
		};
	};

	node R_LNLBE_MED_DIFSLOW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MED_DIFSLOW;
			NAME = "R_LNLBE_MED_DIFSLOW";
		};
		PARENTS = (R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0);
		};
	};

	node R_LNLBE_MEDD2_RD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLBE_MEDD2_RD_EW;
			NAME = "R_LNLBE_MEDD2_RD_EW";
		};
		PARENTS = (R_LNLBE_MED_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0);
		};
	};

	node R_LNLT1_LP_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_APB_MALOSS;
			NAME = "R_LNLT1_LP_APB_MALOSS";
		};
		PARENTS = (R_LNLLP_APB_MALOSS, R_LNLT1_APB_MALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0022, 0.9977, 0.0001, 0, 0, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0, 0, 1, 0.0022, 0.9977, 0.0001, 0, 0, 0, 0.0282, 0.9718, 0, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0, 0, 1, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.01, 0.99, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0, 0, 1, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0.0004, 0.9996, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_MED_DIFSLOW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MED_DIFSLOW;
			NAME = "R_DIFFN_MED_DIFSLOW";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.5, 0.5, 0, 0.4, 0.5, 0.1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.2, 0.6, 0.2, 0.2, 0.4, 0.3, 0.1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.2, 0.6, 0.2, 0.2, 0.4, 0.3, 0.1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3);
		};
	};

	node R_MED_DIFSLOW_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_DIFSLOW_WA;
			NAME = "R_MED_DIFSLOW_WA";
		};
		PARENTS = (R_DIFFN_MED_DIFSLOW, R_LNLBE_MED_DIFSLOW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.0131987, 0.986201, 0.00059994, 0, 0, 0.0181018, 0.981898, 0, 0, 0.0003, 0.0252, 0.9745, 0.0131987, 0.986201, 0.00059994, 0, 0.0001, 0.0952, 0.9047, 0, 0, 0.0009, 0.588, 0.4111, 0, 0, 0.00440044, 0.9956, 0, 0.0181018, 0.981898, 0, 0, 0.0009, 0.588, 0.4111, 0, 0, 0.002, 0.998, 0, 0, 0.0006, 0.9994, 0, 0.0003, 0.0252, 0.9745, 0, 0, 0.00440044, 0.9956, 0, 0, 0.0006, 0.9994, 0, 0, 0.0005, 0.9995);
		};
	};

	node R_DIFFN_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_MALOSS;
			NAME = "R_DIFFN_APB_MALOSS";
		};
		PARENTS = (DIFFN_M_SEV_DIST, DIFFN_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.4, 0.6, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.4, 0.6, 0, 0, 0.4, 0.3, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.4, 0.6, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_DIFFN_MEDD2_BLOCK
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MEDD2_BLOCK;
			NAME = "R_DIFFN_MEDD2_BLOCK";
		};
		PARENTS = (DIFFN_S_SEV_DIST, DIFFN_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 0.2, 0.5, 0.3, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.6, 0.4, 0, 0, 0, 0, 0.2, 0.5, 0.3, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.4, 0.5, 0.1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_DIFFN_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_MUDENS;
			NAME = "R_DIFFN_APB_MUDENS";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_TIME, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.9, 0.1, 0, 0.9, 0.1, 0, 0.6, 0.4, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.7, 0.3, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.8, 0.2, 0, 0.8, 0.2, 0, 0.5, 0.5, 0, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.1, 0.6, 0.3, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.7, 0.3, 0, 0.7, 0.3, 0, 0.15, 0.7, 0.15, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0.6, 0.4, 0, 0.6, 0.4, 0, 0.5, 0.4, 0.1, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3, 0.6, 0.4, 0, 0.6, 0.4, 0, 0, 0.5, 0.5, 0.05, 0.5, 0.45, 0.2, 0.5, 0.3);
		};
	};

	node R_LNLT1_LP_BE_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_MUDENS;
			NAME = "R_LNLT1_LP_BE_APB_MUDENS";
		};
		PARENTS = (R_LNLT1_LP_APB_MUDENS, R_LNLBE_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_MEDD2_RD_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_RD_EW;
			NAME = "R_MEDD2_RD_EW";
		};
		PARENTS = (R_LNLBE_MEDD2_RD_EW);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_LNLW_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_MALOSS;
			NAME = "R_DIFFN_LNLW_APB_MALOSS";
		};
		PARENTS = (R_LNLW_APB_MALOSS, R_DIFFN_APB_MALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0022, 0.9977, 0.0001, 0, 0, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0, 0, 1, 0.0022, 0.9977, 0.0001, 0, 0, 0, 0.0282, 0.9718, 0, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0, 0, 1, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.01, 0.99, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0, 0, 1, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0.0004, 0.9996, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_LNLT1_LP_BE_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_NEUR_ACT;
			NAME = "R_LNLT1_LP_BE_APB_NEUR_ACT";
		};
		PARENTS = (R_LNLBE_APB_NEUR_ACT, R_LNLT1_LP_APB_NEUR_ACT);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node DIFFN_M_SEV_PROX
	{
		TYPE = CPT;
		HEADER =
		{
			ID = DIFFN_M_SEV_PROX;
			NAME = "DIFFN_M_SEV_PROX";
		};
		PARENTS = (DIFFN_DISTR, DIFFN_MOT_SEV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0.25, 0.45, 0.25, 0.05, 0, 0, 1, 0, 0, 0, 1, 0, 0.1, 0.4, 0.4, 0.1, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0.05, 0.15, 0.4, 0.4);
		};
	};

	node R_DIFFN_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_MUSIZE;
			NAME = "R_DIFFN_APB_MUSIZE";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_TIME, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 0.7, 0.2, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.8, 0.2, 0, 0, 0, 0.7, 0.2, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.5, 0.5, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.5, 0.5, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_DIFFN_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_DENERV;
			NAME = "R_DIFFN_APB_DENERV";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_TIME, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.9, 0.1, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.9, 0.1, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.3, 0.5, 0.2, 0, 0.6, 0.4, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 1, 0, 0, 0, 1, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0.8, 0.2, 0, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.1, 0.5, 0.4, 0, 0.4, 0.5, 0.1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05, 0.5, 0.4, 0.1, 0, 0.5, 0.5, 0, 0, 0.6, 0.3, 0.1, 0, 0, 0, 0.5, 0.5, 0.05, 0.4, 0.5, 0.05);
		};
	};

	node R_LNLT1_LP_BE_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_MUSIZE;
			NAME = "R_LNLT1_LP_BE_APB_MUSIZE";
		};
		PARENTS = (R_LNLBE_APB_MUSIZE, R_LNLT1_LP_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MED_RDLDDEL
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_RDLDDEL;
			NAME = "R_MED_RDLDDEL";
		};
		PARENTS = (R_MED_LD_WA, R_MED_RD_WA);
		DEFINITION =
		{
			NAMESTATES = (MS3_1, MS3_9, MS4_7, MS10_1, MS20_1);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0019, 0.5257, 0.4724, 0, 0, 0.0002, 0.0304, 0.9694, 0, 0, 0.0964, 0.7981, 0.1055, 0, 0, 0.00019998, 0.0414959, 0.958304, 0, 0, 0.0001, 0.0142, 0.9857, 0, 0, 0.0032, 0.127, 0.8698, 0, 0, 0.0001, 0.0144, 0.9855, 0, 0, 0, 0.009, 0.9808, 0.0102, 0, 0.00090009, 0.00280028, 0.0147015, 0.981598, 0, 0.00019998, 0.00059994, 0.00369963, 0.9955, 0, 0, 0.0002, 0.0012, 0.9984, 0.0002);
		};
	};

	node R_MEDD2_DISP_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DISP_EW;
			NAME = "R_MEDD2_DISP_EW";
		};
		PARENTS = (R_DIFFN_MEDD2_DISP, R_LNLBE_MEDD2_DISP_EW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.0749, 0.8229, 0.1022, 0, 0, 0.0626063, 0.937394, 0, 0, 0.00790079, 0.60286, 0.389239, 0.0749, 0.8229, 0.1022, 0, 0.0047, 0.1786, 0.8167, 0, 0, 0.019, 0.9795, 0.0015, 0, 0.0001, 0.0069, 0.993, 0, 0.0626063, 0.937394, 0, 0, 0.019, 0.9795, 0.0015, 0, 0.0001, 0.0833, 0.9166, 0, 0, 0, 1, 0, 0.00790079, 0.60286, 0.389239, 0, 0.0001, 0.0069, 0.993, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_DE_REGEN;
			NAME = "R_DIFFN_APB_DE_REGEN";
		};
		PARENTS = (DIFFN_M_SEV_DIST, DIFFN_PATHO, DIFFN_TIME);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.5, 0.5, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.2, 0.8, 0.2, 0.8, 0.2, 0.8, 0, 1, 0, 1, 0.8, 0.2, 0.8, 0.2, 0.8, 0.2, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.1, 0.9, 0, 1, 0, 1, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0, 1, 0, 1);
		};
	};

	node R_DIFFN_LNLW_MEDD2_DISP_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_MEDD2_DISP_WD;
			NAME = "R_DIFFN_LNLW_MEDD2_DISP_WD";
		};
		PARENTS = (R_LNLW_MEDD2_DISP_WD, R_DIFFN_MEDD2_DISP);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.0749, 0.8229, 0.1022, 0, 0, 0.0626063, 0.937394, 0, 0, 0, 0, 1, 0.0749, 0.8229, 0.1022, 0, 0.0047, 0.1786, 0.8167, 0, 0, 0.019, 0.9795, 0.0015, 0, 0, 0, 1, 0, 0.0626063, 0.937394, 0, 0, 0.019, 0.9795, 0.0015, 0, 0.0001, 0.0833, 0.9166, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_MED_BLOCK
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MED_BLOCK;
			NAME = "R_DIFFN_MED_BLOCK";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.6, 0.4, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.25, 0.5, 0.25, 0, 0, 0, 0, 0.4, 0.5, 0.1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0);
		};
	};

	node R_DIFFN_LNLW_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_DE_REGEN;
			NAME = "R_DIFFN_LNLW_APB_DE_REGEN";
		};
		PARENTS = (R_DIFFN_APB_DE_REGEN, R_LNLW_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node R_DIFFN_MEDD2_DIFSLOW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MEDD2_DIFSLOW;
			NAME = "R_DIFFN_MEDD2_DIFSLOW";
		};
		PARENTS = (DIFFN_PATHO, DIFFN_S_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.2, 0.6, 0.2, 0.2, 0.4, 0.3, 0.1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3, 0, 0.1, 0.6, 0.3, 0.2, 0.4, 0.3, 0.1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.7, 0.3);
		};
	};

	node R_LNLT1_LP_BE_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_MALOSS;
			NAME = "R_LNLT1_LP_BE_APB_MALOSS";
		};
		PARENTS = (R_LNLBE_APB_MALOSS, R_LNLT1_LP_APB_MALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0022, 0.9977, 0.0001, 0, 0, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0, 0, 1, 0.0022, 0.9977, 0.0001, 0, 0, 0, 0.0282, 0.9718, 0, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0, 0, 1, 0.00019998, 0.0368963, 0.958804, 0.00409959, 0, 0, 0.0009, 0.3409, 0.6582, 0, 0, 0, 0.01, 0.99, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0, 0, 1, 0, 0.0002, 0.0329, 0.9669, 0, 0, 0, 0.0038, 0.9962, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0.0004, 0.9996, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_LNLT1_LP_BE_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNLT1_LP_BE_APB_DENERV;
			NAME = "R_LNLT1_LP_BE_APB_DENERV";
		};
		PARENTS = (R_LNLBE_APB_DENERV, R_LNLT1_LP_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_APB_MALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MALOSS;
			NAME = "R_APB_MALOSS";
		};
		PARENTS = (R_DIFFN_LNLW_APB_MALOSS, R_LNLT1_LP_BE_APB_MALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL, OTHER);
			PROBABILITIES = (0.98, 0, 0, 0, 0, 0.02, 0.00219978, 0.977702, 9.999e-05, 0, 0, 0.019998, 0.0002, 0.0471, 0.9297, 0.003, 0, 0.02, 0, 0.0003, 0.0424, 0.9373, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0.00219978, 0.977702, 9.999e-05, 0, 0, 0.019998, 0, 0.0361, 0.9439, 0, 0, 0.02, 0, 0.0014, 0.3987, 0.5799, 0, 0.02, 0, 0, 0.005, 0.975, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0.0002, 0.0471, 0.9297, 0.003, 0, 0.02, 0, 0.0014, 0.3987, 0.5799, 0, 0.02, 0, 0, 0.013, 0.967, 0, 0.02, 0, 0, 0.0014, 0.9786, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0.0003, 0.0424, 0.9373, 0, 0.02, 0, 0, 0.005, 0.975, 0, 0.02, 0, 0, 0.0014, 0.9786, 0, 0.02, 0, 0, 0.0005, 0.9795, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02);
		};
	};

	node R_MEDD2_LSLOW_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_LSLOW_WD;
			NAME = "R_MEDD2_LSLOW_WD";
		};
		PARENTS = (R_MEDD2_LD_WD, R_MEDD2_RD_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, V_SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0021, 0.0042, 0.0295, 0.9642, 0, 0.0001, 0.0002, 0.0014, 0.083, 0.9153, 0.0185, 0.9561, 0.0254, 0, 0, 0.0012, 0.0025, 0.0194, 0.9769, 0, 0.0001, 0.0001, 0.0008, 0.0533, 0.9457, 0, 0.0166, 0.9834, 0, 0, 0.00069993, 0.00149985, 0.0119988, 0.985801, 0, 0, 0.0001, 0.0004, 0.0319, 0.9676, 0.0007, 0.002, 0.0219, 0.9754, 0, 0.00020002, 0.00050005, 0.00460046, 0.994499, 0.00020002, 0, 0, 0, 0.00350035, 0.9965);
		};
	};

	node R_MEDD2_DIFSLOW_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DIFSLOW_WD;
			NAME = "R_MEDD2_DIFSLOW_WD";
		};
		PARENTS = (R_DIFFN_MEDD2_DIFSLOW, R_LNLBE_MEDD2_DIFSLOW_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.0127, 0.9867, 0.0006, 0, 0, 0.0181018, 0.981898, 0, 0, 0.0006, 0.0492, 0.9502, 0.0127, 0.9867, 0.0006, 0, 0.0001, 0.0952, 0.9047, 0, 0, 0.0011, 0.7402, 0.2587, 0, 0, 0.00880088, 0.991199, 0, 0.0181018, 0.981898, 0, 0, 0.0011, 0.7402, 0.2587, 0, 0, 0.004, 0.996, 0, 0, 0.0012, 0.9988, 0, 0.0006, 0.0492, 0.9502, 0, 0, 0.00880088, 0.991199, 0, 0, 0.0012, 0.9988, 0, 0, 0.0009, 0.9991);
		};
	};

	node R_DIFFN_MEDD2_SALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_MEDD2_SALOSS;
			NAME = "R_DIFFN_MEDD2_SALOSS";
		};
		PARENTS = (DIFFN_S_SEV_DIST, DIFFN_PATHO);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0.4, 0.3, 0.3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.7, 0.3, 0, 0.3, 0.5, 0.2, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0);
		};
	};

	node R_DIFFN_LNLW_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_MUSIZE;
			NAME = "R_DIFFN_LNLW_APB_MUSIZE";
		};
		PARENTS = (R_DIFFN_APB_MUSIZE, R_LNLW_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9981, 0.0019, 0, 0, 0, 0, 0.0019, 0.9981, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.0019, 0.9981, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_LNLW_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_MUDENS;
			NAME = "R_DIFFN_LNLW_APB_MUDENS";
		};
		PARENTS = (R_DIFFN_APB_MUDENS, R_LNLW_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_MED_DIFSLOW_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_DIFSLOW_EW;
			NAME = "R_MED_DIFSLOW_EW";
		};
		PARENTS = (R_DIFFN_MED_DIFSLOW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.0126, 0.9869, 0.0005, 0, 0, 0.0179, 0.9821, 0, 0, 0.0003, 0.0252, 0.9745);
		};
	};

	node R_LNL_DIFFN_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNL_DIFFN_APB_DE_REGEN;
			NAME = "R_LNL_DIFFN_APB_DE_REGEN";
		};
		PARENTS = (R_DIFFN_LNLW_APB_DE_REGEN, R_LNLT1_LP_BE_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node R_MEDD2_DISP_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DISP_WD;
			NAME = "R_MEDD2_DISP_WD";
		};
		PARENTS = (R_DIFFN_LNLW_MEDD2_DISP_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_APB_NEUR_ACT;
			NAME = "R_DIFFN_APB_NEUR_ACT";
		};
		PARENTS = (DIFFN_TIME, DIFFN_M_SEV_DIST);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.9, 0.1, 0, 0, 0, 0, 0.7, 0.3, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0, 0.3, 0.7, 0, 0, 0, 0);
		};
	};

	node R_MED_BLOCK_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_BLOCK_EW;
			NAME = "R_MED_BLOCK_EW";
		};
		PARENTS = (R_DIFFN_MED_BLOCK, R_LNLBE_MED_BLOCK);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0077, 0.9923, 0, 0, 0, 0.0007, 0.0234, 0.9759, 0, 0, 0.0019, 0.006, 0.0588, 0.9333, 0, 0, 0, 0, 0, 1, 0.0077, 0.9923, 0, 0, 0, 0.0001, 0.498, 0.5019, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0, 0, 0, 0, 1, 0.0007, 0.0234, 0.9759, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0, 0.0007, 0.4328, 0.5665, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0, 0, 0, 0, 1, 0.0019, 0.006, 0.0588, 0.9333, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0.00030003, 0.00110011, 0.0125013, 0.986099, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_LNLW_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_NEUR_ACT;
			NAME = "R_DIFFN_LNLW_APB_NEUR_ACT";
		};
		PARENTS = (R_LNLW_APB_NEUR_ACT, R_DIFFN_APB_NEUR_ACT);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_LSLOW_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_LSLOW_EW;
			NAME = "R_MEDD2_LSLOW_EW";
		};
		PARENTS = (R_MEDD2_LD_EW, R_MEDD2_RD_EW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, V_SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.00840084, 0.0119012, 0.0619062, 0.917392, 0.00040004, 0.00529947, 0.00619938, 0.0263974, 0.208179, 0.753925, 0.0185, 0.9561, 0.0254, 0, 0, 0.00690069, 0.010001, 0.0535054, 0.928693, 0.00090009, 0.0049, 0.0057, 0.0244, 0.1966, 0.7684, 0, 0.0166, 0.9834, 0, 0, 0.00559944, 0.00829917, 0.0451955, 0.938906, 0.0019998, 0.00440044, 0.00520052, 0.0224022, 0.183918, 0.784078, 0.0007, 0.002, 0.0219, 0.9754, 0, 0.0023, 0.0036, 0.0217, 0.8326, 0.1398, 0.0028, 0.0033, 0.0145, 0.1304, 0.849);
		};
	};

	node R_MEDD2_BLOCK_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_BLOCK_EW;
			NAME = "R_MEDD2_BLOCK_EW";
		};
		PARENTS = (R_DIFFN_MEDD2_BLOCK, R_LNLBE_MEDD2_BLOCK_EW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0077, 0.9923, 0, 0, 0, 0.0007, 0.0234, 0.9759, 0, 0, 0.0019, 0.006, 0.0588, 0.9333, 0, 0, 0, 0, 0, 1, 0.0077, 0.9923, 0, 0, 0, 0.0001, 0.498, 0.5019, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0, 0, 0, 0, 1, 0.0007, 0.0234, 0.9759, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0, 0.0007, 0.4328, 0.5665, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0, 0, 0, 0, 1, 0.0019, 0.006, 0.0588, 0.9333, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0.00030003, 0.00110011, 0.0125013, 0.986099, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_DE_REGEN
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_DE_REGEN;
			NAME = "R_APB_DE_REGEN";
		};
		PARENTS = (R_MYOP_MYDY_APB_DE_REGEN, R_LNL_DIFFN_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (1, 0, 0, 1, 0, 1, 0, 1);
		};
	};

	node R_MED_DCV_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_DCV_WA;
			NAME = "R_MED_DCV_WA";
		};
		PARENTS = (R_APB_MALOSS, R_MED_DIFSLOW_WA);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0.008, 0.168, 0.7407, 0.0833, 0, 0, 0, 0, 0, 0.00069986, 0.00589882, 0.0614877, 0.30064, 0.552589, 0.0781844, 0.0004999, 0, 0, 0.0001, 0.0003, 0.0018, 0.011, 0.067, 0.2945, 0.5047, 0.1206, 0, 0.1136, 0.8864, 0, 0, 0, 0, 0, 0, 0, 0.00089991, 0.0367963, 0.557644, 0.40246, 0.00219978, 0, 0, 0, 0, 0.0002, 0.0018, 0.0263, 0.1887, 0.5884, 0.1916, 0.003, 0, 0, 0, 0.00010001, 0.00090009, 0.00590059, 0.0422042, 0.235624, 0.522552, 0.192719, 0, 0.0006, 0.0764, 0.8866, 0.0364, 0, 0, 0, 0, 0, 0, 0.00070007, 0.0525053, 0.571257, 0.375238, 0.00030003, 0, 0, 0, 0, 0.0001, 0.0024, 0.0358, 0.316, 0.5741, 0.0716, 0, 0, 0, 0, 0.0001, 0.0012, 0.0121, 0.1131, 0.4415, 0.432, 0, 0, 0, 0.0655, 0.9299, 0.0046, 0, 0, 0, 0, 0, 0, 0.0007, 0.0745, 0.8859, 0.0389, 0, 0, 0, 0, 0, 9.999e-05, 0.00319968, 0.0780922, 0.594641, 0.323568, 0.00039996, 0, 0, 0, 0, 0.00020002, 0.00280028, 0.0439044, 0.291729, 0.661366, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.1523, 0.2904, 0.3678, 0.1726, 0.0169, 0, 0, 0, 0, 0.0168, 0.0618, 0.2223, 0.4015, 0.2803, 0.0172, 0.0001, 0, 0, 0.0009999, 0.00469953, 0.0279972, 0.118388, 0.364664, 0.392261, 0.0906909, 0.00029997, 0, 0, 0.0002, 0.0011, 0.0057, 0.0332, 0.1704, 0.4424, 0.347, 0);
		};
	};

	node R_LNL_DIFFN_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNL_DIFFN_APB_MUDENS;
			NAME = "R_LNL_DIFFN_APB_MUDENS";
		};
		PARENTS = (R_DIFFN_LNLW_APB_MUDENS, R_LNLT1_LP_BE_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_MEDD2_BLOCK_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_BLOCK_WD;
			NAME = "R_MEDD2_BLOCK_WD";
		};
		PARENTS = (R_DIFFN_MEDD2_BLOCK, R_LNLW_MEDD2_BLOCK_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0077, 0.9923, 0, 0, 0, 0.0007, 0.0234, 0.9759, 0, 0, 0.0019, 0.006, 0.0588, 0.9333, 0, 0, 0, 0, 0, 1, 0.0077, 0.9923, 0, 0, 0, 0.0001, 0.498, 0.5019, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0, 0, 0, 0, 1, 0.0007, 0.0234, 0.9759, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0, 0.0007, 0.4328, 0.5665, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0, 0, 0, 0, 1, 0.0019, 0.006, 0.0588, 0.9333, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0.00030003, 0.00110011, 0.0125013, 0.986099, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_DE_REGEN_APB_NMT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DE_REGEN_APB_NMT;
			NAME = "R_DE_REGEN_APB_NMT";
		};
		PARENTS = (R_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD_PRE, SEV_PRE, MLD_POST, MOD_POST, SEV_POST, MIXED);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0.65, 0.02, 0.01, 0.27, 0.02, 0.01, 0.02);
		};
	};

	node R_MED_DCV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_DCV_EW;
			NAME = "R_MED_DCV_EW";
		};
		PARENTS = (R_APB_MALOSS, R_MED_DIFSLOW_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S56, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00410041, 0.0247025, 0.154615, 0.738974, 0.0776078, 0, 0, 0, 0, 0, 0.0004, 0.0012, 0.0055, 0.0628, 0.295, 0.5485, 0.0861, 0.0005, 0, 0, 0, 9.999e-05, 0.00019998, 0.00189981, 0.0106989, 0.0657934, 0.284572, 0.505449, 0.131287, 0, 0.109, 0.8903, 0.0007, 0, 0, 0, 0, 0, 0, 0, 0.0011, 0.0082, 0.0683, 0.7087, 0.2135, 0.0002, 0, 0, 0, 0, 0.00020002, 0.00050005, 0.00280028, 0.0389039, 0.230923, 0.582958, 0.142214, 0.00150015, 0, 0, 0, 0, 0.00010001, 0.00120012, 0.00750075, 0.0510051, 0.252425, 0.518552, 0.169217, 0, 0.0039996, 0.114389, 0.862114, 0.0194981, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.00079992, 0.0089991, 0.30297, 0.665433, 0.0216978, 0, 0, 0, 0, 0, 0.00010001, 0.00060006, 0.0123012, 0.112911, 0.527253, 0.335534, 0.0113011, 0, 0, 0, 0, 0, 0.0005, 0.0034, 0.028, 0.1822, 0.5098, 0.2761, 0, 0.0001, 0.0028, 0.064, 0.9243, 0.0088, 0, 0, 0, 0, 0, 0, 0, 0.0006, 0.0547, 0.6199, 0.3247, 0.0001, 0, 0, 0, 0, 0, 9.999e-05, 0.00249975, 0.0359964, 0.324068, 0.571043, 0.0662934, 0, 0, 0, 0, 0, 0.00010001, 0.00120012, 0.0125013, 0.111811, 0.445245, 0.429143, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0835, 0.1153, 0.2417, 0.3746, 0.1682, 0.0167, 0, 0, 0, 0, 0.00929907, 0.0180982, 0.0545945, 0.227677, 0.393361, 0.277572, 0.0192981, 9.999e-05, 0, 0, 0.00059994, 0.00119988, 0.00419958, 0.0283972, 0.114889, 0.357564, 0.39636, 0.0963904, 0.00039996, 0, 0, 0, 0.0002, 0.0011, 0.0056, 0.033, 0.1653, 0.4414, 0.3534, 0);
		};
	};

	node R_APB_NMT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_NMT;
			NAME = "R_APB_NMT";
		};
		PARENTS = (R_MYAS_APB_NMT, R_DE_REGEN_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD_PRE, SEV_PRE, MLD_POST, MOD_POST, SEV_POST, MIXED);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_DIFFN_LNLW_MEDD2_SALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_MEDD2_SALOSS;
			NAME = "R_DIFFN_LNLW_MEDD2_SALOSS";
		};
		PARENTS = (R_DIFFN_MEDD2_SALOSS, R_LNLW_MEDD2_SALOSS_WD);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0073, 0.9812, 0.0115, 0, 0, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0, 0, 1, 0.0073, 0.9812, 0.0115, 0, 0, 0, 0.0289, 0.9711, 0, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0, 0, 1, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0.00049995, 0.0330967, 0.966403, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0, 0, 1, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_MED_AMPR_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_AMPR_EW;
			NAME = "R_MED_AMPR_EW";
		};
		PARENTS = (R_MED_BLOCK_EW);
		DEFINITION =
		{
			NAMESTATES = (R_1_1, R1_0, R0_9, R0_8, R0_7, R0_6, R0_5, R0_4, R0_3, R0_2, R0_1, R0_0);
			PROBABILITIES = (0.0879, 0.4192, 0.4232, 0.0693, 0.0004, 0, 0, 0, 0, 0, 0, 0, 0.00189981, 0.0343966, 0.256674, 0.529147, 0.173483, 0.00439956, 0, 0, 0, 0, 0, 0, 0.0001, 0.001, 0.0076, 0.0403, 0.172, 0.373, 0.342, 0.0633, 0.0007, 0, 0, 0, 0.00090009, 0.00150015, 0.00260026, 0.00490049, 0.00950095, 0.0176018, 0.0347035, 0.0768077, 0.166817, 0.341434, 0.343234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_SF_JITTER
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SF_JITTER;
			NAME = "R_APB_SF_JITTER";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, 2_5, 5_10, __10);
			PROBABILITIES = (0.95, 0.05, 0, 0, 0.02, 0.2, 0.7, 0.08, 0, 0.1, 0.4, 0.5, 0.05, 0.7, 0.2, 0.05, 0.01, 0.19, 0.7, 0.1, 0, 0.1, 0.4, 0.5, 0.1, 0.3, 0.3, 0.3);
		};
	};

	node R_DIFFN_LNLW_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_DIFFN_LNLW_APB_DENERV;
			NAME = "R_DIFFN_LNLW_APB_DENERV";
		};
		PARENTS = (R_LNLW_APB_DENERV, R_DIFFN_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_DISP_EWD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DISP_EWD;
			NAME = "R_MEDD2_DISP_EWD";
		};
		PARENTS = (R_MEDD2_DISP_WD, R_MEDD2_DISP_EW);
		DEFINITION =
		{
			NAMESTATES = (R0_15, R0_25, R0_35, R0_45, R0_55, R0_65, R0_75, R0_85, R0_95);
			PROBABILITIES = (0, 0, 0.0742, 0.9045, 0.0213, 0, 0, 0, 0, 0.0001, 0.2215, 0.7732, 0.0052, 0, 0, 0, 0, 0, 0.1315, 0.8577, 0.0108, 0, 0, 0, 0, 0, 0, 0.9933, 0.0067, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0742, 0.9045, 0.0213, 0, 0, 0, 0, 0, 0.131513, 0.857686, 0.0108011, 0, 0, 0, 0, 0, 0.0742, 0.9045, 0.0213, 0, 0, 0, 0, 0.0404, 0.9192, 0.0404, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.133, 0.867, 0, 0, 0, 0, 0, 0, 0.0108, 0.8577, 0.1315, 0, 0, 0, 0, 0, 0.0052, 0.7732, 0.2215, 0.0001, 0, 0, 0.0213, 0.9045, 0.0742, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.133, 0.867, 0, 0, 0, 0, 0, 0, 0.0108, 0.8577, 0.1315, 0, 0, 0, 0, 0, 0.0052, 0.7732, 0.2215, 0.0001, 0, 0, 0.0213, 0.9045, 0.0742, 0, 0, 0, 0);
		};
	};

	node R_APB_MUPSATEL
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUPSATEL;
			NAME = "R_APB_MUPSATEL";
		};
		PARENTS = (R_APB_DE_REGEN);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (0.95, 0.05, 0.2, 0.8);
		};
	};

	node R_APB_REPSTIM_DECR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_REPSTIM_DECR;
			NAME = "R_APB_REPSTIM_DECR";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, INCON);
			PROBABILITIES = (0.949, 0.02, 0.01, 0.001, 0.02, 0.04, 0.2, 0.7, 0.04, 0.02, 0.001, 0.01, 0.04, 0.929, 0.02, 0.35, 0.57, 0.05, 0.01, 0.02, 0.02, 0.1, 0.8, 0.06, 0.02, 0.001, 0.01, 0.04, 0.929, 0.02, 0.245, 0.245, 0.245, 0.245, 0.02);
		};
	};

	node R_MED_BLOCK_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_BLOCK_WA;
			NAME = "R_MED_BLOCK_WA";
		};
		PARENTS = (R_LNLW_MED_BLOCK, R_DIFFN_MED_BLOCK);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0077, 0.9923, 0, 0, 0, 0.0007, 0.0234, 0.9759, 0, 0, 0.0019, 0.006, 0.0588, 0.9333, 0, 0, 0, 0, 0, 1, 0.0077, 0.9923, 0, 0, 0, 0.0001, 0.498, 0.5019, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0, 0, 0, 0, 1, 0.0007, 0.0234, 0.9759, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0, 0.0007, 0.4328, 0.5665, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0, 0, 0, 0, 1, 0.0019, 0.006, 0.0588, 0.9333, 0, 0.001, 0.0033, 0.0376, 0.9581, 0, 0.0003, 0.0011, 0.0159, 0.9827, 0, 0.00030003, 0.00110011, 0.0125013, 0.986099, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_DIFSLOW_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DIFSLOW_EW;
			NAME = "R_MEDD2_DIFSLOW_EW";
		};
		PARENTS = (R_DIFFN_MEDD2_DIFSLOW);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_LNL_DIFFN_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNL_DIFFN_APB_MUSIZE;
			NAME = "R_LNL_DIFFN_APB_MUSIZE";
		};
		PARENTS = (R_LNLT1_LP_BE_APB_MUSIZE, R_DIFFN_LNLW_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.9981, 0.0019, 0, 0, 0, 0, 0.0019, 0.9981, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.0019, 0.9981, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0.9994, 0.00050005, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_MUDENS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUDENS;
			NAME = "R_APB_MUDENS";
		};
		PARENTS = (R_LNL_DIFFN_APB_MUDENS, R_MUSCLE_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR, V_INCR);
			PROBABILITIES = (1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1);
		};
	};

	node R_NMT_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_NMT_APB_DENERV;
			NAME = "R_NMT_APB_DENERV";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0.4, 0.45, 0.15, 0, 0.15, 0.35, 0.35, 0.15, 0.85, 0.15, 0, 0, 0.3, 0.45, 0.2, 0.05, 0.15, 0.35, 0.35, 0.15, 0.25, 0.25, 0.25, 0.25);
		};
	};

	node R_MUSCLE_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MUSCLE_APB_DENERV;
			NAME = "R_MUSCLE_APB_DENERV";
		};
		PARENTS = (R_MYOP_MYDY_APB_DENERV, R_NMT_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_APB_MUPINSTAB
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUPINSTAB;
			NAME = "R_APB_MUPINSTAB";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (0.95, 0.05, 0.1, 0.9, 0.03, 0.97, 0.2, 0.8, 0.1, 0.9, 0.03, 0.97, 0.1, 0.9);
		};
	};

	node R_MED_ALLDEL_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_ALLDEL_WA;
			NAME = "R_MED_ALLDEL_WA";
		};
		PARENTS = (R_MED_DCV_WA, R_MED_RDLDDEL);
		DEFINITION =
		{
			NAMESTATES = (MS0_0, MS0_4, MS0_8, MS1_6, MS3_2, MS6_4, MS12_8, MS25_6, INFIN);
			PROBABILITIES = (0.9996, 0.0004, 0, 0, 0, 0, 0, 0, 0, 0.5607, 0.4393, 0, 0, 0, 0, 0, 0, 0, 0.0069, 0.7963, 0.1968, 0, 0, 0, 0, 0, 0, 0, 0.0184, 0.9806, 0.001, 0, 0, 0, 0, 0, 0, 0.0001, 0.0179, 0.982, 0, 0, 0, 0, 0, 0, 0.0002, 0.003, 0.1393, 0.8575, 0, 0, 0, 0, 0, 0, 0, 0.0006, 0.8145, 0.1849, 0, 0, 0, 0, 0, 0, 0.00040004, 0.0113011, 0.59736, 0.390939, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0511949, 0.229077, 0.566243, 0.153485, 0, 0, 0, 0, 0, 0.0192981, 0.128687, 0.492851, 0.359164, 0, 0, 0, 0, 0, 0.0027, 0.0328, 0.2398, 0.7246, 0.0001, 0, 0, 0, 0, 0.00010001, 0.00330033, 0.0547055, 0.938194, 0.00370037, 0, 0, 0, 0, 0, 0.00019998, 0.00479952, 0.50395, 0.491051, 0, 0, 0, 0, 0, 0, 0.0003, 0.0188, 0.9804, 0.0005, 0, 0, 0, 0, 0, 0, 0, 0.0494, 0.9506, 0, 0, 0, 0, 0, 0, 0.00010001, 0.00340034, 0.29903, 0.69747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0001, 0.0035, 0.0632, 0.9326, 0.0006, 0, 0, 0, 0, 0, 0.0011, 0.0283, 0.9651, 0.0055, 0, 0, 0, 0, 0, 0.0002, 0.0075, 0.8889, 0.1034, 0, 0, 0, 0, 0, 0, 0.00030003, 0.185019, 0.814681, 0, 0, 0, 0, 0, 0, 0, 0.0032, 0.9968, 0, 0, 0, 0, 0, 0, 0, 0.00150015, 0.931393, 0.0671067, 0, 0, 0, 0, 0, 0, 0, 0.001, 0.999, 0, 0, 0, 0, 0, 0, 0, 0.0007, 0.1112, 0.8881, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.00129987, 0.0019998, 0.00439956, 0.0186981, 0.121288, 0.747925, 0.10439, 0, 0, 0.0010001, 0.00160016, 0.00370037, 0.0161016, 0.109311, 0.742174, 0.126113, 0, 0, 0.00079992, 0.00119988, 0.00279972, 0.0124988, 0.0915908, 0.723528, 0.167583, 0, 0, 0.00049995, 0.00079992, 0.00179982, 0.00869913, 0.070293, 0.680232, 0.237676, 0, 0, 0.0002, 0.0004, 0.0009, 0.0047, 0.044, 0.5737, 0.3761, 0, 0, 0.0001, 0.0001, 0.0002, 0.0013, 0.015, 0.3152, 0.6681, 0, 0, 0, 0, 0, 0.0001, 0.0017, 0.0786, 0.9196, 0, 0, 0, 0, 0, 0, 0.00010001, 0.00940094, 0.959396, 0.0311031, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.00010001, 0.00010001, 0.00030003, 0.00090009, 0.00450045, 0.0434043, 0.576758, 0.373937, 0, 0.0001, 0.0001, 0.0002, 0.0008, 0.0041, 0.0399, 0.5568, 0.398, 0, 0.0001, 0.0001, 0.0002, 0.0007, 0.0034, 0.0348, 0.5239, 0.4368, 0, 0.0001, 0.0001, 0.0001, 0.0005, 0.0027, 0.0287, 0.4783, 0.4895, 0, 0, 0, 0.00010001, 0.00030003, 0.00180018, 0.0210021, 0.407241, 0.569557, 0, 0, 0, 0, 0.00010002, 0.00080016, 0.0111022, 0.280456, 0.707542, 0, 0, 0, 0, 0, 0.0002, 0.0035, 0.138, 0.8583, 0, 0, 0, 0, 0, 0, 0.00030003, 0.0253025, 0.974397, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_REPSTIM_POST_DECR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_REPSTIM_POST_DECR;
			NAME = "R_APB_REPSTIM_POST_DECR";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, INCON);
			PROBABILITIES = (0.949, 0.02, 0.01, 0.001, 0.02, 0.02, 0.1, 0.8, 0.06, 0.02, 0.001, 0.01, 0.02, 0.949, 0.02, 0.25, 0.61, 0.1, 0.02, 0.02, 0.01, 0.1, 0.8, 0.07, 0.02, 0.001, 0.01, 0.02, 0.949, 0.02, 0.23, 0.23, 0.22, 0.22, 0.1);
		};
	};

	node R_MEDD2_AMPR_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_AMPR_EW;
			NAME = "R_MEDD2_AMPR_EW";
		};
		PARENTS = (R_MEDD2_BLOCK_EW, R_MEDD2_DISP_EWD);
		DEFINITION =
		{
			NAMESTATES = (R0_0, R0_1, R0_2, R0_3, R0_4, R0_5, R0_6, R0_7, R0_8, R0_9, R1_0, R_1_1);
			PROBABILITIES = (0, 0.282672, 0.716428, 0.00089991, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9103, 0.0897, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9974, 0.0026, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.946705, 0.0491951, 0.00349965, 0.00049995, 9.999e-05, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5547, 0.4268, 0.0183, 0.0002, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.8945, 0.1036, 0.0015, 0, 0, 0, 0, 0, 0, 0, 0, 0.3856, 0.6048, 0.0095, 0.0001, 0, 0, 0, 0, 0, 0, 0, 0, 0.872113, 0.110989, 0.0134987, 0.00249975, 0.00059994, 0.00019998, 9.999e-05, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0090009, 0.512751, 0.415242, 0.0589059, 0.00390039, 0.00020002, 0, 0, 0, 0, 0, 0, 0.114011, 0.716972, 0.159816, 0.00890089, 0.00030003, 0, 0, 0, 0, 0, 0, 0.0073, 0.8191, 0.1638, 0.0095, 0.0003, 0, 0, 0, 0, 0, 0, 0, 0.778256, 0.180136, 0.0312062, 0.0075015, 0.0020004, 0.00060012, 0.00020004, 0.00010002, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0635, 0.4459, 0.3732, 0.0995, 0.0162, 0.0015, 0.0002, 0, 0, 0, 0, 0.0026, 0.3111, 0.5155, 0.1499, 0.019, 0.0018, 0.0001, 0, 0, 0, 0, 0.0001, 0.386, 0.4993, 0.1036, 0.0101, 0.0008, 0.0001, 0, 0, 0, 0, 0, 0.678, 0.2436, 0.0548, 0.0156, 0.005, 0.0018, 0.0007, 0.0003, 0.0001, 0.0001, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00290029, 0.114111, 0.39514, 0.319832, 0.131513, 0.029803, 0.00580058, 0.00090009, 0, 0, 0, 0, 0.0451, 0.3717, 0.4039, 0.1433, 0.0313, 0.0041, 0.0005, 0.0001, 0, 0, 0, 0.0913, 0.5259, 0.3027, 0.0681, 0.0104, 0.0014, 0.0002, 0, 0, 0, 0, 0.5789, 0.2957, 0.082, 0.027, 0.0096, 0.0037, 0.0017, 0.0007, 0.0004, 0.0002, 0.0001, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0136, 0.1578, 0.3285, 0.2966, 0.1404, 0.0483, 0.0135, 0.0012, 0, 0, 0, 0.0035, 0.1122, 0.3738, 0.3186, 0.1444, 0.0376, 0.0083, 0.0015, 0.0001, 0, 0, 0.0149985, 0.306869, 0.420358, 0.192881, 0.0511949, 0.0113989, 0.00189981, 0.00029997, 9.999e-05, 0, 0, 0.485, 0.334, 0.1106, 0.0411, 0.0162, 0.0068, 0.0033, 0.0015, 0.0008, 0.0004, 0.0003, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00120024, 0.0385077, 0.174635, 0.30156, 0.261352, 0.144729, 0.065113, 0.0129026, 0, 0, 0, 0.00020002, 0.0226023, 0.189019, 0.331733, 0.274427, 0.125213, 0.0431043, 0.0124012, 0.00130013, 0, 0, 0.0023, 0.1333, 0.3728, 0.3075, 0.1292, 0.0421, 0.0099, 0.0024, 0.0005, 0, 0, 0.40484, 0.356636, 0.136714, 0.0562056, 0.0240024, 0.0108011, 0.00540054, 0.00270027, 0.00140014, 0.00080008, 0.00050005, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0062, 0.058, 0.1828, 0.2779, 0.2392, 0.1675, 0.0683, 0, 0, 0, 0, 0.00310031, 0.0615062, 0.210921, 0.305131, 0.234423, 0.121712, 0.0528053, 0.010401, 0, 0, 0.00030003, 0.0444044, 0.241324, 0.343134, 0.220822, 0.10251, 0.0337034, 0.010401, 0.0030003, 0.00040004, 0, 0.331634, 0.367127, 0.161268, 0.0726855, 0.0334933, 0.0159968, 0.0084983, 0.00439912, 0.00239952, 0.0014997, 0.0009998, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0009, 0.0157, 0.0823, 0.2012, 0.2516, 0.2559, 0.1924, 0, 0, 0, 0, 0.0004, 0.0166, 0.1002, 0.2325, 0.2777, 0.2039, 0.1251, 0.0436, 0, 0, 0, 0.0138, 0.1311, 0.2956, 0.2729, 0.1711, 0.0745, 0.0286, 0.0104, 0.002, 0, 0.2731, 0.3669, 0.1808, 0.0881, 0.0434, 0.0217, 0.012, 0.0064, 0.0036, 0.0023, 0.0017, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		};
	};

	node R_APB_NEUR_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_NEUR_ACT;
			NAME = "R_APB_NEUR_ACT";
		};
		PARENTS = (R_DIFFN_LNLW_APB_NEUR_ACT, R_LNLT1_LP_BE_APB_NEUR_ACT);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_SALOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_SALOSS;
			NAME = "R_MEDD2_SALOSS";
		};
		PARENTS = (R_LNLBE_MEDD2_SALOSS_EW, R_DIFFN_LNLW_MEDD2_SALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0073, 0.9812, 0.0115, 0, 0, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0, 0, 1, 0.0073, 0.9812, 0.0115, 0, 0, 0, 0.0289, 0.9711, 0, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0, 0, 1, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0.00049995, 0.0330967, 0.966403, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0, 0, 1, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_SF_DENSITY
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SF_DENSITY;
			NAME = "R_APB_SF_DENSITY";
		};
		PARENTS = (R_APB_MUDENS);
		DEFINITION =
		{
			NAMESTATES = (__2SD, 2_4SD, __4SD);
			PROBABILITIES = (0.97, 0.03, 0, 0.05, 0.9, 0.05, 0.01, 0.04, 0.95);
		};
	};

	node R_APB_MULOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MULOSS;
			NAME = "R_APB_MULOSS";
		};
		PARENTS = (R_MED_BLOCK_WA, R_APB_MALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL, OTHER);
			PROBABILITIES = (0.98, 0, 0, 0, 0, 0.02, 0.0167, 0.9613, 0.002, 0, 0, 0.02, 0.00019998, 0.0534947, 0.923708, 0.00259974, 0, 0.019998, 0, 0.00030003, 0.0481048, 0.931593, 0, 0.020002, 0, 0, 0, 0, 0.98, 0.02, 0.1427, 0.2958, 0.4254, 0.1161, 0, 0.02, 0.9746, 0.0054, 0, 0, 0, 0.02, 0.00340034, 0.952995, 0.0236024, 0, 0, 0.020002, 0, 0.0234023, 0.945095, 0.0115012, 0, 0.020002, 0, 0.00010001, 0.0270027, 0.952895, 0, 0.020002, 0, 0, 0, 0, 0.98, 0.02, 0.1157, 0.2677, 0.4444, 0.1522, 0, 0.02, 0.0664, 0.9136, 0, 0, 0, 0.02, 0.0002, 0.2725, 0.7073, 0, 0, 0.02, 0, 0.0048, 0.7523, 0.2229, 0, 0.02, 0, 0, 0.0091, 0.9709, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0.0693931, 0.20108, 0.452655, 0.256874, 0, 0.019998, 0.016, 0.1801, 0.7138, 0.0701, 0, 0.02, 0.0009, 0.0263, 0.4192, 0.5336, 0, 0.02, 0, 0.00130013, 0.0637064, 0.914991, 0, 0.020002, 0, 0.0001, 0.0087, 0.9712, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0.0173, 0.0696, 0.2854, 0.6077, 0, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02, 0, 0, 0, 0, 0.98, 0.02);
		};
	};

	node R_MED_LAT_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_LAT_WA;
			NAME = "R_MED_LAT_WA";
		};
		PARENTS = (R_MED_ALLDEL_WA);
		DEFINITION =
		{
			NAMESTATES = (MS2_3, MS2_7, MS3_1, MS3_5, MS3_9, MS4_3, MS4_7, MS5_3, MS5_9, MS6_5, MS7_1, MS8_0, MS9_0, MS10_0, MS12_0, MS14_0, MS16_0, MS18_0, INFIN);
			PROBABILITIES = (0.00590059, 0.132613, 0.50325, 0.322632, 0.0350035, 0.00060006, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00069993, 0.019598, 0.168983, 0.425457, 0.312769, 0.0671933, 0.00529947, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0007, 0.0194, 0.1669, 0.4202, 0.3089, 0.0829, 0.001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0010001, 0.0109011, 0.0635064, 0.194719, 0.393439, 0.292129, 0.0428043, 0.00150015, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0003, 0.0011, 0.0034, 0.009, 0.021, 0.0528, 0.137, 0.2128, 0.2375, 0.1826, 0.1229, 0.0179, 0.0016, 0, 0, 0, 0, 0, 9.999e-05, 0.00019998, 0.00039996, 0.00079992, 0.00139986, 0.00249975, 0.00509949, 0.0119988, 0.0214979, 0.0353965, 0.0891911, 0.139786, 0.182882, 0.281172, 0.188981, 0.0358964, 0.00259974, 9.999e-05, 0, 0.0002, 0.0002, 0.0003, 0.0004, 0.0005, 0.0007, 0.0012, 0.0022, 0.0032, 0.0047, 0.0109, 0.0176, 0.028, 0.0629, 0.1563, 0.2269, 0.2569, 0.2269, 0, 0.00079992, 0.00089991, 0.00109989, 0.00119988, 0.00139986, 0.00169983, 0.00249975, 0.00369963, 0.00459954, 0.00559944, 0.0105989, 0.0155984, 0.0213979, 0.0432957, 0.10069, 0.164884, 0.253575, 0.366463, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_REPSTIM_FACILI
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_REPSTIM_FACILI;
			NAME = "R_APB_REPSTIM_FACILI";
		};
		PARENTS = (R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (NO, MOD, SEV, REDUCED);
			PROBABILITIES = (0.95, 0.02, 0.01, 0.02, 0.01, 0.889, 0.1, 0.001, 0.01, 0.08, 0.909, 0.001, 0.89, 0.08, 0.01, 0.02, 0.48, 0.5, 0.01, 0.01, 0.02, 0.949, 0.03, 0.001, 0.25, 0.25, 0.25, 0.25);
		};
	};

	node R_APB_MUSIZE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUSIZE;
			NAME = "R_APB_MUSIZE";
		};
		PARENTS = (R_MYOP_MYDY_APB_MUSIZE, R_LNL_DIFFN_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE, OTHER);
			PROBABILITIES = (0.9791, 0.0009, 0, 0, 0, 0, 0.02, 0.9637, 0.0163, 0, 0, 0, 0, 0.02, 0.922192, 0.0578058, 0, 0, 0, 0, 0.020002, 0.39794, 0.566357, 0.0155016, 0.00020002, 0, 0, 0.020002, 0.0435, 0.7319, 0.193, 0.0114, 0.0002, 0, 0.02, 0.00120012, 0.231723, 0.588259, 0.147415, 0.0112011, 0.00020002, 0.020002, 0.9637, 0.0163, 0, 0, 0, 0, 0.02, 0.7493, 0.2257, 0.0049, 0.0001, 0, 0, 0.02, 0.0537, 0.8568, 0.0684, 0.0011, 0, 0, 0.02, 0.00389961, 0.381062, 0.506549, 0.0840916, 0.00429957, 9.999e-05, 0.019998, 0, 0.0395, 0.5059, 0.355, 0.0758, 0.0038, 0.02, 0, 0.00110011, 0.135714, 0.40254, 0.363136, 0.0775078, 0.020002, 0.922192, 0.0578058, 0, 0, 0, 0, 0.020002, 0.0537, 0.8568, 0.0684, 0.0011, 0, 0, 0.02, 0, 0, 0.98, 0, 0, 0, 0.02, 0, 0, 0.0908091, 0.818582, 0.0706071, 0, 0.020002, 0, 0, 0.0001, 0.0721, 0.8357, 0.0721, 0.02, 0, 0, 0, 0.0001, 0.0778, 0.9021, 0.02, 0.39794, 0.566357, 0.0155016, 0.00020002, 0, 0, 0.020002, 0.00389961, 0.381062, 0.506549, 0.0840916, 0.00429957, 9.999e-05, 0.019998, 0, 0, 0.0908091, 0.818582, 0.0706071, 0, 0.020002, 0, 0, 0.00359964, 0.165483, 0.645435, 0.165483, 0.019998, 0, 0, 0, 0.0034, 0.1993, 0.7773, 0.02, 0, 0, 0, 0, 0.0162016, 0.963796, 0.020002, 0.0435, 0.7319, 0.193, 0.0114, 0.0002, 0, 0.02, 0, 0.0395, 0.5059, 0.355, 0.0758, 0.0038, 0.02, 0, 0, 0.0001, 0.0721, 0.8357, 0.0721, 0.02, 0, 0, 0, 0.0034, 0.1993, 0.7773, 0.02, 0, 0, 0, 0, 0.0162016, 0.963796, 0.020002, 0, 0, 0, 0, 0.0011, 0.9789, 0.02, 0.00120012, 0.231723, 0.588259, 0.147415, 0.0112011, 0.00020002, 0.020002, 0, 0.00110011, 0.135714, 0.40254, 0.363136, 0.0775078, 0.020002, 0, 0, 0, 0.0001, 0.0778, 0.9021, 0.02, 0, 0, 0, 0, 0.0162016, 0.963796, 0.020002, 0, 0, 0, 0, 0.0011, 0.9789, 0.02, 0, 0, 0, 0, 0.0001, 0.9799, 0.02);
		};
	};

	node R_MEDD2_EFFAXLOSS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_EFFAXLOSS;
			NAME = "R_MEDD2_EFFAXLOSS";
		};
		PARENTS = (R_MEDD2_BLOCK_WD, R_MEDD2_SALOSS);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV, TOTAL);
			PROBABILITIES = (1, 0, 0, 0, 0, 0.0073, 0.9812, 0.0115, 0, 0, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0, 0, 1, 0.0073, 0.9812, 0.0115, 0, 0, 0, 0.0289, 0.9711, 0, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0, 0, 1, 0.0017, 0.1306, 0.867, 0.0007, 0, 0, 0.0097, 0.5989, 0.3914, 0, 0, 0.00049995, 0.0330967, 0.966403, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0, 0, 1, 0, 0.0003, 0.0212, 0.9785, 0, 0, 0, 0.0017, 0.9983, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		};
	};

	node R_MED_ALLCV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_ALLCV_EW;
			NAME = "R_MED_ALLCV_EW";
		};
		PARENTS = (R_MED_DCV_EW, R_MED_RDLDCV_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S56, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00660066, 0.145515, 0.838084, 0.00980098, 0, 0, 0, 0, 0, 0, 0, 0, 0.0047, 0.9951, 0.0002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0012, 0.1305, 0.8445, 0.0238, 0, 0, 0, 0, 0, 0, 0, 0.0002, 0.0321, 0.9677, 0, 0.0699, 0.8102, 0.1199, 0, 0, 0, 0, 0, 0, 0, 0.0005, 0.0239, 0.4369, 0.5387, 0, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.9005, 0.0991, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.997, 0.003, 0, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.074, 0.8589, 0.0667, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0184, 0.9815, 0, 0, 0.0479048, 0.952095, 0, 0, 0, 0, 0, 0, 0, 0, 0.0003, 0.0239, 0.9748, 0.001, 0, 0, 0, 0, 0, 0, 0, 0, 0.1288, 0.8712, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2336, 0.7664, 0, 0, 0, 0, 0, 0, 0, 0, 0.00010001, 0.0373037, 0.80288, 0.159716, 0, 0, 0, 0, 0, 0, 0, 0, 0.010101, 0.989899, 0, 0.00089991, 0.0089991, 0.0887911, 0.818618, 0.0826917, 0, 0, 0, 0, 0, 0, 0.0002, 0.0036, 0.2467, 0.7339, 0.0156, 0, 0, 0, 0, 0, 0, 0, 0.0113011, 0.571157, 0.417542, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00259974, 0.993401, 0.0039996, 0, 0, 0, 0, 0, 0, 0, 0, 0.007, 0.3517, 0.6413, 0, 0, 0, 0, 0, 0, 0, 0, 0.0042, 0.9958, 0, 0, 0, 0.00050005, 0.10111, 0.847785, 0.0506051, 0, 0, 0, 0, 0, 0, 0, 0.005, 0.3134, 0.6811, 0.0005, 0, 0, 0, 0, 0, 0, 0, 0.0214, 0.9354, 0.0432, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2469, 0.7531, 0, 0, 0, 0, 0, 0, 0, 0, 0.0003, 0.0581, 0.9416, 0, 0, 0, 0, 0, 0, 0, 0, 0.0011, 0.9989, 0, 0, 0, 0, 0.00050005, 0.0791079, 0.90019, 0.020202, 0, 0, 0, 0, 0, 0, 0, 0.00710071, 0.549355, 0.443344, 0.00020002, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.0564944, 0.932307, 0.0110989, 0, 0, 0, 0, 0, 0, 0, 0, 0.002, 0.9939, 0.0041, 0, 0, 0, 0, 0, 0, 0, 0, 0.006, 0.994, 0, 0, 0, 0, 0, 0, 0, 0, 0.0003, 0.9997, 0, 0, 0, 0, 0, 0.0006, 0.0734, 0.8393, 0.0867, 0, 0, 0, 0, 0, 0, 0, 0.00910091, 0.528353, 0.462546, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.143, 0.8551, 0.0015, 0, 0, 0, 0, 0, 0, 0, 0, 0.0314, 0.9686, 0, 0, 0, 0, 0, 0, 0, 0, 0.0008, 0.9992, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0.00030003, 0.0947095, 0.894589, 0.010401, 0, 0, 0, 0, 0, 0, 0, 0.0216, 0.8359, 0.1425, 0, 0, 0, 0, 0, 0, 0, 0.0019998, 0.354865, 0.643136, 0, 0, 0, 0, 0, 0, 0, 0, 0.0002, 0.9998, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.002, 0.0932, 0.9048, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.0355, 0.9641, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0122, 0.9877, 0, 0, 0, 0, 0, 0, 0, 0, 0.0003, 0.9997, 0, 0, 0, 0, 0, 0, 0, 0, 0.0002, 0.9998, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.9999, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_SPONT_NEUR_DISCH
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SPONT_NEUR_DISCH;
			NAME = "R_APB_SPONT_NEUR_DISCH";
		};
		PARENTS = (R_APB_NEUR_ACT);
		DEFINITION =
		{
			NAMESTATES = (NO, FASCIC, NEUROMYO, MYOKYMIA, TETANUS, OTHER);
			PROBABILITIES = (0.98, 0.02, 0, 0, 0, 0, 0.1, 0.9, 0, 0, 0, 0, 0.01, 0.04, 0.75, 0.05, 0.05, 0.1, 0.01, 0.04, 0.05, 0.75, 0.05, 0.1, 0.01, 0.04, 0.05, 0.05, 0.75, 0.1, 0.01, 0.05, 0.05, 0.05, 0.05, 0.79);
		};
	};

	node R_APB_EFFMUS
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_EFFMUS;
			NAME = "R_APB_EFFMUS";
		};
		PARENTS = (R_APB_MUSIZE, R_APB_NMT);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE, OTHER);
			PROBABILITIES = (0.935, 0.045, 0, 0, 0, 0, 0.02, 0.0609, 0.8781, 0.04, 0.001, 0, 0, 0.02, 0, 0, 0.98, 0, 0, 0, 0.02, 0, 0, 0.0155016, 0.942794, 0.0217022, 0, 0.020002, 0, 0, 0, 0.0216, 0.9368, 0.0216, 0.02, 0, 0, 0, 0, 0.0221, 0.9579, 0.02, 0.122, 0.2515, 0.1645, 0.2065, 0.149, 0.0865, 0.02, 0.97, 0.01, 0, 0, 0, 0, 0.02, 0.7259, 0.2514, 0.0026, 0.0001, 0, 0, 0.02, 0.0039, 0.9275, 0.0482, 0.0004, 0, 0, 0.02, 9.999e-05, 0.216078, 0.609539, 0.151285, 0.0029997, 0, 0.019998, 0, 0.0029997, 0.267673, 0.60074, 0.106489, 0.00209979, 0.019998, 0, 0, 0.0164016, 0.333733, 0.535054, 0.0948095, 0.020002, 0.263926, 0.327633, 0.139614, 0.135914, 0.0772077, 0.0357036, 0.020002, 0.978, 0.002, 0, 0, 0, 0, 0.02, 0.9451, 0.0348, 0.0001, 0, 0, 0, 0.02, 0.7359, 0.2437, 0.0004, 0, 0, 0, 0.02, 0.140286, 0.79742, 0.040496, 0.00179982, 0, 0, 0.019998, 0.00679932, 0.564344, 0.336466, 0.070193, 0.00219978, 0, 0.019998, 0, 0.0609061, 0.426643, 0.426843, 0.0636064, 0.0020002, 0.020002, 0.442456, 0.334967, 0.0924908, 0.0689931, 0.030197, 0.0108989, 0.019998, 0.9487, 0.0313, 0, 0, 0, 0, 0.02, 0.10159, 0.850615, 0.0271973, 0.00059994, 0, 0, 0.019998, 0, 0, 0.98, 0, 0, 0, 0.02, 0, 0, 0.0582, 0.91, 0.0118, 0, 0.02, 0, 0, 0, 0.079508, 0.888989, 0.0115012, 0.020002, 0, 0, 0, 0, 0.0805, 0.8995, 0.02, 0.135, 0.2632, 0.1638, 0.1995, 0.1397, 0.0788, 0.02, 0.9715, 0.0085, 0, 0, 0, 0, 0.02, 0.431257, 0.543546, 0.00509949, 9.999e-05, 0, 0, 0.019998, 0, 0.3917, 0.5883, 0, 0, 0, 0.02, 0, 0.0004, 0.6513, 0.3273, 0.001, 0, 0.02, 0, 0, 0.0119, 0.7118, 0.2555, 0.0008, 0.02, 0, 0, 0, 0.0166, 0.7089, 0.2545, 0.02, 0.1946, 0.3041, 0.1558, 0.1682, 0.1047, 0.0526, 0.02, 0.9786, 0.0014, 0, 0, 0, 0, 0.02, 0.890511, 0.0892911, 0.00019998, 0, 0, 0, 0.019998, 0.0029, 0.9767, 0.0004, 0, 0, 0, 0.02, 0.0001, 0.564, 0.4045, 0.0114, 0, 0, 0.02, 0, 0.00509949, 0.567643, 0.39916, 0.00809919, 0, 0.019998, 0, 0, 0.0357036, 0.624362, 0.313631, 0.00630063, 0.020002, 0.318732, 0.343234, 0.126413, 0.111311, 0.0568057, 0.0235024, 0.020002, 0.780078, 0.186619, 0.0108011, 0.00230023, 0.00020002, 0, 0.020002, 0.50305, 0.380838, 0.0634063, 0.0266027, 0.00540054, 0.00070007, 0.020002, 0.164516, 0.449845, 0.191019, 0.129213, 0.0384038, 0.0070007, 0.020002, 0.0466953, 0.265673, 0.234677, 0.260974, 0.130687, 0.0412959, 0.019998, 0.00789921, 0.0984902, 0.179582, 0.316668, 0.251475, 0.125887, 0.019998, 0.00089991, 0.0250975, 0.0942906, 0.263774, 0.332167, 0.263774, 0.019998, 0.238124, 0.287029, 0.136414, 0.152315, 0.10451, 0.0616062, 0.020002);
		};
	};

	node R_LNL_DIFFN_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_LNL_DIFFN_APB_DENERV;
			NAME = "R_LNL_DIFFN_APB_DENERV";
		};
		PARENTS = (R_DIFFN_LNLW_APB_DENERV, R_LNLT1_LP_BE_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_APB_MVA_RECRUIT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MVA_RECRUIT;
			NAME = "R_APB_MVA_RECRUIT";
		};
		PARENTS = (R_APB_VOL_ACT, R_APB_MULOSS);
		DEFINITION =
		{
			NAMESTATES = (FULL, REDUCED, DISCRETE, NO_UNITS);
			PROBABILITIES = (0.9295, 0.0705, 0, 0, 0.1707, 0.7, 0.1293, 0, 0.0038, 0.174, 0.8222, 0, 0, 0, 0, 1, 0.4821, 0.5165, 0.0014, 0, 0.0366, 0.5168, 0.4466, 0, 0.0005, 0.0594, 0.9401, 0, 0, 0, 0, 1, 0.0661, 0.7993, 0.1346, 0, 0.0043, 0.1788, 0.8169, 0, 0.0001, 0.0205, 0.9794, 0, 0, 0, 0, 1, 0.00149985, 0.136586, 0.861914, 0, 0.00029997, 0.0347965, 0.964904, 0, 0, 0.0061, 0.9939, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0.263974, 0.434357, 0.30167, 0, 0.1146, 0.3465, 0.5389, 0, 0.036, 0.2144, 0.7496, 0, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_DSLOW_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DSLOW_WD;
			NAME = "R_MEDD2_DSLOW_WD";
		};
		PARENTS = (R_MEDD2_DIFSLOW_WD, R_MEDD2_SALOSS);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0.0532, 0.2387, 0.495, 0.2072, 0.0059, 0, 0, 0, 0, 0.0007, 0.0063, 0.0614, 0.3006, 0.5524, 0.0781, 0.0005, 0, 0, 0.0003, 0.0011, 0.005, 0.0205, 0.087, 0.2822, 0.4514, 0.1525, 0, 0.1036, 0.8964, 0, 0, 0, 0, 0, 0, 0, 0.0178018, 0.119012, 0.448145, 0.385439, 0.029603, 0, 0, 0, 0, 0.0002, 0.0021, 0.0283, 0.1995, 0.5939, 0.1737, 0.0023, 0, 0, 0.0002, 0.0006, 0.0029, 0.0133, 0.0634, 0.2436, 0.4657, 0.2103, 0, 0.00059994, 0.9973, 0.00209979, 0, 0, 0, 0, 0, 0, 0.0048, 0.0476, 0.3148, 0.5311, 0.1016, 0.0001, 0, 0, 0, 0, 0.00060006, 0.0114011, 0.114711, 0.544554, 0.319632, 0.00910091, 0, 0, 0.00010001, 0.00030003, 0.00170017, 0.00830083, 0.0447045, 0.20312, 0.463246, 0.278528, 0, 0.0001, 0.0629, 0.9278, 0.0092, 0, 0, 0, 0, 0, 0.00060006, 0.00920092, 0.116012, 0.485749, 0.383538, 0.00490049, 0, 0, 0, 0, 0.00010001, 0.00240024, 0.0374037, 0.332733, 0.567057, 0.060306, 0, 0, 0, 0.00010001, 0.00070007, 0.00380038, 0.0243024, 0.141614, 0.424042, 0.405441, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_ALLAMP_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_ALLAMP_WA;
			NAME = "R_APB_ALLAMP_WA";
		};
		PARENTS = (R_APB_MULOSS, R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (ZERO, A0_01, A0_10, A0_30, A0_70, A1_00, A2_00, A4_00, A8_00);
			PROBABILITIES = (0.00260026, 0.368737, 0.607561, 0.0208021, 0.00030003, 0, 0, 0, 0, 0.0409, 0.8924, 0.0661, 0.0006, 0, 0, 0, 0, 0, 0.2926, 0.7043, 0.0031, 0, 0, 0, 0, 0, 0, 0.781, 0.2189, 0.0001, 0, 0, 0, 0, 0, 0, 0.9907, 0.0093, 0, 0, 0, 0, 0, 0, 0, 0.359564, 0.514749, 0.0940906, 0.0239976, 0.00459954, 0.0019998, 0.00079992, 0.00019998, 0, 0, 0.0002, 0.4149, 0.4809, 0.0802, 0.0218, 0.002, 0, 0, 0, 0.01, 0.77, 0.2049, 0.0128, 0.0022, 0.0001, 0, 0, 0.0091, 0.4203, 0.5312, 0.038, 0.0012, 0.0002, 0, 0, 0, 0.266927, 0.716172, 0.0166017, 0.00030003, 0, 0, 0, 0, 0, 0.9858, 0.0142, 0, 0, 0, 0, 0, 0, 0, 0.133587, 0.385461, 0.269773, 0.130787, 0.040096, 0.0218978, 0.0126987, 0.00439956, 0.00129987, 0, 0, 0, 0, 0.0215, 0.9785, 0, 0, 0, 0, 0, 0, 0.2489, 0.7398, 0.0113, 0, 0, 0, 0, 0, 0.309469, 0.680732, 0.00949905, 0.00029997, 0, 0, 0, 9.999e-05, 0.10279, 0.879212, 0.0177982, 9.999e-05, 0, 0, 0, 0, 0.9865, 0.0135, 0, 0, 0, 0, 0, 0, 0, 0.0096, 0.0788, 0.2992, 0.2816, 0.1319, 0.0873, 0.0689, 0.0313, 0.0114, 0, 0, 0, 0, 0.0018, 0.0536, 0.8696, 0.075, 0, 0, 0, 0, 0.00420042, 0.346835, 0.534853, 0.113711, 0.00040004, 0, 0, 0, 0.018, 0.6298, 0.2744, 0.0746, 0.0032, 0, 0, 0, 0.00440044, 0.811181, 0.176218, 0.00730073, 0.00090009, 0, 0, 0, 0.982, 0.018, 0, 0, 0, 0, 0, 0, 0, 0.00260052, 0.0289058, 0.204041, 0.265753, 0.159432, 0.119924, 0.119024, 0.0684137, 0.0319064, 0, 0, 0, 0, 0, 0, 0.0736, 0.8528, 0.0736, 0, 0, 0, 0, 0.0064, 0.0855, 0.788, 0.1197, 0.0004, 0, 0, 0.00010001, 0.10461, 0.428143, 0.356836, 0.107111, 0.00320032, 0, 0, 9.999e-05, 0.412959, 0.49655, 0.0718928, 0.0172983, 0.00119988, 0, 0, 0.9779, 0.0221, 0, 0, 0, 0, 0, 0, 0, 0.00049995, 0.00839916, 0.118188, 0.213879, 0.162984, 0.138186, 0.169083, 0.119888, 0.0688931, 0, 0, 0, 0, 0, 0, 0.0001, 0.0794, 0.9205, 0, 0, 0, 0, 0, 0.00029997, 0.116488, 0.766723, 0.116488, 0, 0, 0, 0.00250025, 0.0978098, 0.249825, 0.532353, 0.114111, 0.00340034, 0, 0, 0.0781078, 0.519652, 0.261026, 0.116712, 0.0234023, 0.00110011, 0, 0.973, 0.027, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0021, 0.0586, 0.1473, 0.1427, 0.1363, 0.2057, 0.1798, 0.1274, 0.0003, 0.006, 0.105, 0.205, 0.1633, 0.141, 0.1771, 0.1279, 0.0744, 0.00190019, 0.0230023, 0.190019, 0.262326, 0.162916, 0.124412, 0.126413, 0.0740074, 0.0350035, 0.0144014, 0.0993099, 0.30283, 0.270227, 0.124312, 0.0821082, 0.0652065, 0.030203, 0.0114011, 0.1169, 0.3697, 0.2867, 0.1411, 0.0431, 0.0234, 0.0133, 0.0045, 0.0013, 0.937006, 0.0628937, 9.999e-05, 0, 0, 0, 0, 0, 0, 0.0521052, 0.160816, 0.227223, 0.20132, 0.106611, 0.0792079, 0.0831083, 0.0559056, 0.0337034);
		};
	};

	node R_MEDD2_ALLAMP_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_ALLAMP_WD;
			NAME = "R_MEDD2_ALLAMP_WD";
		};
		PARENTS = (R_MEDD2_DISP_WD, R_MEDD2_EFFAXLOSS);
		DEFINITION =
		{
			NAMESTATES = (ZERO, A0_01, A0_10, A0_30, A0_70, A1_00);
			PROBABILITIES = (0, 0, 0, 0, 0.0215, 0.9785, 0, 0, 0, 0.3443, 0.6228, 0.0329, 0, 0, 0.0248, 0.9704, 0.0048, 0, 0, 0.1028, 0.8793, 0.0178, 0.0001, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0.3192, 0.6448, 0.036, 0, 0, 0.0474047, 0.939494, 0.0129013, 0.00020002, 0, 0, 0.933093, 0.0669067, 0, 0, 0, 0.8756, 0.1237, 0.0007, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.0051, 0.994, 0.0009, 0, 0, 0, 0.9599, 0.0401, 0, 0, 0, 0.0001, 0.9994, 0.0005, 0, 0, 0, 0.9969, 0.0031, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0.9945, 0.0055, 0, 0, 0, 0, 0.9999, 0.0001, 0, 0, 0, 0.7508, 0.2492, 0, 0, 0, 0, 0.9999, 0.0001, 0, 0, 0, 1, 0, 0, 0, 0, 0);
		};
	};

	node R_APB_MUSCLE_VOL
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUSCLE_VOL;
			NAME = "R_APB_MUSCLE_VOL";
		};
		PARENTS = (R_APB_MALOSS, R_APB_MUSIZE);
		DEFINITION =
		{
			NAMESTATES = (ATROPHIC, NORMAL);
			PROBABILITIES = (0.9896, 0.0104, 0.9976, 0.0024, 0.999, 0.001, 0.9995, 0.0005, 0.9989, 0.0011, 0.9363, 0.0637, 0.8137, 0.1863, 0.9603, 0.0397, 0.9893, 0.0107, 0.9969, 0.0031, 0.9984, 0.0016, 0.8403, 0.1597, 0.0209, 0.9791, 0.5185, 0.4815, 0.9588, 0.0412, 0.9953, 0.0047, 0.9984, 0.0016, 0.6534, 0.3466, 0.009, 0.991, 0.1087, 0.8913, 0.6377, 0.3623, 0.9518, 0.0482, 0.9975, 0.0025, 0.4689, 0.5311, 0.003, 0.997, 0.0278, 0.9722, 0.2716, 0.7284, 0.8234, 0.1766, 0.9965, 0.0035, 0.3174, 0.6826, 0.0004, 0.9996, 0.0046, 0.9954, 0.0779, 0.9221, 0.5986, 0.4014, 0.9956, 0.0044, 0.1948, 0.8052, 0.4212, 0.5788, 0.5185, 0.4815, 0.6336, 0.3664, 0.7685, 0.2315, 0.9857, 0.0143, 0.5681, 0.4319);
		};
	};

	node R_MED_CV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_CV_EW;
			NAME = "R_MED_CV_EW";
		};
		PARENTS = (R_MED_ALLCV_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S00, M_S04, M_S08, M_S12, M_S16, M_S20, M_S24, M_S28, M_S32, M_S36, M_S40, M_S44, M_S48, M_S52, M_S56, M_S60, M_S64, M_S68, M_S72);
			PROBABILITIES = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0006, 0.0168, 0.1184, 0.296, 0.3227, 0.1783, 0.056, 0.0112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0005, 0.0155, 0.1165, 0.2969, 0.3229, 0.1782, 0.0564, 0.0114, 0.0016, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0006, 0.0039, 0.0589, 0.2434, 0.3586, 0.235, 0.0808, 0.0164, 0.0022, 0.0002, 0, 0, 0, 0, 0, 0, 0, 0.0007, 0.0176, 0.1966, 0.2515, 0.2699, 0.1688, 0.069, 0.0203, 0.0046, 0.0009, 0.0001, 0, 0, 0, 0, 0, 0, 0, 0.00560056, 0.090009, 0.305631, 0.309331, 0.20392, 0.0673067, 0.0152015, 0.00260026, 0.00040004, 0, 0, 0, 0, 0, 0, 0, 0, 0.0006, 0.0496, 0.2972, 0.4059, 0.2097, 0.0258, 0.0098, 0.0013, 0.0001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0178982, 0.294571, 0.446955, 0.190181, 0.0433957, 0.00659934, 0.00029997, 9.999e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0265, 0.5431, 0.3624, 0.0622, 0.0054, 0.0004, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.126513, 0.765477, 0.10061, 0.0070007, 0.00040004, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9999, 0.0001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		};
	};

	node R_APB_MVA_AMP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MVA_AMP;
			NAME = "R_APB_MVA_AMP";
		};
		PARENTS = (R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (INCR, NORMAL, REDUCED);
			PROBABILITIES = (0, 0.04, 0.96, 0.01, 0.15, 0.84, 0.05, 0.9, 0.05, 0.5, 0.49, 0.01, 0.85, 0.15, 0, 0.96, 0.04, 0, 0.33, 0.34, 0.33);
		};
	};

	node R_MEDD2_DSLOW_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_DSLOW_EW;
			NAME = "R_MEDD2_DSLOW_EW";
		};
		PARENTS = (R_MEDD2_SALOSS, R_MEDD2_DIFSLOW_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0.0532, 0.2387, 0.495, 0.2072, 0.0059, 0, 0, 0, 0, 0.0007, 0.0063, 0.0614, 0.3006, 0.5524, 0.0781, 0.0005, 0, 0, 0.0003, 0.0011, 0.005, 0.0205, 0.087, 0.2822, 0.4514, 0.1525, 0, 0.1036, 0.8964, 0, 0, 0, 0, 0, 0, 0, 0.0178018, 0.119012, 0.448145, 0.385439, 0.029603, 0, 0, 0, 0, 0.0002, 0.0021, 0.0283, 0.1995, 0.5939, 0.1737, 0.0023, 0, 0, 0.0002, 0.0006, 0.0029, 0.0133, 0.0634, 0.2436, 0.4657, 0.2103, 0, 0.00059994, 0.9973, 0.00209979, 0, 0, 0, 0, 0, 0, 0.0048, 0.0476, 0.3148, 0.5311, 0.1016, 0.0001, 0, 0, 0, 0, 0.00060006, 0.0114011, 0.114711, 0.544554, 0.319632, 0.00910091, 0, 0, 0.00010001, 0.00030003, 0.00170017, 0.00830083, 0.0447045, 0.20312, 0.463246, 0.278528, 0, 0.0001, 0.0629, 0.9278, 0.0092, 0, 0, 0, 0, 0, 0.00060006, 0.00920092, 0.116012, 0.485749, 0.383538, 0.00490049, 0, 0, 0, 0, 0.00010001, 0.00240024, 0.0374037, 0.332733, 0.567057, 0.060306, 0, 0, 0, 0.00010001, 0.00070007, 0.00380038, 0.0243024, 0.141614, 0.424042, 0.405441, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_MEDD2_ALLCV_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_ALLCV_WD;
			NAME = "R_MEDD2_ALLCV_WD";
		};
		PARENTS = (R_MEDD2_DSLOW_WD, R_MEDD2_LSLOW_WD);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0.030497, 0.967203, 0.00229977, 0, 0, 0, 0, 0, 0, 0.00039996, 0.0618938, 0.888111, 0.049595, 0, 0, 0, 0, 0, 0, 0.0001, 0.0655, 0.9082, 0.0262, 0, 0, 0, 0, 0, 0, 0.0001, 0.0555, 0.937, 0.0074, 0, 0, 0, 0, 0, 0, 0.0002, 0.0491, 0.8863, 0.0644, 0, 0, 0, 0, 0, 0, 0.00029997, 0.0955904, 0.89661, 0.00749925, 0, 0.00020002, 0.00060006, 0.00190019, 0.00640064, 0.0247025, 0.0974097, 0.285529, 0.583258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0235976, 0.257874, 0.640336, 0.0780922, 9.999e-05, 0, 0, 0, 0, 0.0017, 0.0421, 0.4597, 0.4852, 0.0113, 0, 0, 0, 0, 0, 0.00190019, 0.0749075, 0.569457, 0.353235, 0.00050005, 0, 0, 0, 0, 0, 0.00220022, 0.105111, 0.825183, 0.0675068, 0, 0, 0, 0, 0, 0, 0.00220022, 0.170017, 0.806281, 0.0215022, 0, 0, 0, 0, 0, 0, 0.00219978, 0.233777, 0.762024, 0.0019998, 0, 0, 0, 0, 0, 0, 0.010099, 0.489451, 0.50045, 0, 0.0001, 0.0002, 0.0007, 0.0026, 0.0117, 0.0585, 0.2222, 0.704, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0.0021, 0.1149, 0.7277, 0.1553, 0, 0, 0, 0, 0, 0.0001, 0.0146, 0.3403, 0.6424, 0.0026, 0, 0, 0, 0, 0, 0.0007, 0.0498, 0.764, 0.1854, 0.0001, 0, 0, 0, 0, 0, 0.0012, 0.137, 0.8421, 0.0197, 0, 0, 0, 0, 0, 0, 0.0034, 0.4375, 0.5591, 0, 0, 0, 0, 0, 0, 0, 0.0208021, 0.809481, 0.169717, 0, 0, 0, 0, 0, 0, 0.00010001, 0.0392039, 0.960696, 0, 0, 0.0001, 0.0002, 0.0009, 0.0051, 0.0329, 0.1636, 0.7972, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0003, 0.0011, 0.005, 0.0205, 0.087, 0.2822, 0.4514, 0.1525, 0, 0.00010002, 0.00040008, 0.00210042, 0.010102, 0.0516103, 0.218444, 0.464693, 0.252551, 0, 0, 0.00010001, 0.00060006, 0.00340034, 0.0223022, 0.133613, 0.414541, 0.425443, 0, 0, 0, 0.0001, 0.0008, 0.0073, 0.0649, 0.3063, 0.6206, 0, 0, 0, 0, 0.00010001, 0.00160016, 0.0226023, 0.174717, 0.80098, 0, 0, 0, 0, 0, 0.00020002, 0.00520052, 0.0726073, 0.921992, 0, 0, 0, 0, 0, 0, 0.00120012, 0.029603, 0.969197, 0, 0, 0, 0.0001, 0.0004, 0.0022, 0.0159, 0.0994, 0.882, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 9.999e-05, 0.00029997, 0.00219978, 0.0191981, 0.125187, 0.853015, 0, 0, 0, 0, 0.00019998, 0.00139986, 0.0136986, 0.10289, 0.881812, 0, 0, 0, 0, 0.00010001, 0.00070007, 0.00860086, 0.0782078, 0.912391, 0, 0, 0, 0, 0, 0.00030003, 0.0050005, 0.0564056, 0.938294, 0, 0, 0, 0, 0, 0.0001, 0.0026, 0.0379, 0.9594, 0, 0, 0, 0, 0, 0, 0.0012, 0.023, 0.9758, 0, 0, 0, 0, 0, 0, 0.0005, 0.014, 0.9855, 0, 0, 0, 0, 0.0001, 0.0006, 0.0058, 0.0523, 0.9412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_QUAN_MUPPOLY
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAN_MUPPOLY;
			NAME = "R_APB_QUAN_MUPPOLY";
		};
		PARENTS = (R_APB_DE_REGEN, R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (__12%, 12_24%, __24%);
			PROBABILITIES = (0.109, 0.548, 0.343, 0.34, 0.564, 0.096, 0.925, 0.075, 0, 0.796, 0.201, 0.003, 0.637, 0.348, 0.015, 0.34, 0.564, 0.096, 0.34, 0.564, 0.096, 0.004, 0.122, 0.874, 0.015, 0.261, 0.724, 0.091, 0.526, 0.383, 0.061, 0.465, 0.474, 0.039, 0.396, 0.565, 0.015, 0.261, 0.724, 0.015, 0.261, 0.724);
		};
	};

	node R_MED_AMP_WA
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MED_AMP_WA;
			NAME = "R_MED_AMP_WA";
		};
		PARENTS = (R_APB_ALLAMP_WA);
		DEFINITION =
		{
			NAMESTATES = (MV_000, MV_13, MV_18, MV_25, MV_35, MV_5, MV_71, MV1, MV1_4, MV2, MV2_8, MV4, MV5_6, MV8, MV11_3, MV16, MV22_6);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00030003, 0.285029, 0.234623, 0.178118, 0.124512, 0.080308, 0.0476048, 0.0261026, 0.0132013, 0.00610061, 0.00260026, 0.0010001, 0.00040004, 0.00010001, 0, 0, 0, 0, 0.0135014, 0.0369037, 0.0794079, 0.135314, 0.181818, 0.193219, 0.162216, 0.107711, 0.0564056, 0.0234023, 0.00760076, 0.0020002, 0.00040004, 0.00010001, 0, 0, 0, 0, 9.999e-05, 0.00059994, 0.00359964, 0.0164984, 0.0534947, 0.123288, 0.20128, 0.233477, 0.192181, 0.112089, 0.0464954, 0.0136986, 0.00279972, 0.00039996, 0, 0, 0, 0, 0, 0, 0.00010001, 0.0010001, 0.00690069, 0.0309031, 0.0926093, 0.185519, 0.249625, 0.225023, 0.135814, 0.0549055, 0.0149015, 0.00270027, 0, 0, 0, 0, 0, 0, 0, 0.00030003, 0.00310031, 0.0190019, 0.0723072, 0.173617, 0.262226, 0.249725, 0.149715, 0.0567057, 0.0133013, 0, 0, 0, 0, 0, 0, 0, 0, 0.0002, 0.0017, 0.0093, 0.0355, 0.0964, 0.186, 0.2545, 0.2471, 0.1693, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.0029, 0.0155, 0.0599, 0.1641, 0.3182, 0.439, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00010001, 0.00150015, 0.0115012, 0.0631063, 0.244524, 0.679268);
		};
	};

	node R_APB_TA_CONCL
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_TA_CONCL;
			NAME = "R_APB_TA_CONCL";
		};
		PARENTS = (R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (__5ABOVE, 2_5ABOVE, NORMAL, 2_5BELOW, __5BELOW, OTHER);
			PROBABILITIES = (0, 0, 0.01, 0.1, 0.88, 0.01, 0, 0, 0.1, 0.8, 0.09, 0.01, 0, 0.03, 0.93, 0.03, 0, 0.01, 0.17, 0.5, 0.3, 0.02, 0, 0.01, 0.44, 0.5, 0.05, 0, 0, 0.01, 0.8, 0.18, 0.01, 0, 0, 0.01, 0.17, 0.17, 0.17, 0.17, 0.16, 0.16);
		};
	};

	node R_MEDD2_AMP_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_AMP_WD;
			NAME = "R_MEDD2_AMP_WD";
		};
		PARENTS = (R_MEDD2_ALLAMP_WD);
		DEFINITION =
		{
			NAMESTATES = (UV_0_63, UV0_88, UV1_25, UV1_77, UV2_50, UV3_50, UV5_00, UV7_10, UV10_0, UV14_0, UV20_0, UV28_0, UV40_0, UV57_0, UV_80_0);
			PROBABILITIES = (0.750075, 0.187819, 0.0476048, 0.0112011, 0.00260026, 0.00060006, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0, 0.3184, 0.2478, 0.178, 0.1158, 0.0688, 0.038, 0.0189, 0.0086, 0.0036, 0.0014, 0.0005, 0.0002, 0, 0, 0, 0.0117012, 0.0392039, 0.0935094, 0.167617, 0.218922, 0.209521, 0.146515, 0.0747075, 0.0287029, 0.00780078, 0.00160016, 0.00020002, 0, 0, 0, 0, 0, 9.999e-05, 0.00129987, 0.0108989, 0.0526947, 0.156284, 0.270173, 0.274273, 0.163584, 0.0568943, 0.0121988, 0.00149985, 9.999e-05, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.00219978, 0.0217978, 0.10329, 0.259174, 0.325467, 0.209179, 0.0670933, 0.0107989, 0.00089991, 0, 0, 0, 0, 0, 0, 0, 0.0004, 0.0072, 0.0656, 0.2449, 0.3735, 0.2388, 0.0624, 0.0072);
		};
	};

	node R_APB_REPSTIM_CMAPAMP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_REPSTIM_CMAPAMP;
			NAME = "R_APB_REPSTIM_CMAPAMP";
		};
		PARENTS = (R_APB_ALLAMP_WA);
		DEFINITION =
		{
			NAMESTATES = (MV_000, MV_032, MV_044, MV_063, MV_088, MV_13, MV_18, MV_25, MV_35, MV_5, MV_71, MV1, MV1_4, MV2, MV2_8, MV4, MV5_6, MV8, MV11_3, MV16, MV22_6);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00130013, 0.115912, 0.128013, 0.133013, 0.130013, 0.119412, 0.10311, 0.0838084, 0.0639064, 0.0459046, 0.0310031, 0.019702, 0.0117012, 0.00660066, 0.00350035, 0.00170017, 0.00080008, 0.00040004, 0.00010001, 0.00010001, 0, 0, 0.00029997, 0.00129987, 0.00409959, 0.0111989, 0.0260974, 0.0515948, 0.0867913, 0.124688, 0.152485, 0.158884, 0.141086, 0.106789, 0.0687931, 0.0377962, 0.0176982, 0.00709929, 0.00239976, 0.00069993, 0.00019998, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.00069993, 0.00309969, 0.0110989, 0.0312969, 0.070393, 0.124788, 0.174783, 0.193481, 0.169283, 0.116988, 0.0639936, 0.0275972, 0.00939906, 0.00249975, 0.00049995, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00030003, 0.00210021, 0.0107011, 0.0386039, 0.0990099, 0.179618, 0.231623, 0.211921, 0.137514, 0.0632063, 0.0207021, 0.00470047, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00030003, 0.00310031, 0.0190019, 0.0723072, 0.173617, 0.262226, 0.249725, 0.149715, 0.0567057, 0.0133013, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0007, 0.0037, 0.0151, 0.0464, 0.1065, 0.1843, 0.2393, 0.2335, 0.1704, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00020002, 0.00110011, 0.00610061, 0.0249025, 0.0768077, 0.177918, 0.308931, 0.40404, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0005, 0.0038, 0.0208, 0.086, 0.2659, 0.6229);
		};
	};

	node R_APB_MUPDUR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUPDUR;
			NAME = "R_APB_MUPDUR";
		};
		PARENTS = (R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE, OTHER);
			PROBABILITIES = (0.9388, 0.0412, 0, 0, 0, 0, 0.02, 0.0396, 0.9008, 0.0396, 0, 0, 0, 0.02, 0, 0, 0.98, 0, 0, 0, 0.02, 0, 0, 0.0396, 0.9008, 0.0396, 0, 0.02, 0, 0, 0, 0.0412, 0.938, 0.0008, 0.02, 0, 0, 0, 0.0039, 0.2546, 0.7215, 0.02, 0.09, 0.235, 0.3236, 0.235, 0.09, 0.0064, 0.02);
		};
	};

	node R_APB_DENERV
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_DENERV;
			NAME = "R_APB_DENERV";
		};
		PARENTS = (R_LNL_DIFFN_APB_DENERV, R_MUSCLE_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, MILD, MOD, SEV);
			PROBABILITIES = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1);
		};
	};

	node R_APB_MUPAMP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_MUPAMP;
			NAME = "R_APB_MUPAMP";
		};
		PARENTS = (R_APB_EFFMUS);
		DEFINITION =
		{
			NAMESTATES = (V_SMALL, SMALL, NORMAL, INCR, LARGE, V_LARGE, OTHER);
			PROBABILITIES = (0.782, 0.195, 0.003, 0, 0, 0, 0.02, 0.10431, 0.771077, 0.10431, 0.00030003, 0, 0, 0.020002, 0, 0, 0.98, 0, 0, 0, 0.02, 0, 0.00029997, 0.10109, 0.747125, 0.131487, 0, 0.019998, 0, 0, 0.0024, 0.1528, 0.7968, 0.028, 0.02, 0, 0, 0, 0.0028, 0.0968, 0.8804, 0.02, 0.1328, 0.1932, 0.2189, 0.1932, 0.1726, 0.0693, 0.02);
		};
	};

	node R_MEDD2_CV_WD
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_CV_WD;
			NAME = "R_MEDD2_CV_WD";
		};
		PARENTS = (R_MEDD2_ALLCV_WD);
		DEFINITION =
		{
			NAMESTATES = (M_S00, M_S04, M_S08, M_S12, M_S16, M_S20, M_S24, M_S28, M_S32, M_S36, M_S40, M_S44, M_S48, M_S52, M_S56, M_S60, M_S64, M_S68, M_S_72);
			PROBABILITIES = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00019998, 0.0108989, 0.0983902, 0.316568, 0.353165, 0.174883, 0.040296, 0.00559944, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00019998, 0.00849915, 0.0767923, 0.281072, 0.332267, 0.20118, 0.0815918, 0.0160984, 0.00209979, 0.00019998, 0, 0, 0, 0, 0, 0, 0, 0.00020002, 0.0139014, 0.112011, 0.291829, 0.313431, 0.191519, 0.0610061, 0.0130013, 0.00280028, 0.00030003, 0, 0, 0, 0, 0, 0, 0, 0, 0.00290029, 0.0706071, 0.315132, 0.383638, 0.170917, 0.0476048, 0.00820082, 0.00090009, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0, 0.00019998, 0.0393961, 0.318468, 0.419458, 0.175582, 0.0418958, 0.00449955, 0.00049995, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0156984, 0.314469, 0.466053, 0.171583, 0.0290971, 0.00279972, 0.00029997, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0308031, 0.576958, 0.339834, 0.0482048, 0.0040004, 0.00020002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0465047, 0.848285, 0.09991, 0.00510051, 0.00020002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		};
	};

	node R_APB_QUAL_MUPPOLY
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAL_MUPPOLY;
			NAME = "R_APB_QUAL_MUPPOLY";
		};
		PARENTS = (R_APB_QUAN_MUPPOLY);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR);
			PROBABILITIES = (0.95, 0.05, 0.3, 0.7, 0.05, 0.95);
		};
	};

	node R_MEDD2_ALLCV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_ALLCV_EW;
			NAME = "R_MEDD2_ALLCV_EW";
		};
		PARENTS = (R_MEDD2_DSLOW_EW, R_MEDD2_LSLOW_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S60, M_S52, M_S44, M_S36, M_S28, M_S20, M_S14, M_S08, M_S00);
			PROBABILITIES = (1, 0, 0, 0, 0, 0, 0, 0, 0, 0.030497, 0.967203, 0.00229977, 0, 0, 0, 0, 0, 0, 0.00039996, 0.0618938, 0.888111, 0.049595, 0, 0, 0, 0, 0, 0, 0.0001, 0.0655, 0.9082, 0.0262, 0, 0, 0, 0, 0, 0, 0.0001, 0.0555, 0.937, 0.0074, 0, 0, 0, 0, 0, 0, 0.0002, 0.0491, 0.8863, 0.0644, 0, 0, 0, 0, 0, 0, 0.00029997, 0.0955904, 0.89661, 0.00749925, 0, 0.00020002, 0.00060006, 0.00190019, 0.00640064, 0.0247025, 0.0974097, 0.285529, 0.583258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.0264, 0.9149, 0.0587, 0, 0, 0, 0, 0, 0, 0.0006, 0.0944, 0.8841, 0.0209, 0, 0, 0, 0, 0, 0, 0.0018, 0.1956, 0.786, 0.0166, 0, 0, 0, 0, 0, 0, 0.0026, 0.2655, 0.7316, 0.0003, 0, 0, 0, 0, 0, 0, 0.0044, 0.5355, 0.46, 0.0001, 0, 0, 0, 0, 0, 0, 0.0047, 0.5053, 0.49, 0, 0, 0, 0, 0, 0, 0, 0.0218, 0.8352, 0.143, 0, 0.0001, 0.0003, 0.001, 0.0036, 0.0154, 0.0712, 0.2467, 0.6617, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0.0218, 0.9469, 0.0313, 0, 0, 0, 0, 0, 0, 0.0007, 0.2183, 0.779, 0.002, 0, 0, 0, 0, 0, 0, 0.0077, 0.4577, 0.5345, 0.0001, 0, 0, 0, 0, 0, 0, 0.0166, 0.9182, 0.0652, 0, 0, 0, 0, 0, 0, 0, 0.0398, 0.946, 0.0142, 0, 0, 0, 0, 0, 0, 0.0002, 0.1104, 0.8881, 0.0013, 0, 0, 0, 0, 0, 0, 0.002, 0.3203, 0.6777, 0, 0, 0.00010001, 0.00050005, 0.00190019, 0.00930093, 0.050405, 0.207221, 0.730573, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.00030003, 0.00190019, 0.0140014, 0.0788079, 0.323832, 0.459646, 0.121212, 0.00030003, 0, 9.999e-05, 0.00039996, 0.00429957, 0.0320968, 0.19558, 0.50485, 0.260374, 0.00229977, 0, 0, 0.0001, 0.0007, 0.0074, 0.0735, 0.4044, 0.4938, 0.0201, 0, 0, 0, 0.0001, 0.001, 0.0172, 0.2179, 0.6479, 0.1159, 0, 0, 0, 0, 9.999e-05, 0.00179982, 0.0558944, 0.460054, 0.482152, 0, 0, 0, 0, 0, 0.0001, 0.0041, 0.1033, 0.8925, 0, 0, 0, 0, 0, 0, 0.0003, 0.0194, 0.9803, 0, 0, 0, 0.0001, 0.0004, 0.0026, 0.019, 0.1153, 0.8626, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0.00010001, 0.00050005, 0.00410041, 0.0384038, 0.214521, 0.742374, 0, 0, 0, 0, 0.00020002, 0.00210021, 0.0239024, 0.164816, 0.808981, 0, 0, 0, 0, 0.0001, 0.0008, 0.0123, 0.1119, 0.8749, 0, 0, 0, 0, 0, 0.0003, 0.0055, 0.0699, 0.9243, 0, 0, 0, 0, 0, 0.0001, 0.0021, 0.039, 0.9588, 0, 0, 0, 0, 0, 0, 0.00069993, 0.0188981, 0.980402, 0, 0, 0, 0, 0, 0, 0.0002, 0.0093, 0.9905, 0, 0, 0, 0, 0.0001, 0.0006, 0.0062, 0.0562, 0.9369, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
		};
	};

	node R_APB_FORCE
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_FORCE;
			NAME = "R_APB_FORCE";
		};
		PARENTS = (R_APB_ALLAMP_WA, R_APB_VOL_ACT);
		DEFINITION =
		{
			NAMESTATES = (5, 4, 3, 2, 1, 0);
			PROBABILITIES = (0, 0, 0, 0.00409959, 0.190781, 0.805119, 0.00159984, 0.0185981, 0.0935906, 0.267873, 0.365863, 0.252475, 0.0149015, 0.235424, 0.533153, 0.20152, 0.0149015, 0.00010001, 0.1538, 0.6493, 0.1936, 0.0033, 0, 0, 0.6667, 0.3291, 0.0042, 0, 0, 0, 0.946895, 0.0531053, 0, 0, 0, 0, 0.9782, 0.0218, 0, 0, 0, 0, 0.9971, 0.0029, 0, 0, 0, 0, 0.9996, 0.0004, 0, 0, 0, 0, 0, 0, 0, 0.001, 0.0578, 0.9412, 0.0002, 0.0034, 0.026, 0.1312, 0.3333, 0.5059, 0.0009, 0.0256, 0.18, 0.4308, 0.3036, 0.0591, 0.0098, 0.1589, 0.4714, 0.3101, 0.0485, 0.0013, 0.0537054, 0.420442, 0.454845, 0.0690069, 0.0020002, 0, 0.117312, 0.565857, 0.30103, 0.0157016, 0.00010001, 0, 0.410159, 0.479352, 0.106989, 0.00349965, 0, 0, 0.7017, 0.2755, 0.0226, 0.0002, 0, 0, 0.8804, 0.1161, 0.0035, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.0125987, 0.987301, 0, 0.0003, 0.0044, 0.0446, 0.2226, 0.7281, 0, 0.0005, 0.0128, 0.1328, 0.4061, 0.4478, 0.0001, 0.0049, 0.0751, 0.3632, 0.429, 0.1277, 0.00050005, 0.0273027, 0.243324, 0.50135, 0.211821, 0.0157016, 0.00120012, 0.060206, 0.386239, 0.454245, 0.0954095, 0.00270027, 0.0256026, 0.247025, 0.483848, 0.217422, 0.0256026, 0.00050005, 0.1171, 0.4443, 0.3699, 0.0654, 0.0033, 0, 0.327067, 0.487951, 0.172683, 0.0119988, 0.00029997, 0, 0, 0, 0, 0, 0.00590059, 0.994099, 0, 0, 0.0005, 0.0098, 0.1058, 0.8839, 0, 0, 0.0003, 0.0091, 0.1181, 0.8725, 0, 0, 0.0008, 0.0215, 0.1873, 0.7904, 0, 9.999e-05, 0.00249975, 0.0471953, 0.275572, 0.674633, 0, 0.0001, 0.0044, 0.0713, 0.3326, 0.5916, 0, 0.0020002, 0.0279028, 0.181218, 0.411241, 0.377638, 0.0004, 0.0107, 0.0847, 0.3035, 0.3988, 0.2019, 0.0030003, 0.0426043, 0.194819, 0.384938, 0.292929, 0.0817082);
		};
	};

	node R_APB_SPONT_INS_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SPONT_INS_ACT;
			NAME = "R_APB_SPONT_INS_ACT";
		};
		PARENTS = (R_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NORMAL, INCR);
			PROBABILITIES = (0.98, 0.02, 0.1, 0.9, 0.05, 0.95, 0.05, 0.95);
		};
	};

	node R_APB_QUAN_MUPDUR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAN_MUPDUR;
			NAME = "R_APB_QUAN_MUPDUR";
		};
		PARENTS = (R_APB_MUPDUR);
		DEFINITION =
		{
			NAMESTATES = (MS3, MS4, MS5, MS6, MS7, MS8, MS9, MS10, MS11, MS12, MS13, MS14, MS15, MS16, MS17, MS18, MS19, MS20, MS_20);
			PROBABILITIES = (0.09982, 0.183337, 0.240248, 0.224545, 0.14973, 0.0712142, 0.0242048, 0.00580116, 0.0010002, 0.00010002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.010201, 0.0369037, 0.0951095, 0.174717, 0.228923, 0.214021, 0.142614, 0.0678068, 0.0230023, 0.00560056, 0.0010001, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0, 0.0002, 0.0025, 0.0177, 0.0739, 0.1852, 0.2785, 0.2515, 0.1363, 0.0444, 0.0087, 0.001, 0.0001, 0, 0, 0, 0, 0, 0, 0, 0, 0.00029997, 0.0019998, 0.010199, 0.0367963, 0.0948905, 0.174283, 0.228377, 0.213479, 0.142286, 0.0676932, 0.0229977, 0.00559944, 0.0009999, 9.999e-05, 0, 0, 0, 0, 0, 0, 0, 0.00029997, 0.0019998, 0.010199, 0.0367963, 0.0948905, 0.174283, 0.228377, 0.213479, 0.142286, 0.0676932, 0.0229977, 0.00559944, 0.0009999, 9.999e-05, 0, 0, 0, 0, 0, 0, 9.999e-05, 0.00039996, 0.00179982, 0.0069993, 0.0218978, 0.0540946, 0.105189, 0.161284, 0.194981, 0.185981, 0.139886, 0.0828917, 0.0387961, 0.00569943, 0.0201, 0.0341, 0.0529, 0.0748, 0.0966, 0.1138, 0.1224, 0.1202, 0.1078, 0.0882, 0.0658, 0.0449, 0.0279, 0.0159, 0.0082, 0.0039, 0.0017, 0.0007, 0.0001);
		};
	};

	node R_APB_SPONT_DENERV_ACT
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SPONT_DENERV_ACT;
			NAME = "R_APB_SPONT_DENERV_ACT";
		};
		PARENTS = (R_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, SOME, MOD, ABUNDANT);
			PROBABILITIES = (0.98, 0.02, 0, 0, 0.07, 0.85, 0.08, 0, 0.01, 0.07, 0.85, 0.07, 0, 0.01, 0.07, 0.92);
		};
	};

	node R_APB_QUAN_MUPAMP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAN_MUPAMP;
			NAME = "R_APB_QUAN_MUPAMP";
		};
		PARENTS = (R_APB_MUPAMP);
		DEFINITION =
		{
			NAMESTATES = (UV34, UV44, UV58, UV74, UV94, UV122, UV156, UV200, UV260, UV330, UV420, UV540, UV700, UV900, UV1150, UV1480, UV1900, UV2440, UV3130, UV4020);
			PROBABILITIES = (0.00080024, 0.00370111, 0.0135041, 0.0381114, 0.0835251, 0.142543, 0.189557, 0.196359, 0.158348, 0.0994298, 0.0486146, 0.0185056, 0.00550165, 0.00130039, 0.00020006, 0, 0, 0, 0, 0, 0, 0.00010002, 0.00080016, 0.00370074, 0.0135027, 0.0381076, 0.0835167, 0.142529, 0.189538, 0.196339, 0.158332, 0.0994199, 0.0486097, 0.0185037, 0.0055011, 0.00130026, 0.00020004, 0, 0, 0, 0, 0, 0, 0, 0, 0.00050005, 0.00370037, 0.0187019, 0.0639064, 0.147515, 0.230223, 0.243124, 0.173717, 0.0840084, 0.0275028, 0.00610061, 0.00090009, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0008, 0.0037, 0.0135, 0.0381, 0.0835, 0.1426, 0.1896, 0.1963, 0.1583, 0.0995, 0.0487, 0.0185, 0.0055, 0.0013, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0008, 0.0038, 0.0136, 0.0383, 0.0841, 0.1435, 0.1909, 0.1977, 0.1594, 0.1001, 0.049, 0.0187, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0001, 0.0006, 0.0024, 0.0082, 0.0232, 0.0542, 0.1041, 0.1645, 0.2137, 0.2283, 0.2007, 0.0045, 0.0078, 0.0127, 0.0197, 0.0289, 0.0403, 0.0531, 0.0664, 0.0787, 0.0883, 0.0939, 0.0946, 0.0903, 0.0817, 0.07, 0.0569, 0.0438, 0.0319, 0.0221, 0.0144);
		};
	};

	node R_APB_QUAL_MUPDUR
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAL_MUPDUR;
			NAME = "R_APB_QUAL_MUPDUR";
		};
		PARENTS = (R_APB_MUPDUR);
		DEFINITION =
		{
			NAMESTATES = (SMALL, NORMAL, INCR);
			PROBABILITIES = (0.8309, 0.1677, 0.0014, 0.49, 0.49, 0.02, 0.1065, 0.787, 0.1065, 0.02, 0.49, 0.49, 0.0014, 0.1677, 0.8309, 0.0001, 0.0392, 0.9607, 0.2597, 0.4806, 0.2597);
		};
	};

	node R_MEDD2_CV_EW
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_MEDD2_CV_EW;
			NAME = "R_MEDD2_CV_EW";
		};
		PARENTS = (R_MEDD2_ALLCV_EW);
		DEFINITION =
		{
			NAMESTATES = (M_S00, M_S04, M_S08, M_S12, M_S16, M_S20, M_S24, M_S28, M_S32, M_S36, M_S40, M_S44, M_S48, M_S52, M_S56, M_S60, M_S64, M_S68, M_S72, M_S_76);
			PROBABILITIES = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0012, 0.0172, 0.1126, 0.2484, 0.321, 0.1935, 0.0803, 0.0258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0009999, 0.0148985, 0.09959, 0.226177, 0.293371, 0.226177, 0.10199, 0.0288971, 0.00649935, 0.00139986, 0, 0, 0, 0, 0, 0, 0, 0, 0.00250025, 0.0317032, 0.144514, 0.263926, 0.292529, 0.167017, 0.0669067, 0.0245025, 0.00540054, 0.00090009, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0.0005, 0.0189, 0.1567, 0.3371, 0.2862, 0.1429, 0.0465, 0.0095, 0.0014, 0.0003, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.010499, 0.156484, 0.377862, 0.30067, 0.125187, 0.0244976, 0.00419958, 0.00049995, 9.999e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0045, 0.1674, 0.4536, 0.2834, 0.0774, 0.0118, 0.0017, 0.0002, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0109011, 0.422842, 0.444644, 0.106611, 0.0137014, 0.00120012, 0.00010001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0199, 0.8041, 0.1629, 0.0125, 0.0006, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		};
	};

	node R_APB_SPONT_HF_DISCH
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_SPONT_HF_DISCH;
			NAME = "R_APB_SPONT_HF_DISCH";
		};
		PARENTS = (R_APB_DENERV);
		DEFINITION =
		{
			NAMESTATES = (NO, YES);
			PROBABILITIES = (0.99, 0.01, 0.97, 0.03, 0.95, 0.05, 0.93, 0.07);
		};
	};

	node R_APB_QUAL_MUPAMP
	{
		TYPE = CPT;
		HEADER =
		{
			ID = R_APB_QUAL_MUPAMP;
			NAME = "R_APB_QUAL_MUPAMP";
		};
		PARENTS = (R_APB_MUPAMP);
		DEFINITION =
		{
			NAMESTATES = (V_RED, REDUCED, NORMAL, INCR, V_INCR);
			PROBABILITIES = (0.4289, 0.5209, 0.0499, 0.0003, 0, 0.0647, 0.5494, 0.3679, 0.018, 0, 0, 0.0479048, 0.875388, 0.0767077, 0, 0, 0.00869913, 0.283772, 0.677932, 0.029597, 0, 0.0002, 0.0376, 0.6283, 0.3339, 0, 0, 0.001, 0.0788, 0.9202, 0.096, 0.1884, 0.283, 0.3014, 0.1312);
		};
	};

};
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief The class for the NML corrections used in 3off2
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    template < typename IdSetAlloc, typename CountAlloc >
    const std::vector< double > KNML< IdSetAlloc, CountAlloc >::__Cvec = {
      2.0,           2.5,           2.88888888889, 3.21875,       3.5104,
      3.77469135802, 4.01813870071, 4.24501800537, 4.45831574489, 4.66021568,
      4.85237205074, 5.0360736751,  5.21234791295, 5.38202942438, 5.54580728515,
      5.70425824707, 5.8578708208,  6.00706309899, 6.15219620096, 6.293584586,
      6.43150408335, 6.56619822786, 6.69788331895, 6.82675250392, 6.95297910625,
      7.07671936269, 7.19811469288, 7.31729359534, 7.43437324249, 7.54946083103,
      7.66265473218, 7.77404547667, 7.88371660286, 7.99174539033, 8.09820349737,
      8.2031575174,  8.30666946646, 8.408797212,   8.50959485133, 8.60911304685,
      8.70739932388, 8.80449833604, 8.90045210256, 8.9953002208,  9.08908005739,
      9.1818269202,  9.27357421386, 9.36435358028, 9.45419502625, 9.54312703934,
      9.63117669346, 9.71836974515, 9.80473072166, 9.89028300153, 9.97504888851,
      10.0590496796, 10.1423057273, 10.2248364976, 10.3066606229, 10.3877959509,
      10.4682595908, 10.5480679547, 10.627236797,  10.7057812508, 10.7837158615,
      10.8610546184, 10.937810984,  11.0139979213, 11.0896279195, 11.1647130175,
      11.239264827,  11.3132945527, 11.3868130127, 11.4598306561, 11.5323575813,
      11.6044035514, 11.6759780103, 11.7470900966, 11.8177486575, 11.8879622617,
      11.9577392113, 12.0270875534, 12.0960150909, 12.1645293929, 12.2326378041,
      12.3003474542, 12.3676652666, 12.4345979668, 12.5011520898, 12.5673339879,
      12.6331498379, 12.6986056474, 12.7637072616, 12.8284603691, 12.8928705079,
      12.9569430709, 13.0206833111, 13.0840963465, 13.1471871653, 13.2099606302,
      13.2724214826, 13.3345743473, 13.3964237358, 13.4579740508, 13.5192295895,
      13.5801945472, 13.6408730205, 13.7012690107, 13.7613864269, 13.8212290886,
      13.8808007292, 13.9401049979, 13.9991454631, 14.0579256142, 14.1164488646,
      14.1747185536, 14.2327379487, 14.2905102481, 14.3480385822, 14.405326016,
      14.4623755507, 14.5191901259, 14.5757726211, 14.6321258572, 14.6882525988,
      14.7441555552, 14.799837382,  14.8553006829, 14.9105480108, 14.9655818692,
      15.0204047137, 15.0750189532, 15.1294269511, 15.1836310264, 15.2376334551,
      15.291436471,  15.3450422672, 15.3984529967, 15.4516707737, 15.5046976745,
      15.5575357382, 15.6101869683, 15.6626533327, 15.7149367651, 15.7670391659,
      15.8189624026, 15.870708311,  15.9222786955, 15.9736753304, 16.0248999601,
      16.0759543002, 16.1268400377, 16.1775588321, 16.2281123159, 16.2785020951,
      16.3287297498, 16.3787968348, 16.4287048805, 16.4784553929, 16.5280498543,
      16.5774897242, 16.6267764392, 16.6759114139, 16.7248960413, 16.7737316932,
      16.8224197207, 16.8709614543, 16.9193582051, 16.9676112644, 17.0157219046,
      17.0636913794, 17.1115209241, 17.1592117562, 17.2067650757, 17.2541820653,
      17.3014638909, 17.3486117017, 17.3956266311, 17.4425097961, 17.4892622985,
      17.5358852248, 17.5823796464, 17.6287466201, 17.6749871882, 17.721102379,
      17.7670932069, 17.8129606726, 17.8587057635, 17.9043294539, 17.9498327054,
      17.9952164666, 18.0404816741, 18.0856292522, 18.130660113,  18.1755751573,
      18.2203752741, 18.2650613411, 18.309634225,  18.3540947814, 18.3984438554,
      18.4426822814, 18.4868108834, 18.5308304754, 18.5747418613, 18.6185458352,
      18.6622431813, 18.7058346748, 18.749321081,  18.7927031564, 18.8359816483,
      18.8791572952, 18.9222308268, 18.9652029643, 19.0080744204, 19.0508458994,
      19.0935180976, 19.1360917031, 19.1785673961, 19.2209458492, 19.2632277271,
      19.3054136872, 19.3475043792, 19.3895004457, 19.431402522,  19.4732112364,
      19.5149272102, 19.5565510578, 19.598083387,  19.6395247987, 19.6808758874,
      19.7221372412, 19.7633094418, 19.8043930645, 19.8453886786, 19.8862968473,
      19.9271181279, 19.9678530715, 20.0085022239, 20.0490661246, 20.0895453079,
      20.1299403023, 20.1702516311, 20.2104798118, 20.250625357,  20.2906887737,
      20.3306705641, 20.3705712249, 20.4103912481, 20.4501311206, 20.4897913245,
      20.5293723369, 20.5688746305, 20.608298673,  20.6476449276, 20.6869138529,
      20.7261059032, 20.7652215281, 20.804261173,  20.843225279,  20.8821142829,
      20.9209286172, 20.9596687104, 20.9983349871, 21.0369278674, 21.0754477677,
      21.1138951007, 21.1522702747, 21.1905736947, 21.2288057616, 21.2669668727,
      21.3050574217, 21.3430777984, 21.3810283893, 21.4189095772, 21.4567217417,
      21.4944652585, 21.5321405002, 21.5697478361, 21.6072876319, 21.6447602503,
      21.6821660507, 21.7195053891, 21.7567786186, 21.7939860892, 21.8311281476,
      21.8682051376, 21.9052174,    21.9421652726, 21.9790490904, 22.0158691853,
      22.0526258864, 22.0893195201, 22.1259504099, 22.1625188766, 22.1990252382,
      22.2354698101, 22.271852905,  22.3081748329, 22.3444359014, 22.3806364152,
      22.4167766768, 22.4528569861, 22.4888776404, 22.5248389346, 22.5607411612,
      22.5965846104, 22.63236957,   22.6680963252, 22.7037651592, 22.7393763528,
      22.7749301845, 22.8104269307, 22.8458668654, 22.8812502606, 22.9165773859,
      22.9518485091, 22.9870638955, 23.0222238086, 23.0573285097, 23.0923782582,
      23.1273733112, 23.1623139241, 23.1972003501, 23.2320328405, 23.2668116448,
      23.3015370103, 23.3362091828, 23.3708284059, 23.4053949214, 23.4399089693,
      23.4743707879, 23.5087806136, 23.5431386809, 23.5774452227, 23.6117004702,
      23.6459046528, 23.6800579981, 23.7141607321, 23.7482130792, 23.7822152621,
      23.8161675018, 23.8500700178, 23.8839230278, 23.9177267482, 23.9514813936,
      23.985187177,  24.0188443102, 24.0524530031, 24.0860134643, 24.1195259009,
      24.1529905184, 24.1864075209, 24.2197771112, 24.2530994905, 24.2863748585,
      24.3196034138, 24.3527853533, 24.3859208726, 24.4190101661, 24.4520534266,
      24.4850508458, 24.5180026138, 24.5509089198, 24.5837699513, 24.6165858946,
      24.6493569349, 24.682083256,  24.7147650404, 24.7474024696, 24.7799957236,
      24.8125449812, 24.8450504202, 24.8775122172, 24.9099305473, 24.9423055847,
      24.9746375025, 25.0069264724, 25.0391726652, 25.0713762504, 25.1035373964,
      25.1356562707, 25.1677330394, 25.1997678678, 25.2317609198, 25.2637123586,
      25.295622346,  25.3274910431, 25.3593186096, 25.3911052044, 25.4228509853,
      25.4545561092, 25.4862207318, 25.517845008,  25.5494290916, 25.5809731355,
      25.6124772915, 25.6439417106, 25.6753665428, 25.7067519372, 25.7380980417,
      25.7694050037, 25.8006729694, 25.8319020841, 25.8630924924, 25.8942443377,
      25.9253577628, 25.9564329095, 25.9874699186, 26.0184689304, 26.0494300839,
      26.0803535176, 26.111239369,  26.1420877748, 26.1728988708, 26.2036727921,
      26.234409673,  26.2651096468, 26.2957728462, 26.326399403,  26.3569894483,
      26.3875431124, 26.4180605247, 26.448541814,  26.4789871082, 26.5093965346,
      26.5397702197, 26.5701082891, 26.6004108679, 26.6306780804, 26.66091005,
      26.6911068996, 26.7212687513, 26.7513957266, 26.781487946,  26.8115455297,
      26.841568597,  26.8715572664, 26.901511656,  26.9314318831, 26.9613180642,
      26.9911703153, 27.0209887518, 27.0507734882, 27.0805246386, 27.1102423164,
      27.1399266342, 27.1695777042, 27.1991956378, 27.2287805458, 27.2583325386,
      27.2878517257, 27.3173382161, 27.3467921183, 27.3762135401, 27.4056025886,
      27.4349593705, 27.4642839918, 27.4935765581, 27.5228371742, 27.5520659444,
      27.5812629725, 27.6104283616, 27.6395622144, 27.6686646331, 27.697735719,
      27.7267755733, 27.7557842962, 27.7847619879, 27.8137087475, 27.842624674,
      27.8715098657, 27.9003644204, 27.9291884353, 27.9579820072, 27.9867452324,
      28.0154782067, 28.0441810252, 28.0728537828, 28.1014965736, 28.1301094914,
      28.1586926296, 28.1872460809, 28.2157699376, 28.2442642915, 28.272729234,
      28.3011648561, 28.329571248,  28.3579484997, 28.3862967009, 28.4146159404,
      28.4429063069, 28.4711678885, 28.499400773,  28.5276050474, 28.5557807987,
      28.5839281132, 28.6120470768, 28.6401377751, 28.6682002931, 28.6962347153,
      28.7242411262, 28.7522196094, 28.7801702483, 28.8080931259, 28.8359883248,
      28.8638559271, 28.8916960145, 28.9195086685, 28.9472939698, 28.9750519991,
      29.0027828366, 29.0304865619, 29.0581632544, 29.0858129932, 29.1134358567,
      29.1410319233, 29.1686012708, 29.1961439766, 29.2236601178, 29.2511497712,
      29.278613013,  29.3060499193, 29.3334605656, 29.3608450273, 29.3882033792,
      29.4155356959, 29.4428420516, 29.47012252,   29.4973771748, 29.5246060889,
      29.5518093353, 29.5789869863, 29.6061391142, 29.6332657905, 29.6603670869,
      29.6874430744, 29.7144938237, 29.7415194054, 29.7685198894, 29.7954953457,
      29.8224458437, 29.8493714526, 29.8762722411, 29.9031482778, 29.9299996308,
      29.9568263682, 29.9836285574, 30.0104062656, 30.03715956,   30.0638885071,
      30.0905931733, 30.1172736245, 30.1439299267, 30.1705621452, 30.1971703452,
      30.2237545916, 30.2503149489, 30.2768514814, 30.3033642532, 30.3298533279,
      30.3563187689, 30.3827606395, 30.4091790024, 30.4355739203, 30.4619454555,
      30.48829367,   30.5146186255, 30.5409203836, 30.5671990054, 30.593454552,
      30.619687084,  30.6458966617, 30.6720833454, 30.698247195,  30.7243882701,
      30.75050663,   30.7766023339, 30.8026754406, 30.8287260087, 30.8547540966,
      30.8807597623, 30.9067430638, 30.9327040585, 30.958642804,  30.9845593572,
      31.010453775,  31.0363261142, 31.0621764309, 31.0880047814, 31.1138112216,
      31.1395958072, 31.1653585935, 31.1910996358, 31.2168189891, 31.242516708,
      31.268192847,  31.2938474606, 31.3194806026, 31.3450923269, 31.3706826872,
      31.3962517367, 31.4217995287, 31.4473261161, 31.4728315515, 31.4983158875,
      31.5237791764, 31.5492214702, 31.5746428207, 31.6000432796, 31.6254228984,
      31.6507817282, 31.67611982,   31.7014372247, 31.7267339927, 31.7520101746,
      31.7772658205, 31.8025009803, 31.8277157038, 31.8529100406, 31.87808404,
      31.9032377513, 31.9283712234, 31.9534845051, 31.978577645,  32.0036506913,
      32.0287036925, 32.0537366963, 32.0787497507, 32.1037429033, 32.1287162015,
      32.1536696925, 32.1786034234, 32.2035174411, 32.2284117922, 32.2532865233,
      32.2781416806, 32.3029773103, 32.3277934584, 32.3525901706, 32.3773674925,
      32.4021254696, 32.426864147,  32.4515835699, 32.4762837831, 32.5009648313,
      32.5256267592, 32.550269611,  32.5748934309, 32.599498263,  32.6240841512,
      32.6486511391, 32.6731992702, 32.6977285879, 32.7222391355, 32.7467309558,
      32.7712040919, 32.7956585863, 32.8200944816, 32.8445118201, 32.8689106442,
      32.8932909959, 32.9176529169, 32.9419964492, 32.9663216342, 32.9906285133,
      33.0149171279, 33.0391875191, 33.0634397278, 33.0876737948, 33.1118897607,
      33.1360876661, 33.1602675513, 33.1844294565, 33.2085734218, 33.2326994871,
      33.256807692,  33.2808980763, 33.3049706794, 33.3290255406, 33.3530626991,
      33.3770821938, 33.4010840637, 33.4250683476, 33.4490350839, 33.4729843113,
      33.4969160679, 33.520830392,  33.5447273216, 33.5686068947, 33.5924691489,
      33.616314122,  33.6401418513, 33.6639523743, 33.6877457282, 33.71152195,
      33.7352810768, 33.7590231452, 33.7827481921, 33.8064562541, 33.8301473674,
      33.8538215684, 33.8774788933, 33.9011193781, 33.9247430588, 33.948349971,
      33.9719401505, 33.9955136329, 34.0190704534, 34.0426106474, 34.06613425,
      34.0896412964, 34.1131318213, 34.1366058595, 34.1600634458, 34.1835046147,
      34.2069294006, 34.2303378377, 34.2537299604, 34.2771058026, 34.3004653982,
      34.3238087813, 34.3471359853, 34.370447044,  34.3937419907, 34.4170208589,
      34.4402836819, 34.4635304926, 34.4867613243, 34.5099762097, 34.5331751816,
      34.5563582728, 34.5795255158, 34.6026769431, 34.625812587,  34.6489324797,
      34.6720366535, 34.6951251402, 34.7181979718, 34.7412551801, 34.7642967968,
      34.7873228535, 34.8103333817, 34.8333284128, 34.8563079779, 34.8792721083,
      34.9022208351, 34.9251541891, 34.9480722013, 34.9709749024, 34.993862323,
      35.0167344938, 35.039591445,  35.0624332071, 35.0852598103, 35.1080712847,
      35.1308676605, 35.1536489675, 35.1764152355, 35.1991664944, 35.2219027738,
      35.2446241031, 35.2673305119, 35.2900220296, 35.3126986853, 35.3353605082,
      35.3580075275, 35.3806397721, 35.4032572708, 35.4258600525, 35.4484481458,
      35.4710215794, 35.4935803816, 35.5161245811, 35.5386542061, 35.5611692847,
      35.5836698452, 35.6061559156, 35.6286275239, 35.6510846979, 35.6735274653,
      35.695955854,  35.7183698915, 35.7407696053, 35.7631550228, 35.7855261714,
      35.8078830784, 35.8302257708, 35.8525542758, 35.8748686203, 35.8971688314,
      35.9194549356, 35.94172696,   35.963984931,  35.9862288753, 36.0084588193,
      36.0306747894, 36.052876812,  36.0750649133, 36.0972391194, 36.1193994565,
      36.1415459504, 36.1636786272, 36.1857975127, 36.2079026325, 36.2299940124,
      36.2520716779, 36.2741356547, 36.296185968,  36.3182226432, 36.3402457057,
      36.3622551806, 36.3842510931, 36.4062334681, 36.4282023307, 36.4501577057,
      36.472099618,  36.4940280922, 36.5159431531, 36.5378448251, 36.559733133,
      36.581608101,  36.6034697535, 36.6253181148, 36.6471532092, 36.6689750607,
      36.6907836934, 36.7125791314, 36.7343613985, 36.7561305187, 36.7778865156,
      36.7996294129, 36.8213592344, 36.8430760035, 36.8647797438, 36.8864704787,
      36.9081482315, 36.9298130255, 36.951464884,  36.97310383,   36.9947298867,
      37.0163430771, 37.037943424,  37.0595309504, 37.081105679,  37.1026676327,
      37.1242168339, 37.1457533055, 37.1672770699, 37.1887881495, 37.2102865667,
      37.231772344,  37.2532455035, 37.2747060675, 37.2961540581, 37.3175894974,
      37.3390124074, 37.36042281,   37.3818207271, 37.4032061805, 37.4245791921,
      37.4459397833, 37.4672879761, 37.4886237918, 37.5099472519, 37.531258378,
      37.5525571913, 37.5738437133, 37.5951179651, 37.616379968,  37.6376297431,
      37.6588673114, 37.680092694,  37.7013059119, 37.7225069858, 37.7436959367,
      37.7648727853, 37.7860375524, 37.8071902586, 37.8283309244, 37.8494595705,
      37.8705762172, 37.8916808851, 37.9127735945, 37.9338543657, 37.9549232189,
      37.9759801743, 37.9970252521, 38.0180584724, 38.039079855,  38.0600894202,
      38.0810871876, 38.1020731772, 38.1230474089, 38.1440099022, 38.164960677,
      38.1858997528, 38.2068271491, 38.2277428857, 38.2486469818, 38.269539457,
      38.2904203306, 38.3112896217, 38.3321473499, 38.3529935342, 38.3738281938,
      38.3946513478, 38.4154630152, 38.4362632149, 38.457051966,  38.4778292873,
      38.4985951977, 38.519349716,  38.5400928607, 38.5608246507, 38.5815451046,
      38.602254241,  38.6229520783, 38.643638635,  38.6643139296, 38.6849779805,
      38.7056308059, 38.7262724241, 38.7469028534, 38.767522112,  38.7881302178,
      38.8087271892, 38.829313044,  38.8498878003, 38.8704514759, 38.8910040889,
      38.9115456569, 38.9320761978, 38.9525957293, 38.9731042691, 38.9936018349,
      39.0140884443, 39.0345641148, 39.0550288639, 39.0754827091, 39.0959256678,
      39.1163577573, 39.1367789949, 39.1571893981, 39.1775889838, 39.1979777695,
      39.2183557721, 39.2387230088, 39.2590794966, 39.2794252525, 39.2997602935,
      39.3200846365, 39.3403982983, 39.3607012958, 39.3809936458, 39.4012753649,
      39.4215464699, 39.4418069775, 39.4620569042, 39.4822962666, 39.5025250812,
      39.5227433644, 39.5429511328, 39.5631484026, 39.5833351903, 39.6035115122,
      39.6236773844, 39.6438328233, 39.663977845,  39.6841124657, 39.7042367013,
      39.7243505681, 39.7444540819, 39.7645472589, 39.7846301148, 39.8047026656,
      39.8247649271, 39.8448169152, 39.8648586456, 39.884890134,  39.9049113961,
      39.9249224476, 39.9449233041, 39.964913981,  39.9848944941, 40.0048648587,
      40.0248250903, 40.0447752044, 40.0647152161, 40.0846451411, 40.1045649943,
      40.1244747913, 40.1443745471, 40.164264277,  40.184143996,  40.2040137194,
      40.223873462,  40.2437232391, 40.2635630655, 40.2833929562, 40.3032129262};


    /// default constructor
    template < typename IdSetAlloc, typename CountAlloc >
    template < typename RowFilter >
    INLINE
    KNML< IdSetAlloc, CountAlloc >::KNML(const RowFilter& filter,
                                         const std::vector< Size >& var_modalities)
        : IndependenceTest< IdSetAlloc, CountAlloc >(filter, var_modalities) {
      // for debugging purposes
      GUM_CONSTRUCTOR(KNML);
    }

    /// destructor
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE KNML< IdSetAlloc, CountAlloc >::~KNML() {
      // for debugging purposes
      GUM_DESTRUCTOR(KNML);
    }

    /// returns the score corresponding to a given nodeset
    template < typename IdSetAlloc, typename CountAlloc >
    double KNML< IdSetAlloc, CountAlloc >::score(Idx nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // get the nodes involved in the score as well as their modalities
      const std::vector< Idx, IdSetAlloc >& all_nodes =
        this->_getAllNodes(nodeset_index);
      const std::vector< Idx, IdSetAlloc >* conditioning_nodes =
        this->_getConditioningNodes(nodeset_index + 1);
      const std::vector< Size >& modals = this->modalities();

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (conditioning_nodes != nullptr) {
        // now, perform
        // 0.5 * sum_Z ( sum_X( log( C^(r_y)_#ZX ) ) - log( C^(r_y)_#Z )
        // 		+ sum_Y( log( C^(r_x)_#ZY ) ) - log( C^(r_x)_#Z ) )

        // get the counts for all the targets and for the conditioning nodes
        const std::vector< double, CountAlloc >& Nzy =
          this->_getConditioningCounts(nodeset_index);
        const std::vector< double, CountAlloc >& Nzx =
          this->_getAllCounts(nodeset_index + 1);
        const std::vector< double, CountAlloc >& Nz =
          this->_getConditioningCounts(nodeset_index + 1);

        const auto Z_size = Size(Nz.size());
        const auto Y_size = Size(modals[all_nodes[all_nodes.size() - 2]]);
        const auto X_size = Size(modals[all_nodes[all_nodes.size() - 1]]);

        double score = 0.0;

        for (Idx z = 0, beg_zx = 0, beg_zy = 0; z < Z_size;
             ++z, beg_zx += X_size, beg_zy += Y_size) {
          double sumX = 0.0, sumY = 0.0;
          for (Idx x = 0, zx = beg_zx; x < X_size; ++x, ++zx) {
            sumX += std::log(this->_C(Y_size, Nzx[zx]));
          }
          score += sumX - std::log(this->_C(Y_size, Nz[z]));
          for (Idx y = 0, zy = beg_zy; y < Y_size; ++y, ++zy) {
            sumY += std::log(this->_C(X_size, Nzy[zy]));
          }
          score += sumY - std::log(this->_C(X_size, Nz[z]));
        }

        score *= 0.5;
        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      } else {
        // here, there is no conditioning set
        // now, perform 0.5 * ( sum_X( log( C^(r_y)_#X ) ) - log( C^(r_y)_N )
        //                     + sum_Y( log( C^(r_x)_#Y ) ) - log( C^(r_x)_N ) )

        // get the counts for all the targets and for the conditioning nodes
        const std::vector< double, CountAlloc >& Ny =
          this->_getConditioningCounts(nodeset_index);
        const std::vector< double, CountAlloc >& Nx =
          this->_getAllCounts(nodeset_index + 1);

        const Size Y_size = Size(Ny.size());
        const Size X_size = Size(Nx.size());

        // count N
        double N = 0;
        for (Idx i = 0; i < Nx.size(); ++i) {
          N += Nx[i];
        }

        double score = 0.0;

        for (Idx x = 0; x < X_size; ++x) {
          score += std::log(this->_C(Y_size, Nx[x]));
        }
        score -= std::log(this->_C(Y_size, N));
        for (Idx y = 0; y < Y_size; ++y) {
          score += std::log(this->_C(X_size, Ny[y]));
        }
        score -= std::log(this->_C(X_size, N));

        score *= 0.5;
        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      }
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double KNML< IdSetAlloc, CountAlloc >::_C(Size r, Size n) {
      if (n == 0 || r == 1) {
        return 1.0;
      }
      if (r == 2) {
        if (n > 1000) {
          if (__use_cache_C) {
            try {
              double val = __cache_C.score(std::vector< Idx >{r, n});
              return val;
            } catch (const NotFound&) {
            }
          }
          const double val =
            std::sqrt(n * M_PI / 2) * std::exp(std::sqrt(8 / (9 * n * M_PI)) +
                                               (3 * M_PI - 16) / (36 * n * M_PI));
          if (__use_cache_C) {
            _insertIntoCCache(r, n, val);
          }
          return val;
        } else {
          return this->__Cvec[n - 1];
        }
      }
      if (__use_cache_C) {
        try {
          double val = __cache_C.score(std::vector< Idx >{r, n});
          return val;
        } catch (const NotFound&) {
        }
      }
      double val = _C(r - 1, n) + n / (r - 2) * _C(r - 2, n);
      if (__use_cache_C) {
        _insertIntoCCache(r, n, val);
      }
      return val;
    }

    /// inserts a new score into the cache for C
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    KNML< IdSetAlloc, CountAlloc >::_insertIntoCCache(Size r, Size n, double c) {
      __cache_C.insert(std::vector< Idx >{r, n}, c);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
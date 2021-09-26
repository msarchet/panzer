#include "bitboard_constants.h"
#include "board_utils.h"
#include <algorithm>
namespace Panzer
{
    void Utils::PrintBoard(bitboard board)
    {
		std::cout << std::endl;

		for (int row = 7; row >= 0; --row) 
		{
			for (int col = 0; col <= 7; ++col) 
			{
				if (board & (1ULL << ((row * 8) + col))) 
				{
					std::cout << "1 ";
				} else 
				{
					std::cout << "0 ";
				}
			}

			std::cout << std::endl;
		}
    }

	std::string Utils::PrintMove(Panzer::Move move)
	{
		auto output = squareToString[move.getFrom()] + squareToString[move.getTo()];

		if (move.isPromo())
		{
			if (move.getFlags() & QUEEN_PROMO)
			{
				output += "q";
			}
			if (move.getFlags() & BISHOP_PROMO)
			{
				output += "b";
			}
			if (move.getFlags() & KNIGHT_PROMO)
			{
				output += "n";
			}
			if (move.getFlags() & ROOK_PROMO)
			{
				output += "r";
			}
		}

		return output;
	}

	bool Utils::MoveSorter(Panzer::Move move_one, Panzer::Move move_two)
	{
		return move_one.m_score > move_two.m_score;
	}

	void Utils::SortMoves(MoveVector &moves)
	{
		std::stable_sort(moves->begin(), moves->end(), MoveSorter);
	}

	int Utils::GetLSB(bitboard b) { return __builtin_ctzll(b); }

	std::shared_ptr<std::array<unsigned int, 624>> Utils::GetSeedData()
	{
		std::array<unsigned int, 624> data
		{
			1807997966,
			514589821,
			1959766121,
			853312699,
			3926196803,
			127218440,
			1785845433,
			1291168671,
			4216280768,
			3370920593,
			38555529,
			758205737,
			3635410841,
			1372425174,
			1200358015,
			3460572739,
			525867534,
			845942711,
			1836653158,
			3753362088,
			3894020539,
			1378786266,
			2999763182,
			690026900,
			572424209,
			1965050136,
			800922823,
			3858368151,
			2113701069,
			1897346838,
			2157976477,
			1434653968,
			677708847,
			3890807770,
			2125152882,
			3762503238,
			1939675719,
			629900934,
			1188589829,
			1446124882,
			695099205,
			504809785,
			3255641134,
			1440426737,
			1552523426,
			201912725,
			3053344042,
			1283967070,
			3199624478,
			90785995,
			856408544,
			1281460349,
			3151424394,
			1952397395,
			1421496978,
			1675488647,
			2077429828,
			4202856632,
			3951176545,
			793760704,
			2535629076,
			2287784618,
			2641259469,
			3839405159,
			3918417556,
			1845479961,
			2346454698,
			1222289505,
			2560274864,
			4151618572,
			3515264787,
			1774893023,
			1305946269,
			4181538339,
			1297707944,
			315619848,
			1129812569,
			3737881203,
			2259651894,
			1141241740,
			3207640910,
			3589317514,
			1710647167,
			3716694467,
			1045347450,
			2605082451,
			1040686752,
			299963767,
			1259538898,
			3511043150,
			1240715687,
			450944928,
			1902799839,
			2440311561,
			878704038,
			3194516513,
			2821139343,
			3025232134,
			3697972460,
			3214100664,
			3530125015,
			3366975466,
			4198875640,
			3583742067,
			1578332235,
			3416071885,
			4285931505,
			1212666351,
			118470350,
			3919809304,
			2160772972,
			694910189,
			413653741,
			3197623634,
			2284887746,
			3411570472,
			325729971,
			1492945711,
			1287173445,
			2096578031,
			2273463171,
			822071335,
			308672257,
			1287940843,
			2397825557,
			316123878,
			1134936165,
			308675383,
			2544607348,
			463783569,
			1184583010,
			2746484896,
			4282445955,
			171496546,
			1467415862,
			2178682027,
			61562917,
			515355419,
			1409940443,
			2487546770,
			4280550787,
			1918329916,
			201841642,
			3574913054,
			3817092062,
			1274120956,
			1315944658,
			33442355,
			3663034441,
			290691427,
			713372352,
			2187819125,
			1241613014,
			4035153762,
			1536748108,
			3139211469,
			3267932019,
			813918374,
			3719928679,
			802382783,
			2190863,
			1263065118,
			3184603900,
			4158839441,
			2521255477,
			2930304649,
			841922454,
			1843292673,
			63762272,
			2267435882,
			378607370,
			1423406635,
			2939238071,
			2782061971,
			2412831606,
			1800809993,
			895473698,
			1821616967,
			1897052051,
			3791036516,
			2914209778,
			3158740110,
			2804611555,
			1899002858,
			1508906940,
			1786018170,
			1369322408,
			4254614603,
			3218151953,
			400758606,
			396072361,
			113296400,
			2585166085,
			201280713,
			3179439271,
			910149629,
			509947393,
			3472982653,
			3914480999,
			3419213534,
			4270191593,
			3377337225,
			2351503787,
			593780310,
			57025977,
			2585525958,
			757413852,
			3456082364,
			242746655,
			3535242749,
			2641532085,
			684507023,
			2917992978,
			1367956160,
			767912552,
			138486310,
			1493986404,
			2567452202,
			4189117477,
			1535342619,
			2590993979,
			3775651870,
			2456188795,
			1057950220,
			864296676,
			365028435,
			2901751398,
			2931979916,
			2537589527,
			1027591612,
			1967238901,
			3348564825,
			4030623173,
			3407992063,
			904881540,
			1597115084,
			3255744304,
			2546205092,
			2799998690,
			1218885237,
			2173596988,
			3893020886,
			1130164456,
			1624596011,
			3285977266,
			689284510 ,
			3282223222,
			1792383675,
			3525837856,
			4162710970,
			2028327155,
			3411960551,
			2716608310,
			1239674388,
			923263790 ,
			3692145577,
			2568313035,
			201028247 ,
			1534030357,
			1123357478,
			4061685614,
			4292917777,
			1261057234,
			3968766991,
			3788006107,
			3045344690,
			2153885094,
			1008108737,
			2408465631,
			646831791 ,
			4210368937,
			1089940734,
			385850176 ,
			3180645773,
			2510935112,
			1739155863,
			1580416370,
			2765761286,
			2491952147,
			1077457442,
			4174278814,
			3341308424,
			3488289854,
			3773185872,
			3696820924,
			283787300 ,
			4240699803,
			640176835 ,
			657511566 ,
			3922525668,
			56418117  ,
			597476743 ,
			1692610813,
			2518499201,
			3017674683,
			326957494 ,
			703827819 ,
			1163823756,
			45071971  ,
			607573440 ,
			3583118893,
			2808304852,
			4136984391,
			137704093 ,
			1496300566,
			4156740037,
			1491343140,
			1239334907,
			1202807459,
			430214144 ,
			656188561 ,
			3601201624,
			4095429682,
			1004757850,
			4148774617,
			1687293231,
			1056838728,
			1963740874,
			789306054 ,
			1846341307,
			1649729571,
			1882684518,
			3814210184,
			3779837340,
			4248518472,
			514725944 ,
			2698197924,
			1677783444,
			1224603373,
			2284055325,
			3313788712,
			2283981227,
			2352276747,
			1547940458,
			1350034637,
			4145535306,
			2885688766,
			468389389 ,
			3437301940,
			3500387711,
			1505070673,
			2796696742,
			1182274540,
			3284455606,
			1743508633,
			1705448227,
			2203317267,
			1432881911,
			2847864088,
			712801967 ,
			32782552  ,
			2992710016,
			1378165190,
			4184585535,
			3552428592,
			3711999072,
			1222204118,
			2052985136,
			541320253 ,
			2211616073,
			859565404 ,
			3881976951,
			113051655 ,
			3856980495,
			2748879726,
			1139029266,
			1406722586,
			2745070136,
			481944859 ,
			2368693931,
			2535004771,
			1610245218,
			368598284 ,
			703141727 ,
			3974017794,
			959813802 ,
			1481082985,
			1461366939,
			2836200583,
			2838504351,
			3720900833,
			3166723854,
			1867108305,
			2412843295,
			4115747174,
			301326648 ,
			1841742473,
			2210937750,
			1050609796,
			3182851746,
			2472817948,
			2788479221,
			2635030183,
			3829134725,
			182024667 ,
			3931365224,
			2350778202,
			1577282392,
			2694210064,
			2671084996,
			1047336029,
			2863781918,
			2143547043,
			63986610  ,
			1238361023,
			2339250519,
			471898279 ,
			161713088 ,
			326573274 ,
			1410972357,
			918540367 ,
			343500770 ,
			3678625584,
			1139590799,
			1751505474,
			2866037101,
			935063416 ,
			1378260927,
			1074008179,
			3781158919,
			3034056519,
			4151864495,
			217774819 ,
			1221758270,
			2561088296,
			2912236187,
			534396824 ,
			779913050 ,
			1092981860,
			32419666  ,
			382587263 ,
			1868195620,
			1133170679,
			34466632  ,
			4237440964,
			3849805679,
			3593222683,
			3056270976,
			1953113767,
			263314825 ,
			261590944 ,
			671404106 ,
			549704970 ,
			2342202179,
			910725235 ,
			792443268 ,
			284281558 ,
			1580961138,
			3111058216,
			3926083275,
			3788401002,
			3993091636,
			2661883139,
			3061774115,
			3334419735,
			1361099181,
			2724792180,
			5463240   ,
			3238576285,
			1088500448,
			2388036764,
			263736459 ,
			2776810368,
			3745011378,
			1941526765,
			2181329045,
			2026832717,
			2692261883,
			399761183 ,
			3310252596,
			2413929921,
			872797119 ,
			380003293 ,
			3706236801,
			1949461200,
			1914062663,
			4112470468,
			4051704739,
			845792981 ,
			4086149326,
			680487281 ,
			1657944765,
			2305503918,
			669684061 ,
			1198773177,
			279095000 ,
			1863596001,
			1941051428,
			1823127959,
			4111494483,
			3047046823,
			4039901374,
			3991393634,
			377860936 ,
			3761280105,
			135092815 ,
			2945711929,
			2234987907,
			2995903796,
			1023971576,
			1910441577,
			2813389647,
			337080524 ,
			938250883 ,
			2530218658,
			1626098247,
			718662273 ,
			2438071058,
			3681217881,
			4002504924,
			3404540667,
			3179182017,
			3403237360,
			1132760843,
			3115659419,
			321176986 ,
			924250804 ,
			3981222700,
			1094456697,
			256191002 ,
			4116873241,
			1189275784,
			2399980696,
			101465981 ,
			3366435705,
			2826057262,
			2646751189,
			1078148048,
			2100206668,
			3305468974,
			67255688  ,
			3210654444,
			2485341007,
			2705472129,
			1233180083,
			1637471406,
			4001374972,
			1415950110,
			3844339556,
			2516914702,
			2210000941,
			2793533214,
			3631437260,
			1791737141,
			171658201 ,
			728557294 ,
			778304870 ,
			1114390508,
			1189825767,
			3338218976,
			1860596555,
			4024846953,
			338854924 ,
			3752534754,
			3667330446,
			3875358402,
			3620769502,
			4263670360,
			3288315789,
			3320444901,
			1308880581,
			1222797359,
			4202167102,
			1329618493,
			1511483051,
			3577264359,
			2610482677,
			3660852916,
			3873181889,
			3358149014,
			2909577034,
			96001252  ,
			3597938700,
			2985334829,
			2649790856,
			2680302113,
			1198993228,
			2831615370,
			2921338845,
			1708570694,
			2990974115,
			3215880187,
			1143304287,
			2526318192,
			1903131841,
			1035643035,
			347294285 ,
			3555982019,
			98623650  ,
			297306263 ,
			2383700248,
			3837107006,
			2740222461,
			1711079220,
			945149201 ,
			1680667941,
			3332485462,
			1867375429,
			1930944840,
			3174636769,
			1666633146,
			2462249797,
			579983968 ,
			3785544523,
			1771297068,
			2423660485,
			591537549 ,
			667548428 ,
			3906730762,
			1683414761,
			3619157879,
			3237240119,
			2790815565,
			679430026 ,
			2057358046,
			4166572394,
			2847614748,
			2597498743,
			1717506201,
			3977393992,
			3409083646,
			2043098554,
			2455059753,
			2814354965
		};

		return std::make_shared<std::array<unsigned int, 624>>(data);
	}
}
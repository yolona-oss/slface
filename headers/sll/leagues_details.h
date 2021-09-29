#ifndef _LEAGUES_DETAILS_H_
#define _LEAGUES_DETAILS_H_

#include <string>
#include <array>

namespace SL {
	class leagueDetails //TODO add api-throw editing table
	{
		private:
			static constexpr std::array<char *, 5>
				leagues = { "Bronze",
						    "Silver",
						    "Gold",
						    "Diamond",
						    "Champion"
						  };

			struct rengeVal {
				unsigned renge[2];
				unsigned val;
			};

			struct byLeagueRengeVal {
				rengeVal league[3];
			};

			//mb do it with bin relation(non 3 val) and use only 2 and check prev val, for first skip?
			static constexpr std::array<byLeagueRengeVal, 100> lp_dictionary
				{ 
					{
						{ { 100, 399 }, 0 },
						{ { 400, 699 }, 1000 },
						{ { 700, 999 }, 5000 },
					},

					{
						{ { 1000, 1299 }, 15000 },
						{ { 1300, 1599 }, 40000 },
						{ { 1600, 1899 }, 70000 },
					},

					{
						{ { 400, 600}, 1000 },
						{ { 400, 600}, 1000 },
						{ { 400, 600}, 1000 },
					},

					{
						{ { 400, 600}, 1000 },
						{ { 400, 600}, 1000 },
						{ { 400, 600}, 1000 },
					}
				};

		public:
			std::string league(int rating, int power)
			{
				return "";
			}
	};

};

#endif

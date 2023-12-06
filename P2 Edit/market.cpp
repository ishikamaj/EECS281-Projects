// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#include <getopt.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "P2random.h"

using namespace std;

class Order {
public:
	Order(int t, int s, int p, int q, int timeStamp, int tCount) :
	traderId(t), stockId(s), price(p), quantity(q), 
	time(timeStamp), tCount(tCount){}

	int traderId;
	int stockId;
	int price;
	mutable int quantity;
	int time;
	int tCount;
};
struct TimeTravel {
	TimeTravel() {
		potentialBuyTime = -1;
		potentialBuyPrice = -1;
		ttBuyTime = -1;
		ttBuyPrice = -1;
		ttSellTime = -1;
		ttSellPrice = -1;
	}
	int potentialBuyTime;
	int potentialBuyPrice;
	int ttBuyTime;
	int ttBuyPrice;
	int ttSellTime;
	int ttSellPrice;
};

struct Median {
	void addNum(int add) {
		if (min.empty() || max.empty()) {
			max.push(add);
		}
		else {
			if (add >= max.top()) {
				min.push(add);
			}
			else {
				max.push(add);
			}
		}
		if (static_cast<int>(max.size() - min.size()) == 2) {
			int temp = max.top();
			max.pop();
			min.push(temp);
		}
		else if (static_cast<int>(max.size() - min.size()) == -2) {
			int temp = min.top();
			min.pop();
			max.push(temp);
		}
	}
	int returnMedian() {
		if (min.empty() && max.empty()) {
			return -1;
		}
		if (static_cast<int>(max.size() - min.size()) == 1) {
			return max.top();
		}
		else if (static_cast<int>(max.size() - min.size()) == -1) {
			return min.top();
		}
		else {
			return (min.top() + max.top()) / 2;
		}
	}
	priority_queue<int> max;
	priority_queue<int, vector<int>, greater<int>> min;
};

struct Buy {
	bool operator()(const Order& lhs, const Order& rhs) const {
		return (lhs.price == rhs.price) ? 
			lhs.tCount > rhs.tCount : lhs.price < rhs.price;
	}
};
struct Sell {
	bool operator()(const Order& lhs, const Order& rhs) const {
		return (lhs.price == rhs.price) ?
			lhs.tCount > rhs.tCount : lhs.price > rhs.price;
	}
};

class market {
public:
	market() {
		isV = false;
		isM = false;
		isI = false;
		isT = false;
	}
	void getMode(int argc, char * argv[]) {
		opterr = true;
		int choice;
		int option_index = 0;
		option long_options[] = {
			{ "verbose",    	no_argument,       nullptr, 'v' },
			{ "median",    		no_argument,       nullptr, 'm' },
			{ "trader_info",    no_argument,       nullptr, 'i' },
			{ "time_travelers", no_argument,       nullptr, 't' },
			{ nullptr, 0, nullptr, '\0' }
		};

		while ((choice = getopt_long(argc, argv, "vmit",
			long_options, &option_index)) != -1) {
			switch (choice) {
			case 'v':
				isV = true;
				break;

			case 'm':
				isM = true;
				break;

			case 'i':
				isI = true;
				break;

			case 't':
				isT = true;
				break;

			default:
				cerr << "Error: invalid option" << '\n';
				exit(1);
			}
		}
		return;
	}

	void updateTimeTravel(int stock, bool BS, int time, int price) {
		if (BS) { //Buy
			if (timeTraveler[stock].potentialBuyTime == -1) {
				if (timeTraveler[stock].ttBuyTime == -1) {
					return;
				}
				else if (timeTraveler[stock].ttSellPrice == -1 && 
				price > timeTraveler[stock].ttBuyPrice) {
					timeTraveler[stock].ttSellTime = time;
					timeTraveler[stock].ttSellPrice = price;
				}
				else if (timeTraveler[stock].ttSellPrice != -1 && 
				price > timeTraveler[stock].ttSellPrice) {
					timeTraveler[stock].ttSellTime = time;
					timeTraveler[stock].ttSellPrice = price;
				}
			}
			else {
				if (timeTraveler[stock].ttSellPrice != -1 && 
				price > timeTraveler[stock].ttSellPrice) {
					timeTraveler[stock].ttSellTime = time;
					timeTraveler[stock].ttSellPrice = price;
				}
				if ((price - timeTraveler[stock].potentialBuyPrice) 
				> (timeTraveler[stock].ttSellPrice 
				- timeTraveler[stock].ttBuyPrice)) {
					timeTraveler[stock].ttBuyTime 
					= timeTraveler[stock].potentialBuyTime;
					timeTraveler[stock].ttBuyPrice 
					= timeTraveler[stock].potentialBuyPrice;
					timeTraveler[stock].potentialBuyTime = -1;
					timeTraveler[stock].potentialBuyPrice = -1;
					timeTraveler[stock].ttSellTime = time;
					timeTraveler[stock].ttSellPrice = price;
				}
			}
			
		}
		else {
			if (timeTraveler[stock].ttBuyTime == -1) {
				timeTraveler[stock].ttBuyTime = time;
				timeTraveler[stock].ttBuyPrice = price;
			}
			else if (price < timeTraveler[stock].ttBuyPrice && 
			timeTraveler[stock].ttSellTime == -1) {
				timeTraveler[stock].ttBuyTime = time;
				timeTraveler[stock].ttBuyPrice = price;
			}
			else if (price < timeTraveler[stock].ttBuyPrice && 
			timeTraveler[stock].ttSellTime != -1) {
				if (timeTraveler[stock].potentialBuyTime == -1 || 
				price < timeTraveler[stock].potentialBuyPrice) {
					timeTraveler[stock].potentialBuyTime = time;
					timeTraveler[stock].potentialBuyPrice = price;
				}
			}
		}
		return;
	}

	void readIn() {
		cout << "Processing orders..." << '\n';
		string buyOrSell;
		string tlOrPR;
		string junk;
		char moreJunk;
		int tCount = 0; //the running time
		int timeStamp; //time of incoming
		int currentTime = 0;
		int traderID;
		int stockID;
		int price;
		int quantity;
		int totalTrades = 0;
		getline(cin, junk);
		cin >> junk >> tlOrPR >> junk >> numTrader >> junk >> numStock;
		buy.reserve(numStock);
		sell.reserve(numStock);
		if (isM) {
			median.reserve(numStock);
		}
		if (isT) {
			timeTraveler.reserve(numStock);
		}
		priority_queue<Order, vector<Order>, Buy> buyPQ;
		priority_queue<Order, vector<Order>, Sell> sellPQ;
		Median med;
		TimeTravel tt;
		for (unsigned int i = 0; i < numStock; i++) {
			buy.push_back(buyPQ);
			sell.push_back(sellPQ);
			if (isM) {
				median.push_back(med);
			}
			if (isT) {
				timeTraveler.push_back(tt);
			}
		}
		if (isI) {
			traderInfo.reserve(numTrader);
			vector<int> temp = { 0,0,0 };
			for (unsigned int i = 0; i < numTrader; i++) {
				traderInfo.push_back(temp);
			}
		}

		stringstream ss;
		if (tlOrPR == "PR") {
			TP = false;
			unsigned int seed;
			unsigned int numOrder;
			unsigned int arrivalRate;
			cin >> junk >> seed >> junk >> numOrder >> junk >> arrivalRate;
			P2random::PR_init(ss, seed, numTrader, numStock, 
			numOrder, arrivalRate);
		}
		else {
			TP = true;
		}
		istream & inputStream = (TP) ? cin : ss;
		
		while (inputStream >> timeStamp) {
			if (timeStamp < 0 || timeStamp < currentTime) {
				cerr << "something wrong with timeStamp" << endl;
				exit(1);
			}
			tCount++;
			bool complete = false;
			inputStream >> buyOrSell >> moreJunk >> traderID >> moreJunk >>
				stockID >> moreJunk >> price >> moreJunk >> quantity;
			if (traderID < 0 || traderID >= static_cast<int>(numTrader) || 
			stockID < 0 || stockID >= static_cast<int>(numStock)) {
				cerr << "something wrong with traders or stocks" << endl;
				exit(1);
			}
			if (price <= 0 || quantity <= 0) {
				cerr << "something wrong with the price or quantity" << endl;
				exit(1);
			}
			BS = (buyOrSell == "BUY") ? true : false;
			if (timeStamp != currentTime) { 
				if (isM) {
					for (unsigned i = 0; i < median.size(); i++) {
						int medtemp = median[i].returnMedian();
						if (medtemp != -1) {
							cout << "Median match price of Stock " << i 
							<< " at time " << currentTime << " is $" 
							<< medtemp << '\n';
						}
					}
				}
				currentTime = timeStamp; 
			}
			Order a(traderID, stockID, price, quantity, timeStamp, tCount);
			if (isT) {
				updateTimeTravel(stockID, BS, timeStamp, price);
			}
			if (BS) {
				while (!sell[stockID].empty() && !complete && 
				(sell[stockID].top()).price <= price) {
					if ((sell[stockID].top()).quantity > a.quantity) {
						(sell[stockID].top()).quantity -= a.quantity;
						if (isV) {
							cout << "Trader " << traderID << " purchased " 
							<< a.quantity << " shares of Stock " << stockID
						  << " from Trader " << (sell[stockID].top()).traderId
							<< " for $" << (sell[stockID].top()).price 
							<< "/share" << '\n';
						}
						if (isI) {
							(traderInfo[traderID])[0] += a.quantity;
							(traderInfo[(sell[stockID].top()).traderId])[1] 
							+= a.quantity;
							(traderInfo[traderID])[2] 
							-= (a.quantity * (sell[stockID].top()).price);
							(traderInfo[(sell[stockID].top()).traderId])[2] 
							+= (a.quantity * (sell[stockID].top()).price);
						}
						totalTrades++;
						if (isM) {
						   median[stockID].addNum((sell[stockID].top()).price);
						}
						complete = true;
					}
					else if ((sell[stockID].top()).quantity < a.quantity) {
						a.quantity -= (sell[stockID].top()).quantity;
						if (isV) {
							cout << "Trader " << traderID << " purchased " 
							<< (sell[stockID].top()).quantity 
						  << " shares of Stock " << stockID << " from Trader "
								<< (sell[stockID].top()).traderId << " for $" 
								<< (sell[stockID].top()).price << "/share" << '\n';
						}
						if (isI) {
							(traderInfo[traderID])[0] 
							+= (sell[stockID].top()).quantity;
							(traderInfo[(sell[stockID].top()).traderId])[1] 
							+= (sell[stockID].top()).quantity;
							(traderInfo[traderID])[2] 
							-= ((sell[stockID].top()).quantity 
							* (sell[stockID].top()).price);
							(traderInfo[(sell[stockID].top()).traderId])[2] 
							+= ((sell[stockID].top()).quantity 
							* (sell[stockID].top()).price);
						}
						totalTrades++;
						if(isM) {
						   median[stockID].addNum((sell[stockID].top()).price);
						}
						sell[stockID].pop();
					}
					else {
						if (isV) {
							cout << "Trader " << traderID << " purchased " 
							<< a.quantity << " shares of Stock " << stockID 
						   << " from Trader " << (sell[stockID].top()).traderId
							<< " for $" << (sell[stockID].top()).price 
							<< "/share" << '\n';
						}
						totalTrades++;
						if (isI) {
							(traderInfo[traderID])[0] += a.quantity;
							(traderInfo[(sell[stockID].top()).traderId])[1] 
							+= a.quantity;
							(traderInfo[traderID])[2] -= (a.quantity 
							* (sell[stockID].top()).price);
							(traderInfo[(sell[stockID].top()).traderId])[2] 
							+= (a.quantity * (sell[stockID].top()).price);
						}
						if (isM) {
						   median[stockID].addNum((sell[stockID].top()).price);
						}
						complete = true;
						sell[stockID].pop();
					}
				}
				if (!complete) {
					buy[stockID].push(a);
				}
			}
			else {
			while (!buy[stockID].empty() && !complete && 
			(buy[stockID].top()).price >= price) {
				if ((buy[stockID].top()).quantity > a.quantity) {
					(buy[stockID].top()).quantity -= a.quantity;
					if (isV) {
						cout << "Trader " << (buy[stockID].top()).traderId 
						<< " purchased " << a.quantity << " shares of Stock " 
						<< stockID << " from Trader " << a.traderId << " for $"
						<< (buy[stockID].top()).price << "/share" << '\n';
						}
						if (isI) {
							(traderInfo[(buy[stockID].top()).traderId])[0] 
							+= a.quantity;
							(traderInfo[a.traderId])[1] += a.quantity;
							(traderInfo[(buy[stockID].top()).traderId])[2] 
							-= (a.quantity * (buy[stockID].top()).price);
							(traderInfo[a.traderId])[2] 
							+= (a.quantity * (buy[stockID].top()).price);
						}
						totalTrades++;
						if (isM) {
							median[stockID].addNum((buy[stockID].top()).price);
						} 
						complete = true;
					}
					else if ((buy[stockID].top()).quantity < a.quantity) {
						a.quantity -= (buy[stockID].top()).quantity;
						if (isV) {
							cout << "Trader " << (buy[stockID].top()).traderId 
							<< " purchased " << (buy[stockID].top()).quantity
							<< " shares of Stock " << stockID << " from Trader "
							<< a.traderId << " for $" << (buy[stockID].top()).price 
							<< "/share" << '\n';
						}
						if (isI) {
							(traderInfo[(buy[stockID].top()).traderId])[0] 
							+= (buy[stockID].top()).quantity;
							(traderInfo[a.traderId])[1] 
							+= (buy[stockID].top()).quantity;
							(traderInfo[(buy[stockID].top()).traderId])[2] 
							-= ((buy[stockID].top()).quantity 
							* (buy[stockID].top()).price);
							(traderInfo[a.traderId])[2] 
							+= ((buy[stockID].top()).quantity 
							* (buy[stockID].top()).price);
						}
						totalTrades++;
						if (isM) {
							median[stockID].addNum((buy[stockID].top()).price);
						} 
						buy[stockID].pop();
					}
					else {
						if (isV) {
							cout << "Trader " << (buy[stockID].top()).traderId 
						  << " purchased " << a.quantity << " shares of Stock "
						<< stockID << " from Trader " << a.traderId << " for $"
							<< (buy[stockID].top()).price << "/share" << '\n';
						}
						if (isI) {
							(traderInfo[(buy[stockID].top()).traderId])[0] 
							+= a.quantity;
							(traderInfo[a.traderId])[1] += a.quantity;
							(traderInfo[(buy[stockID].top()).traderId])[2] 
							-= (a.quantity * (buy[stockID].top()).price);
							(traderInfo[a.traderId])[2] += (a.quantity 
							* (buy[stockID].top()).price);
						}
						totalTrades++;
						if (isM) {
							median[stockID].addNum((buy[stockID].top()).price);
						}
						complete = true;
						buy[stockID].pop();
					}
				}
				if (!complete) {
					sell[stockID].push(a);
				}
			}
		}
		if (isM) {
			for (unsigned i = 0; i < median.size(); i++) {
				int medtemp = median[i].returnMedian();
				if (medtemp != -1) {
					cout << "Median match price of Stock " << i << " at time "
						<< currentTime << " is $" << medtemp << '\n';
				}
			}
		}
		cout << "---End of Day---" << '\n' << "Trades Completed: " 
		<< totalTrades << '\n';
		if (isI) {
			cout << "---Trader Info---" << '\n';
			for (unsigned int i = 0; i < traderInfo.size(); i++) {
				cout << "Trader " << i << " bought " << (traderInfo[i])[0] 
				<< " and sold " 
					<< (traderInfo[i])[1] << " for a net transfer of $" 
					<< (traderInfo[i])[2] << '\n';
			}
		}
		if (isT) {
			cout << "---Time Travelers---" << '\n';
			for (unsigned int i = 0; i < timeTraveler.size(); i++) {
				if (timeTraveler[i].ttSellPrice > timeTraveler[i].ttBuyPrice) {
					cout << "A time traveler would buy Stock "
						<< i << " at time " << timeTraveler[i].ttBuyTime 
						<< " for $" << timeTraveler[i].ttBuyPrice
						<< " and sell it at time " << timeTraveler[i].ttSellTime 
						<< " for $" << timeTraveler[i].ttSellPrice << '\n';
				}
				else {
					cout << "A time traveler could not make a profit on Stock " 
					<< i << '\n';
				}
			}
		}
	}

private:
	vector<priority_queue<Order, vector<Order>, Buy>> buy;
	vector<priority_queue<Order, vector<Order>, Sell>> sell;
	vector<Median> median;
	vector<vector<int>> traderInfo;
	vector<TimeTravel> timeTraveler;
	unsigned int numTrader;
	unsigned int numStock;
	bool isV; //verbose
	bool isM; //median
	bool isI; //-i
	bool isT; //time travel
	bool BS; //BUY == true, SELL == false
	bool TP; // TL == true, PR == false

};

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
	market market;
	market.getMode(argc, argv);
	market.readIn();
	return 0;
}
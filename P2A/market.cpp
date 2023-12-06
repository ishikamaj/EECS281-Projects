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
struct TT {
	TT() {
		solBuyTime = -1;
		solBuyPrice = -1;
		solSellTime = -1;
		solSellPrice = -1;
		potBuyTime = -1;
		potBuyPrice = -1;
	}
	int solBuyTime;
	int solBuyPrice;
	int solSellTime;
	int solSellPrice;
	int potBuyTime;
	int potBuyPrice;
};
struct Med {
	void addNewNum(int toAdd) {
		if (min.empty() || max.empty()) {
			max.push(toAdd);
		}
		else {
			if (toAdd >= max.top()) {
				min.push(toAdd);
			}
			else {
				max.push(toAdd);
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
	int returnMed() {
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

struct BuyCmp {
	bool operator()(const Order& lhs, const Order& rhs) const {
		//return lhs.price < rhs.price;
		return (lhs.price == rhs.price) ? 
			lhs.tCount > rhs.tCount : lhs.price < rhs.price;
	}
};
struct SellCmp {
	bool operator()(const Order& lhs, const Order& rhs) const {
		//return lhs.price > rhs.price;
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
			{ "verbose",    no_argument,       nullptr, 'v' },
			{ "median",    no_argument,       nullptr, 'm' },
			{ "trader_info",    no_argument,       nullptr, 'i' },
			{ "time_travelers",    no_argument,       nullptr, 't' },
			{ nullptr, 0,                 nullptr, '\0' }
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

	void updateTT(int i, bool BS, int time, int price) {
		if (BS) { //B
			if (tt[i].potBuyTime == -1) {
				if (tt[i].solBuyTime == -1) {
					return;
				}
				else if (tt[i].solSellPrice == -1 && price > tt[i].solBuyPrice) {
					tt[i].solSellTime = time;
					tt[i].solSellPrice = price;
				}
				else if (tt[i].solSellPrice != -1 && price > tt[i].solSellPrice) {
					tt[i].solSellTime = time;
					tt[i].solSellPrice = price;
				}
			}
			else {
				if (tt[i].solSellPrice != -1 && price > tt[i].solSellPrice) {
					tt[i].solSellTime = time;
					tt[i].solSellPrice = price;
				}
				if ((price - tt[i].potBuyPrice) > (tt[i].solSellPrice - tt[i].solBuyPrice)) {
					tt[i].solBuyTime = tt[i].potBuyTime;
					tt[i].solBuyPrice = tt[i].potBuyPrice;
					tt[i].potBuyTime = -1;
					tt[i].potBuyPrice = -1;
					tt[i].solSellTime = time;
					tt[i].solSellPrice = price;
				}
			}
			
		}
		else {    //S Buy from
			if (tt[i].solBuyTime == -1) {
				tt[i].solBuyTime = time;
				tt[i].solBuyPrice = price;
			}
			else if (price < tt[i].solBuyPrice && tt[i].solSellTime == -1) {
				tt[i].solBuyTime = time;
				tt[i].solBuyPrice = price;
			}
			else if (price < tt[i].solBuyPrice && tt[i].solSellTime != -1) {
				if (tt[i].potBuyTime == -1 || price < tt[i].potBuyPrice) {
					tt[i].potBuyTime = time;
					tt[i].potBuyPrice = price;
				}
			}
		}
		return;
	}

	void readIn() {
		cout << "Processing orders..." << '\n';
		string temp;
		string trash;
		char temp2;
		int tCount = 0;//the running time for ordering
		int timeStamp;//time of incoming order
		int currentTime = 0;//modified running time by ordertime
		int traderID;
		int stockID;
		int price;
		int quantity;
		int ProcessedNum = 0;
		getline(cin, trash);
		cin >> trash  >> temp >> trash >> numTrader >> trash >> numStock;
		buy.reserve(numStock);
		sell.reserve(numStock);
		if (isM) median.reserve(numStock);
		if (isT) tt.reserve(numStock);
		priority_queue<Order, vector<Order>, BuyCmp> mid1;
		priority_queue<Order, vector<Order>, SellCmp> mid2;
		Med mid3;
		TT mid4;
		for (unsigned int i = 0; i < numStock; i++) {
			buy.push_back(mid1);
			sell.push_back(mid2);
			if (isM) median.push_back(mid3);
			if (isT)tt.push_back(mid4);
		}
		if (isI) {
			trInfo.reserve(numTrader);
			vector<int> mid5 = { 0,0,0 };
			for (unsigned int i = 0; i < numTrader; i++) {
				trInfo.push_back(mid5);
			}
		}

		stringstream ss;
		if (temp == "PR") {
			TP = false;
			unsigned int seed;
			unsigned int numOrder;
			unsigned int arrivalRate;
			cin >> trash >> seed >> trash >> numOrder >> trash >> arrivalRate;
			P2random::PR_init(ss, seed, numTrader, numStock, numOrder, arrivalRate);
		}
		else {
			TP = true;
		}
		istream & inputStream = (TP) ? cin : ss;
		

		while (inputStream >> timeStamp) {
			if (timeStamp < 0 || timeStamp < currentTime) {
				cerr << "haha you screwed up" << endl;
				exit(1);
			}
			tCount++;
			bool complete = false;
			inputStream >> temp >> temp2 >> traderID >> temp2 >>
				stockID >> temp2 >> price >> temp2 >> quantity;
			if (traderID < 0 || traderID >= static_cast<int>(numTrader) || stockID < 0 || stockID >= static_cast<int>(numStock)) {
				cerr << "haha you screwed up" << endl;
				exit(1);
			}
			if (price <= 0 || quantity <= 0) {
				cerr << "haha you screwed up" << endl;
				exit(1);
			}
			BS = (temp == "BUY") ? true : false;
			if (timeStamp != currentTime) { 
				if (isM) {
					for (unsigned i = 0; i < median.size(); i++) {
						int mtemp = median[i].returnMed();
						if (mtemp != -1) {
							cout << "Median match price of Stock " << i << " at time "
								<< currentTime << " is $" << mtemp << '\n';
						}
					}
				}
				currentTime = timeStamp; 
			}
			Order a(traderID, stockID, price, quantity, timeStamp, tCount);
			/*if (stockID == 0 && price == 1) {
				cout << "dwuhfi" << endl;
			}*/
			if (isT) updateTT(stockID, BS, timeStamp, price);
			if (BS) {
				//if (!sell[stockID].empty()) {
					//if ((sell[stockID].top()).price <= price) {
						while (!sell[stockID].empty() && !complete && (sell[stockID].top()).price <= price) {//////////////////
							// sell[i] > buy
							if ((sell[stockID].top()).quantity > a.quantity) {
								(sell[stockID].top()).quantity -= a.quantity;
								if (isV) {
									cout << "Trader " << traderID << " purchased " << a.quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< (sell[stockID].top()).traderId << " for $" << (sell[stockID].top()).price << "/share" << '\n';
								}
								if (isI) {
									(trInfo[traderID])[0] += a.quantity;
									(trInfo[(sell[stockID].top()).traderId])[1] += a.quantity;
									(trInfo[traderID])[2] -= (a.quantity * (sell[stockID].top()).price);
									(trInfo[(sell[stockID].top()).traderId])[2] += (a.quantity * (sell[stockID].top()).price);
								}
								ProcessedNum++;
								if (isM) median[stockID].addNewNum((sell[stockID].top()).price);
								complete = true;
							}
							// sell[i] < buy
							else if ((sell[stockID].top()).quantity < a.quantity) {
								a.quantity -= (sell[stockID].top()).quantity;
								if (isV) {
									cout << "Trader " << traderID << " purchased " << (sell[stockID].top()).quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< (sell[stockID].top()).traderId << " for $" << (sell[stockID].top()).price << "/share" << '\n';
								}
								if (isI) {
									(trInfo[traderID])[0] += (sell[stockID].top()).quantity;
									(trInfo[(sell[stockID].top()).traderId])[1] += (sell[stockID].top()).quantity;
									(trInfo[traderID])[2] -= ((sell[stockID].top()).quantity * (sell[stockID].top()).price);
									(trInfo[(sell[stockID].top()).traderId])[2] += ((sell[stockID].top()).quantity * (sell[stockID].top()).price);
								}
								ProcessedNum++;
								if(isM) median[stockID].addNewNum((sell[stockID].top()).price);
								sell[stockID].pop();
							}
							else {
								if (isV) {
									cout << "Trader " << traderID << " purchased " << a.quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< (sell[stockID].top()).traderId << " for $" << (sell[stockID].top()).price << "/share" << '\n';
								}
								ProcessedNum++;
								if (isI) {
									(trInfo[traderID])[0] += a.quantity;
									(trInfo[(sell[stockID].top()).traderId])[1] += a.quantity;
									(trInfo[traderID])[2] -= (a.quantity * (sell[stockID].top()).price);
									(trInfo[(sell[stockID].top()).traderId])[2] += (a.quantity * (sell[stockID].top()).price);
								}
								if (isM) median[stockID].addNewNum((sell[stockID].top()).price);
								complete = true;
								sell[stockID].pop();
							}
						}
					//}
				//}
				if (!complete) {
					buy[stockID].push(a);
				}
			}
			else {
				//if (!buy[stockID].empty()) {
					//if ((buy[stockID].top()).price >= price) {
						while (!buy[stockID].empty() && !complete && (buy[stockID].top()).price >= price) {
							//buy[i] > sell
							if ((buy[stockID].top()).quantity > a.quantity) {
								(buy[stockID].top()).quantity -= a.quantity;
								if (isV) {
									cout << "Trader " << (buy[stockID].top()).traderId << " purchased " << a.quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< a.traderId << " for $" << (buy[stockID].top()).price << "/share" << '\n';
								}
								if (isI) {
									(trInfo[(buy[stockID].top()).traderId])[0] += a.quantity;
									(trInfo[a.traderId])[1] += a.quantity;
									(trInfo[(buy[stockID].top()).traderId])[2] -= (a.quantity * (buy[stockID].top()).price);
									(trInfo[a.traderId])[2] += (a.quantity * (buy[stockID].top()).price);
								}
								ProcessedNum++;
								if (isM) median[stockID].addNewNum((buy[stockID].top()).price);
								complete = true;
							}
							//buy[i] < sell
							else if ((buy[stockID].top()).quantity < a.quantity) {
								a.quantity -= (buy[stockID].top()).quantity;
								if (isV) {
									cout << "Trader " << (buy[stockID].top()).traderId << " purchased " << (buy[stockID].top()).quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< a.traderId << " for $" << (buy[stockID].top()).price << "/share" << '\n';
								}
								if (isI) {
									(trInfo[(buy[stockID].top()).traderId])[0] += (buy[stockID].top()).quantity;
									(trInfo[a.traderId])[1] += (buy[stockID].top()).quantity;
									(trInfo[(buy[stockID].top()).traderId])[2] -= ((buy[stockID].top()).quantity * (buy[stockID].top()).price);
									(trInfo[a.traderId])[2] += ((buy[stockID].top()).quantity * (buy[stockID].top()).price);
								}
								ProcessedNum++;
								if (isM) median[stockID].addNewNum((buy[stockID].top()).price);
								buy[stockID].pop();
							}
							else {
								if (isV) {
									cout << "Trader " << (buy[stockID].top()).traderId << " purchased " << a.quantity
										<< " shares of Stock " << stockID << " from Trader "
										<< a.traderId << " for $" << (buy[stockID].top()).price << "/share" << '\n';
								}
								if (isI) {
									(trInfo[(buy[stockID].top()).traderId])[0] += a.quantity;
									(trInfo[a.traderId])[1] += a.quantity;
									(trInfo[(buy[stockID].top()).traderId])[2] -= (a.quantity * (buy[stockID].top()).price);
									(trInfo[a.traderId])[2] += (a.quantity * (buy[stockID].top()).price);
								}
								ProcessedNum++;
								if (isM) median[stockID].addNewNum((buy[stockID].top()).price);
								complete = true;
								buy[stockID].pop();
							}
						}
					//}
				//}
				if (!complete) {
					sell[stockID].push(a);
				}
			}
		}
		if (isM) {
			for (unsigned i = 0; i < median.size(); i++) {
				int mtemp = median[i].returnMed();
				if (mtemp != -1) {
					cout << "Median match price of Stock " << i << " at time "
						<< currentTime << " is $" << mtemp << '\n';
				}
			}
		}
		cout << "---End of Day---" << '\n' << "Trades Completed: " << ProcessedNum << '\n';
		if (isI) {
			cout << "---Trader Info---" << '\n';
			for (unsigned int i = 0; i < trInfo.size(); i++) {
				cout << "Trader " << i << " bought " << (trInfo[i])[0] << " and sold " 
					<< (trInfo[i])[1] << " for a net transfer of $" << (trInfo[i])[2] << '\n';
			}
		}
		if (isT) {
			cout << "---Time Travelers---" << '\n';
			for (unsigned int i = 0; i < tt.size(); i++) {
				if (tt[i].solSellPrice > tt[i].solBuyPrice) {
					cout << "A time traveler would buy Stock "
						<< i << " at time " << tt[i].solBuyTime << " for $" << tt[i].solBuyPrice
						<< " and sell it at time " << tt[i].solSellTime << " for $" << tt[i].solSellPrice << '\n';
				}
				else {
					cout << "A time traveler could not make a profit on Stock " << i << '\n';
				}
			}
		}
	}

private:
	vector<priority_queue<Order, vector<Order>, BuyCmp>> buy;
	vector<priority_queue<Order, vector<Order>, SellCmp>> sell;
	vector<Med> median;
	vector<vector<int>> trInfo;
	vector<TT> tt;
	unsigned int numTrader;
	unsigned int numStock;
	bool isV;
	bool isM;
	bool isI;
	bool isT;
	bool TP;
	bool BS;
};

int main(int argc, char *argv[]) {
	ios_base::sync_with_stdio(false);
	market m;
	m.getMode(argc, argv);
	m.readIn();
	return 0;
}
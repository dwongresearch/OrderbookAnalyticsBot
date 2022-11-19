#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <map>
class OrderBook
{
public:
    /** construct, reading a csv data file */
    OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
    std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
    std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                          std::string product,
                                          std::string timestamp);

    /** returns the earliest time in the orderbook*/
    std::string getEarliestTime();
    /** returns the next time after the
     * sent time in the orderbook
     * If there is no next timestamp, wraps around to the start
     * */
    std::string getNextTime(std::string timestamp);
    std::string getPreviousTime(std::string timestamp);

    void insertOrder(OrderBookEntry &order);

    std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

    static double getHighPrice(std::vector<OrderBookEntry> &orders);
    static double getLowPrice(std::vector<OrderBookEntry> &orders);
    static bool requestInKnownProducts(std::string const &requestProduct,
                                       std::vector<std::string> const &currentProducts);

private:
    std::vector<OrderBookEntry> orders;
    int nextStepCounter = 0;
    std::vector<std::string> knownProducts;
    bool sortOrdersByTimestamp();
    std::map<std::string, std::vector<OrderBookEntry>> ordersSorted;
};

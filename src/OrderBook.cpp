#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>

bool OrderBook::sortOrdersByTimestamp()
{
    // written/modified by Derek Wong
    for (OrderBookEntry &e : orders)
    {
        ordersSorted[e.timestamp].push_back(e);
    }
    if (ordersSorted.size() == 0)
        return false;
    else
        return true;
}

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
    sortOrdersByTimestamp();
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    // written/modified by Derek Wong
    std::vector<std::string> products;

    std::map<std::string, bool> prodMap;
    if (knownProducts.empty())
    {
        for (OrderBookEntry &e : orders)
        {
            prodMap[e.product] = true;
        }

        // now flatten the map to a vector of strings
        for (auto const &e : prodMap)
        {
            products.push_back(e.first);
        }
        knownProducts = products;
    }
    return knownProducts;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                                 std::string product,
                                                 std::string timestamp)
{ // written/modified by Derek Wong
    std::vector<OrderBookEntry> orders_sub;
    for (const auto &timeStampedVector : ordersSorted)
    {
        if (timeStampedVector.first == timestamp)
        {
            orders = timeStampedVector.second;
            for (OrderBookEntry &e : orders)
            {
                if (e.orderType == type &&
                    e.product == product &&
                    e.timestamp == timestamp)
                {
                    orders_sub.push_back(e);
                }
            }
        }
    }
    return orders_sub;
}

// Get the Highest price of a vector of OrderBookEntries
double OrderBook::getHighPrice(std::vector<OrderBookEntry> &orders)
{
    double max = orders[0].price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price > max)
            max = e.price;
    }
    return max;
}
// Get the Lowest price of a vector of OrderBookEntries
double OrderBook::getLowPrice(std::vector<OrderBookEntry> &orders)
{
    double min = orders[0].price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price < min)
            min = e.price;
    }
    return min;
}

// Gets the Earliest time available for a set of orders timestamp
std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

// Gets the next available time
std::string OrderBook::getNextTime(std::string timestamp)
{ // written/modified by Derek Wong
    std::string next_timestamp = "";
    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

// Gets the previous time from our current timestamp
std::string OrderBook::getPreviousTime(std::string timestamp)
{ // written/modified by Derek Wong
    std::string previous_timestamp = "";
    for (int i = orders.size() - 1; i >= 0; --i)
    // reverse iteration through the vector
    {
        OrderBookEntry e = orders[i];
        if (e.timestamp < timestamp)
        {
            previous_timestamp = e.timestamp;
            break;
        }
    }
    if (previous_timestamp == "")
    {
        previous_timestamp = orders[orders.size() - 1].timestamp;
    }
    return previous_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry &order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    // asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask,
                                                 product,
                                                 timestamp);
    // bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid,
                                                 product,
                                                 timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales;

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size() - 1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size() - 1].price << std::endl;

    for (OrderBookEntry &ask : asks)
    {
        //     for bid in bids:
        for (OrderBookEntry &bid : bids)
        {
            //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
                //             sale = new order()
                //             sale.price = ask.price
                OrderBookEntry sale{ask.price, 0, timestamp,
                                    product,
                                    OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                //             # now work out how much was sold and
                //             # create new bids and asks covering
                //             # anything that was not sold
                //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # can do no more with this ask
                    //                 # go onto the next ask
                    //                 break
                    break;
                }
                //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # we adjust the bid in place
                    //                 # so it can be used to process the next ask
                    //                 bid.amount = bid.amount - ask.amount
                    bid.amount = bid.amount - ask.amount;
                    //                 # ask is completely gone, so go to next ask
                    //                 break
                    break;
                }

                //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount &&
                    bid.amount > 0)
                {
                    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # update the ask
                    //                 # and allow further bids to process the remaining amount
                    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # some ask remains so go to the next bid
                    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;
}

// sees if the requested product is in our knownProducts
bool OrderBook::requestInKnownProducts(std::string const &requestProduct,
                                       std::vector<std::string> const &currentProducts)
{ // written/modified by Derek Wong
    for (std::string currProd : currentProducts)
    {
        if (requestProduct == currProd)
        {
            return true;
        }
    }
    return false;
}
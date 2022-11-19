#include "AdvisorBot.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
// constructor
void AdvisorBot::AdvisorMain()
{
}
// initialization
void AdvisorBot::init()
{
    std::string input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}
// Prints the menu
void AdvisorBot::printMenu()
{
    // written and modified by Derek Wong
    // Print Menu Function
    std::cout << "Enter a command (type help for a list of all commands) " << std::endl;
    std::cout << "============== " << std::endl;
}
// prints the help menu
void AdvisorBot::printHelp()
{
    std::cout << "List of Commands - help, help <cmd>, prod, min, max, avg, predict, time, step, " << std::endl;
}
// prints the current time
void AdvisorBot::printTime()
{
    std::cout << "Current time is: " << currentTime << std::endl;
}
// prints market statistics
void AdvisorBot::printMarketStats()
{
    for (std::string const &p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                  p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}
// enter an ask
void AdvisorBot::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "AdvisorBot::enterAsk Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask);
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " AdvisorBot::enterAsk Bad input " << std::endl;
        }
    }
}
// Enter a bid
void AdvisorBot::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "AdvisorBot::enterBid Bad input! " << input << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid);
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << " AdvisorBot::enterBid Bad input " << std::endl;
        }
    }
}

void AdvisorBot::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void AdvisorBot::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry &sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
}
// prints the products available to trade
void AdvisorBot::printProducts()
{
    // written/modified by Derek Wong
    std::vector<std::string> products = orderBook.getKnownProducts();
    std::string productString;
    for (std::string prod : products)
    {
        productString.append(prod).append(" ");
    }
    std::cout << productString << std::endl;
}
// Gets the user command input
std::string AdvisorBot::getUserOption()
{ // written/modified by Derek Wong
    std::string userOption;
    std::getline(std::cin, userOption);
    std::cout << "You typed: " << userOption << std::endl;
    return userOption;
}
// Processes 1 word commands
void AdvisorBot::processSingleCommand(std::string const command)
{ // written/modified by Derek Wong
    // 1 word commands

    if (command == "help")
    {
        printHelp();
    }
    else if (command == "step")
    {
        gotoNextTimeframe();
    }
    else if (command == "time")
    {
        std::cout << "current time is: " << currentTime << std::endl;
    }
    else if (command == "prod")
    {
        printProducts();
    }
    else
    {
        std::cout << "not a valid command try using help " << std::endl;
    }
}
// Processes 2 word commands
void AdvisorBot::processDoubleCommand(std::vector<std::string> const tokens)
{ // written/modified by Derek Wong
  // 2 word commands

    if (tokens[0] == "help")
    {
        processHelpCommand(tokens[1]);
    }
    else if (tokens[0] == "midprice" && orderBook.requestInKnownProducts(tokens[1], orderBook.getKnownProducts()))
    // check if we have a token midprice request, the size of the command is correct and we have the product e.g. midprice BTC/USDT
    {
        midprice(tokens);
    }
    else
    {
        std::cout << "Invalid commands. type help if you need to see a list of valid operations." << std::endl;
        return;
    }
}
// Processes 3 word commands
void AdvisorBot::processTripleCommand(std::vector<std::string> const tokens)
{ // written/modified by Derek Wong

    // 3 or more token requests
    if (tokens[0] == "min" && orderBook.requestInKnownProducts(tokens[1], orderBook.getKnownProducts()))
    // check if we have a token min request, the size of the command is correct and we have the product
    {
        printMinimum(tokens);
    }
    else if (tokens[0] == "max" && orderBook.requestInKnownProducts(tokens[1], orderBook.getKnownProducts()))
    // check if we have a token max request, the size of the command is correct and we have the product
    {
        printMaximum(tokens);
    }
    else
    {
        std::cout << "Invalid commands. type help if you need to see a list of valid operations." << std::endl;
        return;
    }
}
// Processes 4 word commands
void AdvisorBot::processQuadCommand(std::vector<std::string> const tokens)
{ // written/modified by Derek Wong
    if (tokens[0] == "avg" && orderBook.requestInKnownProducts(tokens[1], orderBook.getKnownProducts()))
    // check if we have a token max request, the size of the command is correct and we have the product
    {
        double result = getAverage(tokens);
        if (result != -99999)
            std::cout << "Average " << tokens[2] << " for " << tokens[1] << " is " << std::to_string(result) << " for last " << tokens[3] << " time steps" << std::endl;

        else
            std::cout << "Average Calculation has an error please check your command" << std::endl;
    }
    else if (tokens[0] == "predict" && orderBook.requestInKnownProducts(tokens[2], orderBook.getKnownProducts()))
    // check if we have a token predict request, the size of the command is correct and we have the product
    {
        predict(tokens);
    }
    else
    {
        std::cout << "Invalid commands. type help if you need to see a list of valid operations." << std::endl;
        return;
    }
}
// Processes help cmd commands and displays the correct help text
void AdvisorBot::processHelpCommand(const std::string command)
{ // written/modified by Derek Wong
    if (command == "help")
    {
        std::cout << "Prints the Help menu with a list of products " << std::endl;
    }
    else if (command == "step")
    {
        std::cout << "Moves to the next time step." << std::endl;
    }
    else if (command == "time")
    {
        std::cout << "Current simulation time is printed." << std::endl;
    }
    else if (command == "prod")
    {
        std::cout << "List of valid products for trading" << std::endl;
    }
    else if (command == "min")
    {
        std::cout << "find minimum bid or ask for product in current time step. min ETH/BTC ask" << std::endl;
    }
    else if (command == "max")
    {
        std::cout << "find maximum bid or ask for product in current time step. max ETH/BTC ask" << std::endl;
    }
    else if (command == "avg")
    {
        std::cout << "compute average ask or bid for the sent product over the sent number of time steps. min ETH / BTC ask 10 " << std::endl;
    }
    else if (command == "predict")
    {
        std::cout << "predict max or min ask or bid for the sent product for the next time step. predict max ETH/BTC bid " << std::endl;
    }
    else if (command == "midprice")
    {
        std::cout << "find the midprice between the lowest ask and the highest bid. midprice BTC/USDT " << std::endl;
    }
    else
    {
        std::cout << "not a valid command try using help to list valid commands and run help <cmd> again " << std::endl;
    }
}
// prints the minimum
void AdvisorBot::printMinimum(const std::vector<std::string> tokens)
{ // written/modified by Derek Wong
    if (tokens[2] == "bid")
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid,
                                                                  tokens[1], currentTime);
        double minPrice = OrderBook::getLowPrice(entries);

        std::cout << "Min Bid for " << tokens[1] << " " << minPrice << std::endl;
    }
    else if (tokens[2] == "ask")
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                  tokens[1], currentTime);
        double minPrice = OrderBook::getLowPrice(entries);

        std::cout << "Min Bid for " << tokens[1] << " " << minPrice << std::endl;
    }
}
// Prints the maximum
void AdvisorBot::printMaximum(const std::vector<std::string> tokens)
{ // written/modified by Derek Wong
    if (tokens[2] == "bid")
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid,
                                                                  tokens[1], currentTime);
        double maxPrice = OrderBook::getHighPrice(entries);

        std::cout << "Max Bid for " << tokens[1] << " " << maxPrice << std::endl;
    }
    else if (tokens[2] == "ask")
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                  tokens[1], currentTime);
        double maxPrice = OrderBook::getHighPrice(entries);

        std::cout << "Max Bid for " << tokens[1] << " " << maxPrice << std::endl;
    }
}

// Gets the average of all bids or all asks for a set of user command tokens
double AdvisorBot::getAverage(const std::vector<std::string> tokens)
{
    // finds the average of all bids or all asks over the given time steps
    // written/modified by Derek Wong
    // Intializing variables
    double sum = 0;
    int lookBack = 0;
    std::vector<OrderBookEntry> totalEntries;
    std::string time = orderBook.getPreviousTime(currentTime);

    try
    {
        lookBack = std::stoi(tokens[3]);
    }
    catch (const std::exception &e)
    {
        std::cout << " AdvisorBot::processUserOption Avg Bad input " << std::endl;
    }

    for (int i = 0; i < lookBack; ++i)
    {
        if (tokens[2] == "bid")
        {
            std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid,
                                                                      tokens[1], time);
            for (OrderBookEntry entry : entries)
            {
                totalEntries.push_back(entry);
            }
            time = orderBook.getPreviousTime(time);
        }
        else if (tokens[2] == "ask")
        {
            std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                      tokens[1], time);
            for (OrderBookEntry entry : entries)
            {
                totalEntries.push_back(entry);
            }
            time = orderBook.getPreviousTime(time);
        }
        else
        {
            std::cout << "Please enter a valid type Bid/Ask with command formag avg prod bid/ask lookback e.g. avg ETH/BTC bid 10" << std::endl;
        }
    }
    if (totalEntries.empty() == 0)
    {
        for (OrderBookEntry entry : totalEntries)
        {
            sum += entry.price;
        }
        double average = sum / totalEntries.size();
        return average;
    }
    else
    {
        std::cout << "AdvisorBot::processUserOption Avg totalEntries is empty" << std::endl;
        return -99999;
    }
}

// Prediction function for a set of user command tokens
void AdvisorBot::predict(const std::vector<std::string> tokens)
{
    // finds the average of the HIGHEST bid or LOWEST asks over last 50 timesteps
    // written/modified by Derek Wong
    // Intializing variables
    double sum = 0;
    std::vector<double> orderValues;
    std::string time = orderBook.getPreviousTime(currentTime);

    for (int i = 0; i < 50; ++i)
    {
        if (tokens[3] == "bid")
        {
            if (tokens[1] == "max")
            {
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid,
                                                                          tokens[2], time);

                orderValues.push_back(OrderBook::getHighPrice(entries));
                time = orderBook.getPreviousTime(time);
            }
            else if (tokens[1] == "min")
            {
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::bid,
                                                                          tokens[2], time);
                orderValues.push_back(OrderBook::getLowPrice(entries));
                time = orderBook.getPreviousTime(time);
            }
        }
        else if (tokens[3] == "ask")
        {
            if (tokens[1] == "max")
            {
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                          tokens[2], time);

                orderValues.push_back(OrderBook::getHighPrice(entries));
                time = orderBook.getPreviousTime(time);
            }
            else if (tokens[1] == "min")
            {
                std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                                                          tokens[2], time);
                orderValues.push_back(OrderBook::getLowPrice(entries));
                time = orderBook.getPreviousTime(time);
            }
        }
        else
        {
            std::cout << "Please enter a valid type Bid/Ask with command formag avg prod bid/ask lookback e.g. predict max/min ETH/BTC bid/ask" << std::endl;
        }
    }
    if (orderValues.empty() == 0)
    {
        for (double orderPrice : orderValues)
        {
            sum += orderPrice;
        }
        double average = sum / orderValues.size();
        std::cout << "Predict " << tokens[1] << " " << tokens[3] << " for " << tokens[2] << " is " << std::to_string(average) << " for last 50 time steps" << std::endl;
    }
    else
    {
        std::cout << "AdvisorBot::predict orderValues is empty" << std::endl;
        return;
    }
}

// Calculates the midprice function for a tokenized user input
void AdvisorBot::midprice(const std::vector<std::string> tokens)
{
    // written/modified by Derek Wong
    std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask,
                                                           tokens[1], currentTime);

    std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid,
                                                           tokens[1], currentTime);

    double midprice = (OrderBook::getHighPrice(bids) + OrderBook::getLowPrice(asks)) / 2;
    std::cout << "Midprice for " << tokens[1] << " is " << midprice << std::endl;
}

// Processes the User Option that was input
void AdvisorBot::processUserOption(std::string const userOption)
{
    // written/modified by Derek Wong
    std::vector<std::string> tokens;

    // deal with bad inputs
    if (userOption.empty())
    {
        std::cout << "Empty String try again" << std::endl;
        return;
    }

    // try catch to read the string and tokenize
    try
    {
        tokens = CSVReader::tokenise(userOption, ' ');
    }
    catch (const std::exception &e)
    {
        std::cout << " AdvisorBot::processUserOption Bad input " << std::endl;
        return;
    }

    // valid inputs
    if (tokens.size() == 1)
    {
        std::string command = tokens[0];
        processSingleCommand(command);
    }
    // 2 sized token requests
    else if (tokens.size() == 2)
    {
        processDoubleCommand(tokens);
    }
    // 3 sized
    else if (tokens.size() == 3)

    {
        processTripleCommand(tokens);
    }
    // 4 sized
    else if (tokens.size() == 4)
    {
        processQuadCommand(tokens);
    }
    // Errors
    else
    {
        // bad input
        std::cout << "Invalid commands. type help if you need to see a list of valid operations." << std::endl;
        return;
    }
}

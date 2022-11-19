#pragma once

#include <vector>
#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"

class AdvisorBot
{
public:
    void AdvisorMain();
    /** Call this to start the sim */
    void init();

private:
    void printMenu();
    void printProducts();
    void printTime();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    std::string getUserOption();
    void processUserOption(const std::string userOption);
    void processSingleCommand(const std::string command);
    void processDoubleCommand(const std::vector<std::string> tokens);
    void processTripleCommand(const std::vector<std::string> tokens);
    void processQuadCommand(const std::vector<std::string> tokens);
    void processHelpCommand(const std::string command);
    void printMinimum(const std::vector<std::string> tokens);
    void printMaximum(const std::vector<std::string> tokens);
    double getAverage(const std::vector<std::string> tokens);
    void predict(const std::vector<std::string> tokens);
    void midprice(const std::vector<std::string> tokens);

    std::string currentTime;

    //        OrderBook orderBook{"20200317.csv"};
    OrderBook orderBook{"20200601.csv"};
    Wallet wallet;
};

# OrderbookAnalyticsBot
An orderbook replayer + matching engine with CLI interface. An analytics bot that takes in orderbook information

A C++ CLI application that replays 2 days worth of tickdata for  cryptocurrencies (BTC/USD, ETH/USD, and BTC/ETH) with supporting wallet account methods for BTC, ETH, and USD. This is a batteries included 'toy' that includes a trade blotter to facilitate calculation of P&L and a matching engine in order to determine execution of submitted orders for user interaction with the order book. The analytics bot uses order book information to give a simple suggestion to the user if asked. This funciton is currently only using the best bid and offer as input but the archtecture for analytics has been built out to give 'recommendations'.

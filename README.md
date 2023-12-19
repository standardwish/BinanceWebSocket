# Binance Web Socket Project
The receiving information is so called market data of bitcoin, aka ‘btc’. This is equivalent to 
buying/selling information about btc at the moment, it’s same as the stock market. Let’s try to receive 
the market data of btc@usdt (referred to as ‘btcusdt’, ‘usdt’ implies US dollars)

# Output
Data is sent in real-time to a txt file.
```bash
// tail -n 10 stream-data-binance.txt
btcusdt@trade,p:"43194.96000000",q:"0.00052000"
btcusdt@trade,p:"43194.97000000",q:"0.01223000"
btcusdt@trade,p:"43194.96000000",q:"0.00065000"
btcusdt@orderbook,bids:[["43194.96000000","3.46539000"],["43194.49000000","0.23160000"],["43194.29000000","0.23160000"],["43194.00000000","0.11588000"],["43193.34000000","0.00062000"]]
btcusdt@trade,p:"43194.96000000",q:"0.00620000"
btcusdt@trade,p:"43194.96000000",q:"0.00035000"
btcusdt@trade,p:"43194.96000000",q:"0.00031000"
btcusdt@trade,p:"43194.96000000",q:"0.00023000"
btcusdt@orderbook,bids:[["43194.96000000","3.70480000"],["43194.49000000","0.23160000"],["43194.29000000","0.23160000"],["43194.00000000","0.11588000"],["43193.34000000","0.00062000"]]
btcusdt@trade,p:"43194.97000000",q:"0.50000000"
```


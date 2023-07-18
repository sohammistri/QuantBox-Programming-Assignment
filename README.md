# QuantBox Programming Assignment

The following is the description of files in this project:

* ```Packets.h``` contains the ```Packet``` class for an individual Packet and ```Message``` class for individual messages. Different types of messages are addressed by extending the base ```Message``` class into various derived classes based on the type of messages like ```NewMessage, TradeMessage, CancelMessage, ModifyMessage, QueryMessage```.

* ```OrderBook.h``` contains different classes for creation of the Order Book. It has the ```Order``` class for dealing with an individual order and the ```Instrument``` class for keeping track of the bid,ask,quantities and counts of respective instruments.

* ```Decoder.cpp``` is a collection of functions devoted to decoding the messages in the Packets and saving them in a **Min Priority Queue** as per their timestamp. This is done by the ```decodeFile``` function.

* ```CreateOrderBook.cpp``` is a collection of functions devoted to creation of the Order Book by decoding an individual packet. This involves adding new orders to the book, modifying them, removing them in case of trade and cancel orders, and returning appropriate results when queried about them. This is done by the ```createOrderBook``` function.

* ```main.cpp``` is simply the file with the ```main()``` function in it which calls the required functions sequentially and store the query results in a file.

# How to run
To run the code, please use ```run.sh``` file.
Run the following commands.

```
chmod +x run.sh
./run.sh [PATH_OF DIR WITH ALL DAT FILES WITH PACKET INFO] [OUTPUT FILE PATH]
```
Please note only ```.dat``` files are supported for packet decoding, similar to the format provided in sample data.

To test the output file, kindly run
```
itchresponsereader [PATH TO OUTPUT FILE]
```
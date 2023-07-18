/*
    Header file containing class declarations and definitions
    for Order and Instrument.
*/
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
using namespace std;

class Order{
    uint64_t price, quantity, orderId;
    uint32_t symbolId;
    uint8_t side;
public:
    Order(uint64_t p, uint64_t q, uint32_t sId, uint64_t oId, uint8_t s): price(p),quantity(q),symbolId(sId),orderId(oId),side(s)
    {}

    const uint64_t& getPrice() const  { return price; }
    const uint64_t& getQuantity() const  { return quantity; }
    const uint32_t& getSymbolId() const  { return symbolId; }
    const uint64_t& getOrderId() const  { return orderId; }
    const uint8_t& getSide() const  { return side; }
    void setQuantity(uint64_t q) { quantity=q; }
    friend ostream & operator<<(ostream &os, const Order &m){
        os<<"Order Details:"<<endl;
        os<<"Price : "<<m.getPrice()<<endl;
        os<<"Quantity : "<<m.getQuantity()<<endl;
        os<<"SymbolID : "<<m.getSymbolId()<<endl;
        os<<"OrderID : "<<m.getOrderId()<<endl;
        os<<"Side : "<<(unsigned)(m.getSide())<<endl;
        return os;
    }
};

class Instrument{
    uint32_t symbolId;
    uint64_t bidPrice, askPrice;
    unordered_map<uint64_t,uint64_t> bidBook,askBook;
    unordered_map<uint64_t,uint32_t> bidCount,askCount;
    set<uint64_t> bidPrices,askPrices;
public:
    Instrument(uint32_t sId): symbolId(sId),bidPrice(0),askPrice(0){}
    const uint32_t& getSymbolId() const  { return symbolId; }
    const uint64_t& getBidPrice(uint64_t const& def=0) const  { 
        if(bidPrices.size()>0){
            return *(bidPrices.rbegin());
        }
        else{
            return def;
        }
    }
    const uint64_t& getAskPrice(uint64_t const& def=0) const  { 
        if(askPrices.size()>0){
            return *(askPrices.begin());
        }
        else{
            return def;
        }
    }
    const uint64_t& getBidQuantity(uint64_t const& def=0) const  { 
        if(!bidPrices.empty()){
            return bidBook.at(*(bidPrices.rbegin()));
        }    
        else{
            return def;
        }
    }
    const uint64_t& getAskQuantity(uint64_t const& def=0) const  { 
        if(!askPrices.empty()){
            return askBook.at(*(askPrices.begin()));
        }    
        else{
            return def;
        }
    }
    const uint32_t& getBidCount(uint32_t const& def=0) const  { 
        if(!bidPrices.empty()){
            return bidCount.at(*(bidPrices.begin()));
        }    
        else{
            return def;
        }
    }
    const uint32_t& getAskCount(uint32_t const& def=0) const  { 
        if(!askPrices.empty()){
            return askCount.at(*(askPrices.begin()));
        }    
        else{
            return def;
        }
    }
    const unordered_map<uint64_t,uint64_t>& getBidBook() const { return bidBook; }
    const unordered_map<uint64_t,uint64_t>& getAskBook() const { return bidBook; }
    const unordered_map<uint64_t,uint32_t>& getBidCounts() const { return bidCount; }
    const unordered_map<uint64_t,uint32_t>& getAskCounts() const { return askCount; }

    void setBid(uint64_t bp, uint64_t bq, int sign, char msgType){
        //check if bp in bidBook
        if(bidBook.find(bp)!=bidBook.end()){
            if(sign==1)
                bidBook[bp]+=bq;
            else
                bidBook[bp]-=bq;

            if(msgType=='N'){
                bidCount[bp]++;
            }
            else if(msgType=='T'||msgType=='C'){
                bidCount[bp]--;
            }
            if(bidBook[bp]==0){
                this->removeBid(bp);
            }
        }
        else if(msgType!='N'){
            return;
        }
        else{
            //else create record
            bidBook[bp]=bq;
            bidCount[bp]=1;
            bidPrices.insert(bp);
        }
    }

    void setAsk(uint64_t ap, uint64_t aq, int sign, char msgType){
        //check if ap in askBook
        if(askBook.find(ap)!=askBook.end()){
            if(sign==1)
                askBook[ap]+=aq;
            else
                askBook[ap]-=aq;

            if(msgType=='N'){
                askCount[ap]++;
            }
            else if(msgType=='T'||msgType=='C'){
                askCount[ap]--;
            }
            if(askBook[ap]==0){
                this->removeAsk(ap);
            }
        }
        else if(msgType!='N'){
            return;
        }
        else{
            //else create record
            askBook[ap]=aq;
            askCount[ap]=1;
            askPrices.insert(ap);
        }
    }

    void removeBid(uint64_t bp){
        bidBook.erase(bp);
        bidCount.erase(bp);
        bidPrices.erase(bp);
    }

    void removeAsk(uint64_t ap){
        askBook.erase(ap);
        askCount.erase(ap);
        askPrices.erase(ap);
    }

    friend ostream & operator<<(ostream &os, const Instrument &i){
        os<<"Instrument Id : "<<i.getSymbolId()<<endl;
        os<<"------Bid Book------"<<endl;
        os<<"Price    | Quantity    | Count     "<<endl;
        unordered_map<uint64_t,uint64_t> bidBook = i.getBidBook();
        unordered_map<uint64_t,uint32_t> bidCount = i.getBidCounts();
        for(const auto &item:i.bidBook){
            os<<item.first<<"    |"<<item.second<<"    |"<<bidCount[item.first]<<endl;
        }

        os<<"------Ask Book------"<<endl;
        os<<"Price    | Quantity    | Count     "<<endl;
        unordered_map<uint64_t,uint64_t> askBook = i.getAskBook();
        unordered_map<uint64_t,uint32_t> askCount = i.getAskCounts();
        for(const auto &item:i.askBook){
            os<<item.first<<"    |"<<item.second<<"    |"<<askCount[item.first]<<endl;
        }
        os<<"----------------------------------"<<endl;
        return os;
    }
};

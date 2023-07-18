/*
    Header file containing class declarations and definitions
    for Packets and Messages associated with them.
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;

class Message{
protected:
    char msgType;
public:
    Message(char mType):msgType(mType){}
    const char& getMsgType() const  { return msgType; }
    virtual ~Message() = default;
};

class NewMessage: public Message{
    uint64_t price,quantity,orderId;
    uint32_t symbolId;
    uint8_t side;
public:
    NewMessage(char mType, uint64_t p, uint64_t q, uint32_t sId, uint64_t oId, uint8_t s): Message(mType),price(p),quantity(q),symbolId(sId),orderId(oId),side(s)
    {}
    const uint64_t& getPrice() const  { return price; }
    const uint64_t& getQuantity() const  { return quantity; }
    const uint32_t& getSymbolId() const  { return symbolId; }
    const uint64_t& getOrderId() const  { return orderId; }
    const uint8_t& getSide() const  { return side; }
    friend ostream & operator<<(ostream &os, const NewMessage &m){
        os<<"New Message Details:"<<endl;
        os<<"Price : "<<m.getPrice()<<endl;
        os<<"Quantity : "<<m.getQuantity()<<endl;
        os<<"SymbolID : "<<m.getSymbolId()<<endl;
        os<<"OrderID : "<<m.getOrderId()<<endl;
        os<<"Side : "<<(unsigned)(m.getSide())<<endl;
        return os;
    }
};

class CancelMessage: public Message{
    friend ostream & operator<<(ostream &os, const CancelMessage &s);
    uint64_t orderId;
public:
    CancelMessage(char mType, uint64_t oId): Message(mType),orderId(oId)
    {}
    const uint64_t& getOrderId() const  { return orderId; }
    friend ostream & operator<<(ostream &os, const CancelMessage &m){
        os<<"Cancel Message Details:"<<endl;
        os<<"OrderID : "<<m.getOrderId()<<endl;
        return os;
    }

};

class TradeMessage: public Message{
    friend ostream & operator<<(ostream &os, const TradeMessage &s);
    uint64_t orderId;
public:
    TradeMessage(char mType, uint64_t oId): Message(mType),orderId(oId)
    {}
    const uint64_t& getOrderId() const  { return orderId; }
    friend ostream & operator<<(ostream &os, const TradeMessage &m){
        os<<"Trade Message Details:"<<endl;
        os<<"OrderID : "<<m.getOrderId()<<endl;
        return os;
    }
};

class ModifyMessage: public Message{
    friend ostream & operator<<(ostream &os, const ModifyMessage &s);
    uint64_t price,quantity,orderId;
public:
    ModifyMessage(char mType, uint64_t p, uint64_t q, uint64_t oId): Message(mType),price(p),quantity(q),orderId(oId)
    {}
    const uint64_t& getPrice() const  { return price; }
    const uint64_t& getQuantity() const  { return quantity; }
    const uint64_t& getOrderId() const  { return orderId; }
    friend ostream & operator<<(ostream &os, const ModifyMessage &m){
        os<<"Modify Message Details:"<<endl;
        os<<"Price : "<<m.getPrice()<<endl;
        os<<"Quantity : "<<m.getQuantity()<<endl;
        os<<"OrderID : "<<m.getOrderId()<<endl;
        return os;
    }
};

class QueryMessage: public Message{
    friend ostream & operator<<(ostream &os, const QueryMessage &s);
    uint32_t symbolId;
public:
    QueryMessage(char mType, uint32_t sId): Message(mType),symbolId(sId)
    {}
    const uint32_t& getSymbolId() const  { return symbolId; }
    friend ostream & operator<<(ostream &os, const QueryMessage &m){
        os<<"Query Message Details:"<<endl;
        os<<"SymbolID : "<<m.getSymbolId()<<endl;
        return os;
    }
};

class Packet{
    friend ostream & operator<<(ostream &os, const Packet &p);
    uint64_t timeStamp;
    uint32_t numMsg;
    vector<Message*> msgList;

public:
    Packet(){}
    Packet(uint64_t ts, uint32_t nmsg, vector<Message*> &mList){
        timeStamp=ts;
        numMsg=nmsg;
        copy(mList.begin(), mList.end(), back_inserter(msgList));
    }
    const uint64_t& getTimeStamp() const  { return timeStamp; }
    const uint32_t& getNumMsg() const  { return numMsg; }
    vector<Message*> const &getMsgList() const { return msgList; }
    friend ostream & operator<<(ostream &os, const Packet &p){
        os<<"Timestamp : "<<p.getTimeStamp()<<endl;
        os<<"Number of messages : "<<p.getNumMsg()<<endl;
        for(const Message* m:p.getMsgList()){
            char msgType=m->getMsgType();
            if(msgType=='N'){
                const NewMessage *nm = dynamic_cast<const NewMessage*>(m);
                cout<<*nm;
            }
            else if(msgType=='C'){
                const CancelMessage *cm = dynamic_cast<const CancelMessage*>(m);
                cout<<*cm;
            }
            else if(msgType=='T'){
                const TradeMessage *tm = dynamic_cast<const TradeMessage*>(m);
                cout<<*tm;
            }
            else if(msgType=='M'){
                const ModifyMessage *mm = dynamic_cast<const ModifyMessage*>(m);
                cout<<*mm;
            }
            else if(msgType=='Q'){
                const QueryMessage *qm = dynamic_cast<const QueryMessage*>(m);
                cout<<*qm;
            }
        }
        cout<<"-------------------------------------"<<endl;
        return os;
    }
};

struct PacketCmp{
    bool operator()(const Packet* p1, const Packet* p2){
        return p1->getTimeStamp()>p2->getTimeStamp();
    }
};



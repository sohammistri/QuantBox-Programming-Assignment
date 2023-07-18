#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "Packets.h"
#include "OrderBook.h"
using namespace std;
typedef unordered_map<uint64_t, Order*> ol;
typedef unordered_map<uint32_t, Instrument*> il;

void decodeNewMessage(const NewMessage* &nm, ol &orderList, il &instrumentList){
    uint64_t price,quantity,orderId;
    uint32_t symbolId;
    uint8_t side;
    price=nm->getPrice();
    quantity=nm->getQuantity();
    orderId=nm->getOrderId();
    symbolId=nm->getSymbolId();
    side=nm->getSide();
    Order *o=new Order(price, quantity, symbolId, orderId, side);
    orderList[orderId]=o;

    Instrument *tempI=new Instrument(symbolId);
    if(instrumentList.find(symbolId)!=instrumentList.end()){
        Instrument *inst=instrumentList[symbolId];
        if(side==0){
            instrumentList[symbolId]->setBid(price, quantity, 1, 'N');
        }
        else{
            instrumentList[symbolId]->setAsk(price, quantity, 1, 'N');
        }
    }
    else{
        Instrument *inst=new Instrument(symbolId);
        if(side==0){
            inst->setBid(price, quantity, 1, 'N');
        }
        else{
            inst->setAsk(price, quantity, 1, 'N');
        }
        instrumentList[symbolId]=inst;
    }
}

void decodeModifyMessage(const ModifyMessage* &mm, ol &orderList, il &instrumentList){
    uint64_t price,quantity,orderId;
    price=mm->getPrice();
    quantity=mm->getQuantity();
    orderId=mm->getOrderId();
    if(orderList.find(orderId)!=orderList.end()){
        uint64_t oldQ = orderList[orderId]->getQuantity();
        uint32_t symbolId = orderList[orderId]->getSymbolId();
        uint8_t side = orderList[orderId]->getSide();
        orderList[orderId]->setQuantity(quantity);
        if(instrumentList.find(symbolId)!=instrumentList.end()){
            int sign = (quantity>oldQ)?1:-1;
            uint64_t qChange = (quantity>oldQ)?(quantity-oldQ):(oldQ-quantity);
            if(side==0){
                instrumentList[symbolId]->setBid(price, qChange, sign, 'M');
            }
            else{
                instrumentList[symbolId]->setAsk(price, qChange, sign, 'M');
            }
        }
    }
}

void decodeCancelMessage(const CancelMessage* &cm, ol &orderList, il &instrumentList){
    uint64_t orderId=cm->getOrderId();
    if(orderList.find(orderId)!=orderList.end()){
        uint64_t quantity = orderList[orderId]->getQuantity();
        uint64_t price = orderList[orderId]->getPrice();
        uint32_t symbolId = orderList[orderId]->getSymbolId();
        uint8_t side = orderList[orderId]->getSide();
        if(instrumentList.find(symbolId)!=instrumentList.end()){
            if(side==0){
                instrumentList[symbolId]->setBid(price, quantity, -1, 'C');
            }
            else{
                instrumentList[symbolId]->setAsk(price, quantity, -1, 'C');
            }
        }
        orderList.erase(orderId);
    }
}

void decodeTradeMessage(const TradeMessage* &tm, ol &orderList, il &instrumentList){
    uint64_t orderId=tm->getOrderId();
    if(orderList.find(orderId)!=orderList.end()){
        uint64_t quantity = orderList[orderId]->getQuantity();
        uint64_t price = orderList[orderId]->getPrice();
        uint32_t symbolId = orderList[orderId]->getSymbolId();
        uint8_t side = orderList[orderId]->getSide();
        if(instrumentList.find(symbolId)!=instrumentList.end()){
            if(side==0){
                instrumentList[symbolId]->setBid(price, quantity, -1, 'T');
            }
            else{
                instrumentList[symbolId]->setAsk(price, quantity, -1, 'T');
            }
        }
        orderList.erase(orderId);
    }
}

void decodeQueryMessage(const QueryMessage* &qm, ol &orderList, il &instrumentList, string &outputPath){
    uint32_t symbolId=qm->getSymbolId();
    if(instrumentList.find(symbolId)!=instrumentList.end()){
        Instrument *inst = instrumentList[symbolId];
        uint64_t const& bidPrice = inst->getBidPrice();
        uint64_t const& bidQty = inst->getBidQuantity();
        uint32_t const& bidCount = inst->getBidCount();
        uint64_t const& askPrice = inst->getAskPrice();
        uint64_t const& askQty = inst->getAskQuantity();
        uint32_t const& askCount = inst->getAskCount();

        ofstream f;
        f.open(outputPath, ios::app|ios::binary);
        f.write((const char *)(&bidPrice), sizeof(uint64_t));
        f.write((const char*)(&bidQty), sizeof(uint64_t));
        f.write((const char*)(&bidCount), sizeof(uint32_t));
        f.write((const char*)(&askPrice), sizeof(uint64_t));
        f.write((const char*)(&askQty), sizeof(uint64_t));
        f.write((const char*)(&askCount), sizeof(uint32_t));
        f.close();
    }
    else{
        
        ofstream f;
        f.open(outputPath, ios::app|ios::binary);
        f.write((const char*)(0), sizeof(uint64_t));
        f.write((const char*)(0), sizeof(uint64_t));
        f.write((const char*)(0), sizeof(uint32_t));
        f.write((const char*)(0), sizeof(uint64_t));
        f.write((const char*)(0), sizeof(uint64_t));
        f.write((const char*)(0), sizeof(uint32_t));
        f.close(); 
    }
}

void createOrderBook(Packet *p, ol &orderList, il &instrumentList, string &outputPath){
    for(const Message* m:p->getMsgList()){
        char msgType=m->getMsgType();
        if(msgType=='N'){
            const NewMessage *nm = dynamic_cast<const NewMessage*>(m);
            decodeNewMessage(nm, orderList, instrumentList);
        }
        else if(msgType=='M'){
            const ModifyMessage *mm = dynamic_cast<const ModifyMessage*>(m);
            decodeModifyMessage(mm, orderList, instrumentList);
        }
        else if(msgType=='C'){
            const CancelMessage *cm = dynamic_cast<const CancelMessage*>(m);
            decodeCancelMessage(cm, orderList, instrumentList);
        }
        else if(msgType=='T'){
            const TradeMessage *tm = dynamic_cast<const TradeMessage*>(m);
            decodeTradeMessage(tm, orderList, instrumentList);
        }
        else if(msgType=='Q'){
            const QueryMessage *qm = dynamic_cast<const QueryMessage*>(m);
            decodeQueryMessage(qm, orderList, instrumentList, outputPath);
        }
    }
}
#include <fstream>
#include <vector>
#include <queue>
#include <filesystem>
#include "Packets.h"
using namespace std;
typedef priority_queue<Packet*, vector<Packet*>, PacketCmp> pacPQ;

void addNewMsg(istream &in, vector<Message*> &msgList){
    uint64_t price,quantity,orderId;
    uint32_t symbolId;
    uint8_t side;
    in.read((char*)(&price), sizeof(uint64_t));
    in.read((char*)(&quantity), sizeof(uint64_t));
    in.read((char*)(&symbolId), sizeof(uint32_t));
    in.read((char*)(&orderId), sizeof(uint64_t));
    in.read((char*)(&side), sizeof(uint8_t));
    Message *m = new NewMessage('N', price, quantity, symbolId, orderId, side);
    msgList.push_back(m);
}

void addCancelMsg(istream &in, vector<Message*> &msgList){
    uint64_t orderId;  
    in.read((char*)(&orderId), sizeof(uint64_t));
    Message *m = new CancelMessage('C', orderId);
    msgList.push_back(m);
}

void addTradeMsg(istream &in, vector<Message*> &msgList){
    uint64_t orderId;
    in.read((char*)(&orderId), sizeof(uint64_t));
    Message *m = new TradeMessage('T', orderId);
    msgList.push_back(m);
}

void addModifyMsg(istream &in, vector<Message*> &msgList){
    uint64_t price,quantity,orderId;
    in.read((char*)(&price), sizeof(uint64_t));
    in.read((char*)(&quantity), sizeof(uint64_t));
    in.read((char*)(&orderId), sizeof(uint64_t));
    Message *m = new ModifyMessage('M', price, quantity, orderId);
    msgList.push_back(m);
}

void addQueryMsg(istream &in, vector<Message*> &msgList){
    uint32_t symbolId;
    in.read((char*)(&symbolId), sizeof(uint32_t));
    Message *m = new QueryMessage('Q', symbolId);
    msgList.push_back(m);
}


Packet* readPacketHeader(istream &in, uint64_t &prevTS){
    uint64_t time;
    uint32_t numMsg;
    vector<Message*> msgList;
    in.read((char*)(&time), sizeof(uint64_t));
    in.read((char*)(&numMsg), sizeof(uint32_t));
    if(time<prevTS){
        return nullptr;
    }
    else{
        prevTS=time;
    }
    for(int i=0;i<numMsg;i++){
        char msgType;
        in.read((char*)(&msgType), sizeof(char));
        if(msgType=='N'){
            addNewMsg(in, msgList);
        }
        else if(msgType=='C'){
            addCancelMsg(in, msgList);
        }
        else if(msgType=='T'){
            addTradeMsg(in, msgList);
        }
        else if(msgType=='M'){
            addModifyMsg(in, msgList);
        }
        else if(msgType=='Q'){
            addQueryMsg(in, msgList);
        }
    }
    Packet* p = new Packet(time, numMsg, msgList);
    return p;
}

void decodeFile(string &filePath, pacPQ &pq){
    ifstream fin(filePath, ios::in|ios::binary);
    uint64_t prevTS=0;
    while(!fin.eof()){
        Packet* p=readPacketHeader(fin, prevTS);
        if(p!=nullptr){
            pq.push(p);
        }
        else{
            break;
        }
    }
}




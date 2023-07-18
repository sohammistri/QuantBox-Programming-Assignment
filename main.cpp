#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <filesystem>
#include "Packets.h"
#include "OrderBook.h"
using namespace std;
namespace fs=std::filesystem;
typedef priority_queue<Packet*, vector<Packet*>, PacketCmp> pacPQ;
typedef unordered_map<uint64_t, Order*> ol;
typedef unordered_map<uint32_t, Instrument*> il;

void addNewMsg(istream &in, vector<Message*> &msgList);
void addCancelMsg(istream &in, vector<Message*> &msgList);
void addTradeMsg(istream &in, vector<Message*> &msgList);
void addModifyMsg(istream &in, vector<Message*> &msgList);
void addQueryMsg(istream &in, vector<Message*> &msgList);
Packet* readPacketHeader(istream &in, uint64_t &prevTS);
void decodeFile(string &filePath, pacPQ &pq);
void decodeNewMessage(NewMessage *nm, ol &orderList, il &instrumentList);
void decodeModifyMessage(ModifyMessage *mm, ol &orderList, il &instrumentList);
void decodeCancelMessage(CancelMessage *cm, ol &orderList, il &instrumentList);
void decodeTradeMessage(TradeMessage *tm, ol &orderList, il &instrumentList);
void decodeQueryMessage(QueryMessage *qm, ol &orderList, il &instrumentList, string &outputPath);
void createOrderBook(Packet *p, ol &orderList, il &instrumentList, string &outputPath);

int main(int argc, char** argv){
    string dirPath=argv[1];
    string outputPath=argv[2];
    string fileExt=".dat";
    pacPQ packetMinPQ;

    ol orderList;
    il instrumentList;

    for(const auto &entry: fs::directory_iterator(dirPath)){
        string filePath=entry.path();
        if(equal(fileExt.rbegin(), fileExt.rend(), filePath.rbegin())){
            decodeFile(filePath, packetMinPQ);
        }
    }
    
    while(!packetMinPQ.empty()){
        Packet *p=packetMinPQ.top();
        packetMinPQ.pop();
        createOrderBook(p,orderList,instrumentList,outputPath);
    }
}
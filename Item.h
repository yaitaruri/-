#pragma once
#include <string>
#include <iostream>

using namespace std;

// 物品類別定義
class Item {
public:
    string name;      // 名稱
    string desc;      // 描述
    double price;     // 價格
    int effectValue;  // 效果數值
    int type;         // 0:補血, 1:補RAM, 2:永久加攻, 3:戰鬥暫時加攻, 4:永久加HP

    Item(string n, string d, double p, int e, int t) {
        name = n;
        desc = d;
        price = p;
        effectValue = e;
        type = t;
    }

    // 顯示物品資訊
    void display() {
        cout << "[" << name << "] " << desc << " (價格: " << price << " BTC)" << endl;
    }
};
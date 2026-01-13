#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // for max/min
#include <iomanip>   

using namespace std;

class CyberUnit {
protected:
    string name;
    string desc; 
    int hp;
    int maxHp;
    int at; 
    int level;
    double loot;
    
    // 新增：特殊機制變數
    int specialCounter; // 用於倒數計時 (例如衛星砲)
    string specialStatus; // 用於顯示特殊狀態文字

public:
    CyberUnit(string n, string description, int h, int a, int l, double money) {
        name = n;
        desc = description;
        maxHp = h;
        hp = h;
        at = a;
        level = l;
        loot = money;
        specialCounter = -1; // -1 代表無倒數
        specialStatus = "";
    }

    virtual ~CyberUnit() {}

    string getName() { return name; }
    string getDesc() { return desc; } 
    int getHp() { return hp; }
    int getMaxHp() { return maxHp; }
    int getAtk() { return at; }
    double getLoot() { return loot; }
    int getLevel() { return level; } 
    
    // 設定與取得倒數計時
    void setSpecialCounter(int val) { specialCounter = val; }
    int getSpecialCounter() { return specialCounter; }
    void decreaseSpecialCounter() { if(specialCounter > 0) specialCounter--; }

    bool isDead() { return hp <= 0; }

    // 修改：使用方塊符號，並顯示特殊狀態
    virtual void showStatus() {
        // 計算血量百分比
        double ratio = (double)hp / maxHp;
        if(ratio < 0) ratio = 0;
        int barWidth = 10; 
        int fill = (int)(barWidth * ratio);
        
        cout << name << " [HP: " << hp << "/" << maxHp << "]";
        
        // 如果有倒數計時，顯示出來
        if (specialCounter >= 0) {
            cout << " \033[1;31m[倒數: " << specialCounter << "]\033[1;32m"; // 紅色顯示倒數
        }

        cout << " [";
        // 血量顏色判定
        if (ratio < 0.3) cout << "\033[1;31m"; 
        else cout << "\033[1;32m";

        for(int i=0; i<barWidth; i++) {
            if(i < fill) cout << "■"; // 使用方塊
            else cout << " ";        // 使用空格
        }
        cout << "\033[1;32m"; 
        cout << "]" << endl;
    }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
        cout << "\033[1;37m> " << name << " 受到 " << dmg << " 點傷害！\033[1;32m" << endl;
    }

    virtual int attack() {
        int dmg = (int)(at * (0.9 + (rand() % 20) / 100.0));
        if (at > 0 && dmg < 1) dmg = 1; 
        return dmg;
    }
};
#pragma once
#include "CyberUnit.h"
#include "Item.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // for min/max
#include <fstream>   
#include <limits>    
#include <iomanip>   

using namespace std;

// 定義技能結構
struct Skill {
    string name;
    string desc;
    int ramCost;
    int cooldown;  // 最大 CD
    int currentCd; // 當前 CD
    double dmgMult; // 傷害倍率
    int type;      // 1=攻擊, 2=吸血攻擊, 3=防禦/Buff
    bool isLocked; // 是否被封鎖
};

class Player : public CyberUnit {
private:
    int ram;
    int maxRam;
    double bitcoin;
    vector<Item> inventory;
    
    // 技能系統
    vector<Skill> knownSkills;      
    vector<int> equippedIndices;    

    // 暴擊系統
    int critRate; 
    int critDmg;  

    // 狀態
    bool isInvincible;
    int tempAtkBonus;

    // 升級價格
    double atkUpgradeCost;
    double hpUpgradeCost;

    // 高階被動道具紀錄
    bool hasBoughtVirus;     
    bool hasBoughtBlackwall; 

    // 上限常數
    const int HP_CAP = 500;

public:
    Player(string n) : CyberUnit(n, "一名潛入深網的駭客", 100, 15, 1, 0) {
        maxRam = 32;
        ram = maxRam;
        bitcoin = 0.0;
        isInvincible = false;
        tempAtkBonus = 0;

        atkUpgradeCost = 0.5; 
        hpUpgradeCost = 0.4;
        hasBoughtVirus = false;
        hasBoughtBlackwall = false;

        critRate = 10;
        critDmg = 150;

        // 初始技能
        knownSkills.push_back({"短路", "基礎電擊傷害", 5, 2, 0, 1.5, 1, false});
        knownSkills.push_back({"連帶感染", "吸取敵人生命", 8, 3, 0, 1.2, 2, false});
        knownSkills.push_back({"光學迷彩", "迴避下一次攻擊", 10, 5, 0, 0.0, 3, false});

        equippedIndices = {0, 1, 2};
    }

    // --- Getters ---
    int getRam() { return ram; }
    int getMaxRam() { return maxRam; }
    double getBTC() { return bitcoin; }
    vector<Skill>& getKnownSkills() { return knownSkills; }
    vector<int>& getEquippedIndices() { return equippedIndices; }
    int getAtkUpgradeCost() { return (int)atkUpgradeCost; } 
    int getHpCap() { return HP_CAP; } 
    
    int getTotalAtk() { return at + tempAtkBonus; }

    // --- 外部狀態影響接口 (保留給敵方技能互動使用) ---
    
    // 用於[舊時代的數據幽靈]的吸魔技能
    void setRam(int val) { ram = val; }
    
    // 用於[黑牆閘門]戰鬥結束後的生命上限恢復
    void setMaxHp(int val) { maxHp = val; }

    // --- 技能學習與裝備 ---
    void learnSkill(string name, string desc, int cost, int cd, double mult, int type) {
        for(const auto& s : knownSkills) {
            if (s.name == name) return; 
        }
        knownSkills.push_back({name, desc, cost, cd, 0, mult, type, false});
        cout << ">> [系統] 成功下載技能模組: " << name << endl;
    }

    bool hasSkill(string name) {
        for(const auto& s : knownSkills) {
            if (s.name == name) return true;
        }
        return false;
    }

    void equipSkill(int slot, int skillIdx) {
        if (slot < 0 || slot >= 3) return;
        if (skillIdx < 0 || skillIdx >= knownSkills.size()) return;
        
        for(int i=0; i<3; i++) {
            if (equippedIndices[i] == skillIdx) {
                cout << ">> 錯誤：該技能已經裝備在其他槽位！" << endl;
                return;
            }
        }
        equippedIndices[slot] = skillIdx;
        cout << ">> 技能槽 [" << slot+1 << "] 已更新為: " << knownSkills[skillIdx].name << endl;
    }

    Skill& getEquippedSkill(int slot) {
        if (slot < 0 || slot >= 3) return knownSkills[0]; 
        return knownSkills[equippedIndices[slot]];
    }

    // --- 戰鬥相關 ---
    int attack() override {
        int currentAtk = at + tempAtkBonus;
        int dmg = (int)(currentAtk * (0.9 + (rand() % 21) / 100.0));
        
        if ((rand() % 100) < critRate) {
            dmg = (int)(dmg * (critDmg / 100.0));
            cout << "\033[1;33m>> [CRITICAL] 暴擊！造成 " << dmg << " 點傷害！ <<\033[1;32m" << endl;
        }
        return dmg;
    }

    int useSkillBySlot(int slotIdx) {
        if (slotIdx < 0 || slotIdx >= equippedIndices.size()) return 0;
        
        Skill& sk = knownSkills[equippedIndices[slotIdx]];

        // 檢查是否被封鎖
        if (sk.isLocked) {
            cout << ">> [Error] 技能已被網監程式封鎖！無法存取！" << endl;
            return -3; // 錯誤碼：被封鎖
        }

        if (sk.currentCd > 0) return -2; 
        if (ram < sk.ramCost) return -1; 

        ram -= sk.ramCost;
        sk.currentCd = sk.cooldown;

        int currentAtk = at + tempAtkBonus;
        cout << "> [技能] 啟動 " << sk.name << "..." << endl;

        // 計算技能暴擊率
        int skillCritRate = critRate;
        if (sk.name == "記憶體洩漏") {
            skillCritRate += 50; // 記憶體洩漏增加 50% 暴擊率
        }

        if (sk.type == 1) { 
            int baseDmg = (int)(currentAtk * sk.dmgMult);
            if ((rand() % 100) < skillCritRate) {
                baseDmg = (int)(baseDmg * (critDmg / 100.0));
                cout << "\033[1;33m>> [CRITICAL] 技能暴擊！ <<\033[1;32m" << endl;
            }
            return baseDmg;
        }
        else if (sk.type == 2) { 
            int baseDmg = (int)(currentAtk * sk.dmgMult);
            if ((rand() % 100) < skillCritRate) {
                baseDmg = (int)(baseDmg * (critDmg / 100.0));
                cout << "\033[1;33m>> [CRITICAL] 技能暴擊！ <<\033[1;32m" << endl;
            }
            int heal = baseDmg / 2;
            this->hp += heal;
            if (this->hp > maxHp) this->hp = maxHp;
            cout << "> 吸收了 " << heal << " 點 HP！" << endl;
            return baseDmg;
        }
        else if (sk.type == 3) { 
            if (sk.name == "光學迷彩") {
                isInvincible = true;
                cout << "> 進入隱形狀態！" << endl;
            }
            return 0; 
        }
        return 0;
    }

    void regenRam() {
        ram += 2; 
        if (ram > maxRam) ram = maxRam;
        for(auto& s : knownSkills) {
            if (s.currentCd > 0) s.currentCd--;
        }
    }

    void restoreFullRam() {
        ram = maxRam;
        cout << "\033[1;36m>> [系統] 權限提升！RAM 已完全充能！ <<\033[1;32m" << endl;
    }

    void resetBattleState() {
        isInvincible = false;
        // tempAtkBonus 不在這裡歸零，以便支援戰前吃藥
        for(auto& s : knownSkills) {
            s.currentCd = 0;
            s.isLocked = false; // 重置封鎖狀態
        }
    }

    void clearTempStats() {
        if (tempAtkBonus > 0) {
            cout << ">> 戰鬥興奮劑效果已消退。" << endl;
        }
        tempAtkBonus = 0;
        isInvincible = false;
        for(auto& s : knownSkills) {
            s.currentCd = 0;
            s.isLocked = false;
        }
    }

    // --- 網監特殊機制：封鎖技能 ---
    void lockRandomSkill() {
        if (equippedIndices.empty()) return;
        int targetSlot = rand() % 3; // 隨機封鎖 1~3 槽
        // 確保該槽位有裝備技能
        Skill& sk = knownSkills[equippedIndices[targetSlot]];
        sk.isLocked = true;
        cout << "\033[1;31m>> [警告] 網監程式入侵！技能 [" << sk.name << "] 已被強制封鎖！\033[1;32m" << endl;
    }

    // --- 黑牆特殊機制：扣 HP 上限 ---
    void decreaseMaxHp(int amount) {
        maxHp -= amount;
        if (maxHp < 1) maxHp = 1;
        if (hp > maxHp) hp = maxHp;
        cout << "\033[1;35m>> [黑牆侵蝕] 生命上限受到干擾暫時降低 " << amount << " 點！ (當前上限: " << maxHp << ")\033[1;32m" << endl;
    }

    // --- 其他功能 ---
    double getNextUpgradeCost(int type) {
        if (type == 2) return atkUpgradeCost;
        if (type == 4) return hpUpgradeCost;
        return 0.0;
    }

    bool isUniqueBought(int itemId) {
        if (itemId == 6) return hasBoughtVirus;
        if (itemId == 7) return hasBoughtBlackwall;
        return false;
    }

    void markUniqueBought(int itemId) {
        if (itemId == 6) hasBoughtVirus = true;
        if (itemId == 7) hasBoughtBlackwall = true;
    }

    // 修改：復活回復 35% 最大 HP
    void respawn() {
        int reviveHp = (int)(maxHp * 0.35);
        if (reviveHp < 1) reviveHp = 1;
        this->hp = reviveHp;
        cout << "> [系統] 緊急重啟程序完成... 生命值恢復至 " << reviveHp << " (35%)" << endl;
    }

    void addBTC(double amount) {
        bitcoin += amount;
        cout << "> 獲得 " << amount << " BTC" << endl;
    }
    
    bool checkInvincible() {
        if (isInvincible) {
            isInvincible = false; return true;
        }
        return false;
    }

    void addItem(Item it) {
        inventory.push_back(it);
        cout << "> 獲得物品: " << it.name << endl;
    }

    void upgradeStat(int type, int val) {
        if (type == 2) { 
            at += val;
            cout << "> 攻擊力升級為: " << at << endl;
            atkUpgradeCost *= 1.3; 
        } else if (type == 4) { 
             if (maxHp >= HP_CAP) return;

             maxHp += val; 
             hp += val; 
             
             cout << "> 硬體擴充成功！HP 上限變為: " << maxHp << " (當前 HP 同步增加)" << endl;
             hpUpgradeCost *= 1.3; 
        }
    }

    bool useInventoryItem() {
        if (inventory.empty()) {
            cout << "> 背包是空的！" << endl;
            return false;
        }
        cout << "\n=== 背包清單 ===" << endl;
        for(size_t i=0; i<inventory.size(); i++) {
            cout << i+1 << ". " << inventory[i].name << " (" << inventory[i].desc << ")" << endl;
        }
        cout << "0. 取消" << endl;
        cout << "請選擇: ";
        int choice;
        
        if (!(cin >> choice)) {
            cout << ">> 格式錯誤。" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return false;
        }
        
        if (choice <= 0 || choice > inventory.size()) return false;
        Item& it = inventory[choice-1];
        
        if (it.type == 0) { hp = min(maxHp, hp + it.effectValue); cout << "> HP 回復 " << it.effectValue << endl; }
        else if (it.type == 1) { ram = min(maxRam, ram + it.effectValue); cout << "> RAM 回復 " << it.effectValue << endl; }
        else if (it.type == 3) { 
            tempAtkBonus += it.effectValue; 
            cout << "> 攻擊力暫時提升 +" << it.effectValue << " (持續至下一場戰鬥結束)" << endl; 
        }
        else if (it.type == 4) { maxHp += it.effectValue; hp += it.effectValue; cout << "> HP上限提升 " << it.effectValue << endl; } 

        inventory.erase(inventory.begin() + (choice-1));
        return true;
    }

    // --- 存檔與讀檔功能 ---
    void save(ofstream& out) {
        out << name << endl; 
        out << desc << endl;
        out << hp << " " << maxHp << " " << at << " " << level << " " << loot << endl;

        out << ram << " " << maxRam << " " << bitcoin << endl;
        out << critRate << " " << critDmg << endl;
        out << atkUpgradeCost << " " << hpUpgradeCost << endl;
        out << hasBoughtVirus << " " << hasBoughtBlackwall << endl;

        out << knownSkills.size() << endl;
        for (const auto& s : knownSkills) {
            out << s.name << endl;
            out << s.desc << endl;
            out << s.ramCost << " " << s.cooldown << " " << s.currentCd << " " << s.dmgMult << " " << s.type << endl;
        }

        out << equippedIndices.size() << endl;
        for (int idx : equippedIndices) {
            out << idx << " ";
        }
        out << endl;

        out << inventory.size() << endl;
        for (const auto& item : inventory) {
            out << item.name << endl;
            out << item.desc << endl;
            out << item.price << " " << item.effectValue << " " << item.type << endl;
        }
    }

    void load(ifstream& in) {
        string line;
        
        getline(in, name);
        getline(in, desc);
        in >> hp >> maxHp >> at >> level >> loot;

        in >> ram >> maxRam >> bitcoin;
        in >> critRate >> critDmg;
        in >> atkUpgradeCost >> hpUpgradeCost;
        in >> hasBoughtVirus >> hasBoughtBlackwall;

        int skillCount;
        in >> skillCount;
        getline(in, line); 
        knownSkills.clear();
        for(int i=0; i<skillCount; i++) {
            Skill s;
            getline(in, s.name);
            getline(in, s.desc);
            in >> s.ramCost >> s.cooldown >> s.currentCd >> s.dmgMult >> s.type;
            getline(in, line); 
            // 讀檔時預設未鎖定
            s.isLocked = false; 
            knownSkills.push_back(s);
        }

        int equipCount;
        in >> equipCount;
        equippedIndices.resize(equipCount);
        for(int i=0; i<equipCount; i++) {
            in >> equippedIndices[i];
        }

        int invCount;
        in >> invCount;
        getline(in, line); 
        inventory.clear();
        for(int i=0; i<invCount; i++) {
            string n, d;
            double p; int e, t;
            getline(in, n);
            getline(in, d);
            in >> p >> e >> t;
            getline(in, line); 
            inventory.emplace_back(n, d, p, e, t);
        }
    }
};

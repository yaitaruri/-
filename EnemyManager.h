#pragma once
#include "CyberUnit.h"
#include <cstdlib>
#include <string>

using namespace std;

class EnemyManager {
public:
    static CyberUnit* generateEnemy(int starLevel, bool isBoss) {
        CyberUnit* enemy = nullptr;

        // --- LEVEL 1: 街頭公網 (★) ---
        if (starLevel == 1) {
            if (isBoss) enemy = new CyberUnit("[BOSS] 非法挖礦主機", "每回合造成額外燒傷", 150, 12, 1, 0.50);
            else {
                int r = rand() % 100;
                if (r < 40) enemy = new CyberUnit("老舊的家用路由器", "最基礎的練習目標，防護薄弱", 50, 5, 1, 0.05);
                else if (r < 70) enemy = new CyberUnit("路邊監視器", "若戰鬥拖太久，攻擊力會加倍", 60, 8, 1, 0.08);
                else enemy = new CyberUnit("腳本小子", "攻擊不穩定，傷害浮動大", 80, 10, 1, 0.10);
            }
        }
        
        // --- LEVEL 2: 商業公司 (★★) ---
        else if (starLevel == 2) {
            if (isBoss) enemy = new CyberUnit("[BOSS] 機房守衛機器人", "殘血時會緊急修復自身", 300, 20, 2, 0.80);
            else {
                int r = rand() % 100;
                if (r < 40) enemy = new CyberUnit("公司值班電腦", "裝有基礎防護，適合練手", 100, 15, 2, 0.20);
                else if (r < 70) enemy = new CyberUnit("防毒軟體 v2.0", "擁有物理減傷護盾，建議用病毒", 120, 12, 2, 0.25);
                else enemy = new CyberUnit("加密資料庫", "防禦系統運作中，釋放微量電流", 180, 5, 2, 1.00); 
            }
        }

        // --- LEVEL 3: 荒坂塔 (★★★) ---
        else if (starLevel == 3) {
            if (isBoss) enemy = new CyberUnit("[BOSS] 神機錯亂者", "血量越低，攻擊力越狂暴", 500, 35, 3, 1.50);
            else {
                int r = rand() % 100;
                if (r < 35) enemy = new CyberUnit("荒坂鎮暴機甲", "重型裝甲，防禦力極高", 250, 25, 3, 0.40);
                else if (r < 70) enemy = new CyberUnit("智慧型砲塔", "智慧鎖定，攻擊無法閃避", 200, 30, 3, 0.50);
                else enemy = new CyberUnit("軍用無人機", "高速迴旋，有機率二連擊", 150, 25, 3, 0.40);
            }
        }

        // --- LEVEL 4: 網路監察 (★★★★) ---
        else if (starLevel == 4) {
            if (isBoss) enemy = new CyberUnit("[BOSS] 鎮暴部隊總長", "施放戰術煙霧，閃避率極高", 650, 45, 4, 3.00);
            else {
                int r = rand() % 100;
                if (r < 35) enemy = new CyberUnit("網監攔截程式", "開場封鎖玩家技能", 300, 30, 4, 0.60);
                else if (r < 70) {
                    enemy = new CyberUnit("追蹤衛星訊號", "5 回合後進行軌道轟炸", 250, 0, 4, 0.80);
                    enemy->setSpecialCounter(5); // 設定 5 回合倒數
                }
                else enemy = new CyberUnit("黑牆閘門", "恐懼靈氣，攻擊會大幅扣 HP 上限", 400, 20, 4, 0.70);
            }
        }

        // --- LEVEL 5: 深網 (★★★★★) ---
        else {
            if (isBoss) enemy = new CyberUnit("[FINAL BOSS] 亞當碎骨", "傳奇改造人，擁有全能戰力", 1000, 60, 99, 10.0);
            else {
                int r = rand() % 100;
                if (r < 35) enemy = new CyberUnit("失控 AI", "行為混沌，模式隨機切換", 400, 40, 5, 1.00);
                else if (r < 70) enemy = new CyberUnit("毀滅性病毒", "死亡時會自爆造成真實傷害", 200, 50, 5, 0.90);
                else enemy = new CyberUnit("舊時代的數據幽靈", "攻擊會吸取玩家 RAM", 350, 40, 5, 1.00);
            }
        }
        
        return enemy;
    }
};
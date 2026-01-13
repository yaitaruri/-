#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <windows.h> 
#include <thread>
#include <fstream>
#include <limits> // 用於清除緩衝區
#include <iomanip> // 用於格式化輸出
#include <string>  // for stoi, find
#include "Player.h"
#include "EnemyManager.h"

using namespace std;

// 全域變數
int currentLevel = 1;
int explorationProgress = 0; 
const string SAVE_FILE = "savegame.dat";
const double EPSILON = 1e-6;

// 輔助函式：打字機效果
void printSlow(const string& text, int delay = 30) {
    for (char c : text) {
        cout << c;
        cout.flush(); 
        Sleep(delay);
    }
    cout << endl;
}

// 視窗與字體設定
void initWindow() {
    system("chcp 65001"); 
    system("cls"); 

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(csbi);
    if (GetConsoleScreenBufferInfoEx(hOut, &csbi)) {
        csbi.ColorTable[10] = RGB(16, 255, 16); // Neon Green
        csbi.ColorTable[12] = RGB(255, 30, 30); // Bright Red
        csbi.ColorTable[11] = RGB(0, 255, 255); // Cyan
        SetConsoleScreenBufferInfoEx(hOut, &csbi);
    }

    system("color 0A");   
    SetConsoleTitleA("NEURAL LINK: HACKER");

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    if (GetCurrentConsoleFontEx(hOut, FALSE, &cfi)) {
        cfi.dwFontSize.X = 0;
        cfi.dwFontSize.Y = 24; 
        wcscpy_s(cfi.FaceName, L"Consolas"); 
        SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
    }
}

// 安全輸入驗證函式
int getSafeInput(int min, int max) {
    int choice;
    while (true) {
        if (cin >> choice) {
            if (choice >= min && choice <= max) {
                return choice;
            } else {
                cout << ">> 輸入無效，請輸入 " << min << " 到 " << max << " 之間的數字: ";
            }
        } else {
            cout << ">> 格式錯誤！請輸入數字: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// 存檔系統
void saveGame(Player* p) {
    ofstream outFile(SAVE_FILE);
    if (!outFile) {
        cout << ">> [錯誤] 無法建立存檔！" << endl;
        system("pause");
        return;
    }
    
    outFile << currentLevel << endl;
    outFile << explorationProgress << endl;
    p->save(outFile);

    outFile.close();
    cout << ">> [系統] 遊戲進度已儲存至 " << SAVE_FILE << endl;
    system("pause");
}

// 讀檔系統
bool loadGame(Player*& p) {
    ifstream inFile(SAVE_FILE);
    if (!inFile) {
        cout << ">> [錯誤] 找不到存檔記錄！" << endl;
        system("pause");
        return false;
    }

    inFile >> currentLevel;
    inFile >> explorationProgress;
    
    string dummy;
    getline(inFile, dummy); 

    if (p == nullptr) {
        p = new Player("Temp"); 
    }
    
    p->load(inFile);
    inFile.close();
    
    return true; 
}

// 技能與狀態管理介面
void managePlayerStatus(Player* p) {
    while(true) {
        system("cls");
        cout << "========================================" << endl;
        cout << "       狀 態 與 技 能 管 理            " << endl;
        cout << "========================================" << endl;
        cout << "HP: " << p->getHp() << "/" << p->getMaxHp() 
             << " | RAM: " << p->getRam() << "/" << p->getMaxRam() << endl;
        cout << "攻擊力: " << p->getAtk() << " (+" << (p->getTotalAtk() - p->getAtk()) << ")" 
             << " | 資產: " << p->getBTC() << " BTC" << endl;
        cout << "----------------------------------------" << endl;
        cout << "[ 當 前 裝 備 技 能 (Max 3) ]" << endl;
        
        vector<int>& equipped = p->getEquippedIndices();
        vector<Skill>& allSkills = p->getKnownSkills();

        for(int i=0; i<3; i++) {
            if (i >= equipped.size()) break;
            Skill& s = allSkills[equipped[i]];
            cout << "槽位 " << i+1 << ": " << s.name 
                 << " (消耗:" << s.ramCost << " / CD:" << s.cooldown << ")" << endl;
        }
        
        cout << "----------------------------------------" << endl;
        cout << "1. 更換技能" << endl;
        cout << "2. 使用背包物品" << endl;
        cout << "0. 返回遊戲" << endl;
        cout << "指令: ";
        
        int choice = getSafeInput(0, 2);
        
        if (choice == 0) return;
        
        if (choice == 1) {
            cout << "\n請選擇要更換的槽位 (1-3): ";
            int slot = getSafeInput(1, 3);
            
            cout << "\n[ 可 用 技 能 庫 ]" << endl;
            for(size_t i=0; i<allSkills.size(); i++) {
                cout << i+1 << ". " << allSkills[i].name 
                     << " (" << allSkills[i].desc << ")" << endl;
            }
            cout << "請選擇要裝備的技能編號 (1-" << allSkills.size() << "): ";
            int skillIdx = getSafeInput(1, allSkills.size());
            
            p->equipSkill(slot-1, skillIdx-1);
            system("pause");
        }
        else if (choice == 2) {
            p->useInventoryItem(); 
            system("pause");
        }
    }
}

// 商店介面
void openShop(Player* p) {
    while(true) {
        double currentAtkCost = p->getNextUpgradeCost(2); 
        double currentHpCost = p->getNextUpgradeCost(4); 

        system("cls");
        cout << "\n========================================" << endl;
        cout << "       暗  網  市  集 (DARK NET)       " << endl;
        cout << "========================================" << endl;
        cout << "資產: " << fixed << setprecision(3) << p->getBTC() << " BTC" << endl;
        cout << "----------------------------------------" << endl;
        cout << "[ 消耗品 ]" << endl;
        cout << defaultfloat; 
        cout << "1. 簡易修復補丁 (0.1 BTC) - 補 50 HP" << endl;
        cout << "2. RAM 加速劑   (0.2 BTC) - 補 20 RAM" << endl;
        cout << "3. 戰鬥興奮劑   (0.3 BTC) - 本場攻擊 +10" << endl;
        cout << "----------------------------------------" << endl;
        cout << "[ 硬 體 改 造 (永久提升) ]" << endl;
        
        cout << "4. 神經網路優化 (ATK +5)  - " << fixed << setprecision(3) << currentAtkCost << " BTC" << endl;
        
        if (p->getMaxHp() >= p->getHpCap()) {
            cout << "5. \033[1;30m生體纖維織補 (MAX)      - 已達生體極限\033[1;32m" << endl;
        } else {
            cout << "5. 生體纖維織補 (HP +50)  - " << fixed << setprecision(3) << currentHpCost << " BTC" << endl;
        }

        cout << defaultfloat;

        cout << "----------------------------------------" << endl;
        cout << "[ 高 階 被 動 軟 體 (限購一次) ]" << endl;
        
        if (p->isUniqueBought(6)) cout << "6. \033[1;30m[已安裝] 荒坂軍用病毒\033[1;32m" << endl;
        else cout << "6. 荒坂軍用病毒 (2.5 BTC) - 永久 ATK +15" << endl;

        if (p->isUniqueBought(7)) cout << "7. \033[1;30m[已安裝] 黑牆穿透協議\033[1;32m" << endl;
        else cout << "7. 黑牆穿透協議 (8.0 BTC) - 永久 ATK +50" << endl;
        
        cout << "----------------------------------------" << endl;
        cout << "[ 戰 鬥 技 能 書 ]" << endl;
        
        if (p->hasSkill("超頻運算")) cout << "8. \033[1;30m[已擁有] 超頻運算\033[1;32m" << endl;
        else cout << "8. 超頻運算 (1.5 BTC) - 消耗12RAM, 3倍極大傷害, CD:4" << endl;

        if (p->hasSkill("記憶體洩漏")) cout << "9. \033[1;30m[已擁有] 記憶體洩漏\033[1;32m" << endl;
        else cout << "9. 記憶體洩漏 (1.0 BTC) - 消耗3RAM, 低耗能高爆擊, CD:0" << endl;

        if (p->hasSkill("防火牆爆破")) cout << "10. \033[1;30m[已擁有] 防火牆爆破\033[1;32m" << endl;
        else cout << "10. 防火牆爆破 (3.0 BTC) - 消耗15RAM, 2倍傷害+吸血, CD:5" << endl;

        cout << "----------------------------------------" << endl;
        cout << "0. 離開商店" << endl;
        cout << "請輸入: ";
        
        int choice = getSafeInput(0, 10);
        
        if (choice == 0) break;

        if (choice == 1) { 
            if(p->getBTC() >= 0.1 - EPSILON) { 
                p->addBTC(-0.1); 
                p->addItem(Item("修復補丁", "補HP", 0, 50, 0)); 
            } else cout << ">> 餘額不足。" << endl; 
        }
        else if (choice == 2) { 
            if(p->getBTC() >= 0.2 - EPSILON) { 
                p->addBTC(-0.2); 
                p->addItem(Item("RAM加速劑", "補RAM", 0, 20, 1)); 
            } else cout << ">> 餘額不足。" << endl; 
        }
        else if (choice == 3) { 
            if(p->getBTC() >= 0.3 - EPSILON) { 
                p->addBTC(-0.3); 
                p->addItem(Item("戰鬥興奮劑", "暫時加攻", 0, 10, 3)); 
            } else cout << ">> 餘額不足。" << endl; 
        }
        else if (choice == 4) { 
            if(p->getBTC() >= currentAtkCost - EPSILON) { 
                p->addBTC(-currentAtkCost); 
                p->upgradeStat(2, 5); 
            } else cout << ">> 餘額不足。" << endl; 
        }
        else if (choice == 5) { 
            if (p->getMaxHp() >= p->getHpCap()) cout << ">> [警告] 身體排斥反應！" << endl;
            else if(p->getBTC() >= currentHpCost - EPSILON) { 
                p->addBTC(-currentHpCost); 
                p->upgradeStat(4, 50); 
            } else cout << ">> 餘額不足。" << endl; 
        }
        else if (choice == 6) { 
            if(!p->isUniqueBought(6) && p->getBTC() >= 2.5 - EPSILON) { 
                p->addBTC(-2.5); 
                p->upgradeStat(2, 15); 
                p->markUniqueBought(6); 
            } 
        }
        else if (choice == 7) { 
            if(!p->isUniqueBought(7) && p->getBTC() >= 8.0 - EPSILON) { 
                p->addBTC(-8.0); 
                p->upgradeStat(2, 50); 
                p->markUniqueBought(7); 
            } 
        }
        else if (choice == 8) { 
            if(!p->hasSkill("超頻運算") && p->getBTC() >= 1.5 - EPSILON) { 
                p->addBTC(-1.5); 
                p->learnSkill("超頻運算", "造成極大傷害但耗能高", 12, 4, 3.0, 1); 
            } 
        }
        else if (choice == 9) { 
            if(!p->hasSkill("記憶體洩漏") && p->getBTC() >= 1.0 - EPSILON) { 
                p->addBTC(-1.0); 
                p->learnSkill("記憶體洩漏", "低耗能高爆擊", 3, 0, 1.3, 1); 
            } 
        }
        else if (choice == 10) { 
            if(!p->hasSkill("防火牆爆破") && p->getBTC() >= 3.0 - EPSILON) { 
                p->addBTC(-3.0); 
                p->learnSkill("防火牆爆破", "高傷吸血", 15, 5, 2.0, 2); 
            } 
        }
        
        Sleep(500); 
    }
}

// 戰鬥邏輯
void battleLoop(Player* p) {
    CyberUnit* selectedEnemy = nullptr;
    vector<CyberUnit*> missionList;
    bool isBossTarget = false; 

    // 生成選單
    if (explorationProgress >= 100) {
        cout << "\n=== 區 域 完 全 探 索 (100%) ===" << endl;
        missionList.push_back(EnemyManager::generateEnemy(currentLevel, false));
        missionList.push_back(EnemyManager::generateEnemy(currentLevel, false));
        missionList.push_back(EnemyManager::generateEnemy(currentLevel, true)); 
    } else {
        cout << "\n=== 掃 描 結 果 ===" << endl;
        
        int highLevelIndex = -1; 
        if (rand() % 100 < 30) {
            highLevelIndex = rand() % 3; 
        }

        for(int i=0; i<3; i++) {
            int lvl = currentLevel;
            if (i == highLevelIndex) lvl = currentLevel + 1;
            if (lvl > 5) lvl = 5;
            missionList.push_back(EnemyManager::generateEnemy(lvl, false));
        }
    }

    while(true) {
        cout << "----------------------------------------" << endl;
        for(int i=0; i<3; i++) {
            string typeLabel = "[一般]";
            if (explorationProgress >= 100 && i == 2) {
                typeLabel = "[BOSS]";
                cout << "\033[1;31m"; 
            }
            cout << i+1 << ". " << typeLabel << " " << missionList[i]->getName() 
                 << " (Lv." << missionList[i]->getLevel() << ")" << endl;
            cout << "    獎勵: " << missionList[i]->getLoot() << " BTC" << endl;
            if (explorationProgress >= 100 && i == 2) cout << "\033[1;32m"; 
        }
        cout << "0. 返回基地" << endl;
        cout << "請選擇: ";
        
        int choice = getSafeInput(0, 3);

        if (choice == 0) {
            for(auto m : missionList) delete m;
            return;
        }
        if (choice >= 1 && choice <= 3) {
            selectedEnemy = missionList[choice-1];
            if (explorationProgress >= 100 && choice == 3) isBossTarget = true;
            for(int i=0; i<3; i++) { if (i != choice-1) delete missionList[i]; }
            break;
        }
    }

    // 戰鬥開始
    p->resetBattleState(); 
    system("cls"); 
    
    // 網監特殊機制：開場封鎖
    // 使用 find 進行部分比對
    string ename = selectedEnemy->getName();
    if (ename.find("網監攔截程式") != string::npos) {
        printSlow("[WARNING] 偵測到入侵干擾！技能模組受到攻擊...", 30);
        Sleep(500);
        p->lockRandomSkill();
    }

    string alertMsg = "[ALERT] 正在建立加密連線至目標... " + ename;
    printSlow(alertMsg, 30);
    Sleep(600);
    string descMsg = "描述: " + selectedEnemy->getDesc();
    cout << descMsg << endl;
    cout << "----------------------------------------" << endl;

    int totalMaxHpLost = 0;

    // 戰鬥主迴圈
    while(!p->isDead() && !selectedEnemy->isDead()) {
        system("cls");
        cout << alertMsg << endl;
        cout << descMsg << endl;
        cout << "----------------------------------------" << endl;

        cout << "\n----------------------------------------" << endl;
        p->showStatus(); 
        cout << "RAM: " << p->getRam() << " / " << p->getMaxRam() << endl;
        cout << "----------------------------------------" << endl;
        selectedEnemy->showStatus(); 
        cout << "----------------------------------------" << endl;
        cout << "1. 普通攻擊 (可能暴擊)" << endl;

        for(int i=0; i<3; i++) {
            Skill& sk = p->getEquippedSkill(i);
            
            cout << i+2 << ". ";
            if (sk.isLocked) {
                cout << "\033[1;31m" << sk.name << " [LOCKED] (封鎖中)\033[1;32m" << endl;
            } else if (sk.currentCd > 0) {
                cout << "\033[1;30m" << sk.name << " (CD: " << sk.currentCd << ")\033[1;32m" << endl;
            } else {
                cout << sk.name << " (" << sk.ramCost << " RAM)";
                if (p->getRam() < sk.ramCost) cout << " [RAM不足]";
                cout << endl;
            }
        }

        cout << "5. 物品 / 6. 逃跑" << endl;
        cout << "行動: ";
        
        int act = getSafeInput(1, 6);

        bool playerActed = false;
        int dmgToEnemy = 0;

        if (act == 6) {
            if (isBossTarget) {
                cout << "\n\033[1;31m[SYSTEM ERROR] 登出程序請求被拒絕...\033[1;32m" << endl;
                Sleep(800);
                
                if (currentLevel == 4) {
                    printSlow(">> [鎮暴部隊總長] 『封鎖區域。一隻蒼蠅都別放過。』", 30);
                    printSlow(">> 你的登出指令被強行攔截了！網路閘門已關閉！", 30);
                }
                else if (currentLevel == 5) {
                    printSlow(">> [亞當碎骨] 『你想去哪裡？小老鼠。』", 30); 
                    printSlow(">> 物理連線已鎖定... 恐懼癱瘓了你的神經介面！", 30);
                }
                else {
                     cout << ">> 區域領主已鎖定你的訊號，無法逃跑！" << endl;
                }
                Sleep(1500); 
            }
            else { 
                cout << ">> 斷開連線！" << endl; 
                p->clearTempStats(); 
                delete selectedEnemy; 
                
                if (totalMaxHpLost > 0) {
                    p->setMaxHp(p->getMaxHp() + totalMaxHpLost);
                    cout << ">> [系統] 脫離黑牆連結，被侵蝕的生命上限已恢復。" << endl;
                    Sleep(1000);
                }
                return; 
            }
        }
        else if (act == 5) {
            playerActed = p->useInventoryItem();
            if(!playerActed) Sleep(800); 
        }
        else if (act == 1) {
            dmgToEnemy = p->attack(); 
            playerActed = true;
        }
        else if (act >= 2 && act <= 4) {
            int slotIdx = act - 2;
            int result = p->useSkillBySlot(slotIdx);
            
            if (result == -3) { 
                Sleep(800);
                playerActed = false;
            }
            else if (result == -2) {
                cout << ">> [Error] 技能冷卻中！" << endl;
                Sleep(800);
                playerActed = false;
            } else if (result == -1) {
                cout << ">> [Error] RAM 不足！" << endl;
                Sleep(800);
                playerActed = false;
            } else {
                dmgToEnemy = result;
                playerActed = true;
            }
        }

        if (playerActed) {
            if (dmgToEnemy > 0) {
                // [NEW] Lv4 & Lv5 Boss 閃避機制 (使用 find 進行部分比對，更安全)
                bool evaded = false;
                
                // 只要名字裡包含 "鎮暴部隊總長" 就觸發
                if (ename.find("鎮暴部隊總長") != string::npos && (rand() % 100 < 30)) { // 30% 閃避
                    cout << "\n\033[1;30m>> [MISS] 鎮暴部隊總長隱藏在戰術煙霧中，你的攻擊落空了！\033[1;32m" << endl;
                    cout << "\033[1;30m>> 濃煙遮蔽了視野... 無法鎖定目標！\033[1;32m" << endl; // 增加演出
                    evaded = true;
                }
                // 只要名字裡包含 "亞當碎骨" 就觸發
                else if (ename.find("亞當碎骨") != string::npos && (rand() % 100 < 30)) { // 30% 閃避
                    cout << "\n\033[1;36m>> [MISS] 亞當碎骨啟動斯安威斯坦，殘影閃過了攻擊！\033[1;32m" << endl;
                    evaded = true;
                }

                if (!evaded) {
                    selectedEnemy->takeDamage(dmgToEnemy);
                    Sleep(800); 
                }
            }
            
            if (selectedEnemy->isDead()) {
                // [NEW] 毀滅性病毒 死亡自爆機制
                if (ename.find("毀滅性病毒") != string::npos) {
                    cout << "\n\033[1;31m[DANGER] 病毒核心崩潰... 觸發邏輯炸彈！\033[1;32m" << endl;
                    Sleep(500);
                    int boom = 100; // 固定真實傷害
                    p->takeDamage(boom);
                    cout << ">> 受到 " << boom << " 點自爆傷害！" << endl;
                    Sleep(500);
                }
                break;
            }

            // 敵方回合
            Sleep(500); 
            cout << "\033[1;32m"; 
            
            if (selectedEnemy->getSpecialCounter() > 0) {
                selectedEnemy->decreaseSpecialCounter();
                cout << "\n[警告] 衛星訊號鎖定中... 距離轟炸還有 " << selectedEnemy->getSpecialCounter() << " 回合！" << endl;
                Sleep(800);
            } 
            else if (selectedEnemy->getSpecialCounter() == 0) {
                cout << "\n\033[1;31m[DANGER] 軌道轟炸啟動！毀滅打擊！\033[1;32m" << endl;
                p->takeDamage(9999);
                Sleep(1000);
                break; 
            }

            cout << "\n[敵方] " << selectedEnemy->getName() << " 發起反制攻擊！" << endl;
            Sleep(800); 

            if (p->checkInvincible()) {
                cout << ">> MISS！光學迷彩生效，完全迴避！" << endl;
            } else {
                int dmg = selectedEnemy->attack();

                // [NEW] 特殊敵人攻擊效果 (使用 find)
                if (ename.find("失控 AI") != string::npos) {
                    int mode = rand() % 3;
                    if (mode == 0) {
                        cout << ">> [AI] 運算邏輯發生衝突... 攻擊中止。" << endl;
                        dmg = 0;
                    } else if (mode == 1) {
                        cout << ">> [AI] 執行序重導向... 造成雙倍運算負載！" << endl;
                        dmg *= 2;
                    } 
                }
                else if (ename.find("舊時代的數據幽靈") != string::npos) {
                    if (p->getRam() > 0) {
                        cout << ">> [幽靈] 數據流被虹吸... RAM 受到侵蝕！" << endl;
                        int drain = 3;
                        int newRam = p->getRam() - drain;
                        if (newRam < 0) newRam = 0;
                        p->setRam(newRam);
                        cout << ">> RAM -3" << endl;
                    }
                }
                else if (ename.find("亞當碎骨") != string::npos) {
                    if (rand() % 100 < 30) {
                        cout << ">> [亞當碎骨] 『你覺得你很特別嗎？』 重砲轟炸！" << endl;
                        dmg = (int)(dmg * 1.5);
                    }
                }

                p->takeDamage(dmg);
                
                if (ename.find("黑牆閘門") != string::npos) {
                    int prevMax = p->getMaxHp(); 
                    p->decreaseMaxHp(20); 
                    int lost = prevMax - p->getMaxHp();
                    totalMaxHpLost += lost;
                }
            }
            
            Sleep(800); 
            p->regenRam(); 
            cout << ">> [系統] RAM 自動重整中... (+2)" << endl;
            Sleep(500); 
        }
    }

    if (totalMaxHpLost > 0) {
        p->setMaxHp(p->getMaxHp() + totalMaxHpLost);
        cout << "\033[1;36m>> [系統] 脫離戰鬥，被黑牆侵蝕的生命上限已修復 (+" << totalMaxHpLost << ")\033[1;32m" << endl;
        Sleep(1000);
    }

    p->clearTempStats(); 

    if (p->isDead()) {
        printSlow("\n>> MISSION FAILED << 連線中斷。", 50);
        p->respawn(); 
    } else {
        printSlow("\n>> TARGET DESTROYED << 目標已清除。", 30);
        p->addBTC(selectedEnemy->getLoot());

        if (isBossTarget) {
            printSlow("\n★ 警告：偵測到區域 " + to_string(currentLevel) + " 領主訊號消失... 權限提升！ ★", 50);
            p->restoreFullRam();
            
            currentLevel++;
            explorationProgress = 0; 
            if (currentLevel > 5) {
                printSlow("\n\n恭喜你！你已經攻破了深網的核心，成為了網路世界的傳奇！", 100);
                system("pause");
                exit(0);
            }
        } else {
            if (explorationProgress < 100) {
                explorationProgress += 20;
                if (explorationProgress > 100) explorationProgress = 100;
                cout << ">> 探索進度 +20%" << endl;
            }
        }
    }
    delete selectedEnemy;
    cout << endl;
    system("pause");
}

// 遊戲主迴圈
void runGameLoop(Player* player) {
    while(true) {
        system("cls");
        cout << "========================================" << endl;
        cout << "   C Y B E R P U N K   H A C K E R      " << endl;
        cout << "========================================" << endl;
        cout << "探員: " << player->getName() << " | 權限 Lv: " << currentLevel << endl;
        cout << "HP: " << player->getHp() << " | RAM: " << player->getRam() << endl;
        
        cout << "區域探索: [";
        int bars = explorationProgress / 10;
        for(int i=0; i<10; i++) { if(i < bars) cout << "■"; else cout << "."; } 
        cout << "] " << explorationProgress << "%" << endl;
        
        if (explorationProgress >= 100) cout << "\033[1;33m>> BOSS READY <<\033[1;32m" << endl;

        cout << "----------------------------------------" << endl;
        cout << "1. 掃描網路 (戰鬥)" << endl;
        cout << "2. 暗網市集 (買技能/升級)" << endl;
        cout << "3. 狀態/背包/換技能" << endl;
        cout << "4. 存檔 (Save)" << endl;
        cout << "5. 返回標題畫面 (Back to Title)" << endl;
        cout << "指令: ";

        int cmd = getSafeInput(1, 5);

        if (cmd == 1) battleLoop(player);
        else if (cmd == 2) openShop(player);
        else if (cmd == 3) managePlayerStatus(player);
        else if (cmd == 4) saveGame(player);
        else if (cmd == 5) {
            return;
        }
    }
}

int main() {
    initWindow();
    srand(time(0));

    while(true) {
        system("cls");
        cout << "\033[1;36m"; 
        cout << R"(
   ______      __              __  __           __            
  / ____/_  __/ /_  ___  _____/ / / /___ ______/ /_____  _____
 / /   / / / / __ \/ _ \/ ___/ /_/ / __ `/ ___/ //_/ _ \/ ___/
/ /___/ /_/ / /_/ /  __/ /  / __  / /_/ / /__/ ,< /  __/ /    
\____/\__, /_.___/\___/_/  /_/ /_/\__,_/\___/_/|_|\___/_/     
     /____/                                                   )" << endl;
        
        cout << "\033[1;36m"; 
        cout << "       >>>  NEURAL LINK: PROTOCOL INITIATED  <<<" << endl;
        
        cout << "\033[1;32m"; 
        cout << endl;
        cout << "           1. 開 始 新 游 戲 (New Game)       " << endl;
        cout << "           2. 讀 取 進 度 (Load Game)        " << endl;
        cout << "           3. 離 開 系 統 (Exit)             " << endl;
        cout << endl;
        cout << "請輸入指令: ";

        int choice = getSafeInput(1, 3);

        if (choice == 1) {
            cin.ignore(); 
            cout << "\n請輸入探員代號: ";
            string name;
            getline(cin, name);
            if(name.empty()) name = "Unknown";

            currentLevel = 1;
            explorationProgress = 0;

            printSlow("Initializing Neural Link...", 20);
            Sleep(200);
            printSlow("Bypassing Regional Firewall... [OK]", 20);
            Sleep(500);
            printSlow("Welcome, Agent " + name, 50);
            Sleep(800);

            Player* player = new Player(name);
            runGameLoop(player);
            delete player;
        }
        else if (choice == 2) {
            Player* player = new Player("Temp");
            if (loadGame(player)) {
                printSlow("Restoring session...", 20);
                Sleep(500);
                printSlow("Welcome back, Agent " + player->getName(), 50);
                Sleep(800);
                
                runGameLoop(player);
            }
            delete player;
        }
        else if (choice == 3) {
            printSlow("Terminating connection...", 30);
            break;
        }
    }

    return 0;
}

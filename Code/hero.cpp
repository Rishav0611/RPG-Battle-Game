// ============================================================
// BATTLE LOG - File Handling System
// ============================================================

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

using namespace std;

// ============================================================
// CHARACTER BASE CLASS
// ============================================================

class Character
{
protected:
    string name;
    int hp;
    int maxHp;
    int attack;
    int defense;

public:
    Character(string n, int h, int max, int a, int de)
    {
        name = n;
        hp = h;
        maxHp = max;
        attack = a;
        defense = de;
    }

    bool isAlive()
    {
        return hp > 0;
    }

    string getName()
    {
        return name;
    }

    int getHP()
    {
        return hp;
    }
};

// ============================================================
// BATTLE LOG STRUCTURE
// ============================================================

struct BattleLog
{
    string playerName;
    string heroType;
    string enemyName;
    string enemyType;
    int turnsPlayed;
    bool playerWon;
    int damageDealt;
    int damageTaken;
    int potionsUsed;
    string timestamp;
};

// ============================================================
// SAVE BATTLE LOG
// ============================================================

void saveBattleLog(BattleLog &log)
{
    ofstream file("battle_log.txt", ios::app);

    if (!file.is_open())
    {
        cout << "\n[ERROR] Battle log save nahi ho saka!" << endl;
        return;
    }

    file << "========================================" << endl;
    file << "          BATTLE REPORT" << endl;
    file << "========================================" << endl;
    file << "Date/Time    : " << log.timestamp << endl;
    file << "Player Name  : " << log.playerName << endl;
    file << "Hero Class   : " << log.heroType << endl;
    file << "Enemy        : " << log.enemyName
         << " (" << log.enemyType << ")" << endl;
    file << "----------------------------------------" << endl;
    file << "Total Turns  : " << log.turnsPlayed << endl;
    file << "Damage Dealt : " << log.damageDealt << endl;
    file << "Damage Taken : " << log.damageTaken << endl;
    file << "Potions Used : " << log.potionsUsed << endl;
    file << "Result       : "
         << (log.playerWon ? "VICTORY!" : "DEFEAT!") << endl;
    file << "========================================" << endl;
    file << endl;

    file.close();

    cout << "\n[File Saved] Battle summary "
         << "'battle_log.txt' mein save ho gayi!" << endl;
}

// ============================================================
// SHOW BATTLE SUMMARY
// ============================================================

void showBattleSummary(BattleLog &log)
{
    cout << "\n========================================" << endl;
    cout << "           BATTLE SUMMARY" << endl;
    cout << "========================================" << endl;

    cout << "Player       : " << log.playerName
         << " (" << log.heroType << ")" << endl;

    cout << "Enemy        : " << log.enemyName
         << " (" << log.enemyType << ")" << endl;

    cout << "Total Turns  : " << log.turnsPlayed << endl;
    cout << "Damage Dealt : " << log.damageDealt << endl;
    cout << "Damage Taken : " << log.damageTaken << endl;
    cout << "Potions Used : " << log.potionsUsed << endl;

    cout << "Result       : "
         << (log.playerWon ? "VICTORY!" : "DEFEAT!")
         << endl;

    cout << "========================================" << endl;
}

// ============================================================
// HERO CLASS
// ============================================================

class Hero : public Character
{
private:
    int mana;
    int level;
    bool isDefending;

public:

    Hero(string n, int h, int max, int a, int de, int m, int lv)
        : Character(n, h, max, a, de)
    {
        mana = m;
        level = lv;
        isDefending = false;
    }

    // Hero ki HP kam hone par potion se HP recover karta hai
    void usePotion()
    {
        if (hp == maxHp)
        {
            cout << name << " ki HP pehle se full hai!" << endl;
        }
        else if (mana <= 0)
        {
            cout << name << " ke paas potion nahi bachi!" << endl;
        }
        else
        {
            int healAmount = 30;

            mana = mana - 1;
            hp = hp + healAmount;

            if (hp > maxHp)
            {
                hp = maxHp;
            }

            cout << name << " ne potion use ki! +"
                 << healAmount << " HP recover hua." << endl;

            cout << "Potions baaki : " << mana << endl;
        }
    }

    // Defense mode activate karta hai
    void defend()
    {
        isDefending = true;

        cout << name
             << " defend kar raha hai! Is turn enemy ka attack kam hoga."
             << endl;
    }

    // Hero ko damage dene ka function
    void takeDamage(int dmg)
    {
        if (isDefending)
        {
            dmg = dmg / 2;

            cout << name
                 << " ne defend kiya! Damage half ho gaya."
                 << endl;

            isDefending = false;
        }

        int finalDamage = dmg - defense;

        if (finalDamage < 0)
        {
            finalDamage = 0;
        }

        hp = hp - finalDamage;

        if (hp <= 0)
        {
            hp = 0;
        }
    }

    // Potions track karne ke liye
    int getMana()
    {
        return mana;
    }

    // Hero ka level increase karta hai
    void levelUp()
    {
        if (isAlive())
        {
            level = level + 1;
            attack = attack + 5;
            maxHp = maxHp + 20;

            cout << name
                 << " Level Up! Level : "
                 << level << endl;

            cout << "Attack +5 | Max HP +20" << endl;
        }
    }

    // Hero ki current information display karta hai
    void displayStatus()
    {
        cout << "---------------------------" << endl;
        cout << "Name    : " << name << endl;
        cout << "HP      : " << hp << " / " << maxHp << endl;
        cout << "Attack  : " << attack << endl;
        cout << "Defense : " << defense << endl;
        cout << "Potions : " << mana << endl;
        cout << "Level   : " << level << endl;
        cout << "---------------------------" << endl;
    }

    int getAttack()
    {
        return attack;
    }
};

// ============================================================
// WARRIOR CLASS
// ============================================================

class Warrior : public Hero
{
public:

    Warrior(string n)
        : Hero(n, 150, 150, 30, 10, 3, 1)
    {
    }

    int heavyAttack()
    {
        cout << name
             << " ne HEAVY ATTACK kiya! Extra damage!"
             << endl;

        return attack + 20;
    }
};

// ============================================================
// ARCHER CLASS
// ============================================================

class Archer : public Hero
{
public:

    Archer(string n)
        : Hero(n, 100, 100, 20, 8, 5, 1)
    {
    }

    int shootArrow()
    {
        cout << name
             << " ne ARROW SHOOT kiya! Quick damage!"
             << endl;

        return attack + 15;
    }
};

// ============================================================
// TEST MAIN
// ============================================================

int main()
{
    Warrior warrior("Thor");
    Archer archer("Robin");

    cout << "\n===== WARRIOR =====" << endl;
    warrior.displayStatus();

    cout << "\nHeavy Attack Damage: "
         << warrior.heavyAttack() << endl;

    cout << "\n===== ARCHER =====" << endl;
    archer.displayStatus();

    cout << "\nArrow Attack Damage: "
         << archer.shootArrow() << endl;

    return 0;
}

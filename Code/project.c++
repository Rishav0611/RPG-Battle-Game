// Radhe Radhe
//======================//
//    Rishav ka kaam    //
//=====================//
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdint>

using namespace std;

// Global audio engine - poore program mein accessible rahega
ma_engine engine;

// ============================================================
//  PHASE 2 - BASE CLASS + HERO CLASSES + ENEMY CLASSES
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
        name = n; hp = h; maxHp = max; attack = a;
        defense = de; // ye defense ka kaam karega..mtlb total damage mai se kuch kaam karke hp ko affect hoga
    }

    void takedamage(int dmg)
    {
        int finalDamage = dmg - defense;
        if (finalDamage < 0) finalDamage = 0; // ye final damage minus no. na ho iske liye
        hp = hp - finalDamage;
        if (hp <= 0) hp = 0; // ye bhi same hp minus no. mai na jye
    }

    bool isAlive() { return hp > 0; } // hp 0 se zyada hai to zinda hai
    int getAttack() { return attack; }
    string getName() { return name; }
    int getHp() { return hp; }
    int getMaxHp() { return maxHp; }
};

//======================//
//    Saumya ka kaam    //
//=====================//

class Hero : public Character
{
protected:
    int mana;      // ek extra power jo hero emergency me use krta hai for survival (potion count)
    int level;     // defines level
    bool isDefending;

public:
    Hero(string n, int h, int max, int a, int de, int m, int lv) : Character(n, h, max, a, de)
    {
        mana = m; level = lv; isDefending = false;
    }

    string usePotion() // hero ki hp agar kam ho gyi hai to uska hp recover krne ke liye
    {
        if (hp == maxHp) return name + " ki HP pehle se full hai!";
        if (mana <= 0) return name + " ke paas potion nahi bachi!";

        int healAmount = 30;
        mana--;
        hp += healAmount;
        if (hp > maxHp) hp = maxHp; // hp maxHp se zyada na ho jaye
        return name + " ne potion use ki! +30 HP recover hua.";
    }

    string defend()
    {
        isDefending = true; // defend on karne se agla attack half damage karega
        return name + " defend kar raha hai!";
    }

    void takedamage(int dmg)
    {
        if (isDefending)
        {
            dmg = dmg / 2;         // defend tha to damage half ho jayega
            isDefending = false;   // defend sirf ek turn ke liye kaam karega
        }
        int finalDamage = dmg - defense;
        if (finalDamage < 0) finalDamage = 0;
        hp -= finalDamage;
        if (hp <= 0) hp = 0;
    }

    string levelUp()
    {
        if (isAlive())
        {
            level++;
            attack += 5;    // level up hone pe attack badh jata hai
            maxHp += 20;    // aur max hp bhi badh jata hai
            return name + " Level Up! Level: " + to_string(level);
        }
        return "";
    }

    int getMana() { return mana; }
    int getLevel() { return level; }

    virtual int specialAttack() { return attack; }   // har hero ka apna special attack hoga (override hoga)
    virtual string specialName() { return "Special"; }
    virtual ~Hero() {}
};

class Warrior : public Hero
{
public:
    Warrior(string n) : Hero(n, 150, 150, 30, 10, 3, 1) {} // Warrior stats: HP150 ATK30

    int specialAttack() override { return attack + 20; } // Heavy Attack -> extra damage
    string specialName() override { return "Heavy Attack"; }
};

class Archer : public Hero
{
public:
    Archer(string n) : Hero(n, 100, 100, 20, 8, 5, 1) {} // Archer stats: HP100 ATK20

    int specialAttack() override { return attack + 15; } // Shoot Arrow -> quick damage
    string specialName() override { return "Shoot Arrow"; }
};

//======================//
//    Rohit ka kaam     //
//=====================//

// enemy class me kuch special function use hua hai (random chance)
// isliye #include <cstdlib> aur #include <ctime> dono header file use karna hoga

class Enemy : public Character
{
protected:
    string type;
    int reward;

public:
    Enemy(string n, int h, int max, int a, int de, string t, int r) : Character(n, h, max, a, de)
    {
        type = t; reward = r;
    }

    int aiAttack() { return attack; }   // normal enemy attack
    int getReward() { return reward; }
    string getType() { return type; }
};

class Goblin : public Enemy
{
public:
    Goblin(string n) : Enemy(n, 50, 50, 15, 2, "Goblin", 10) {} // Goblin stats: HP50 ATK15

    int quickStrike()
    {
        int chance = rand() % 100;          // 0 se 99 ke beech random number
        if (chance < 30) return attack * 2; // 30% chance of double damage
        return attack;                      // normal damage
    }
};

class Dragon : public Enemy
{
public:
    Dragon(string n) : Enemy(n, 190, 190, 40, 15, "Dragon", 50) {} // Dragon stats: HP190 ATK40

    int fireAttack() { return 60; } // hamesha 60 fix damage deta hai
};

// ============================================================
//  PHASE 4 - GRAPHICS (Sprites, Animations, Backgrounds, UI)
// ============================================================
//    Rishav ka kaam    //
//=====================//

// SpriteSheet ek texture ke andar se frame-by-frame image nikalne ka kaam karta hai
// (jaise ek strip image ho jisme kai chote-chote frames ho, unhe crop karke animation banti hai)
struct SpriteSheet
{
    sf::Texture texture;
    int frameWidth = 0;
    int frameHeight = 0;
    int columns = 1;
    int frameCount = 1;
    float frameTime = 0.1f;   // ek frame kitni der dikhega (animation speed)
    int rowOffset = 0;        // sprite sheet me kaunsi row se frames uthane hai

    // manual mode: frame size khud dena hai
    bool load(const string& path, int fw, int fh, int cols, int totalFrames, float fTime)
    {
        frameWidth = fw;
        frameHeight = fh;
        columns = cols;
        frameCount = totalFrames;
        frameTime = fTime;
        rowOffset = 0;

        if (!texture.loadFromFile(path))
        {
            cout << "WARNING: Sprite load fail: " << path << " (is the file in the exe folder?)" << endl;
            return false;
        }
        return true;
    }

    // auto mode: texture size se khud frame width/height calculate karega
    bool loadAuto(const string& path, int totalCols, int totalRows, int rowOff, int framesToUse, float fTime)
    {
        columns = totalCols;
        frameCount = framesToUse;
        frameTime = fTime;
        rowOffset = rowOff;

        if (!texture.loadFromFile(path))
        {
            cout << "WARNING: Sprite load fail: " << path << " (is the file in the exe folder?)" << endl;
            return false;
        }

        sf::Vector2u texSize = texture.getSize();
        frameWidth = (int)texSize.x / totalCols;   // total width ko columns se divide karke ek frame ki width
        frameHeight = (int)texSize.y / totalRows;  // same height ke liye
        return true;
    }

    // diye gaye frame index ka crop-rectangle nikalta hai (kaunsa portion dikhana hai)
    sf::IntRect frameRect(int index) const
    {
        if (index < 0) index = 0;
        if (index >= frameCount) index = frameCount - 1; // index frameCount se bahar na jaye

        int totalIndex = rowOffset * columns + index; // row offset ke hisab se sahi row tak pohochna
        int col = totalIndex % columns;
        int row = totalIndex / columns;
        return sf::IntRect({col * frameWidth, row * frameHeight}, {frameWidth, frameHeight});
    }
};

// SpriteAnimator ek SpriteSheet ko time ke saath update karke animation chalata hai
struct SpriteAnimator
{
    SpriteSheet* sheet = nullptr;
    int currentFrame = 0;
    float timer = 0.f;
    bool loop = true;      // animation baar baar repeat hogi ya ek baar chal ke ruk jayegi
    bool finished = false; // non-loop animation khatam ho gayi to true

    void reset(SpriteSheet* s, bool doLoop)
    {
        sheet = s;
        currentFrame = 0;
        timer = 0.f;
        loop = doLoop;
        finished = false;
    }

    // dt = delta time (last frame se ab tak kitna time beeta)
    void update(float dt, sf::Sprite& sprite)
    {
        if (!sheet || finished) return;

        timer += dt;
        if (timer >= sheet->frameTime) // agla frame dikhane ka time ho gaya
        {
            timer -= sheet->frameTime;
            currentFrame++;

            if (currentFrame >= sheet->frameCount)
            {
                if (loop)
                {
                    currentFrame = 0; // wapas shuru se
                }
                else
                {
                    currentFrame = sheet->frameCount - 1; // last frame pe ruk jao
                    finished = true;
                }
            }
        }
        sprite.setTextureRect(sheet->frameRect(currentFrame));
    }
};

// Game ki alag alag screens/states (menu, hero select, enemy select, battle, game over)
enum class GameState
{
    MainMenu,
    HeroSelect,
    EnemySelect,
    Battle,
    GameOver
};

// Buttons aur banners ke gol-gol corner banane ke liye helper function
sf::ConvexShape makeRoundedRect(sf::Vector2f size, float radius, int segments = 8)
{
    sf::ConvexShape shape;
    shape.setPointCount(segments * 4);

    float w = size.x, h = size.y;
    if (radius > w / 2.f) radius = w / 2.f;   // radius rectangle se bada na ho
    if (radius > h / 2.f) radius = h / 2.f;

    // char corners ke centers (top-right, top-left, bottom-left, bottom-right jaisa)
    sf::Vector2f centers[4] = {
        {w - radius, h - radius},
        {radius, h - radius},
        {radius, radius},
        {w - radius, radius}
    };
    float startAngleDeg[4] = {0.f, 90.f, 180.f, 270.f};

    int idx = 0;
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < segments; i++)
        {
            // har corner ke liye ek arc (curve) ke points nikal rahe hai
            float angleDeg = startAngleDeg[c] + (90.f * (float)i / (float)(segments - 1));
            float angleRad = angleDeg * 3.14159265f / 180.f;
            sf::Vector2f pt = centers[c] + sf::Vector2f(radius * cos(angleRad), radius * sin(angleRad));
            shape.setPoint(idx++, pt);
        }
    }
    return shape;
}

// Button struct - clickable UI button (normal state + hover state dono ke shapes)
struct Button
{
    sf::ConvexShape boxNormal;
    sf::ConvexShape boxHover;
    sf::ConvexShape shadowNormal;
    sf::ConvexShape shadowHover;
    sf::Text label;
    sf::Vector2f basePos;
    sf::Vector2f baseSize;
    sf::Vector2f hoverPos;
    sf::Vector2f hoverSize;
    bool hovered = false;
    bool wasHovered = false; // pichle frame me hover tha ya nahi (hover sound sirf ek baar bajane ke liye)
    int baseFontSize = 22;
    int hoverFontSize = 25;

    Button(const sf::Font& font, string text, float x, float y, float w, float h, int fontSize = 22)
        : label(font), basePos(x, y), baseSize(w, h)
    {
        baseFontSize = fontSize;
        hoverFontSize = fontSize + 3;      // hover pe font thoda bada dikhega
        hoverSize = {w + 16.f, h + 10.f};  // hover pe box bhi thoda bada dikhega
        hoverPos = {x - 8.f, y - 5.f};

        // normal state ka box
        boxNormal = makeRoundedRect(baseSize, 14.f);
        boxNormal.setPosition(basePos);
        boxNormal.setFillColor(sf::Color(42, 22, 68));
        boxNormal.setOutlineColor(sf::Color(255, 195, 80));
        boxNormal.setOutlineThickness(3.f);

        // hover state ka box (mouse aane pe dikhega)
        boxHover = makeRoundedRect(hoverSize, 18.f);
        boxHover.setPosition(hoverPos);
        boxHover.setFillColor(sf::Color(108, 55, 160));
        boxHover.setOutlineColor(sf::Color(255, 225, 130));
        boxHover.setOutlineThickness(4.f);

        // dono states ke shadow (thoda offset karke neeche-daayein bana diya)
        shadowNormal = makeRoundedRect(baseSize, 14.f);
        shadowNormal.setFillColor(sf::Color(0, 0, 0, 150));
        shadowNormal.setPosition({basePos.x + 6.f, basePos.y + 6.f});

        shadowHover = makeRoundedRect(hoverSize, 18.f);
        shadowHover.setFillColor(sf::Color(0, 0, 0, 150));
        shadowHover.setPosition({hoverPos.x + 6.f, hoverPos.y + 6.f});

        label.setString(text);
        label.setCharacterSize(22);
        label.setFillColor(sf::Color(255, 220, 140));
        label.setOutlineColor(sf::Color(50, 20, 10));
        label.setOutlineThickness(2.f);
        centerLabel();
    }

    // text ko button ke box ke bilkul beech mein rakhne ke liye
    void centerLabel()
    {
        sf::Vector2f pos = hovered ? hoverPos : basePos;
        sf::Vector2f size = hovered ? hoverSize : baseSize;
        label.setCharacterSize(hovered ? hoverFontSize : baseFontSize);
        sf::FloatRect tb = label.getLocalBounds();
        label.setPosition({pos.x + (size.x - tb.size.x) / 2.f - tb.position.x,
                            pos.y + (size.y - tb.size.y) / 2.f - tb.position.y});
    }

    // mouse button ke upar hai ya nahi check karta hai
    bool isHovered(sf::Vector2f mousePos)
    {
        return boxNormal.getGlobalBounds().contains(mousePos);
    }

    // click hua aur mouse button ke upar tha to true
    bool isClicked(sf::Vector2f mousePos, bool clicked)
    {
        return clicked && isHovered(mousePos);
    }

    // har frame me call hoga - hover state update karta hai + hover sound bajata hai
    void update(sf::Vector2f mousePos, ma_engine* audioEngine)
    {
        hovered = isHovered(mousePos);

        if (hovered && !wasHovered && audioEngine) // pehli baar hover hua to hi sound bajao
            ma_engine_play_sound(audioEngine, "hover.wav", NULL);

        centerLabel();

        sf::Vector2f activePos = hovered ? hoverPos : basePos;
        if (hovered)
            shadowHover.setPosition({activePos.x + 6.f, activePos.y + 6.f});
        else
            shadowNormal.setPosition({activePos.x + 6.f, activePos.y + 6.f});

        wasHovered = hovered;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(hovered ? shadowHover : shadowNormal);
        window.draw(hovered ? boxHover : boxNormal);
        window.draw(label);
    }
};

// Health bar draw karne ka function - background (red) + fill (green, hp ke ratio ke hisab se) + border
void drawHealthBar(sf::RenderWindow& window, float x, float y, float w, float h, int hp, int maxHp)
{
    sf::RectangleShape back({w, h});
    back.setPosition({x, y});
    back.setFillColor(sf::Color(60, 20, 20)); // pura bar ka background (khaali hissa)
    window.draw(back);

    float ratio = maxHp > 0 ? (float)hp / (float)maxHp : 0.f; // hp kitna % bacha hai
    if (ratio < 0.f) ratio = 0.f;

    sf::RectangleShape front({w * ratio, h}); // sirf itni width bharega jitna hp bacha hai
    front.setPosition({x, y});
    front.setFillColor(sf::Color(60, 200, 80));
    window.draw(front);

    sf::RectangleShape border({w, h}); // bar ke upar white outline
    border.setPosition({x, y});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(1.5f);
    window.draw(border);
}

// ============================================================
//   Sab ka kaam - MAIN FUNCTION
// ============================================================

int main()
{
    srand((unsigned)time(0)); // random seed set kiya (goblin/dragon ke random attacks ke liye)

    // ---------- PHASE 1: Setup - Audio Engine ----------
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
    {
        cout << "Failed to initialize audio engine\n";
        return -1;
    }

    // background menu music load karke loop pe chala diya
    ma_sound menuMusic;
    bool menuMusicLoaded = false;
    if (ma_sound_init_from_file(&engine, "Audio1.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &menuMusic) == MA_SUCCESS)
    {
        menuMusicLoaded = true;
        ma_sound_set_looping(&menuMusic, MA_TRUE);
        ma_sound_start(&menuMusic);
    }
    else
    {
        cout << "NOTE: Audio1.mp3 load nahi hui - background music nahi bajegi." << endl;
    }

    // ---------- PHASE 1: Setup - Window ----------
    sf::RenderWindow window(sf::VideoMode({900u, 600u}), "RPG Battle Game");
    window.setFramerateLimit(60);

    // ---------- Fonts load karna (text/fonts SFML) ----------
    sf::Font font;
    if (!font.openFromFile("font.ttf"))
    {
        cout << "ERROR: font.ttf nahi mili! Isi folder me ek .ttf file daalo." << endl;
        if (menuMusicLoaded) ma_sound_uninit(&menuMusic);
        ma_engine_uninit(&engine);
        return -1;
    }

    sf::Font pixelFont;
    bool pixelFontLoaded = pixelFont.openFromFile("pixel_font.ttf");
    if (!pixelFontLoaded)
        cout << "NOTE: pixel_font.ttf nahi mili - normal font se chalega. (Press Start 2P jaisa koi pixel font daal dena)" << endl;
    sf::Font& uiFont = pixelFontLoaded ? pixelFont : font; // pixel font mila to wahi use hoga warna normal font

    // ---------- Background images load karna ----------
    sf::Texture gameBgTexture;
    bool gameBgLoaded = gameBgTexture.loadFromFile("Battleground1.png");
    sf::Sprite* gameBgSprite = nullptr;
    if (gameBgLoaded)
    {
        gameBgSprite = new sf::Sprite(gameBgTexture);
        sf::Vector2u texSize = gameBgTexture.getSize();
        gameBgSprite->setScale({900.f / (float)texSize.x, 600.f / (float)texSize.y}); // window size ke hisab se stretch
    }

    sf::Texture battleground2Texture;
    bool battleground2Loaded = battleground2Texture.loadFromFile("battleground2.png");
    sf::Sprite* battleground2Sprite = nullptr;
    if (battleground2Loaded)
    {
        battleground2Sprite = new sf::Sprite(battleground2Texture);
        sf::Vector2u bg2Size = battleground2Texture.getSize();
        battleground2Sprite->setScale({900.f / (float)bg2Size.x, 600.f / (float)bg2Size.y});
    }
    else
    {
        cout << "NOTE: battleground2.png nahi mili - background switch nahi hoga." << endl;
    }

    sf::Sprite* activeBgSprite = gameBgSprite; // abhi kaunsa background dikhna hai (state change hone pe badlega)

    // Goblin fight ka special background
    sf::Texture goblinBattleBgTexture;
    bool goblinBattleBgLoaded = goblinBattleBgTexture.loadFromFile("Battleground3.png");
    sf::Sprite* goblinBattleBgSprite = nullptr;
    if (goblinBattleBgLoaded)
    {
        goblinBattleBgSprite = new sf::Sprite(goblinBattleBgTexture);
        sf::Vector2u texSize2 = goblinBattleBgTexture.getSize();
        goblinBattleBgSprite->setScale({900.f / (float)texSize2.x, 600.f / (float)texSize2.y});
    }

    // Dragon fight ka special background
    sf::Texture dragonBattleBgTexture;
    bool dragonBattleBgLoaded = dragonBattleBgTexture.loadFromFile("Battleground4.png");
    sf::Sprite* dragonBattleBgSprite = nullptr;
    if (dragonBattleBgLoaded)
    {
        dragonBattleBgSprite = new sf::Sprite(dragonBattleBgTexture);
        sf::Vector2u texSize3 = dragonBattleBgTexture.getSize();
        dragonBattleBgSprite->setScale({900.f / (float)texSize3.x, 600.f / (float)texSize3.y});
    }
    else
    {
        cout << "NOTE: Battleground4.png nahi mili - dragon fight ka special background nahi dikhega." << endl;
    }

    // ---------- Goblin sprite + animation ----------
    SpriteSheet goblinIdleSheet;
    SpriteSheet goblinAttackSheet;
    bool goblinIdleLoaded  = goblinIdleSheet.load("idle.png", 150, 150, 4, 11, 0.15f);
    bool goblinAttackLoaded = goblinAttackSheet.load("attack2.png", 150, 150, 4, 16, 0.06f);
    (void)goblinAttackLoaded;

    sf::Sprite* goblinSprite = nullptr;
    SpriteAnimator goblinAnimator;

    const float goblinScale = 2.9f;

    const float goblinPosX = 160.f;
    const float goblinPosY = 60.f;

    if (goblinIdleLoaded)
    {
        goblinSprite = new sf::Sprite(goblinIdleSheet.texture);
        goblinSprite->setTextureRect(goblinIdleSheet.frameRect(0));
        goblinSprite->setScale({goblinScale, goblinScale});
        goblinSprite->setPosition({goblinPosX, goblinPosY});
        goblinAnimator.reset(&goblinIdleSheet, true); // idle animation loop pe chalegi
    }

    // ---------- Dragon sprite + animation ----------
    SpriteSheet dragonIdleSheet;
    SpriteSheet dragonAttackSheet;
    bool dragonIdleLoaded   = dragonIdleSheet.loadAuto("flying_dragon-red.png", 3, 4, 1, 3, 0.18f);
    bool dragonAttackLoaded = dragonAttackSheet.loadAuto("flying_dragon-red.png", 3, 4, 3, 3, 0.35f);
    (void)dragonAttackLoaded;

    // Dragon ki fire attack ka effect (alag sprite sheet)
    SpriteSheet fireSheet;
    bool fireLoaded = fireSheet.loadAuto("fire5_64.png", 10, 6, 0, 60, 0.02f);

    sf::Sprite* dragonSprite = nullptr;
    SpriteAnimator dragonAnimator;
    const float dragonScale = 3.6f;

    const float dragonRestX = 100.f;    // battle me dragon ki normal (resting) position
    float dragonPosY = 60.f;
    float dragonCurrentX = dragonRestX;
    bool dragonEntering = false;        // entrance animation chal rahi hai kya (bahar se andar udd ke aana)
    const float dragonEntranceSpeed = 550.f;

    const bool dragonIdleFlipNeeded = false;   // idle me sprite flip karna hai ya nahi
    const bool dragonAttackFlipNeeded = true;  // attack me flip karna hai (direction sahi dikhane ke liye)
    bool dragonCurrentFlip = dragonIdleFlipNeeded;

    sf::Sprite* fireSprite = nullptr;
    SpriteAnimator fireAnimator;
    bool fireActive = false;                       // fire effect abhi chal raha hai kya
    float fireStartX = 0.f, fireEndX = 700.f, fireY = 0.f;

    if (dragonIdleLoaded)
    {
        dragonSprite = new sf::Sprite(dragonIdleSheet.texture);
        dragonSprite->setTextureRect(dragonIdleSheet.frameRect(0));

        dragonPosY = 500.f - (dragonIdleSheet.frameHeight * dragonScale) - 15.f; // ground line ke hisab se position

        if (dragonIdleFlipNeeded)
        {
            // flip karne ke liye negative scale + position adjust karna padta hai (mirror effect)
            dragonSprite->setScale({-dragonScale, dragonScale});
            dragonSprite->setPosition({dragonCurrentX + dragonIdleSheet.frameWidth * dragonScale, dragonPosY});
        }
        else
        {
            dragonSprite->setScale({dragonScale, dragonScale});
            dragonSprite->setPosition({dragonCurrentX, dragonPosY});
        }

        dragonAnimator.reset(&dragonIdleSheet, true);
    }

    if (fireLoaded)
    {
        fireSprite = new sf::Sprite(fireSheet.texture);
        fireSprite->setTextureRect(fireSheet.frameRect(0));
        fireSprite->setScale({1.4f, 1.4f});
    }

    // ---------- Warrior sprite + animation ----------
    SpriteSheet warriorIdleSheet;
    SpriteSheet warriorAttackSheet;
    bool warriorIdleLoaded   = warriorIdleSheet.load("warrior_idle.png", 120, 80, 10, 10, 0.15f);
    bool warriorAttackLoaded = warriorAttackSheet.load("warrior_attack.png", 120, 80, 6, 6, 0.09f);
    (void)warriorAttackLoaded;

    sf::Sprite* warriorSprite = nullptr;
    SpriteAnimator warriorAnimator;
    const float warriorScale = 5.0f;
    const bool warriorFlipNeeded = true; // hero enemy ki taraf face kare isliye flip

    float warriorPosX = 340.f;
    float warriorPosY = 60.f;

    if (warriorIdleLoaded)
    {
        warriorSprite = new sf::Sprite(warriorIdleSheet.texture);
        warriorSprite->setTextureRect(warriorIdleSheet.frameRect(0));

        warriorPosY = 500.f - (warriorIdleSheet.frameHeight * warriorScale) - 55.f;

        if (warriorFlipNeeded)
        {
            warriorSprite->setScale({-warriorScale, warriorScale});
            warriorSprite->setPosition({warriorPosX + warriorIdleSheet.frameWidth * warriorScale, warriorPosY});
        }
        else
        {
            warriorSprite->setScale({warriorScale, warriorScale});
            warriorSprite->setPosition({warriorPosX, warriorPosY});
        }

        warriorAnimator.reset(&warriorIdleSheet, true);
    }

    // ---------- Archer sprite + animation ----------
    SpriteSheet archerIdleSheet;
    SpriteSheet archerAttackSheet;
    bool archerIdleLoaded   = archerIdleSheet.loadAuto("archer_sheet.png", 11, 5, 0, 5, 0.15f);
    bool archerAttackLoaded = archerAttackSheet.loadAuto("archer_sheet.png", 11, 5, 1, 10, 0.07f);
    (void)archerAttackLoaded;

    sf::Sprite* archerSprite = nullptr;
    SpriteAnimator archerAnimator;
    const float archerScale = 4.4f;
    const bool archerFlipNeeded = true;
    float archerPosX = 536.f;
    float archerPosY = 60.f;

    if (archerIdleLoaded)
    {
        archerSprite = new sf::Sprite(archerIdleSheet.texture);
        archerSprite->setTextureRect(archerIdleSheet.frameRect(0));

        archerPosY = 500.f - (archerIdleSheet.frameHeight * archerScale) - 15.f;

        if (archerFlipNeeded)
        {
            archerSprite->setScale({-archerScale, archerScale});
            archerSprite->setPosition({archerPosX + archerIdleSheet.frameWidth * archerScale, archerPosY});
        }
        else
        {
            archerSprite->setScale({archerScale, archerScale});
            archerSprite->setPosition({archerPosX, archerPosY});
        }

        archerAnimator.reset(&archerIdleSheet, true);
    }

    // Archer ke attack ka arrow (teer) - alag se ek simple sprite (no animation sheet)
    sf::Texture arrowTexture;
    bool arrowLoaded = arrowTexture.loadFromFile("arrow.png");
    sf::Sprite* arrowSprite = nullptr;
    const float arrowScale = 0.022f;
    bool arrowActive = false; // arrow abhi fly kar raha hai kya
    float arrowStartX = 0.f, arrowEndX = 220.f, arrowY = 0.f;

    if (arrowLoaded)
    {
        arrowSprite = new sf::Sprite(arrowTexture);
        arrowSprite->setScale({-arrowScale, arrowScale});
    }

    // ---------- Main Menu ke decorative sprites (dragon udta hua, warrior/archer khade) ----------
    sf::Sprite* menuDragonSprite = nullptr;
    SpriteAnimator menuDragonAnimator;
    float menuFlyClock = 0.f;
    const float menuDragonScale = 4.4f;
    const float menuDragonSpeed = 90.f;
    const float menuDragonLeftBound = -450.f;
    const float menuDragonRightBound = 950.f;
    const float menuDragonBaseY = -10.f;
    float menuDragonX = menuDragonLeftBound;

    if (dragonIdleLoaded)
    {
        menuDragonSprite = new sf::Sprite(dragonIdleSheet.texture);
        menuDragonSprite->setTextureRect(dragonIdleSheet.frameRect(0));
        menuDragonSprite->setScale({menuDragonScale, menuDragonScale});
        menuDragonAnimator.reset(&dragonIdleSheet, true);
    }

    sf::Sprite* menuWarriorSprite = nullptr;
    SpriteAnimator menuWarriorAnimator;
    const float menuWarriorScale = 6.8f;
    float menuWarriorX = 0.f, menuWarriorY = 0.f;

    if (warriorIdleLoaded)
    {
        menuWarriorSprite = new sf::Sprite(warriorIdleSheet.texture);
        menuWarriorSprite->setTextureRect(warriorIdleSheet.frameRect(0));
        menuWarriorSprite->setScale({menuWarriorScale, menuWarriorScale});

        menuWarriorX = 10.f;
        menuWarriorY = 600.f - (warriorIdleSheet.frameHeight * menuWarriorScale) - 10.f;

        menuWarriorSprite->setPosition({menuWarriorX, menuWarriorY});
        menuWarriorAnimator.reset(&warriorIdleSheet, true);
    }

    sf::Sprite* menuArcherSprite = nullptr;
    SpriteAnimator menuArcherAnimator;
    const float menuArcherScale = 6.0f;
    const bool menuArcherFlipNeeded = true;
    float menuArcherX = 0.f, menuArcherY = 0.f;

    if (archerIdleLoaded)
    {
        menuArcherX = 900.f - (archerIdleSheet.frameWidth * menuArcherScale) - 10.f;
        menuArcherY = 600.f - (archerIdleSheet.frameHeight * menuArcherScale) - 10.f;

        menuArcherSprite = new sf::Sprite(archerIdleSheet.texture);
        menuArcherSprite->setTextureRect(archerIdleSheet.frameRect(0));

        if (menuArcherFlipNeeded)
        {
            menuArcherSprite->setScale({-menuArcherScale, menuArcherScale});
            menuArcherSprite->setPosition({menuArcherX + archerIdleSheet.frameWidth * menuArcherScale, menuArcherY});
        }
        else
        {
            menuArcherSprite->setScale({menuArcherScale, menuArcherScale});
            menuArcherSprite->setPosition({menuArcherX, menuArcherY});
        }

        menuArcherAnimator.reset(&archerIdleSheet, true);
    }

    // warrior ko archer ke saamne symmetrically center karna (mirror position)
    if (menuWarriorSprite && menuArcherSprite)
    {
        float archerCenterX = menuArcherX + (archerIdleSheet.frameWidth * menuArcherScale) / 2.f;
        float mirroredCenterX = 900.f - archerCenterX;
        menuWarriorX = mirroredCenterX - (warriorIdleSheet.frameWidth * menuWarriorScale) / 2.f;
        menuWarriorSprite->setPosition({menuWarriorX, menuWarriorY});
    }

    GameState state = GameState::MainMenu; // game hamesha Main Menu se start hoga

    // ---------- Title text + glow + banner (Main Menu heading) ----------
    sf::Text title(uiFont, "RPG BATTLE GAME", 40);
    title.setFillColor(sf::Color(255, 210, 70));
    title.setOutlineColor(sf::Color(70, 25, 10));
    title.setOutlineThickness(4.f);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin({titleBounds.position.x + titleBounds.size.x / 2.f, titleBounds.position.y + titleBounds.size.y / 2.f});
    title.setPosition({450.f, 95.f});

    sf::Text titleGlow = title; // ek copy jo glow effect ke liye pulse karegi
    titleGlow.setFillColor(sf::Color(255, 140, 30, 130));
    titleGlow.setOutlineThickness(0.f);

    sf::ConvexShape titleBanner = makeRoundedRect({titleBounds.size.x + 90.f, titleBounds.size.y + 55.f}, 20.f);
    titleBanner.setFillColor(sf::Color(20, 12, 35, 190));
    titleBanner.setOutlineColor(sf::Color(255, 195, 80));
    titleBanner.setOutlineThickness(4.f);
    titleBanner.setOrigin({(titleBounds.size.x + 90.f) / 2.f, (titleBounds.size.y + 55.f) / 2.f});
    titleBanner.setPosition({450.f, 95.f});

    sf::ConvexShape titleBannerInner = makeRoundedRect({titleBounds.size.x + 66.f, titleBounds.size.y + 34.f}, 14.f);
    titleBannerInner.setFillColor(sf::Color::Transparent);
    titleBannerInner.setOutlineColor(sf::Color(255, 230, 150, 160));
    titleBannerInner.setOutlineThickness(2.f);
    titleBannerInner.setOrigin({(titleBounds.size.x + 66.f) / 2.f, (titleBounds.size.y + 34.f) / 2.f});
    titleBannerInner.setPosition({450.f, 95.f});

    sf::Clock menuPulseClock; // title ka glow/pulse animation is clock se chalta hai

    // Har screen (Hero Select, Enemy Select) ki heading banane ka reusable function
    auto drawSectionHeading = [&](const string& text, float y)
    {
        sf::Text heading(uiFont, text, 32);
        heading.setFillColor(sf::Color(255, 210, 70));
        heading.setOutlineColor(sf::Color(70, 25, 10));
        heading.setOutlineThickness(4.f);
        sf::FloatRect hb = heading.getLocalBounds();
        heading.setOrigin({hb.position.x + hb.size.x / 2.f, hb.position.y + hb.size.y / 2.f});
        heading.setPosition({450.f, y});

        sf::ConvexShape banner = makeRoundedRect({hb.size.x + 90.f, hb.size.y + 55.f}, 20.f);
        banner.setFillColor(sf::Color(20, 12, 35, 190));
        banner.setOutlineColor(sf::Color(255, 195, 80));
        banner.setOutlineThickness(4.f);
        banner.setOrigin({(hb.size.x + 90.f) / 2.f, (hb.size.y + 55.f) / 2.f});
        banner.setPosition({450.f, y});

        sf::ConvexShape bannerInner = makeRoundedRect({hb.size.x + 66.f, hb.size.y + 34.f}, 14.f);
        bannerInner.setFillColor(sf::Color::Transparent);
        bannerInner.setOutlineColor(sf::Color(255, 230, 150, 160));
        bannerInner.setOutlineThickness(2.f);
        bannerInner.setOrigin({(hb.size.x + 66.f) / 2.f, (hb.size.y + 34.f) / 2.f});
        bannerInner.setPosition({450.f, y});

        window.draw(banner);
        window.draw(bannerInner);
        window.draw(heading);
    };

    // ---------- Saare buttons yahan banaye gaye hai ----------
    Button startBtn(uiFont, "Start Game", 365, 248, 170, 52, 15);
    Button exitBtn(uiFont, "Exit", 365, 328, 170, 52, 15);

    Button warriorBtn(uiFont, "Warrior (HP150/ATK30)", 250, 250, 400, 60, 18);
    Button archerBtn(uiFont, "Archer (HP100/ATK20)", 250, 330, 400, 60, 18);

    Button goblinBtn(uiFont, "Goblin (HP50/ATK15)", 250, 250, 400, 60, 18);

    Button dragonBtn(uiFont, "Dragon (HP190/ATK40)", 250, 330, 400, 60, 18);

    Button attackBtn(uiFont, "Attack", 60, 500, 150, 60, 18);
    Button potionBtn(uiFont, "Potion", 220, 500, 150, 60, 18);
    Button defendBtn(uiFont, "Defend", 380, 500, 150, 60, 18);
    Button* specialBtn = nullptr; // hero select hone ke baad banega (Heavy Attack / Shoot Arrow)

    // ---------- PHASE 3: Game Logic - state variables ----------
    Hero* hero = nullptr;
    Enemy* enemy = nullptr;
    int enemyChoice = 0;   // 1 = Goblin, 2 = Dragon
    int heroChoice = 0;    // 1 = Warrior, 2 = Archer
    string battleLog = "Battle shuru!";
    bool playersTurn = true;   // turn system: pehle player ki baari
    bool gameOverSoundPlayed = false;

    bool enemyAttackAnimating = false; // enemy ka attack animation chal raha hai kya
    int pendingEnemyDamage = 0;        // animation khatam hone ke baad ye damage lagega

    bool heroAttackAnimating = false;  // hero ka attack animation chal raha hai kya
    int pendingHeroDamage = 0;
    string pendingHeroLog = "";

    sf::Clock deltaClock; // har frame ke beech ka time (dt) nikalne ke liye

    // ============================================================
    //  PHASE 3 - GAME LOOP (Turn System / State Machine)
    // ============================================================
    while (window.isOpen())
    {
        float dt = deltaClock.restart().asSeconds(); // is frame aur pichle frame ke beech ka time

        bool mouseClicked = false;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mb->button == sf::Mouse::Button::Left)
                    mouseClicked = true;
            }
        }

        if (state == GameState::MainMenu)
        {
            // ---------- Menu ke decorative sprites animate karna ----------
            menuFlyClock += dt;

            menuDragonX += menuDragonSpeed * dt;
            if (menuDragonX > menuDragonRightBound)
                menuDragonX = menuDragonLeftBound; // dragon screen paar ho gaya to wapas left se
            float dragonBobY = menuDragonBaseY + 12.f * sin(menuFlyClock * 1.6f); // upar-neeche bobbing motion

            if (menuDragonSprite)
            {
                menuDragonSprite->setPosition({menuDragonX, dragonBobY});
                menuDragonAnimator.update(dt, *menuDragonSprite);
            }
            if (menuWarriorSprite) menuWarriorAnimator.update(dt, *menuWarriorSprite);
            if (menuArcherSprite)  menuArcherAnimator.update(dt, *menuArcherSprite);

            // title ka pulse/glow effect (size aur alpha dono sine wave se badal rahe)
            float pulse = 1.f + 0.04f * sin(menuPulseClock.getElapsedTime().asSeconds() * 2.5f);
            title.setScale({pulse, pulse});
            titleGlow.setScale({pulse * 1.05f, pulse * 1.05f});
            uint8_t glowAlpha = (uint8_t)(100 + 60 * sin(menuPulseClock.getElapsedTime().asSeconds() * 2.5f));
            sf::Color glowColor = titleGlow.getFillColor();
            glowColor.a = glowAlpha;
            titleGlow.setFillColor(glowColor);

            startBtn.update(mousePos, &engine);
            exitBtn.update(mousePos, &engine);

            if (startBtn.isClicked(mousePos, mouseClicked))
            {
                ma_engine_play_sound(&engine, "Audio2.wav", NULL);

                if (battleground2Sprite)
                    activeBgSprite = battleground2Sprite; // background badal diya
                state = GameState::HeroSelect;
            }
            if (exitBtn.isClicked(mousePos, mouseClicked)) window.close();
        }
        else if (state == GameState::HeroSelect)
        {
            if (warriorBtn.isClicked(mousePos, mouseClicked))
            {
                hero = new Warrior("Hero");
                heroChoice = 1;
                ma_engine_play_sound(&engine, "Audio3.mp3", NULL);
                state = GameState::EnemySelect;
            }
            if (archerBtn.isClicked(mousePos, mouseClicked))
            {
                hero = new Archer("Hero");
                heroChoice = 2;
                ma_engine_play_sound(&engine, "Audio3.mp3", NULL);
                state = GameState::EnemySelect;
            }
        }
        else if (state == GameState::EnemySelect)
        {
            if (goblinBtn.isClicked(mousePos, mouseClicked))
            {
                // ---------- naya battle start - sab kuch reset karna ----------
                enemy = new Goblin("Green Goblin");
                enemyChoice = 1;
                battleLog = "Battle shuru hui!";
                gameOverSoundPlayed = false;
                enemyAttackAnimating = false;
                heroAttackAnimating = false;

                playersTurn = true; // player pehle chalega

                if (menuMusicLoaded) ma_sound_stop(&menuMusic);

                delete specialBtn;
                specialBtn = new Button(uiFont, hero->specialName(), 540, 500, 150, 60, 15);

                if (goblinSprite) goblinAnimator.reset(&goblinIdleSheet, true);

                if (heroChoice == 1 && warriorSprite) warriorAnimator.reset(&warriorIdleSheet, true);
                if (heroChoice == 2 && archerSprite) archerAnimator.reset(&archerIdleSheet, true);
                arrowActive = false;

                state = GameState::Battle;
            }
            if (dragonBtn.isClicked(mousePos, mouseClicked))
            {
                enemy = new Dragon("Ballerion");
                enemyChoice = 2;
                battleLog = "Battle shuru hui!";
                gameOverSoundPlayed = false;
                enemyAttackAnimating = false;
                heroAttackAnimating = false;
                fireActive = false;
                arrowActive = false;

                playersTurn = true;

                if (menuMusicLoaded) ma_sound_stop(&menuMusic);

                ma_engine_play_sound(&engine, "Audio4.wav", NULL);
                delete specialBtn;
                specialBtn = new Button(uiFont, hero->specialName(), 540, 500, 150, 60, 15);

                dragonCurrentX = -450.f;   // dragon door se udd ke andar aayega
                dragonEntering = true;
                if (dragonSprite) dragonAnimator.reset(&dragonIdleSheet, true);

                if (heroChoice == 1 && warriorSprite) warriorAnimator.reset(&warriorIdleSheet, true);
                if (heroChoice == 2 && archerSprite) archerAnimator.reset(&archerIdleSheet, true);

                state = GameState::Battle;
            }
        }
        else if (state == GameState::Battle)
        {
            // ---------- Enemy animation update (Goblin / Dragon) ----------
            if (enemyChoice == 1 && goblinSprite)
                goblinAnimator.update(dt, *goblinSprite);

            if (enemyChoice == 2 && dragonSprite)
            {
                if (dragonEntering)
                {
                    dragonCurrentX += dragonEntranceSpeed * dt; // dragon dheere dheere andar aayega
                    if (dragonCurrentX >= dragonRestX)
                    {
                        dragonCurrentX = dragonRestX;
                        dragonEntering = false; // resting position pe pohonch gaya
                    }
                }

                if (dragonCurrentFlip)
                    dragonSprite->setPosition({dragonCurrentX + dragonIdleSheet.frameWidth * dragonScale, dragonPosY});
                else
                    dragonSprite->setPosition({dragonCurrentX, dragonPosY});

                dragonAnimator.update(dt, *dragonSprite);

                if (fireActive && fireSprite)
                {
                    // fire ka progress dragon ke attack animation ke frame ke hisab se calculate ho raha hai
                    float progress = (dragonAttackSheet.frameCount > 1)
                        ? (float)dragonAnimator.currentFrame / (float)(dragonAttackSheet.frameCount - 1)
                        : 1.f;
                    float fx = fireStartX + (fireEndX - fireStartX) * progress; // fire dragon se enemy(hero) tak jayega
                    fireSprite->setPosition({fx, fireY});
                    fireAnimator.update(dt, *fireSprite);
                }
            }

            // ---------- Hero animation update (Warrior / Archer) ----------
            if (heroChoice == 1 && warriorSprite)
                warriorAnimator.update(dt, *warriorSprite);

            if (heroChoice == 2 && archerSprite)
                archerAnimator.update(dt, *archerSprite);

            if (heroChoice == 2 && arrowActive && arrowSprite)
            {
                // arrow ka progress bhi archer ke attack animation ke frame ke hisab se
                float progress = (archerAttackSheet.frameCount > 1)
                    ? (float)archerAnimator.currentFrame / (float)(archerAttackSheet.frameCount - 1)
                    : 1.f;
                float ax = arrowStartX + (arrowEndX - arrowStartX) * progress;
                arrowSprite->setPosition({ax + arrowTexture.getSize().x * arrowScale, arrowY});
            }

            // ---------- PLAYER TURN (Rishav ka kaam - attack/potion/defend logic) ----------
            if (playersTurn && hero->isAlive() && enemy->isAlive())
            {
                if (heroAttackAnimating)
                {
                    // pehle animation khatam hone ka wait karna hai, tabhi actual damage lagega
                    bool animFinished = true;
                    if (heroChoice == 1 && warriorSprite) animFinished = warriorAnimator.finished;
                    else if (heroChoice == 2 && archerSprite) animFinished = archerAnimator.finished;

                    if (animFinished)
                    {
                        enemy->takedamage(pendingHeroDamage);
                        battleLog = pendingHeroLog + " " + to_string(pendingHeroDamage) + " damage.";
                        playersTurn = false; // ab enemy ki baari
                        heroAttackAnimating = false;
                        arrowActive = false;

                        // attack khatam, wapas idle animation pe switch
                        if (heroChoice == 1 && warriorSprite)
                        {
                            warriorSprite->setTexture(warriorIdleSheet.texture, true);
                            warriorAnimator.reset(&warriorIdleSheet, true);
                        }
                        else if (heroChoice == 2 && archerSprite)
                        {
                            archerSprite->setTexture(archerIdleSheet.texture, true);
                            archerAnimator.reset(&archerIdleSheet, true);
                        }
                    }
                }
                else if (attackBtn.isClicked(mousePos, mouseClicked)) // normal Attack button
                {
                    pendingHeroDamage = hero->getAttack();
                    pendingHeroLog = hero->getName() + " ne attack kiya!";
                    ma_engine_play_sound(&engine, "Audio6.mp3", NULL);
                    ma_engine_play_sound(&engine, "Audio7.mp3", NULL);

                    if (heroChoice == 1 && warriorSprite)
                    {
                        warriorSprite->setTexture(warriorAttackSheet.texture, true);
                        warriorAnimator.reset(&warriorAttackSheet, false); // attack animation ek baar chalegi
                        heroAttackAnimating = true;
                    }
                    else if (heroChoice == 2 && archerSprite)
                    {
                        archerSprite->setTexture(archerAttackSheet.texture, true);
                        archerAnimator.reset(&archerAttackSheet, false);
                        heroAttackAnimating = true;

                        arrowActive = true; // arrow bhi fly karna shuru karega
                        arrowStartX = archerPosX + archerIdleSheet.frameWidth * archerScale * 0.15f;
                        arrowEndX = 220.f;
                        arrowY = archerPosY + archerIdleSheet.frameHeight * archerScale * 0.45f;
                    }
                    else
                    {
                        // agar sprite load na hua ho to bina animation ke direct damage
                        enemy->takedamage(pendingHeroDamage);
                        battleLog = pendingHeroLog + " " + to_string(pendingHeroDamage) + " damage.";
                        playersTurn = false;
                    }
                }
                else if (potionBtn.isClicked(mousePos, mouseClicked)) // Potion button
                {
                    battleLog = hero->usePotion();
                    playersTurn = false;
                }
                else if (defendBtn.isClicked(mousePos, mouseClicked)) // Defend button
                {
                    battleLog = hero->defend();
                    playersTurn = false;
                }
                else if (specialBtn && specialBtn->isClicked(mousePos, mouseClicked)) // Special Attack button
                {
                    pendingHeroDamage = hero->specialAttack();
                    pendingHeroLog = hero->getName() + " ne " + hero->specialName() + " kiya!";
                    ma_engine_play_sound(&engine, "Audio6.mp3", NULL);

                    if (heroChoice == 1 && warriorSprite)
                    {
                        warriorSprite->setTexture(warriorAttackSheet.texture, true);
                        warriorAnimator.reset(&warriorAttackSheet, false);
                        heroAttackAnimating = true;
                    }
                    else if (heroChoice == 2 && archerSprite)
                    {
                        archerSprite->setTexture(archerAttackSheet.texture, true);
                        archerAnimator.reset(&archerAttackSheet, false);
                        heroAttackAnimating = true;

                        arrowActive = true;
                        arrowStartX = archerPosX + archerIdleSheet.frameWidth * archerScale * 0.15f;
                        arrowEndX = 220.f;
                        arrowY = archerPosY + archerIdleSheet.frameHeight * archerScale * 0.45f;
                    }
                    else
                    {
                        enemy->takedamage(pendingHeroDamage);
                        battleLog = pendingHeroLog + " " + to_string(pendingHeroDamage) + " damage.";
                        playersTurn = false;
                    }
                }
            }
            // ---------- ENEMY TURN (AI attack logic) ----------
            else if (!playersTurn && hero->isAlive() && enemy->isAlive())
            {
                if (enemyChoice == 1) // Goblin ka turn
                {
                    if (!enemyAttackAnimating)
                    {
                        Goblin* g = (Goblin*)enemy;
                        pendingEnemyDamage = g->quickStrike(); // 30% chance double damage
                        enemyAttackAnimating = true;
                        if (goblinSprite)
                        {
                            goblinSprite->setTexture(goblinAttackSheet.texture, true);
                            goblinAnimator.reset(&goblinAttackSheet, false);
                        }
                    }
                    else if (!goblinSprite || goblinAnimator.finished)
                    {
                        // attack animation khatam hone ke baad hi actual damage lagta hai
                        hero->takedamage(pendingEnemyDamage);
                        battleLog += "  |  " + enemy->getName() + " ne " + to_string(pendingEnemyDamage) + " damage diya.";
                        playersTurn = true; // wapas player ki baari
                        enemyAttackAnimating = false;

                        if (goblinSprite)
                        {
                            goblinSprite->setTexture(goblinIdleSheet.texture, true);
                            goblinAnimator.reset(&goblinIdleSheet, true);
                        }
                    }
                }
                else // Dragon ka turn
                {
                    if (!enemyAttackAnimating)
                    {
                        Dragon* d = (Dragon*)enemy;
                        bool isFire = (rand() % 100 < 40); // 40% chance fire attack, warna normal attack
                        pendingEnemyDamage = isFire ? d->fireAttack() : d->aiAttack();
                        enemyAttackAnimating = true;

                        if (dragonSprite)
                        {
                            dragonSprite->setTexture(dragonAttackSheet.texture, true);
                            dragonAnimator.reset(&dragonAttackSheet, false);

                            dragonCurrentFlip = dragonAttackFlipNeeded; // attack ke liye flip direction badalna
                            dragonSprite->setScale({dragonCurrentFlip ? -dragonScale : dragonScale, dragonScale});
                        }

                        if (isFire && fireSprite)
                        {
                            fireActive = true;
                            float dragonRightEdge = dragonCurrentX + dragonIdleSheet.frameWidth * dragonScale * 0.85f;
                            fireStartX = dragonRightEdge;
                            fireEndX = 700.f;
                            fireY = dragonPosY + dragonIdleSheet.frameHeight * dragonScale * 0.35f;
                            fireAnimator.reset(&fireSheet, true);
                        }
                        else
                        {
                            fireActive = false;
                        }
                    }
                    else if (!dragonSprite || dragonAnimator.finished)
                    {
                        hero->takedamage(pendingEnemyDamage);
                        battleLog += "  |  " + enemy->getName() + " ne " + to_string(pendingEnemyDamage) + " damage diya.";
                        playersTurn = true;
                        enemyAttackAnimating = false;
                        fireActive = false;

                        if (dragonSprite)
                        {
                            dragonSprite->setTexture(dragonIdleSheet.texture, true);
                            dragonAnimator.reset(&dragonIdleSheet, true);

                            dragonCurrentFlip = dragonIdleFlipNeeded; // wapas idle direction
                            dragonSprite->setScale({dragonCurrentFlip ? -dragonScale : dragonScale, dragonScale});
                        }
                    }
                }
            }

            // ---------- Win / Lose condition check ----------
            if (!enemy->isAlive() || !hero->isAlive())
            {
                if (!gameOverSoundPlayed)
                {
                    ma_engine_play_sound(&engine, "AudioL.mp3", NULL);
                    gameOverSoundPlayed = true;

                    if (menuMusicLoaded)
                    {
                        ma_sound_seek_to_pcm_frame(&menuMusic, 0);
                        ma_sound_start(&menuMusic); // menu music wapas se shuru
                    }
                }
                state = GameState::GameOver;
            }
        }
        else if (state == GameState::GameOver)
        {
            if (mouseClicked)
            {
                // game reset karke wapas Main Menu pe bhej diya
                delete hero; hero = nullptr;
                delete enemy; enemy = nullptr;

                activeBgSprite = gameBgSprite;
                state = GameState::MainMenu;
            }
        }

        // ============================================================
        //  PHASE 4 - RENDERING (sab kuch screen pe draw karna)
        // ============================================================
        window.clear(sf::Color(20, 20, 35));

        if (activeBgSprite)
            window.draw(*activeBgSprite);

        if (state == GameState::MainMenu)
        {
            if (menuDragonSprite)  window.draw(*menuDragonSprite);
            if (menuWarriorSprite) window.draw(*menuWarriorSprite);
            if (menuArcherSprite)  window.draw(*menuArcherSprite);

            window.draw(titleBanner);
            window.draw(titleBannerInner);
            window.draw(titleGlow);
            window.draw(title);
            startBtn.draw(window);
            exitBtn.draw(window);
        }
        else if (state == GameState::HeroSelect)
        {
            drawSectionHeading("Choose Your Hero", 150.f);
            warriorBtn.draw(window);
            archerBtn.draw(window);
        }
        else if (state == GameState::EnemySelect)
        {
            drawSectionHeading("Choose Your Enemy", 150.f);
            goblinBtn.draw(window);
            dragonBtn.draw(window);
        }
        else if (state == GameState::Battle)
        {
            // enemy ke hisab se sahi special background dikhana
            if (enemyChoice == 1 && goblinBattleBgSprite)
                window.draw(*goblinBattleBgSprite);

            if (enemyChoice == 2 && dragonBattleBgSprite)
                window.draw(*dragonBattleBgSprite);

            // ---------- Hero ka naam + health bar ----------
            sf::Text heroName(uiFont, hero->getName() + " (Lvl " + to_string(hero->getLevel()) + ")", 16);
            heroName.setFillColor(sf::Color::White);
            heroName.setOutlineColor(sf::Color::Black);
            heroName.setOutlineThickness(2.f);
            heroName.setPosition({520, 60});
            window.draw(heroName);
            drawHealthBar(window, 520, 95, 220, 20, hero->getHp(), hero->getMaxHp());

            sf::Text heroHpText(uiFont, to_string(hero->getHp()) + " / " + to_string(hero->getMaxHp()), 13);
            heroHpText.setFillColor(sf::Color::White);
            heroHpText.setOutlineColor(sf::Color::Black);
            heroHpText.setOutlineThickness(2.f);
            heroHpText.setPosition({528, 97});
            window.draw(heroHpText);

            // ---------- Enemy ka naam + health bar ----------
            float enemyNameX = 80.f, enemyNameY = 60.f;
            float enemyBarX = 80.f, enemyBarY = 95.f, enemyBarW = 220.f;

            sf::Text enemyName(uiFont, enemy->getName(), 16);
            enemyName.setFillColor(sf::Color::White);
            enemyName.setOutlineColor(sf::Color::Black);
            enemyName.setOutlineThickness(2.f);
            enemyName.setPosition({enemyNameX, enemyNameY});
            window.draw(enemyName);
            drawHealthBar(window, enemyBarX, enemyBarY, enemyBarW, 20, enemy->getHp(), enemy->getMaxHp());

            sf::Text enemyHpText(uiFont, to_string(enemy->getHp()) + " / " + to_string(enemy->getMaxHp()), 13);
            enemyHpText.setFillColor(sf::Color::White);
            enemyHpText.setOutlineColor(sf::Color::Black);
            enemyHpText.setOutlineThickness(2.f);
            enemyHpText.setPosition({enemyBarX + 8.f, enemyBarY + 2.f});
            window.draw(enemyHpText);

            // ---------- Sprites draw karna (enemy pehle, phir hero, phir effects) ----------
            if (enemyChoice == 1 && goblinSprite)
                window.draw(*goblinSprite);

            if (enemyChoice == 2 && dragonSprite)
                window.draw(*dragonSprite);
            if (enemyChoice == 2 && fireActive && fireSprite)
                window.draw(*fireSprite);

            if (heroChoice == 1 && warriorSprite)
                window.draw(*warriorSprite);

            if (heroChoice == 2 && archerSprite)
                window.draw(*archerSprite);
            if (heroChoice == 2 && arrowActive && arrowSprite)
                window.draw(*arrowSprite);

            // ---------- Battle log text (last action kya hua) ----------
            sf::Text logText(uiFont, battleLog, 14);
            logText.setFillColor(sf::Color(255, 230, 150));
            logText.setOutlineColor(sf::Color(60, 30, 10));
            logText.setOutlineThickness(2.f);
            logText.setPosition({80, 300});
            window.draw(logText);

            // player ka turn hai to buttons dikhao, warna "Enemy's Turn..." dikhao
            if (playersTurn)
            {
                attackBtn.draw(window);
                potionBtn.draw(window);
                defendBtn.draw(window);
                if (specialBtn) specialBtn->draw(window);
            }
            else
            {
                sf::Text waitText(uiFont, "Enemy's Turn...", 18);
                waitText.setFillColor(sf::Color::White);
                waitText.setOutlineColor(sf::Color::Black);
                waitText.setOutlineThickness(2.f);
                waitText.setPosition({100, 510});
                window.draw(waitText);
            }
        }
        else if (state == GameState::GameOver)
        {
            // ---------- PHASE 5: Testing & Polish - Game Over screen ----------
            bool won = enemy && !enemy->isAlive();
            sf::Text resultText(uiFont, won ? "YOU WON!" : "GAME OVER", 44);
            resultText.setFillColor(won ? sf::Color::Green : sf::Color::Red);
            resultText.setOutlineColor(sf::Color::Black);
            resultText.setOutlineThickness(4.f);
            resultText.setPosition({230, 250});
            window.draw(resultText);
        }

        window.display();
    }

    // ---------- Cleanup - saara allocated memory free karna (memory leak na ho) ----------
    delete hero;
    delete enemy;
    delete specialBtn;
    delete goblinSprite;
    delete dragonSprite;
    delete fireSprite;
    delete warriorSprite;
    delete archerSprite;
    delete arrowSprite;
    delete gameBgSprite;
    delete battleground2Sprite;
    delete goblinBattleBgSprite;
    delete dragonBattleBgSprite;
    delete menuDragonSprite;
    delete menuWarriorSprite;
    delete menuArcherSprite;

    if (menuMusicLoaded) ma_sound_uninit(&menuMusic);
    ma_engine_uninit(&engine);
    return 0;
}

//g++ Project.c++ -o sfml_game.exe -I C:\msys64\ucrt64\include -L C:\msys64\ucrt64\lib -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lole32
//.\sfml_game.exe
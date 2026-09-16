============================================================
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
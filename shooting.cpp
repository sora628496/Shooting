// CUI.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <ctime>
#include <cstdlib>

#define WIDTH 8
#define HEIGHT 10

#define FALLPROB 0.1

#define STARTTEMP 100
#define ENDTEMP 60

#define FPS 15
#define BULLET_SPEED 1
#define ITEM_SPEED 1

//----------------------------------------------------------

bool isKeyPressed(int vKey) {
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}

//----------------------------------------------------------

class Item {
protected:
    int kind;
    int x;
    int y;
    bool isFalling;
public:
    Item() {
        initialize(0);
    }

    void initialize(int setX) {
        kind = 5;
        x = setX;
        y = 0;
        isFalling = false;
    }

    int getX() { return x; }
    int getY() { return y; }
    int getKind() { return kind; }
    bool getIsFalling() { return isFalling; }

    void move() {
        if (!isFalling) {
            if ((rand() % 10) / 10.0 < FALLPROB) {
                fall();
            }
            return;
        }
        if (y + 1 >= HEIGHT) {
            isFalling = false;
        }
        else {
            y++;
        }
    }

    void fall() {
        isFalling = true;
        int r = rand() % 5;
        if (r == 0 || r == 1) {
            kind = 5;
        }
        else {
            kind = r + 4;
        }
        y = 0;
    }

    bool isAt(const int& objX, const int& objY) {
        if (!isFalling)return false;
        if (x == objX && y == objY) {
            return true;
        }
        else {
            return false;
        }
    }

    int isInTea(int level, int height) {
        if (!isFalling)return -1;
        if (y >= height - level) {
            return kind;
        }
        else {
            return -1;
        }
    }

    void crash() {
        isFalling = false;
    }
};

class Player {
private:
    int x;
    int y;
    bool alive;
public:
    Player() {
        initialize();
    }

    void initialize() {
        x = 5;
        y = 7;
        alive = true;
    }

    int getX() { return x; }
    int getY() { return y; }

    bool collide(Item items[], const int& n) {
        if (!alive)return false;
        for (int i = 0;i < n;i++) {
            if (items[i].isAt(x, y)) {
                alive=false;
                return true;
            }
        }
        return false;
    }

    bool isInTea(int level, int height) {
        if (!alive)return false;
        if (y >= height - level) {
            return true;
        }
        else {
            return false;
        }
    }

    void move() {
        if (isKeyPressed(VK_RIGHT)) {
            if (x + 1 <= WIDTH)x++;
        }
        else if (isKeyPressed(VK_UP)) {
            if (0 < y - 1)y--;
        }
        else if (isKeyPressed(VK_LEFT)) {
            if (0 < x - 1)x--;
        }
        else if (isKeyPressed(VK_DOWN)) {
            if (y + 1 < HEIGHT)y++;
        }
    }
};

class Bullet {
private:
    int x;
    int y;
    bool isShot;
public:
    Bullet() {
        initialize();
    }

    void initialize() {
        x = 0;
        y = 0;
        isShot = false;
    }

    int getX() { return x; }
    int getY() { return y; }

    bool getIsShot() {
        return isShot;
    }
    
    void move() {
        if (!isShot)return;
        if (y - 1 < 0) {
            isShot = false;
        }
        else {
            y--;
        }
    }

    void collide(Item items[], const int& n) {
        if (!isShot)return;
        for (int i = 0;i < n;i++) {
            if (items[i].isAt(x, y) || items[i].isAt(x, y+1)) {
                items[i].crash();
                crash();
                break;
            }
        }
    }

    void crash() {
        isShot = false;
    }

    void shoot(int setX,int setY) {
        if (isShot)return;
        x = setX;
        y = setY;
        isShot = true;
    }
};

class Main {
private:
    const std::string itemText[9] = { "　","｜" ,"茶" ,"自" ,"弾" ,"氷" ,"塩" ,"糖" ,"乳" };

    Player player;
    Bullet bullet;
    Item items[WIDTH];

    int iceCount;
    int level;
    int temperature;

    int field[HEIGHT][WIDTH + 2] = { 0 };

    bool gameFinished;

    void setCursorPos(int x, int y) {
        HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos;
        pos.X = 2*x;
        pos.Y = y;
        SetConsoleCursorPosition(hCons, pos);
    }

public:
    //コンストラクタ
    Main() {
        srand(time(NULL));
        player = Player();
        bullet = Bullet();
        for (int i = 0;i < WIDTH;i++) {
            items[i].initialize(i + 1);
        }
        iceCount = 0;
        level = 1;
        temperature = STARTTEMP;
        for (int y = 0;y < HEIGHT;y++) {
            for (int x = 0;x < WIDTH + 2;x++) {
                if (x == 0 || x == WIDTH + 1) {
                    field[y][x] = 1;
                }
                else if (y >= HEIGHT - level) {
                    field[y][x] = 2;
                }
            }
        }
        gameFinished = true;
    }

    void gameover(const std::string& text, bool cleared = false) {
        gameFinished = true;
        setCursorPos(0, HEIGHT + 2);
        if (cleared) {
            std::cout << "クリアー！\n\nエンターキーでリトライ\n";
        }
        else {
            std::cout << "ゲームオーバー\n" << text << "\nエンターキーでリトライ\n";
        }
    }

    void retry() {
        system("cls");
        player.initialize();
        bullet.initialize();
        for (int i = 0;i < WIDTH;i++) {
            items[i].initialize(i+1);
        }
        iceCount = 0;
        level = 1;
        temperature = STARTTEMP;
        for (int y = 0;y < HEIGHT;y++) {
            for (int x = 0;x < WIDTH + 2;x++) {
                if (x == 0 || x == WIDTH + 1) {
                    field[y][x] = 1;
                }
                else if (y >= HEIGHT - level) {
                    field[y][x] = 2;
                }
                else {
                    field[y][x] = 0;
                }
            }
        }
        gameFinished = false;
    }

    void collide() {
        if (player.isInTea(level,HEIGHT)) {
            gameover("プレイヤーが茶にダイブしました\n");
        }
        for (int i = 0;i < WIDTH;i++) {
            int inTeaItem = items[i].isInTea(level, HEIGHT);
            if (inTeaItem == -1)continue;
            if (inTeaItem == 5) {
                iceCount++;
                temperature-=2;
                if (temperature == ENDTEMP) {
                    gameover("",true);
                }
                if (temperature%10 == 0) {
                    level++;
                    for (int i = 1;i < WIDTH + 1;i++) {
                        field[HEIGHT - level][i] = 2;
                    }
                }
            }
            else {
                gameover(itemText[inTeaItem] + "が茶に混入しました\n");
                break;
            }
            items[i].crash();
        }
        if (player.collide(items, WIDTH)) {
            gameover("プレイヤーがアイテムに衝突しました\n");
        }
        bullet.collide(items,WIDTH);
    }

    void start() {
        draw();
        setCursorPos(0, HEIGHT + 2);
        std::cout << "エンターキーでスタート\n";
        int itemCount = 0;
        int bulletCount = 0;
        while (true) {
            if (isKeyPressed(VK_ESCAPE)) {
                break;
            }
            
            if (gameFinished) {
                if (isKeyPressed(VK_RETURN)) {
                    retry();
                    itemCount = 0;
                    bulletCount = 0;
                    draw();
                }
                Sleep(1000/FPS);
            }
            else {
                if (isKeyPressed(VK_SPACE)) {
                    bullet.shoot(player.getX(), player.getY() - 1);
                }

                if (itemCount >= FPS/ITEM_SPEED-1) {
                    itemCount = 0;
                    for (int i = 0;i < WIDTH;i++) {
                        items[i].move();
                    }
                }

                player.move();

                if (bulletCount >= FPS/BULLET_SPEED-1) {
                    bulletCount = 0;
                    bullet.move();
                }
                collide();
                draw();
                Sleep(1000/FPS);

                itemCount++;
                bulletCount++;
            }
        }
    }

    void draw() {
        for (int y = 0;y < HEIGHT-level;y++) {
            for (int x = 1;x < WIDTH + 1;x++) {
                field[y][x] = 0;
            }
        }
        
        if (player.getY() < HEIGHT - level) {
            field[player.getY()][player.getX()] = 3;
        }
        if (bullet.getIsShot()) {
            field[bullet.getY()][bullet.getX()] = 4;
        }

        for (int i = 0;i < WIDTH;i++) {
            if (!items[i].getIsFalling())continue;
            if (items[i].getY() >= HEIGHT - level)continue;
            int x = items[i].getX();
            int y = items[i].getY();
            int kind = items[i].getKind();
            field[y][x] = kind;
        }

        std::string text = "\n";
        for (int y = 0;y < HEIGHT;y++) {
            for (int x = 0;x < WIDTH + 2;x++) {
                text += itemText[field[y][x]];
            }
            if (y == 0) {
                text += "温度：" + std::to_string(temperature) + "　";
            }
            if (y == 2) {
                text += "矢印キー：移動";
            }
            if (y == 3) {
                text += "スペースキー：弾を撃つ";
            }
            text += "\n";
        }
        text += "\n\n\n\n";
        setCursorPos(0, 0);
        std::cout << text;
    }
};

int main()
{
    Main main = Main();
    main.start();
    return 0;
}

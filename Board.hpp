#pragma once 
#include <vector> 
#include <SFML/Graphics.hpp> 

// 1マスを表すクラス
class Block {
public:
    bool filled;          // ブロックが埋まっているかどうか
    sf::Color color;      // ブロックの色

    // コンストラクタ（デフォルトは空で黒色）
    Block(bool f = false, sf::Color c = sf::Color::Black);

    // ブロックを描画する
    void draw(sf::RenderWindow& window, int x, int y, int size = 40);
};

// テトリスの盤面を表すクラス
class Board {
public:
    static const int WIDTH = 10;   // 横幅（列数）
    static const int HEIGHT = 20;  // 縦幅（行数）

    // 盤面データ（高さ×幅の2次元配列）
    std::vector<std::vector<Block>> grid;

    // コンストラクタ（空の盤面を作成）
    Board();

    // 盤面を描画する
    void draw(sf::RenderWindow& window);

    // 指定座標が埋まっているかどうかを判定
    bool isOccupied(int x, int y);

    // 指定座標にブロックを配置する
    void placeBlock(int x, int y, sf::Color color);

    // そろったラインを消去し、消した行数を返す
    int clearLines();
};

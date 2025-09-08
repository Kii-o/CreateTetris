#include "Board.hpp" 
#include <iostream>

// Blockクラスのコンストラクタ
Block::Block(bool f, sf::Color c) : filled(f), color(c) {}

// ブロックを描画する
void Block::draw(sf::RenderWindow& window, int x, int y, int size) {
    // 枠付きで描画するため、1px 小さくしている
    sf::RectangleShape rect(sf::Vector2f(size - 1, size - 1));
    rect.setPosition(x, y);
    rect.setFillColor(filled ? color : sf::Color(30, 30, 30)); // 空は濃いグレー
    window.draw(rect);
}

// Boardのコンストラクタ（空の20×10盤面を作る）
Board::Board() {
    grid.resize(HEIGHT, std::vector<Block>(WIDTH));
}

// 盤面全体を描画
void Board::draw(sf::RenderWindow& window) {
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            grid[y][x].draw(window, x * 40, y * 40);
}

// 指定座標にブロックがあるかどうかを判定
bool Board::isOccupied(int x, int y) {
    // 横がはみ出したらtrue（移動できない）
    if (x < 0 || x >= WIDTH || y >= HEIGHT) return true;
    // 上側（y < 0）はまだ盤面外なのでfalse
    if (y < 0) return false;
    return grid[y][x].filled;
}

// 指定座標にブロックを配置する
void Board::placeBlock(int x, int y, sf::Color color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        grid[y][x].filled = true;
        grid[y][x].color = color;
    }
}

// 揃ったラインを削除し、削除した行数を返す
int Board::clearLines() {
    //関数が呼び出されているかの確認
    //std::cout << "[DEBUG] clearLines() called\n";
    int linesCleared = 0;
    for (int y = HEIGHT - 1; y >= 0; --y) {
        //ライン消去デバッグ
        //std::cout << "Row " << y << ": "<<std::endl;
        bool full = true;
        // 1行がすべて埋まっているか確認
        for (int x = 0; x < WIDTH; ++x)
            if (!grid[y][x].filled) full = false;

        if (full) {
            ++linesCleared;
            // 該当行を上から1つずつ落とす
            for (int row = y; row > 0; --row)
                for (int x = 0; x < WIDTH; ++x)
                    grid[row][x] = grid[row - 1][x];
            // 一番上の行を空にする
            for (int x = 0; x < WIDTH; ++x)
                grid[0][x] = Block(false, sf::Color::Black);
            // 同じyを再チェック（連続消去対応）
            ++y;
        }
    }
    return linesCleared;
}

#pragma once 
#include "Board.hpp" 
#include <SFML/Graphics.hpp> 
#include <array> 
#include <deque> 
#include <random> 

// ==== ピースの種類（7種のテトリミノ） ====
enum class PieceType { T, S, Z, I, O, J, L };

// ==== ピース形状と色の定義（実体は .cpp 側で定義） ====
// それぞれのミノの4マス分の相対座標
extern const std::array<std::array<sf::Vector2i, 4>, 7> PIECE_SHAPES;
// それぞれのミノの色
extern const std::array<sf::Color, 7> PIECE_COLORS;

// ==== ピースを表すクラス ====
class Piece {
public:
    PieceType type;                          // 自分の種類を覚える(Holdで使用する)
    sf::Color color;                         // このピースの色
    std::array<sf::Vector2i, 4> blocks;      // 4つのブロックの相対座標
    int x = 3, y = 0;                        // フィールド上での位置（左上が基準）

    Piece(PieceType type);                   // コンストラクタ（種類を指定して生成）
    void draw(sf::RenderWindow& window);     // フィールド上に描画
    void drawPreview(sf::RenderWindow& window, int px, int py, int size = 20); // NextやHoldの小さな表示用
    bool canMove(Board& board, int dx, int dy); // 指定方向に動けるか判定
    void move(int dx, int dy);               // 実際に移動する
    // 右回転なら clockwise = true、左回転なら false
    void rotate(Board& board, bool clockwise);
    void place(Board& board);                // ボードに固定する
};

// ==== 7種1巡の「bag方式」を管理するクラス ====
class Bag {
private:
    std::vector<PieceType> pieces;           // シャッフル済みの7種類を入れる袋
    std::mt19937 rng;                        // 乱数生成器
    void shuffleBag();                       // 新しい7種をシャッフルして袋に補充
public:
    Bag();                                   // コンストラクタ（乱数初期化）
    PieceType getNext();                     // 1つ取り出し、袋が空なら再補充
};

// ==== ゲーム全体を管理するクラス ====
class Game {
private:
    sf::RenderWindow window;                 // ゲームウィンドウ
    Board board;                             // 盤面（フィールド）
    Bag bag;                                 // 7種1巡の袋
    Piece currentPiece;                      // 現在操作中のピース
    std::deque<PieceType> nextQueue;         // Next表示用のキュー（複数個分）
    PieceType holdPiece;                     // Holdに入っているピース
    bool holdUsed = false, holdExists = false; // Holdを使ったかどうか、存在するか

    sf::Clock fallClock, moveClock;          // 自動落下タイマー、横移動タイマー
    float fallInterval = 0.5f;               // 自動落下の間隔（秒）
    float moveInterval = 0.15f;              // 横移動の連続入力の間隔（秒）

    PieceType holdPieceType; // Hold中のピースの種類

    sf::Font font;                           // GUI用フォント（スコアやNext表示に利用）

public:
    Game();                                  // コンストラクタ（初期化）
    void run();                              // メインループ（イベント・更新・描画を回す）
private:
    void handleEvents();                     // イベント処理（閉じるボタンなど）
    void handleInput();                      // 入力処理（移動・回転・Holdなど）
    void handleFall();                       // 自動落下の処理
    void render();                           // 描画処理（盤面・ピース・UI表示）
};

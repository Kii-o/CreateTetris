#pragma once 
#include "Board.hpp" 
#include <SFML/Graphics.hpp> 
#include <array> 
#include <deque> 
#include <random> 
#include <optional>

// 列挙型 (enum) = 限られた選択肢を名前付きで表す型
//enum classとすることで、Tではなく、PieceType::Tと必ず型を指定して使うようになり、安全性が上がる

//sf::について
//sfとは、SFMLライブラリの名前空間のこと
//sf::colorで色を扱うクラス、sf::RenderWindowでゲーム画面を描画するウィンドウ、sf::Vector2iで2次元の整数ベクトル(x,y)など
//sf::Color c = sf::Color::Redで赤色、sf::Vector2i v(1, 2)でx=1, y = 2

//stdについて
//stdはCpp標準ライブラリの名前空間のこと
//std::arrayで固定長配列、std::coutで出力など

//externについて
//externはここでは宣言だけで、実態はcppファイルにあるという意味
//今回はミノの相対座標と色の宣言で使用

//std::dequeについて
// dequeはCpp標準ライブラリ(Double Ended QUEue、両端キューのこと)
//今回は、PieceTypeが格納されている
//そのため、nexeQUeueではNextに表示するテトリミノの種類を保持するための両端キュー
//dequeには、push_front(先頭に要素を追加)やpop_back(末尾の要素を削除)等の関数がある
//テトリスのNext表示は、先頭から取り出して (pop_front())、末尾に新しいピースを補充 (push_back()) する処理で表現できる


// ==== ピースの種類（7種のテトリミノ） ====
enum class PieceType { T, S, Z, I, O, L, J };
// 回転状態
enum class Rotation { Spawn = 0, Right = 1, Reverse = 2, Left = 3 };
int getKickIndex(Rotation oldRot, Rotation newRot); //ウォールキックテーブルを適応するための関数

// ==== ピース形状と色の定義（実体は .cpp 側で定義） ====
// それぞれのミノの4マス分の相対座標
extern const std::array<std::array<sf::Vector2i, 4>, 7> PIECE_SHAPES;
// それぞれのミノの色
extern const std::array<sf::Color, 7> PIECE_COLORS;

// ==== ピースを表すクラス ====
class Piece {
public:
    PieceType type;                          // 自分の種類を覚える(Holdで使用する)
    Rotation rotation = Rotation::Spawn;
    sf::Color color;                         // このピースの色
    std::array<sf::Vector2i, 4> blocks;      // 4つのブロックの相対座標
    int x = 3, y = 0;                        // フィールド上での位置（左上が基準）

    Piece(PieceType type);                   // コンストラクタ（種類を指定して生成）
    void draw(sf::RenderWindow& window);     // フィールド上に描画
    void drawPreview(sf::RenderWindow& window, int px, int py, int size = 20); // NextやHoldの小さな表示用
    std::array<sf::Vector2i, 4> getAbsolutePositions() const; //現在のブロックの座標を取得する
    bool canMove(Board& board, int dx, int dy); // 指定方向に動けるか判定
    // 任意のブロック配列で判定する canMove としてオーバーロード
    bool canMove(Board& board, const std::array<sf::Vector2i, 4>& testBlocks, int dx, int dy);
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
    std::optional<Piece> holdPiece;          // Holdに入っているピース
    bool holdUsed = false, holdExists = false; // Holdを使ったかどうか、存在するか

    sf::Clock fallClock, moveClock;          // 自動落下タイマー、横移動タイマー
    float fallInterval = 500.5f;               // 自動落下の間隔（秒）
    float moveInterval = 0.15f;              // 横移動の連続入力の間隔（秒）

    PieceType holdPieceType;                 // Hold中のピースの種類

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

#include "Piece.hpp" 
#include <algorithm> 
#include <iostream> 

// ==== 各ピースの形状定義 ====
// 各ピースは「4つの相対座標」で構成される
const std::array<std::array<sf::Vector2i, 4>, 7> PIECE_SHAPES = { {
    { sf::Vector2i(0,0), sf::Vector2i(-1,0), sf::Vector2i(1,0), sf::Vector2i(0,1) }, // Tミノ 
    { sf::Vector2i(0,0), sf::Vector2i(1,0), sf::Vector2i(0,1), sf::Vector2i(-1,1) }, // Sミノ
    { sf::Vector2i(0,0), sf::Vector2i(-1,0), sf::Vector2i(0,1), sf::Vector2i(1,1) }, // Zミノ
    { sf::Vector2i(0,0), sf::Vector2i(-1,0), sf::Vector2i(1,0), sf::Vector2i(2,0) }, // Iミノ
    { sf::Vector2i(0,0), sf::Vector2i(1,0), sf::Vector2i(0,1), sf::Vector2i(1,1) },  // Oミノ
    { sf::Vector2i(0,0), sf::Vector2i(-1,0), sf::Vector2i(-1,1), sf::Vector2i(1,0) },// Jミノ
    { sf::Vector2i(0,0), sf::Vector2i(-1,0), sf::Vector2i(1,0), sf::Vector2i(1,1) }  // Lミノ
} };

// ==== 各ピースの色定義 ====
const std::array<sf::Color, 7> PIECE_COLORS = {
    sf::Color(255,0,255),   // T = 紫
    sf::Color::Green,       // S = 緑
    sf::Color::Red,         // Z = 赤
    sf::Color::Cyan,        // I = 水色
    sf::Color::Yellow,      // O = 黄色
    sf::Color(255,165,0),   // J = 青
    sf::Color::Blue        // L = オレンジ
};

// Piece.cpp に追加
std::string toString(PieceType t) {
    switch (t) {
    case PieceType::I: return "I";
    case PieceType::O: return "O";
    case PieceType::T: return "T";
    case PieceType::S: return "S";
    case PieceType::Z: return "Z";
    case PieceType::J: return "J";
    case PieceType::L: return "L";
    default: return "?";
    }
}

// ------------------- ウォールキックテーブル -------------------
// T, J, L, S, Z 用
const std::array<std::array<sf::Vector2i, 5>, 8> WALL_KICKS = { {
    {{ {0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2} }}, // 0->R
    {{ {0,0}, {1,0}, {1,-1}, {0,2}, {1,2} }},     // R->0
    {{ {0,0}, {1,0}, {1,-1}, {0,2}, {1,2} }},     // R->2
    {{ {0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2} }},  // 2->R

    {{ {0,0}, {1,0}, {1,1}, {0,-2}, {1,-2} }},      // 2->L
    {{ {0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2} }},    // L->2
    {{ {0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2} }},    // L->0
    {{ {0,0}, {1,0}, {1,1}, {0,-2}, {1,-2} }}       // 0->L

} };

// Iミノ用
const std::array<std::array<sf::Vector2i, 5>, 8> WALL_KICKS_I = { {
    {{ {0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2} }},      // 0->R
    {{ {0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2} }},      // R->0
    {{ {0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1} }},      // R->2
    {{ {0,0}, {1,0}, {-2,1}, {1,-2}, {-2,1} }},      // 2->R

    {{ {0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2} }},      // 2->L
    {{ {0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2} }},      // L->2
    {{ {0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1} }},      // L->0
    {{ {0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1} }}        // 0->L
} };




// ==================== Piece クラス ==================== 
// コンストラクタ：種類に応じて色と形を設定
Piece::Piece(PieceType type) {
    this->type = type;
    color = PIECE_COLORS[(int)type];
    blocks = PIECE_SHAPES[(int)type];
}

// フィールド上に現在のピースを描画
void Piece::draw(sf::RenderWindow& window) {
    for (auto& b : blocks) {
        int px = (x + b.x) * 40;   // 盤面上の描画位置X
        int py = (y + b.y) * 40;   // 盤面上の描画位置Y
        sf::RectangleShape rect(sf::Vector2f(39, 39)); // マスサイズ(39x39)の四角形
        rect.setPosition(px, py);
        rect.setFillColor(color);
        window.draw(rect);
    }
}

// Next / Hold用の小さなプレビュー描画
void Piece::drawPreview(sf::RenderWindow& window, int px, int py, int size) {
    for (auto& b : blocks) {
        sf::RectangleShape rect(sf::Vector2f(size - 1, size - 1));
        rect.setPosition(px + b.x * size, py + b.y * size);
        rect.setFillColor(color);
        window.draw(rect);
    }
}

// 指定した移動量 (dx,dy) で動けるかどうか判定
bool Piece::canMove(Board& board, int dx, int dy) {
    for (auto& b : blocks) {
        int nx = x + b.x + dx, ny = y + b.y + dy;
        if (board.isOccupied(nx, ny)) return false; // 盤面外またはブロック衝突
    }
    return true;
}

// 実際にピースを移動する
void Piece::move(int dx, int dy) {
    x += dx;
    y += dy;
}

// ------------------- 回転用 enum -------------------
// 追加: 回転状態を保持する enum
enum class Rotation { R0, R90, R180, R270 };

// Piece クラスに rotation メンバを追加する必要があります
// 例: piece.hpp 内の Piece クラスに:
// Rotation rotation = Rotation::R0; // 初期は0度

// ==================== rotate 関数改訂 ==================== 
void Piece::rotate(Board& board, bool clockwise) {
    std::array<sf::Vector2i, 4> oldBlocks = blocks; // 元の形状を保存
    Rotation oldRotation = rotation;

    // ---- 回転行列適用 ----
    for (auto& b : blocks) {
        int tmp = b.x;
        if (clockwise) {
            b.x = -b.y;
            b.y = tmp;
        } else {
            b.x = b.y;
            b.y = -tmp;
        }
    }

    // 次の回転状態を計算
    rotation = static_cast<Rotation>((static_cast<int>(rotation) + (clockwise ? 1 : 3)) % 4);

    // ---- ウォールキック適用 ----
    const auto& kicks = (type == PieceType::I) ? WALL_KICKS_I : WALL_KICKS;
    bool moved = false;

    // キックテーブルのインデックス計算
    int kickIndex = 0;
    if (clockwise) {
        kickIndex = static_cast<int>(oldRotation) * 2; // 右回転
    } else {
        kickIndex = static_cast<int>(oldRotation) * 2 + 1; // 左回転
    }

    for (const auto& offset : kicks[kickIndex]) {
        if (canMove(board, offset.x, offset.y)) {
            x += offset.x;
            y += offset.y;
            moved = true;
            break;
        }
    }

    if (!moved) {
        // 全て失敗したら元に戻す
        blocks = oldBlocks;
        rotation = oldRotation;
    }
}

// ========================================
// ここまでで I / T/J/L/S/Z 全ての SRS 回転＆キックが適用可能
// ========================================


// ピースを盤面に固定
void Piece::place(Board& board) {
    for (auto& b : blocks)
        board.placeBlock(x + b.x, y + b.y, color);
}

// ==================== Bag クラス ==================== 
// コンストラクタ：乱数生成器を初期化し、バッグをシャッフル
Bag::Bag() : rng(std::random_device{}()) {
    shuffleBag();
}

// 7種類のピースを袋に詰めてシャッフル
void Bag::shuffleBag() {
    pieces = { PieceType::T, PieceType::S, PieceType::Z, PieceType::I,
               PieceType::O, PieceType::J, PieceType::L };
    std::shuffle(pieces.begin(), pieces.end(), rng);
}

// 次のピースを1つ取り出す
PieceType Bag::getNext() {
    if (pieces.empty()) shuffleBag(); // 袋が空なら補充
    PieceType p = pieces.back();      // 最後の1つを取り出す
    pieces.pop_back();
    return p;
}

// ==================== Game クラス ==================== 
// コンストラクタ：ウィンドウ生成、ピース初期化、Nextキュー準備
Game::Game()
    : window(sf::VideoMode(Board::WIDTH * 40 + 200, Board::HEIGHT * 40), "Tetris"),
    currentPiece(bag.getNext())

{
    //std::cout << "コンストラクタ: Current piece is " << toString(currentPiece.type) << std::endl;
    // Nextキューに最初の5つを補充
    for (int i = 0; i < 5; ++i) nextQueue.push_back(bag.getNext());
}

// メインループ（イベント処理・入力処理・落下処理・描画を繰り返す）
void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        handleInput();
        handleFall();
        render();
    }
}

// イベント処理（ウィンドウを閉じるなど）
void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event))
        if (event.type == sf::Event::Closed) window.close();
}

void Game::handleInput() {
    // 横移動はmoveIntervalで制限
    if (moveClock.getElapsedTime().asSeconds() < moveInterval) return;

    // --- 左右移動 ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        if (currentPiece.canMove(board, -1, 0)) currentPiece.move(-1, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        if (currentPiece.canMove(board, 1, 0)) currentPiece.move(1, 0);

    // --- 下移動 ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        if (currentPiece.canMove(board, 0, 1)) currentPiece.move(0, 1);

    // --- 回転 ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        currentPiece.rotate(board, false); // 左回転
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        currentPiece.rotate(board, true);  // 右回転

    // --- Hold機能 --- 
// Cキーが押されていて、まだこのターンでHoldを使っていない場合のみ処理
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !holdUsed) {
        if (!holdExists) {
            // === 初回ホールド ===
            // 現在のピースの種類をhold用変数に保存
            holdPiece = currentPiece;
            holdPieceType = currentPiece.type;
            // holdにピースが存在することを記録
            holdExists = true;
            // 現在のピースを「Nextキューの先頭のピース」に置き換える
            currentPiece = Piece(nextQueue.front());
            std::cout << "Current piece is " << toString(currentPiece.type) << std::endl;
            // Nextキューの先頭を削除（消費したので）
            nextQueue.pop_front();
            // 新しくbagから1つ取り出してNextキューの末尾に追加
            nextQueue.push_back(bag.getNext());
            std::cout << "Hold piece is " << toString(holdPiece->type) << std::endl;
        }
        else {
            // === 2回目以降のHold ===
            // 現在のピースの種類を一時的に保存
            std::cout << "before swap " << toString(holdPiece->type) << std::endl;
            Piece temppiece = currentPiece;
            PieceType temptype = currentPiece.type;
            // 一時保存しておいた種類をholdに戻す（入れ替え完了）
            // hold中のピースを現在のピースとして生成し直す
            currentPiece = Piece(holdPieceType);
            holdPiece = temppiece;
            holdPieceType = temptype;
            std::cout << "Hold piece is " << toString(holdPiece->type) << std::endl;
            std::cout << "Current piece is " << toString(currentPiece.type) << std::endl;
        }

        // 新しく取り出した/入れ替えたピースを初期位置(左上からx=3,y=0)に配置
        currentPiece.x = 3;
        currentPiece.y = 0;
        // このターンではもうHoldを使えないようにフラグを立てる
        holdUsed = true;
        //std::cout << "Current piece is " << toString(currentPiece.type) << std::endl;
    }

    // 移動入力のタイマーをリセット（Cを押した直後に再度連続入力されないようにする）
    moveClock.restart();

}


// 自動落下処理
void Game::handleFall() {
    if (fallClock.getElapsedTime().asSeconds() >= fallInterval) {
        if (currentPiece.canMove(board, 0, 1))
            currentPiece.move(0, 1);
        else {
            // 動けない＝着地 → 盤面に固定
            currentPiece.place(board);
            int lines = board.clearLines(); // ライン消去
            // 次のピースをセット
            currentPiece = Piece(nextQueue.front());
            nextQueue.pop_front();
            nextQueue.push_back(bag.getNext());
            holdUsed = false; // ホールド使用可能に戻す
        }
        fallClock.restart();
    }
}

// 描画処理
void Game::render() {
    window.clear();
    board.draw(window);         // 盤面
    currentPiece.draw(window);  // 現在のピース
    //std::cout << "Current piece is " << toString(currentPiece.type) << std::endl;

    // --- Next5の表示 ---
    int px = Board::WIDTH * 40 + 20, py = 20;
    int i = 0;
    for (auto& pType : nextQueue) {
        Piece p(pType);
        p.drawPreview(window, px, py + i * 100);
        ++i;
    }

    // --- Holdの表示 ---
    if (holdPiece) {  // has_value() の糖衣構文
        Piece p(*holdPiece);  // *で中身を取り出す
        p.drawPreview(window, px, 600);
    }

    window.display();
}

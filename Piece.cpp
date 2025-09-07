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

// ピースの全ブロックの絶対座標を返す
std::array<sf::Vector2i, 4> Piece::getAbsolutePositions() const {
    std::array<sf::Vector2i, 4> abs;
    for (int i = 0; i < 4; i++) {
        abs[i] = { x + blocks[i].x, y + blocks[i].y };
    }
    return abs;
}



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
/*
// 任意のブロック配列を使って移動可能か判定
bool Piece::canMove(Board& board, const std::array<sf::Vector2i, 4>& testBlocks, int dx, int dy) {
    for (auto& b : testBlocks) {
        int nx = x + b.x + dx;
        int ny = y + b.y + dy;
        if (board.isOccupied(nx, ny)) return false;
    }
    return true;
}

// 既存の平行移動用 canMove はオーバーロード
bool Piece::canMove(Board& board, int dx, int dy) {
    return canMove(board, blocks, dx, dy);
}
*/

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

// ------------------- ウォールキックテーブル -------------------
// T, J, L, S, Z 用
const std::array<std::array<sf::Vector2i, 5>, 8> WALL_KICKS = { {
    {{ {0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2} }}, // 0->R
    {{ {0,0}, {1,0}, {1,1}, {0,-2}, {1,-2} }},     // R->0
    {{ {0,0}, {1,0}, {1,1}, {0,-2}, {1,-2} }},     // R->2
    {{ {0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2} }},  // 2->R

    {{ {0,0}, {1,0}, {1,-1}, {0,2}, {1,2} }},      // 2->L
    {{ {0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2} }},    // L->2
    {{ {0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2} }},    // L->0
    {{ {0,0}, {1,0}, {1,-1}, {0,2}, {1,2} }}       // 0->L

} };

// Iミノ用
const std::array<std::array<sf::Vector2i, 5>, 8> WALL_KICKS_I = { {
    {{ {0,0}, {-2,0}, {1,0}, {-2,1}, {1,-2} }},      // 0->R
    {{ {0,0}, {2,0}, {-1,0}, {2,-1}, {-1,2} }},      // R->0
    {{ {0,0}, {-1,0}, {2,0}, {-1,-2}, {2,1} }},      // R->2
    {{ {0,0}, {1,0}, {-2,-1}, {1,2}, {-2,-1} }},      // 2->R

    {{ {0,0}, {2,0}, {-1,0}, {2,-1}, {-1,2} }},      // 2->L
    {{ {0,0}, {-2,0}, {1,0}, {-2,1}, {1,-2} }},      // L->2
    {{ {0,0}, {1,0}, {-2,0}, {1,2}, {-2,-1} }},      // L->0
    {{ {0,0}, {-1,0}, {2,0}, {-1,-2}, {2,1} }}        // 0->L
} };

/*
// // ------------------- ウォールキックテーブル -------------------
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
*/

//ウォールキックテーブルからどの値を適応するかを返す関数
int getKickIndex(Rotation oldRot, Rotation newRot) {
    if (oldRot == Rotation::Spawn && newRot == Rotation::Right) return 0;
    if (oldRot == Rotation::Right && newRot == Rotation::Spawn) return 1;
    if (oldRot == Rotation::Right && newRot == Rotation::Reverse) return 2;
    if (oldRot == Rotation::Reverse && newRot == Rotation::Right) return 3;
    if (oldRot == Rotation::Reverse && newRot == Rotation::Left) return 4;
    if (oldRot == Rotation::Left && newRot == Rotation::Reverse) return 5;
    if (oldRot == Rotation::Left && newRot == Rotation::Spawn) return 6;
    if (oldRot == Rotation::Spawn && newRot == Rotation::Left) return 7;
    return -1;
}

/*
void Piece::rotate(Board& board, bool clockwise) {
    // --- 回転前の情報を保持 ---
    std::array<sf::Vector2i, 4> oldBlocks = blocks;
    Rotation oldRotation = rotation;

    // --- 回転前の絶対座標を表示（デバッグ） ---
    std::cout << "Before rotation: ";
    for (auto& p : getAbsolutePositions()) {
        std::cout << "(" << p.x << "," << p.y << ") ";
    }
    std::cout << std::endl;

    // --- 新しい回転状態を計算 ---
    Rotation newRotation = static_cast<Rotation>(
        (static_cast<int>(rotation) + (clockwise ? 1 : 3)) % 4
        );

    // --- 回転後の相対座標を計算（ここで rotatedBlocks を定義） ---
    std::array<sf::Vector2i, 4> rotatedBlocks;
    for (int i = 0; i < 4; ++i) {
        int x0 = oldBlocks[i].x;
        int y0 = oldBlocks[i].y;
        if (clockwise) {
            // 右回転 (x,y) -> (-y, x)
            rotatedBlocks[i] = sf::Vector2i(-y0, x0);
        }
        else {
            // 左回転 (x,y) -> (y, -x)
            rotatedBlocks[i] = sf::Vector2i(y0, -x0);
        }
    }

    // --- デバッグ: 回転後の相対座標表示 ---
    std::cout << "rotatedBlocks: ";
    for (auto& b : rotatedBlocks) std::cout << "(" << b.x << "," << b.y << ") ";
    std::cout << std::endl;

    // --- ウォールキック処理 ---
    int kickIndex = getKickIndex(oldRotation, newRotation);
    if (kickIndex < 0) {
        // 万一の保険
        std::cout << "Warning: invalid kickIndex for rotation " << static_cast<int>(oldRotation)
            << " -> " << static_cast<int>(newRotation) << std::endl;
    }

    const auto& kicks = (type == PieceType::I) ? WALL_KICKS_I : WALL_KICKS;

    bool moved = false;
    if (kickIndex >= 0) {
        std::cout << "Trying kicks for index=" << kickIndex << ": ";
        for (const auto& off : kicks[kickIndex]) std::cout << "(" << off.x << "," << off.y << ") ";
        std::cout << std::endl;

        for (const auto& offset : kicks[kickIndex]) {
            // canMove のオーバーロードを使って回転後ブロックで判定
            bool ok = canMove(board, rotatedBlocks, offset.x, offset.y);
            std::cout << " try offset(" << offset.x << "," << offset.y << ") -> canMove=" << (ok ? "true" : "false") << std::endl;
            if (ok) {
                // 回転を確定（回転後座標を反映してから kick を適用）
                blocks = rotatedBlocks;
                x += offset.x;
                y += offset.y;
                rotation = newRotation;
                moved = true;
                break;
            }
        }
    }

    // --- 全部失敗したら元に戻す（blocksは oldBlocks のまま） ---
    if (!moved) {
        // blocks = oldBlocks; // 既に oldBlocks のままなら不要だが明示してもOK
        rotation = oldRotation;
        std::cout << "Rotation failed (kicks all rejected)" << std::endl;
    }

    // 回転後の絶対座標を出力
    std::cout << "After rotation: ";
    for (auto& p : getAbsolutePositions()) {
        std::cout << "(" << p.x << "," << p.y << ") ";
    }
    std::cout << std::endl;
}

Citrusさん返信待ち
*/


void Piece::rotate(Board& board, bool clockwise) {
    std::array<sf::Vector2i, 4> oldBlocks = blocks;
    Rotation oldRotation = rotation;

    // 回転前の絶対座標を出力
    std::cout << "Before rotation: ";
    for (auto& p : getAbsolutePositions()) {
        std::cout << "(" << p.x << "," << p.y << ") ";
    }
    std::cout << std::endl;

    // 回転
    for (auto& b : blocks) {
        int tmp = b.x;
        if (clockwise) {
            b.x = -b.y;
            b.y = tmp;
        }
        else {
            b.x = b.y;
            b.y = -tmp;
        }
    }

    // 次の回転状態を計算
    Rotation newRotation = static_cast<Rotation>((static_cast<int>(rotation) + (clockwise ? 1 : 3)) % 4);

    // --- ウォールキック処理 ---
    int kickIndex = getKickIndex(oldRotation, newRotation);
    const auto& kicks = (type == PieceType::I) ? WALL_KICKS_I : WALL_KICKS;

    bool moved = false;
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
        newRotation = oldRotation;
    }

    // 回転後の絶対座標を出力
    std::cout << "After rotation: ";
    for (auto& p : getAbsolutePositions()) {
        std::cout << "(" << p.x << "," << p.y << ") ";
    }
    std::cout << std::endl;
}




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

        /*

        // --- デバッグ出力 ---
        auto abs = currentPiece.getAbsolutePositions();
        std::cout << "Current piece absolute positions: ";
        for (auto& p : abs) {
            std::cout << "(" << p.x << "," << p.y << ") ";
        }
        std::cout << std::endl;

        */
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

    // --- 上移動（↑キーで1段上げる） ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        if (currentPiece.canMove(board, 0, -1)) currentPiece.move(0, -1);

    // --- ハードドロップ（スペースキー） ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        while (currentPiece.canMove(board, 0, 1)) {
            currentPiece.move(0, 1);  // 一番下まで落とす
        }
        currentPiece.place(board);     // 盤面に固定
        // 次のピース生成などの処理
        currentPiece = Piece(nextQueue.front());
        nextQueue.pop_front();
        nextQueue.push_back(bag.getNext());
        holdUsed = false;
    }

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

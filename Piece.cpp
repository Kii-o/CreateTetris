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
    sf::Color::Blue,        // L = オレンジ
    sf::Color(255,165,0)    // J = 青
};

// ==================== Piece クラス ==================== 
// コンストラクタ：種類に応じて色と形を設定
Piece::Piece(PieceType type) {
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

void Piece::rotate(Board& board, bool clockwise) {
    std::array<sf::Vector2i, 4> oldBlocks = blocks; // 元の形を退避

    for (auto& b : blocks) {
        int tmp = b.x;
        if (clockwise) {
            b.x = -b.y;
            b.y = tmp;
        }
        else { // 左回転
            b.x = b.y;
            b.y = -tmp;
        }
    }

    // 壁やブロックに衝突したら元に戻す（簡易SRS）
    if (!canMove(board, 0, 0)) {
        blocks = oldBlocks;
    }
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
    // Nextキューに最初の5つを補充
    for (int i = 0; i < 5; ++i) nextQueue.push_back(bag.getNext());
    // フォント読み込み（Windows環境用）
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
        std::cerr << "Font error\n";
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !holdUsed) {
    if (!holdExists) {
        // 初回ホールド: 現在のピースの種類を保存
        holdPieceType = currentPiece.type;
        holdExists = true;
        currentPiece = Piece(nextQueue.front());
        nextQueue.pop_front();
        nextQueue.push_back(bag.getNext());
    } else {
        // 入れ替え
        std::swap(currentPiece.type, holdPieceType);
        currentPiece = Piece(currentPiece.type); // Pieceオブジェクトを再生成
    }

    currentPiece.x = 3;
    currentPiece.y = 0;
    holdUsed = true;
}

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

    // --- Next5の表示 ---
    int px = Board::WIDTH * 40 + 20, py = 20;
    int i = 0;
    for (auto& pType : nextQueue) {
        Piece p(pType);
        p.drawPreview(window, px, py + i * 100);
        ++i;
    }

    // --- Holdの表示 ---
    if (holdExists) {
        Piece p(holdPiece);
        p.drawPreview(window, px, 600);
    }

    window.display();
}

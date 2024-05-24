#include"SceneGame.h"
#include "SceneGame.h"
#include "../GameLib/game_lib.h"
#include "WinMain.h"
#include"SpriteData.h"

#include "Obj2d.h"

// 変数宣言
//VECTOR2 enemyPos;
//VECTOR2 playerPos;
OBJ2D back[4];
OBJ2D player;
OBJ2D enemy;

// プロトタイプ宣言
void playerUpdate(OBJ2D* obj);
void enemyUpdate(OBJ2D* obj);
void backUpdate(OBJ2D* obj);


void SceneGame::init()
{
    // テクスチャのロード
    GameLib::texture::load(loadTexture);

    // 背景初期設定

    VECTOR2 backPosition[] = {
        {0,0},{1280,0},
        {0,720},{1280,720},
    };
    size_t i{};

    for (auto& b : back)
    {

        b.sprData = &sprBack;
        b.position = backPosition[i++];
    }

    // プレイヤー初期設定
    player.sprData = &sprPlayer;
    player.position = { 640,360 };
    player.blendState = GameLib::Blender::BS_ADD;
    player.color = { 1,0,0,1 };


    // 敵キャラ初期設定
    enemy.sprData = &sprEnemy;
    enemy.position = { static_cast<float>(rand() % 1280),static_cast<float>(rand() % 720) };
    enemy.scale = { 2,2 };
    enemy.blendState = GameLib::Blender::BS_ALPHA;
    enemy.color = { 1,1,1,0.5f };
}

void SceneGame::deinit()
{
    GameLib::texture::release(loadTexture);
}
void SceneGame::update()
{
    // 背景更新
    for (auto& b : back)
    {
        backUpdate(&b);

    }


    // プレイヤー更新
    playerUpdate(&player);

    // 敵キャラ更新
    enemyUpdate(&enemy);

    // シーン切り替えチェック
    if (GameLib::input::STATE(0) & GameLib::input::PAD_TRG1) { setScene(SCENE::OVER); }
    if (GameLib::input::STATE(0) & GameLib::input::PAD_TRG2) { setScene(SCENE::CLEAR); }
}

// 背景更新処理
void backUpdate(OBJ2D* obj)
{
    VECTOR2 mousePos{
    static_cast<float>(GameLib::input::getCursorPosX()),
    static_cast<float>(GameLib::input::getCursorPosY()),
    };//マウスの位置
    VECTOR2 dir = mousePos - player.position;//画面中央からマウスへの方向
    float length = vec2Length(dir);
    float angle = atan2(dir.y, dir.x);//yの差分、xの差分を入れると角度が入手できる


    obj->position.x -= cosf(angle) * 4 * length * 0.01f;
    obj->position.y -= sinf(angle) * 4 * length * 0.01f;
    if (obj->position.x < -1280)obj->position.x += 1280 * 2;
    if (obj->position.x > 1280)obj->position.x -= 1280 * 2;
    if (obj->position.y < -720)obj->position.y += 720 * 2;
    if (obj->position.y > 720)obj->position.y -= 720 * 2;



}


// プレイヤー更新処理
void playerUpdate(OBJ2D* obj)
{
    VECTOR2 mousePos{
        static_cast<float>(GameLib::input::getCursorPosX()),
        static_cast<float>(GameLib::input::getCursorPosY()),
    };//マウスの位置
    VECTOR2 dir = mousePos - obj->position;//画面中央からマウスへの方向
    obj->rotation = atan2(dir.y, dir.x);//yの差分、xの差分を入れると角度が入手できる


    //if (GameLib::input::STATE(0) & GameLib::input::PAD_UP)obj->position.y -= 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_DOWN)obj->position.y += 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_LEFT)obj->position.x -= 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_RIGHT)obj->position.x += 8;
}

// 敵キャラ更新処理
void enemyUpdate(OBJ2D* obj)
{
    obj->position.x -= 4;
    if (obj->position.x < 0)obj->position.x += 1280;
    obj->position.y -= 4;
    if (obj->position.y < 0)obj->position.y += 720;
}


void SceneGame::draw()
{
    // 画面をクリア
    GameLib::clear(0, 0, 0);

    // 背景描画
    for (auto& b : back)
    {
        b.draw();
    }

    // プレイヤー描画
    player.draw();
    //sprPlayer.draw({ player.position.x,player.position.y });

    // 敵キャラ描画
    enemy.draw();
    //sprEnemy.draw({ enemy.position.x,enemy.position.y });


    // 文字列描画
    GameLib::font::textOut(4, "[Z]GAME OVER", { 0, 660 }, { 2, 2 },
        { 1, 0, 0, 1 }, GameLib::TEXT_ALIGN::MIDDLE_LEFT);

    GameLib::font::textOut(4, "[X]GAME CLEAR", { 0, 700 }, { 2, 2 },
        { 0, 1, 1, 1 }, GameLib::TEXT_ALIGN::MIDDLE_LEFT);
}



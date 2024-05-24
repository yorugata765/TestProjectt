// インクルード
#include "WinMain.h"
#include "../GameLib/game_lib.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneOver.h"
#include "SceneClear.h"

// 実体宣言
SceneGame sceneGame;
SceneTitle sceneTitle;
SceneOver sceneOver;
SceneClear sceneClear;
Scene* pScene, * pNextScene;

int APIENTRY WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    // 初期設定
    GameLib::init(L"C++ Shooting Game", 1280, 720, false);

    // ブレンドモードの設定
    GameLib::setBlendMode(GameLib::Blender::BS_ALPHA);

    setScene(SCENE::TITLE);

    // メインループ
    while (GameLib::gameLoop())
    {
        // 入力処理
        GameLib::input::update();

        // 更新処理
        if (pNextScene != nullptr)
        {

            if (pScene)pScene->deinit();
            pScene = pNextScene;
            pNextScene = nullptr;
            pScene->init();
        }
        pScene->update();

        // 描画処理
        pScene->draw();

        // バックバッファとフロントバッファを切り替える
        GameLib::present(1, 0);
    }

    // 終了処理
    GameLib::uninit();

    return 0;
}

void setScene(int nextScene)
{
    Scene* scenes[] = {
        &sceneTitle,
        &sceneGame,
        &sceneOver,
        &sceneClear,
    };

    pNextScene = scenes[nextScene];
}
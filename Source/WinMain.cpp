// �C���N���[�h
#include "WinMain.h"
#include "../GameLib/game_lib.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneOver.h"
#include "SceneClear.h"

// ���̐錾
SceneGame sceneGame;
SceneTitle sceneTitle;
SceneOver sceneOver;
SceneClear sceneClear;
Scene* pScene, * pNextScene;

int APIENTRY WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    // �����ݒ�
    GameLib::init(L"C++ Shooting Game", 1280, 720, false);

    // �u�����h���[�h�̐ݒ�
    GameLib::setBlendMode(GameLib::Blender::BS_ALPHA);

    setScene(SCENE::TITLE);

    // ���C�����[�v
    while (GameLib::gameLoop())
    {
        // ���͏���
        GameLib::input::update();

        // �X�V����
        if (pNextScene != nullptr)
        {

            if (pScene)pScene->deinit();
            pScene = pNextScene;
            pNextScene = nullptr;
            pScene->init();
        }
        pScene->update();

        // �`�揈��
        pScene->draw();

        // �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@��؂�ւ���
        GameLib::present(1, 0);
    }

    // �I������
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
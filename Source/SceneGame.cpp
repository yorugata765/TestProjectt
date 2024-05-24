#include"SceneGame.h"
#include "SceneGame.h"
#include "../GameLib/game_lib.h"
#include "WinMain.h"
#include"SpriteData.h"

#include "Obj2d.h"

// �ϐ��錾
//VECTOR2 enemyPos;
//VECTOR2 playerPos;
OBJ2D back[4];
OBJ2D player;
OBJ2D enemy;

// �v���g�^�C�v�錾
void playerUpdate(OBJ2D* obj);
void enemyUpdate(OBJ2D* obj);
void backUpdate(OBJ2D* obj);


void SceneGame::init()
{
    // �e�N�X�`���̃��[�h
    GameLib::texture::load(loadTexture);

    // �w�i�����ݒ�

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

    // �v���C���[�����ݒ�
    player.sprData = &sprPlayer;
    player.position = { 640,360 };
    player.blendState = GameLib::Blender::BS_ADD;
    player.color = { 1,0,0,1 };


    // �G�L���������ݒ�
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
    // �w�i�X�V
    for (auto& b : back)
    {
        backUpdate(&b);

    }


    // �v���C���[�X�V
    playerUpdate(&player);

    // �G�L�����X�V
    enemyUpdate(&enemy);

    // �V�[���؂�ւ��`�F�b�N
    if (GameLib::input::STATE(0) & GameLib::input::PAD_TRG1) { setScene(SCENE::OVER); }
    if (GameLib::input::STATE(0) & GameLib::input::PAD_TRG2) { setScene(SCENE::CLEAR); }
}

// �w�i�X�V����
void backUpdate(OBJ2D* obj)
{
    VECTOR2 mousePos{
    static_cast<float>(GameLib::input::getCursorPosX()),
    static_cast<float>(GameLib::input::getCursorPosY()),
    };//�}�E�X�̈ʒu
    VECTOR2 dir = mousePos - player.position;//��ʒ�������}�E�X�ւ̕���
    float length = vec2Length(dir);
    float angle = atan2(dir.y, dir.x);//y�̍����Ax�̍���������Ɗp�x������ł���


    obj->position.x -= cosf(angle) * 4 * length * 0.01f;
    obj->position.y -= sinf(angle) * 4 * length * 0.01f;
    if (obj->position.x < -1280)obj->position.x += 1280 * 2;
    if (obj->position.x > 1280)obj->position.x -= 1280 * 2;
    if (obj->position.y < -720)obj->position.y += 720 * 2;
    if (obj->position.y > 720)obj->position.y -= 720 * 2;



}


// �v���C���[�X�V����
void playerUpdate(OBJ2D* obj)
{
    VECTOR2 mousePos{
        static_cast<float>(GameLib::input::getCursorPosX()),
        static_cast<float>(GameLib::input::getCursorPosY()),
    };//�}�E�X�̈ʒu
    VECTOR2 dir = mousePos - obj->position;//��ʒ�������}�E�X�ւ̕���
    obj->rotation = atan2(dir.y, dir.x);//y�̍����Ax�̍���������Ɗp�x������ł���


    //if (GameLib::input::STATE(0) & GameLib::input::PAD_UP)obj->position.y -= 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_DOWN)obj->position.y += 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_LEFT)obj->position.x -= 8;
    //if (GameLib::input::STATE(0) & GameLib::input::PAD_RIGHT)obj->position.x += 8;
}

// �G�L�����X�V����
void enemyUpdate(OBJ2D* obj)
{
    obj->position.x -= 4;
    if (obj->position.x < 0)obj->position.x += 1280;
    obj->position.y -= 4;
    if (obj->position.y < 0)obj->position.y += 720;
}


void SceneGame::draw()
{
    // ��ʂ��N���A
    GameLib::clear(0, 0, 0);

    // �w�i�`��
    for (auto& b : back)
    {
        b.draw();
    }

    // �v���C���[�`��
    player.draw();
    //sprPlayer.draw({ player.position.x,player.position.y });

    // �G�L�����`��
    enemy.draw();
    //sprEnemy.draw({ enemy.position.x,enemy.position.y });


    // ������`��
    GameLib::font::textOut(4, "[Z]GAME OVER", { 0, 660 }, { 2, 2 },
        { 1, 0, 0, 1 }, GameLib::TEXT_ALIGN::MIDDLE_LEFT);

    GameLib::font::textOut(4, "[X]GAME CLEAR", { 0, 700 }, { 2, 2 },
        { 0, 1, 1, 1 }, GameLib::TEXT_ALIGN::MIDDLE_LEFT);
}



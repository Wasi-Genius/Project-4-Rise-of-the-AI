#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LoseScene.h"
#include "CS3113/MenuScene.h"

constexpr int SCREEN_WIDTH = 1000,
              SCREEN_HEIGHT = 600,
              FPS = 120;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

constexpr Vector2 ORIGIN = {
    SCREEN_WIDTH / 2.0f,
    SCREEN_HEIGHT / 2.0f};

AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;
Camera2D gCamera = {};

// The map owns every allocated scene. The current pointer only selects which
// scene receives input, updates, and rendering this frame.
Scene *gCurrentScene = nullptr;
std::map<SceneID, Scene *> gLevels;

void switchToScene(Scene *scene)
{
    if (scene == nullptr)
    {
        LOG("Cannot switch to a scene that has not been created.");
        gAppStatus = TERMINATED;
        return;
    }

    if (gCurrentScene != nullptr)
        gCurrentScene->shutdown();

    gCurrentScene = scene;
    gCurrentScene->initialise();
    Entity *player = gCurrentScene->getState().player;
    gCamera.target = player != nullptr
                         ? Vector2{player->getPosition().x, ORIGIN.y}
                         : gCurrentScene->getOrigin();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Knight's Run");
    InitAudioDevice();
    gGameProgress.hurtSound = LoadSound("assets/Misc. Sound/Hurt Sound.mp3");

    // Named keys make the scene collection readable and let scenes request a destination without relying on fragile numeric array positions.
    gLevels[MAIN_MENU] = new MenuScene(ORIGIN, "#1b4998");
    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#C0897E");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#011627");
    gLevels[LOSE_SCREEN] = new LoseScene(ORIGIN, "#A00000");
    switchToScene(gLevels[MAIN_MENU]);

    // Camera2D keeps the player horizontally centred while preserving the
    // lesson's fixed vertical camera position.
    gCamera.offset = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 1.0f;
    SetTargetFPS(FPS);

    // Start measuring frame time after loading the initial scene.
    gPreviousTicks = static_cast<float>(GetTime());
}

void processInput()
{
    Entity *player = gCurrentScene->getState().player;

    if (player != nullptr)
    {
        player->resetMovement();

        if (IsKeyDown(KEY_A))
            player->moveLeft();
        else if (IsKeyDown(KEY_D))
            player->moveRight();

        if (IsKeyPressed(KEY_W) && player->isCollidingBottom())
        {
            player->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        // Normalising prevents diagonal input from increasing movement speed.
        if (GetLength(player->getMovement()) > 1.0f)
            player->normaliseMovement();
    }
    else if (IsKeyPressed(KEY_ENTER))
    {
        gGameProgress.lives = 5;
        gCurrentScene->requestSceneChange(LEVEL_A);
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
    // Delta time is elapsed seconds. The accumulator carries any leftover fraction forward so every physics update uses the same fixed interval.
    const float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    deltaTime += gTimeAccumulator;

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Entity *player = gCurrentScene->getState().player;
        if (player != nullptr)
        {
            gCamera.target = {
                player->getPosition().x,
                ORIGIN.y};
        }
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
    gCurrentScene->render();
    EndMode2D();
    EndDrawing();
}

void shutdown()
{
    // Deleting scenes through the base pointer demonstrates virtual
    // destruction. Their destructors perform safe, idempotent cleanup.
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;
    gLevels.clear();
    gCurrentScene = nullptr;

    UnloadSound(gGameProgress.hurtSound);
    gGameProgress.hurtSound = {};
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        // Scenes request a transition by setting nextSceneID; the game loop
        // looks up that ID and performs the switch outside the scene itself.
        const SceneID nextSceneID = gCurrentScene->getState().nextSceneID;
        if (nextSceneID != NO_SCENE)
        {
            std::map<SceneID, Scene *>::iterator nextScene =
                gLevels.find(nextSceneID);

            if (nextScene == gLevels.end())
            {
                LOG("Cannot switch: the requested scene ID does not exist.");
                gAppStatus = TERMINATED;
            }
            else
            {
                switchToScene(nextScene->second);
            }
        }

        render();
    }

    shutdown();
    return 0;
}

#define NOMINMAX
#include <Novice.h>
#include "Debug.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include "imgui.h"

const char kAppTitle[] = "LE2C_03_ウノ_リュウト";
int kScreenWidth = 1280;
int kScreenHeight = 720;

void UpdateBallPosition(Ball& ball, const Plane& surface, float deltaTime, float restitution) {
    // 速度と位置を更新
    ball.velocity += ball.acceleration * deltaTime;
    Vector3 nextPosition = ball.position + ball.velocity * deltaTime;

    // 現在の位置から新しい位置までのセグメントを作成
    Capsule movementPath;
    movementPath.segment.origin = ball.position;
    movementPath.segment.diff = nextPosition - ball.position;
    movementPath.radius = ball.radius;

    // 平面との衝突をチェック
    if (IsCollision(movementPath, surface)) {
        // 衝突を処理
        Vector3 reflectedVelocity = Reflect(ball.velocity, surface.normal);
        Vector3 normalComponent = Project(reflectedVelocity, surface.normal);
        Vector3 tangentComponent = reflectedVelocity - normalComponent;
        ball.velocity = normalComponent * restitution + tangentComponent;

        // 位置を修正
        float t = (surface.distance - Dot(movementPath.segment.origin, surface.normal)) / Dot(surface.normal, movementPath.segment.diff);
        ball.position = movementPath.segment.origin + movementPath.segment.diff * t - surface.normal * ball.radius;
    }
    else {
        // 衝突がない場合は新しい位置に移動
        ball.position = nextPosition;
    }
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kAppTitle, kScreenWidth, kScreenHeight);

    Vector3 camOffset{ 0.0f, 1.9f, -6.49f };
    Vector3 camRotation{ 0.26f, 0.0f, 0.0f };
    Vector3 camPosition{ 0.0f, 1.0f, -5.0f };
    static bool debugCameraEnabled = false;
    Vector2Int mousePosition;

    Vector3 rotationAxis = Normalize({ 1.0f, 1.0f, 1.0f });
    float rotationAngle = 0.44f;
    Matrix4x4 rotationMatrix = MakeRotateAxisAngle(rotationAxis, rotationAngle);

    // キー入力結果を受け取る箱
    char currentKeys[256] = { 0 };
    char previousKeys[256] = { 0 };

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(previousKeys, currentKeys, 256);
        Novice::GetHitKeyStateAll(currentKeys);

        ///
        /// ↓更新処理ここから
        ///

        if (currentKeys[DIK_C] && previousKeys[DIK_C] == 0) {
            debugCameraEnabled = !debugCameraEnabled;
        }
        CameraMove(camRotation, camOffset, mousePosition, debugCameraEnabled);
        Matrix4x4 cameraTransform = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camRotation, Add(camPosition, camOffset));
        Matrix4x4 viewTransform = Inverse(cameraTransform);
        Matrix4x4 projectionTransform = MakePerspectiveFovMatrix(0.45f, float(kScreenWidth) / float(kScreenHeight), 0.1f, 100.0f);
        Matrix4x4 viewProjectionTransform = Multiply(viewTransform, projectionTransform);
        Matrix4x4 viewportTransform = MakeViewportMatrix(0, 0, float(kScreenWidth), float(kScreenHeight), 0.0f, 1.0f);

        MatrixScreenPrintf(0, 0, rotationMatrix);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        ImGui::Begin("CameraView");
        ImGui::DragFloat3("CameraOffset", &camOffset.x, 0.01f);
        ImGui::DragFloat3("CameraRotation", &camRotation.x, 0.01f);
        ImGui::DragFloat3("CameraPosition", &camPosition.x, 0.01f);
        ImGui::End();

        DrawGrid(viewProjectionTransform, viewportTransform);

        ///
        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (previousKeys[DIK_ESCAPE] == 0 && currentKeys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}

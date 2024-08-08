#define NOMINMAX
#include <Novice.h>
#include"Debug.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include "imgui.h"
const char kWindowTitle[] = "LE2C_05_ウノ_リュウト";
int kWindowWidth = 1280;
int kWindowHeight = 720;
// 円運動の設定
const float radius = 0.8f; // 半径
const float angularSpeed = 3.14f; // 角速度 (ラジアン/秒)
const float deltaTime = 1.0f / 60.0f; // 時間の増分 (秒)
Vector3 pointP = { radius, 0.0f, 0.0f }; // 初期位置 (x = 半径, y = 0)
Vector3 center = { 0.0f,0.0f,0.0f };
float angle = 0.0f; // 初期角度
// 等速円運動の計算
void updatePosition(float deltaTimes) {
	angle += angularSpeed * deltaTimes;
	pointP.x = radius * cos(angle) + center.x;
	pointP.y = radius * sin(angle) + center.x;
	pointP.z = 0.0f; // z座標は0で固定 (2D円運動)
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Vector3 cameraPosition{ 0.0f, 1.0f, -5.0f };
	static bool isDebugCamera = false;
	Vector2Int mouse;

	//float angularVelocity = 3.14f;
	//float angle = 0.0f;
	bool move = false;
	Sphere sphere{};
	sphere.radius = 0.05f;
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
			isDebugCamera = !isDebugCamera;
		}
		CameraMove(cameraRotate, cameraTranslate, mouse, isDebugCamera);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, Add(cameraPosition, cameraTranslate));
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 ViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		if (move) {
			updatePosition(deltaTime);
		}
		sphere.center = pointP;
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Camera");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("CameraPosition", &cameraPosition.x, 0.01f);
		ImGui::End();

		ImGui::Begin("Window");

		if (ImGui::Button("START")) {
			move = true;
		}

		ImGui::End();
		DrawSphere(sphere, ViewProjectionMatrix, viewportMatrix, WHITE);
		DrawGrid(ViewProjectionMatrix, viewportMatrix);
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
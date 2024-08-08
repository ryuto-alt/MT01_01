#define NOMINMAX
#include <Novice.h>
#include"Debug.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include "imgui.h"
const char kWindowTitle[] = "LE2C_05_ウノ_リュウト";
int kWindowWidth = 1280;
int kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Vector3 cameraPosition{ 0.0f, 1.0f, -5.0f };
	static bool isDebugCamera = false;
	Vector2Int mouse;

	float deltaTime = 1.0f / 60.0f;

	ConicalPendulum conicalPendulum;
	conicalPendulum.anchor = { 0.0f,1.0f,0.0f };
	conicalPendulum.length = 0.8f;
	conicalPendulum.halfApexAngle = 0.7f;
	conicalPendulum.angle = 0.0f;
	conicalPendulum.angularVelocity = 0.0f;

	Vector3 p{};

	Sphere sphere{};
	bool move = false;
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
			conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
			conicalPendulum.angle += conicalPendulum.angularVelocity * deltaTime;

			float radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
			float height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;
			p.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
			p.y = conicalPendulum.anchor.y - height;
			p.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;
		}
		sphere.center = p;
		sphere.radius = 0.05f;
		Vector3 transformedPrev = Transform(p, ViewProjectionMatrix);
		transformedPrev = Transform(transformedPrev, viewportMatrix);
		Vector3 transformedPrev1 = Transform(conicalPendulum.anchor, ViewProjectionMatrix);
		transformedPrev1 = Transform(transformedPrev1, viewportMatrix);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		ImGui::Begin("Camera");
		ImGui::DragFloat3("CTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("CPosition", &cameraPosition.x, 0.01f);
		ImGui::End();

		ImGui::Begin("Window");

		if (ImGui::Button("START")) {
			move = true;
		}

		ImGui::End();

		Novice::DrawLine(static_cast<int>(transformedPrev.x), static_cast<int>(transformedPrev.y),static_cast<int>(transformedPrev1.x), static_cast<int>(transformedPrev1.y), WHITE);
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
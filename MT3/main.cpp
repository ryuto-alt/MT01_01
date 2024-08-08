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

	Spring spring{};
	spring.anchor = { 0.0f,1.0f,0.0f };
	spring.naturalLength = 0.7f;
	spring.stiffness = 100.0f;
	spring.dampingCoefficient = 2.0f;

	Ball ball{};
	ball.position = { 0.8f,0.2f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = BLUE;
	ball.velocity = 0.01f;
	bool move = false;
	Sphere sphere{};
	float deltaTime = 1.0f / 60.0f;
	const Vector3 kGravity = { 0.0f,-9.8f,0.0f };
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
			Vector3 diff = ball.position - spring.anchor;
			float length = Length(diff);
			if (length != 0.0f) {
				Vector3 direction = Normalize(diff);
				Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
				Vector3 displacement = length * (ball.position - restPosition);
				Vector3 restoringForce = -spring.stiffness * displacement;
				Vector3 dampingForce = -spring.dampingCoefficient * ball.velocity;
				Vector3 gravityForce = kGravity * ball.mass;
				Vector3 force = restoringForce + dampingForce + gravityForce;
				ball.acceleration = force / ball.mass;

			}
			ball.velocity += ball.acceleration * deltaTime;
			ball.position += ball.velocity * deltaTime;

		}
		sphere.center = ball.position;
		sphere.radius = ball.radius;
		Vector3 transformedPrev = Transform(sphere.center, ViewProjectionMatrix);
		transformedPrev = Transform(transformedPrev, viewportMatrix);
		Vector3 transformedPrev1 = Transform(spring.anchor, ViewProjectionMatrix);
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
		Novice::DrawLine(static_cast<int>(transformedPrev.x), static_cast<int>(transformedPrev.y),
			static_cast<int>(transformedPrev1.x), static_cast<int>(transformedPrev1.y), WHITE);
		DrawSphere(sphere, ViewProjectionMatrix, viewportMatrix, ball.color);
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
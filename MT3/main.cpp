#define NOMINMAX
#include <Novice.h>
#include"Debug.h"
#include <math.h>
#define _USE_MATH_DEFINES
#include "imgui.h"
const char kWindowTitle[] = "LE2C_03_ウノ_リュウト";

int kWindowWidth = 1280;
int kWindowHeight = 720;


void UpdateBall(Ball& ball, const Plane& plane, float deltaTime, float e) {
	// 速度と位置を更新
	ball.velocity += ball.acceleration * deltaTime;
	Vector3 newPosition = ball.position + ball.velocity * deltaTime;

	// 現在の位置から新しい位置までのセグメントを作成
	Capsule capsule;
	capsule.segment.origin = ball.position;
	capsule.segment.diff = newPosition - ball.position;
	capsule.radius = ball.radius;

	// 平面との衝突をチェック
	if (IsCollision(capsule, plane)) {
		// 衝突を処理
		Vector3 reflected = Reflect(ball.velocity, plane.normal);
		Vector3 projectToNormal = Project(reflected, plane.normal);
		Vector3 movingDirection = reflected - projectToNormal;
		ball.velocity = projectToNormal * e + movingDirection;

		// 位置を修正
		float t = (plane.distance - Dot(capsule.segment.origin, plane.normal)) / Dot(plane.normal, capsule.segment.diff);
		ball.position = capsule.segment.origin + capsule.segment.diff * t - plane.normal * ball.radius;
	}
	else {
		// 衝突がない場合は新しい位置に移動
		ball.position = newPosition;
	}
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
	Vector3 from0 = Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = -from0;
	Vector3 from1 = Normalize(Vector3{ -0.6f,0.9f,0.2f });
	Vector3 to1 = Normalize(Vector3{ 0.4f,0.7f,-0.5f });
	Matrix4x4 rotateMatrix0 = DirectionToDirection(
		Normalize(Vector3{ 1.0f,0.0f,0.0f }), Normalize(Vector3{ -1.0f, 0.0f, 0.0f }));
	Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);



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

		if (keys[DIK_C] && preKeys[DIK_C] == 0) {
			isDebugCamera = !isDebugCamera;
		}
		CameraMove(cameraRotate, cameraTranslate, mouse, isDebugCamera);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, Add(cameraPosition, cameraTranslate));
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 ViewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		MatrixScreenPrintf(0, 0, rotateMatrix0);
		MatrixScreenPrintf(0, kRowHeight * 5, rotateMatrix1);
		MatrixScreenPrintf(0, kRowHeight * 10, rotateMatrix2);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(ViewProjectionMatrix, viewportMatrix);

		ImGui::Begin("CameraView");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("CameraPosition", &cameraPosition.x, 0.01f);
		ImGui::End();

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
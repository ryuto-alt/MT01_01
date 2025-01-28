#include <Novice.h>
#include <cmath>
#include <algorithm> // For std::max

const char kWindowTitle[] = "LE2C_03_ウノ_リュウト";

struct Vector3 {
    float x, y, z;
};

struct Quaternion {
    float x, y, z, w;
};

struct Matrix4x4 {
    float m[4][4];
};

// ベクトル正規化
Vector3 Normalize(const Vector3& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) {
        return { 0.0f, 0.0f, 0.0f };
    }
    return { v.x / length, v.y / length, v.z / length };
}

// クォータニオン生成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
    Vector3 normalizedAxis = Normalize(axis);
    float sinHalfAngle = std::sin(angle / 2.0f);
    float cosHalfAngle = std::cos(angle / 2.0f);

    return {
        normalizedAxis.x * sinHalfAngle,
        normalizedAxis.y * sinHalfAngle,
        normalizedAxis.z * sinHalfAngle,
        cosHalfAngle
    };
}

// 球面線形補間
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
    // 内積計算
    float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

    // クォータニオンの逆転が必要かを確認（内積が負の場合）
    Quaternion Quaternion1 = q1;
    if (dot < 0.0f) {
        dot = -dot;
        Quaternion1.x = -q1.x;
        Quaternion1.y = -q1.y;
        Quaternion1.z = -q1.z;
        Quaternion1.w = -q1.w;
    }

    // 線形補間を使用（角度が非常に小さい場合）
    if (dot > 0.9995f) {
        return {
            q0.x + t * (Quaternion1.x - q0.x),
            q0.y + t * (Quaternion1.y - q0.y),
            q0.z + t * (Quaternion1.z - q0.z),
            q0.w + t * (Quaternion1.w - q0.w)
        };
    }

    // 球面補間計算
    float theta = std::acos(dot);
    float sinTheta = std::sqrt(1.0f - dot * dot);
    float a = std::sin((1.0f - t) * theta) / sinTheta;
    float b = std::sin(t * theta) / sinTheta;

    return {
        a * q0.x + b * Quaternion1.x,
        a * q0.y + b * Quaternion1.y,
        a * q0.z + b * Quaternion1.z,
        a * q0.w + b * Quaternion1.w
    };
}

// 表示関数
static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void QuaternionScreenPrintf(int x, int y, const Quaternion& quaternion, const char* label) {
    Novice::ScreenPrintf(x, y, "%.02f", quaternion.x);
    Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", quaternion.y);
    Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", quaternion.z);
    Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%.02f", quaternion.w);
    Novice::ScreenPrintf(x + kColumnWidth * 4, y, label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    const int kWindowWidth = 1280;
    const int kWindowHeight = 720;
    Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    while (Novice::ProcessMessage() == 0) {
        Novice::BeginFrame();
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        // クォータニオンの設定
        Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
        Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);
        // 球面線形補間
        Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
        Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
        Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
        Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
        Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

        // 結果
        QuaternionScreenPrintf(0, 0, interpolate0, "interpolate0,Slerp(q0,q1,0.0f)\n");
        QuaternionScreenPrintf(0, 20, interpolate1, "interpolate1,Slerp(q0,q1,0.3f)\n");
        QuaternionScreenPrintf(0, 40, interpolate2, "interpolate2,Slerp(q0,q1,0.5f)\n");
        QuaternionScreenPrintf(0, 60, interpolate3, "interpolate3,Slerp(q0,q1,0.7f)\n");
        QuaternionScreenPrintf(0, 80, interpolate4, "interpolate4,Slerp(q0,q1,1.0f)\n");

        Novice::EndFrame();

        // ESCキーで終了
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    Novice::Finalize();
    return 0;
}

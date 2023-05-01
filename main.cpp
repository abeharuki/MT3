#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>


const char kWindowTitle[] = "学籍番号";

struct Vector3 {
	float x, y, z;
};

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4 {
	float m[4][4];
};

//クォータニオン作成
//　axis　回転させる軸
// radian 回転させる角度
//return  作成したクォータニオン
Vector4 MakeQuaternion(Vector3 axis, float radian) {
	Vector4 quaternion;
	float halfSin, halfCos;      //動かす角度の半分のsin,cos
	float normal;

	quaternion = { 0,0,0,0 };
	// 回転軸の長さを求める
	normal = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
	if (normal <= 0.0f) return quaternion;

	// 方向ベクトルへ（単位ベクトル：長さは1）
	normal = 1.0f / sqrtf(normal);
	axis.x = axis.x * normal;
	axis.y = axis.y * normal;
	axis.z = axis.z * normal;


	halfSin = sinf(radian * 0.5f);
	halfCos = cosf(radian * 0.5f);

	//四次元ベクトル (λ.x*sinθ/2,λ.y*sinθ/2,λ.z*sinθ/2,cosθ/2)
 
	quaternion.w = halfCos;
	quaternion.x = axis.x * halfSin;
	quaternion.y = axis.y * halfSin;
	quaternion.z = axis.z * halfSin;

	return quaternion;
}

//クォータニオンの掛け算
// left   計算の左の項
// right  計算の右の項
// return 計算したクォータニオン
Vector4 CalcQuaternion(Vector4 left, Vector4 right)
{
	Vector4 quaternion;
	float   num1, num2, num3, num4;

	num1 = left.w * right.w;
	num2 = -left.x * right.x;
	num3 = -left.y * right.y;
	num4 = -left.z * right.z;
	quaternion.w = num1 + num2 + num3 + num4;

	num1 = left.w * right.x;
	num2 = left.x * right.w;
	num3 = left.y * right.z;
	num4 = -left.z * right.y;
	quaternion.x = num1 + num2 + num3 + num4;

	num1 = left.w * right.y;
	num2 = left.y * right.w;
	num3 = left.z * right.x;
	num4 = -left.x * right.z;
	quaternion.y = num1 + num2 + num3 + num4;

	num1 = left.w * right.z;
	num2 = left.z * right.w;
	num3 = left.x * right.y;
	num4 = -left.y * right.x;
	quaternion.z = num1 + num2 + num3 + num4;

	return   quaternion;
}

//クォータニオンによる回転
// axis    回転させたい軸
// pos     回転させるオブジェクトの座標
// radius  回転させる角度
// return  回転後の座標
Vector3 RotateQuaternionPosition(Vector3 axis, Vector3 pos, float radius)
{
	Vector4  complexNumber, complexConjugateNumber;
	Vector4 posQuaternion = { 0, pos.x, pos.y, pos.z };
	Vector3     resultPosition;

	if (axis.x == 0 && axis.y == 0 && axis.z == 0 ||
		radius == 0) {
		return pos;
	}

	complexNumber = MakeQuaternion(axis, radius);
	complexConjugateNumber = MakeQuaternion(axis, -radius);

	posQuaternion = CalcQuaternion(complexNumber, posQuaternion);
	posQuaternion = CalcQuaternion(posQuaternion, complexConjugateNumber);

	resultPosition.x = posQuaternion.x;
	resultPosition.y = posQuaternion.y;
	resultPosition.z = posQuaternion.z;

	return resultPosition;
}

static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraPos = { 1,0,0 };  //!< カメラの座標
	Vector3 cameraUp = { 0,1,0 };   //!< カメラのアップベクトル
	Vector3 axis = { 0,1,0 };       //!< 回転させる軸
	float rad = 90 * 3.14f / 180;   //!< 回転角度

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

		
		// 横移動
		cameraPos = RotateQuaternionPosition(axis, cameraPos, rad);

		// 縦移動
		axis.x = cameraPos.y * cameraUp.z - cameraPos.z * cameraUp.y;
		axis.y = cameraPos.z * cameraUp.x - cameraPos.x * cameraUp.z;
		axis.z = cameraPos.x * cameraUp.y - cameraPos.y * cameraUp.x;
		cameraPos = RotateQuaternionPosition(axis, cameraPos, rad);
		cameraUp = RotateQuaternionPosition(axis, cameraUp, rad);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cameraPos, "cameraPos");
		VectorScreenPrintf(0, kColumnWidth, cameraUp, "cameraUp");

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

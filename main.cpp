#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <atlsimpstr.h>



const char kWindowTitle[] = "学籍番号";

struct Vector2 final {
	float x;
	float y;
};


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

//  クォータニオン作成
// q = w + xi + yj + zk
//　axis　回転させる軸
// radian 回転させる角度
//return  回転させる軸と角度を決めてクォータニオンにする
// オイラー角の実数が入ってるとこを1,実数の値はradianに入れる
// (例：オイラー角が{5,0,0}だったら　axis{1,0,0} radian 5)
//マウスで視点移動する場合はradianにマウスの移動量を入れる(多分!)
Vector4 MakeQuaternion(Vector3 axis, float radian) {
	Vector4 quaternion;
	float halfSin, halfCos;      //動かす角度の半分のsin,cos
	float normal;

	quaternion = { 0,0,0,0 };
	// 回転軸の長さを求める
	//λ2x+λ2y+λ2z=1方向が重要だからノルムを１に統一
	normal = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
	if (normal <= 0.0f) return quaternion;

	// 方向ベクトルへ（単位ベクトル：長さは1）
	//ノルムは１という決まり事
	//sqrtfは平方根
	normal = 1.0f / sqrtf(normal);
	axis.x = axis.x * normal;
	axis.y = axis.y * normal;
	axis.z = axis.z * normal;

	//四次元ベクトル (λ.x*sinθ/2,λ.y*sinθ/2,λ.z*sinθ/2,cosθ/2)
	halfSin = sinf(radian * 0.5f);
	halfCos = cosf(radian * 0.5f);
	
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
//掛け算したクォータニオンは、それ自体 1 つの回転
//つまり(q1*q2)はq1で回転した後にq2さらに回転した結果になる
Vector4 CalcQuaternion(Vector4 left, Vector4 right)
{
	Vector4 quaternion;
	float   num1, num2, num3, num4;

	//w
	num1 = left.w * right.w;
	num2 = -left.x * right.x;
	num3 = -left.y * right.y;
	num4 = -left.z * right.z;
	quaternion.w = num1 + num2 + num3 + num4;

	//x
	num1 = left.w * right.x;
	num2 = left.x * right.w;
	num3 = left.y * right.z;
	num4 = -left.z * right.y;
	quaternion.x = num1 + num2 + num3 + num4;

	//y
	num1 = left.w * right.y;
	num2 = left.y * right.w;
	num3 = left.z * right.x;
	num4 = -left.x * right.z;
	quaternion.y = num1 + num2 + num3 + num4;

	//z
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
//わからないけどオイラー角をクォータニオンに変換それを回転行列に変換してるかも
Vector3 RotateQuaternionPosition(Vector3 axis, Vector3 pos, float radius)
{
	Vector4 complexNumber, complexConjugateNumber;
	Vector4 posQuaternion = { 0, pos.x, pos.y, pos.z };
	Vector3 resultPosition;

	if (axis.x == 0 && axis.y == 0 && axis.z == 0 ||
		radius == 0) {
		return pos;
	}

	//右手系と左手系
	//クォータニオンの作成
	complexNumber = MakeQuaternion(axis, radius);
	//逆回転のクォータニオン
	complexConjugateNumber = MakeQuaternion(axis, -radius);

	//クォータニオンの積
	posQuaternion = CalcQuaternion(complexNumber, posQuaternion);
	posQuaternion = CalcQuaternion(posQuaternion, complexConjugateNumber);

	resultPosition.x = posQuaternion.x;
	resultPosition.y = posQuaternion.y;
	resultPosition.z = posQuaternion.z;

	return resultPosition;
}

//オイラー角からクォータニオンに変換だけ
Vector4 toQuaternion(Vector3 euler) {
	Vector4 quaternion;
	float   num1, num2, num3, num4,num5,num6;

	//w
	num1 = cosf(euler.x / 2.0f);
	num2 = cosf(euler.y / 2.0f);
	num3 = cosf(euler.z / 2.0f);
	num4 = sinf(euler.x / 2.0f);
	num5 = sinf(euler.y / 2.0f);
	num6 = sinf(euler.z / 2.0f);
	quaternion.w = num1 * num2 * num3 - num4 * num5 * num6;

	//x
	num1 = sinf(euler.x / 2.0f);
	num2 = cosf(euler.y / 2.0f);
	num3 = cosf(euler.z / 2.0f);
	num4 = cosf(euler.x / 2.0f);
	num5 = sinf(euler.y / 2.0f);
	num6 = sinf(euler.z / 2.0f);
	quaternion.x = num1 * num2 * num3 + num4 * num5 * num6;

	//y
	num1 = cosf(euler.x / 2.0f);
	num2 = sinf(euler.y / 2.0f);
	num3 = cosf(euler.z / 2.0f);
	num4 = sinf(euler.x / 2.0f);
	num5 = cosf(euler.y / 2.0f);
	num6 = sinf(euler.z / 2.0f);
	quaternion.y = num1 * num2 * num3 - num4 * num5 * num6;

	//z
	num1 = cosf(euler.x / 2.0f);
	num2 = cosf(euler.y / 2.0f);
	num3 = sinf(euler.z / 2.0f);
	num4 = sinf(euler.x / 2.0f);
	num5 = sinf(euler.y / 2.0f);
	num6 = cosf(euler.z / 2.0f);
	quaternion.z = num1 * num2 * num3 + num4 * num5 * num6;

	return   quaternion;
}
//クォータニオンから回転行列に変換
Matrix4x4 QuaternionMatrix(Vector4 quaternion) {
	Matrix4x4 result;
	result.m[0][0] = 2 * (quaternion.w * quaternion.w) + 2 * (quaternion.x * quaternion.x) - 1;
	result.m[0][1] = 2 * (quaternion.x * quaternion.y) - 2 * (quaternion.z * quaternion.w);
	result.m[0][2] = 2 * (quaternion.x * quaternion.z) + 2 * (quaternion.y * quaternion.w);
	result.m[0][3] = 0;

	result.m[1][0] = 2 * (quaternion.x * quaternion.y) + 2 * (quaternion.z * quaternion.w);
	result.m[1][1] = 2 * (quaternion.w * quaternion.w) + 2 * (quaternion.y * quaternion.y) - 1;
	result.m[1][2] = 2 * (quaternion.y * quaternion.z) - 2 * (quaternion.x * quaternion.w);
	result.m[1][3] = 0;

	result.m[2][0] = 2 * (quaternion.x * quaternion.z) + 2 * (quaternion.y * quaternion.w);
	result.m[2][1] = 2 * (quaternion.y * quaternion.z) + 2 * (quaternion.x * quaternion.w);
	result.m[2][2] = 2 * (quaternion.w * quaternion.w) + 2 * (quaternion.z * quaternion.z) - 1;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
//クォータニオンからオイラー角に変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +
		1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +
		1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +
		1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] +
		1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
};
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void VectorScreenPrintf(int x, int y, const Vector4& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3 ,y, "%.02f", vector.w);
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraPos = { 0,0,0 };  //!< カメラの座標
	Vector3 cameraUp = { 0,1,0 };   //!< カメラのアップベクトル
	Vector3 axis = { 0,1,0 };       //!< 回転させる軸　この場合はｙ軸を回転させる
	float rad = 90 * 3.14f / 180;   //!< 回転角度(90度の回転）
	
	Vector3 cameraPos2 = { 1,0,0 };
	Vector3 axis2 = {0,1,0};
	//
	Vector3 a = {0.0f,1.0f,0.0f};
	Vector4 b = {};

	Vector3 c = { 0.0f,0.0f,0.0f };
	Vector4 d = {};

	
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

	 
		//MT3_04で学んだのはオイラー角を回転行列に変換の仕方
		
		

		// 横移動 axis{0,1,0}で横
		//x座標をrad回転
		cameraPos = RotateQuaternionPosition(axis, cameraPos, rad);
		

		// 縦移動
		axis.x = cameraPos.y * cameraUp.z - cameraPos.z * cameraUp.y;
		axis.y = cameraPos.z * cameraUp.x - cameraPos.x * cameraUp.z;
		axis.z = cameraPos.x * cameraUp.y - cameraPos.y * cameraUp.x;
		cameraPos2 = RotateQuaternionPosition(axis, cameraPos, rad);
		cameraUp = RotateQuaternionPosition(axis, cameraUp, rad);

		//同じ値になる
		b = MakeQuaternion(a,rad); //a{5,0,0}
		Matrix4x4 rotationMatrix = QuaternionMatrix(b);
	
		Vector3 e =  Transform({0,0,0}, rotationMatrix);

		/*--------------------------------------------------------------
		
		//マウスで視点移動するときのコード(試し書き)

		Vector3 xRot = {};
		Vector3 yRot = {};

		Vector3 axis1 = {};
		Vector3 axis2 = {};

		float cameraRotaSpeed = 3.0f; ←カメラの回転速度

		Vector2 Radian = {};

		//マウスの移動量
		//度数法からラジアンに変換
		Radian.x = GetMousePosX * cameraRoteSpeed * M_PI/180
		Radian.Y = GetMousePosY * cameraRoteSpeed * M_PI/180

		//角度の変更
		xRot.y += Radian.x ;  axis1 = {0,1,0};
		yRot.x -= Radian.y ;  axis2 = {1,0,0};
		
		//オイラー角をクォータニオンに変換
		//縦向きの回転
		Vector4 Euler1 = toQuaternion(yRot);　= MakeQuaternion(axis2,yRot.x);
		//横向きの回転
		Vector4 Euler2 = toQuaternion(xRot);  = MakeQuaternion(axis1,xRot.y);
		
		//クォータニオンの掛け算
		Vector4 comeraRot = CalcQuaternion (Euler1,Euler2);

		この後にcomeraRotを回転行列に変換→ワールド変換→ビュー変換→射影変換で一人称視点の完成？
		
		----------------------------------------------------------------*/


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, b, "b");
		VectorScreenPrintf(0, kColumnWidth, d, "d");
		VectorScreenPrintf(0, kColumnWidth*2, e, "e");
		Novice::ScreenPrintf(0, kColumnWidth * 3,"%f", rad);


		VectorScreenPrintf(0, kColumnWidth * 4, cameraPos, "b");
		VectorScreenPrintf(0, kColumnWidth*5, cameraPos2, "d");
		VectorScreenPrintf(0, kColumnWidth * 6, e, "e");
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

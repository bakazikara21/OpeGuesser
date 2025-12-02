# include <Siv3D.hpp> // Siv3D v0.6.16
# include <utility>	// pair<>を使いたい
# include <cmath>	// floorを使いたい
# include <set>	// setを使いたい
using namespace std;

//////////////////////////////////
//
//	演算子の処理で役立つ関数
//
//////////////////////////////////
double Calculate(double x, double y, char op)
{
	// x op yの計算をしてくれる関数

	if (op == '+') return x + y;

	if (op == '-') return x - y;

	if (op == '*') return x * y;

	if (op == '/') {
		if (abs(y) < 1e-9) throw runtime_error("Zero division");
		return x / y;
	}

	throw runtime_error("Invalid operator");
}
String Change(char op)
{
	// 演算子を算数でよく見る形になおしてくれる関数

	if (op == '+') return U"＋";

	if (op == '-') return U"－";

	if (op == '*') return U"×";

	if (op == '/') return U"÷";
	

	throw runtime_error("Invalid operator");
}

//////////////////////////////////
//
//	ゲームの状態を管理するクラス
//
//////////////////////////////////
struct GameCalculator
{
	// 画面に表示される左辺の4つの整数(A + B - C + Dみたいな感じになる)
	int32 A = 0, B = 0, C = 0, D = 0;

	// 解がanswerとなるように演算子を決めていくゲーム
	int answer = 0;

	// 0->問題を解いている途中 1->問題を提出した状態 2->退出ボタンを押した状態
	int status = 0;

	// 式の値と演算子の文字列をペアにして、それらの集合を持っておく
	set<pair<int, string>> correct;

	// プレイヤーが選んだ3つの演算子をここに格納する。初期値は'?'
	string submit = "???";

	void update()
	{
		// ゲームの状態を更新する(新しい問題に更新する)

		// AからDを1～9のランダムな整数に書き換える
		A = Random(1,9);
		B = Random(1,9);
		C = Random(1,9);
		D = Random(1,9);

		string op = "+-*/";	// 四則演算で使われる演算子の文字列

		Array<int> answers;	// AからDを使って得られるすべての整数解を記憶する

		correct.clear();	// 正解となる組み合わせを毎回ちゃんと初期化する

		submit = "???";		// プレイヤーが選択した演算子も毎回ちゃんと初期化する

		// 以下は演算子の文字列と、それに対応する整数解の全探索

		for (int32 opAB = 0; opAB < 4; opAB++)	// AとBの間の演算記号
		{
			for (int32 opBC = 0; opBC < 4; opBC++)	// BとCの間の演算記号
			{
				for (int32 opCD = 0; opCD < 4; opCD++)	// CとDの間の演算記号
				{
					// 演算子の種類の順番によって場合分けして上手く計算結果を得る

					double ans = 0.0;
					double a = 1.0 * A;
					double b = 1.0 * B;
					double c = 1.0 * C;
					double d = 1.0 * D;
					if (opBC < 2 and opCD >= 2)
					{
						// ?+*みたいな順番のとき 1->3->2
						b = Calculate(a, b, op[opAB]);

						c = Calculate(c, d, op[opCD]);	

						ans = Calculate(b, c, op[opBC]);
					}
					else if (opAB < 2 and opBC >= 2)
					{
						// +*?みたいな順番のとき 2->3->1
						c = Calculate(b, c, op[opBC]);	

						b = Calculate(c, d, op[opCD]);	

						ans = Calculate(a, b, op[opAB]);
					}
					else
					{
						// その他は前から順に計算すればよい 1->2->3
						b = Calculate(a, b, op[opAB]);	

						c = Calculate(b, c, op[opBC]);	

						ans = Calculate(c, d, op[opCD]);
					}

					string str;
					str.push_back(op[opAB]);
					str.push_back(op[opBC]);
					str.push_back(op[opCD]);
					
					if (ans == floor(ans))	// 小数部分が0のとき(整数のとき)
					{
						answers.push_back(ans);	// 解を追加する

						correct.insert({ans,str});	// 答えの組み合わせをsetとして追加する
					}
				}
			}
		}

		answer = answers.choice();	//	解が存在する整数の中からランダムで一つ選ぶ
	}

	bool IsCorrect()
	{
		// プレイヤーが選んだ演算子が正解なのかどうか
		return correct.count({ answer,submit });
	}

	int32 getNextSelect()
	{
		// 次に埋めるべき演算子の場所を返す関数 すべて埋まっていたら-1を返す
		int32 select = -1;

		if (submit[0] == '?') select = 0;
		else if (submit[1] == '?') select = 1;
		else if (submit[2] == '?') select = 2;

		return select;
	}
};

/////////////////////////
//
//	描画に関する関数
//
/////////////////////////
void DrawEquation(GameCalculator& game, const Font& font, Audio cancel)
{
	// 等式を描画する関数

	int32 strSize = 80;
	Size rectSize(120, 120);

	Rect rect{ 80,200,rectSize };
	int32 select = game.getNextSelect();

	// 整数Aを描く
	font(U"{}"_fmt(game.A)).drawAt(strSize, rect.center(), Palette::Black);

	// プレイヤーが決定する1つ目の演算子
	rect.moveBy(120, 0).stretched(-10).drawFrame(2);
	if (game.submit[0] != '?')
	{
		if (rect.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		font(U"{}"_fmt(Change(game.submit[0]))).drawAt(80, rect.center(), Palette::Black);

		if (rect.leftClicked())
		{
			game.submit[0] = '?';
			cancel.playOneShot();
		}
	}

	// 整数Bを描く
	font(U"{}"_fmt(game.B)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	// プレイヤーが決定する2つ目の演算子
	rect.moveBy(120, 0).stretched(-10).drawFrame(2);
	if (game.submit[1] != '?')
	{
		if (rect.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		font(U"{}"_fmt(Change(game.submit[1]))).drawAt(80, rect.center(), Palette::Black);

		if (rect.leftClicked())
		{
			game.submit[1] = '?';
			cancel.playOneShot();
		}
	}

	// 整数Cを描く
	font(U"{}"_fmt(game.C)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	// プレイヤーが決定する3つ目の演算子
	rect.moveBy(120, 0).stretched(-10).drawFrame(2);
	if (game.submit[2] != '?')
	{
		if (rect.mouseOver())
		{
			// マウスカーソルを手の形にする
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		font(U"{}"_fmt(Change(game.submit[2]))).drawAt(80, rect.center(), Palette::Black);

		if (rect.leftClicked())
		{
			game.submit[2] = '?';
			cancel.playOneShot();
		}
	}

	// 整数Dを描く
	font(U"{}"_fmt(game.D)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	// '='を描く
	font(U"=").drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	// 4つの整数の演算結果を描く
	font(U"{}"_fmt(game.answer)).draw(strSize, Arg::leftCenter = rect.moveBy(120, 0).leftCenter(), Palette::Black);
}
void DrawOperater(const Font& font, GameCalculator& game, Audio decide)
{
	// 二段目の4つの演算子を表示する関数

	Size rectSize(120, 120);

	Rect rect{ 300,400,rectSize };

	int32 select = game.getNextSelect();

	// +を描く
	rect.drawFrame(2);
	font(U"＋").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver() and select >= 0)
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked())
		{
			game.submit[select] = '+';
			decide.playOneShot();
		}
	}

	// -を描く
	rect.moveBy(180, 0).drawFrame(2);
	font(U"－").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver() and select >= 0)
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked())
		{
			game.submit[select] = '-';
			decide.playOneShot();
		}
	}

	// ×を描く
	rect.moveBy(180, 0).drawFrame(2);
	font(U"×").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver() and select >= 0)
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked())
		{
			game.submit[select] = '*';
			decide.playOneShot();
		}
	}

	// ÷を描く
	rect.moveBy(180, 0).drawFrame(2);
	font(U"÷").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver() and select >= 0)
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked())
		{
			game.submit[select] = '/';
			decide.playOneShot();
		}
	}
}
void DrawEffectAnswer(bool answer, const Font& font)
{
	// 提出された答えが合っているか間違っているかを描画する関数
	
	// 背景の色を重ねて濃くすることで前景と背景をわかりやすくした
	Rect{ 0,0,1280,720 }.draw(ColorF{ 0.6, 0.8, 0.7 ,0.8 });
	if (answer)
	{
		// 答えがあっていたら〇を描画する
		Circle{ Vec2{ 640, 360 },200 }.drawFrame(15, ColorF{ 1.0,0,0,0.8 });
		font(U"正解！").drawAt(Vec2{ 640,100 }, ColorF{1.0,0,0,0.8});
	}
	else
	{
		// 間違っていたら×を描画する
		Shape2D::Cross(200, 15, Vec2{ 640, 360 }).draw(ColorF{0,0,1.0,0.8});
		font(U"ざんねん！").drawAt(Vec2{ 640,100 }, ColorF{0,0,1.0,0.8});
	}
}
bool Button(const Rect& rect, const Font& font, const String& text, bool enabled, const ColorF& color)
{
	// ボタンを描き、押されたかどうかを判定する関数
	
	// マウスカーソルがボタンの上にある場合
	if (enabled && rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	const RoundRect roundRect = rect.rounded(6);

	// 影と背景を描く
	roundRect
		.drawShadow(Vec2{ 2, 2 }, 12, 0)
		.draw(color);

	// 枠を描く
	rect.stretched(-3).rounded(3)
		.drawFrame(2, Palette::Black);

	// テキストを描く
	font(text).drawAt(40, rect.center(), Palette::Black);

	// 無効の場合は
	if (not enabled)
	{
		// グレーの半透明を重ねる
		roundRect.draw(ColorF{ 0.8, 0.8 });
	}

	// ボタンが押されたら true を返す
	return (enabled && rect.leftClicked());
}
void Main()
{
	// ウィンドウタイトルを設定する
	Window::SetTitle(U"OpeGuesser v1.0");

	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	// ウインドウサイズを変更する
	Window::Resize(1280, 720);

	// 太文字のフォントを作成する
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	const Audio AC{ Resource(U"sound/AC.mp3") };	// 正解の音
	const Audio WA{ Resource(U"sound/WA.mp3")};		// 不正解の音
	const Audio Exit{ Resource(U"sound/ExitButton.mp3") };	// 退出ボタンを押したときの音

	const Audio Decide{ Resource(U"sound/DecideButton.mp3") };	// 演算子を押したときの音
	const Audio Cancel{ Resource(U"sound/CancelButton.mp3") };	// 演算子を取り消すときの音

	// ゲーム本体を宣言
	GameCalculator game;

	// ゲームの状態を初期化
	game.update();

	// 経過時間を測ることができるstopwatch
	Stopwatch stopwatch{ StartImmediately::No };

	while (System::Update())
	{
		/////////////////////////
		//
		//	描画
		//
		/////////////////////////
	
		DrawEquation(game, font, Cancel);
		DrawOperater(font, game, Decide);
		if (game.status == 1)
		{
			// 解答を提出したら
			
			// 正解・不正解の描画を1秒くらい続ける
			DrawEffectAnswer(game.IsCorrect(), font);

			// stopwatchが1秒以上になるまで次には進まない!
			if (stopwatch.s() < 1.0) continue;

			// stopwatchが1秒以上になったらgame.statusを0に戻す
			game.status = 0;

			// .update()で問題文を変更する
			game.update();

			// stopwatchをリセットする
			stopwatch.reset();
		}
		else if (game.status == 2)
		{
			// 退出ボタンを押したら

			// Yes か No のメッセージボックスを表示する
			const MessageBoxResult result = System::MessageBoxYesNo(U"アプリケーションを終了しますか？");

			// Yes が選ばれたら
			if (result == MessageBoxResult::Yes)
			{
				// ゲームをを終了する
				break;
			}
			else if (result == MessageBoxResult::No)
			{
				// ゲームを続ける
				game.status = 0;
				continue;
			}
		}

		/////////////////////////
		//
		//	更新
		//
		/////////////////////////

		// 提出ボタンが押されたら
		if (Button(Rect{ Arg::center(640,600),240,80 }, font, U"答えを提出", (game.getNextSelect()==-1), ColorF{ 0.9, 0.8, 0.6 }))
		{
			// ボタンが有効で、かつそのボタンが押されたらgame.statusを1に変更
			game.status = 1;

			// 1秒間〇 or ×を画面に表示させるためにstopwatchをstart
			stopwatch.start();

			if (game.IsCorrect()) {
				// 正解だったらピンポンピンポン!
				AC.play();
			}
			else
			{
				// 不正解だったらブブー!
				WA.play();
			}
			// statusが変更されたら後の処理は行わない
			continue;
		}

		/////////////////////////
		//
		//	途中退出
		//
		/////////////////////////
		if (Button(Rect{ Arg::rightCenter(1250,600),120,60 }, font, U"退出", (true), ColorF{ 0.9, 0.2, 0.1 }))
		{
			// statusを2に変更して後の処理に任せる
			game.status = 2;
			Exit.play();
		}
	}
}

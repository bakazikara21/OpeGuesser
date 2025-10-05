# include <Siv3D.hpp> // Siv3D v0.6.16
# include <utility>	// pair<>を使いたい
# include <cmath>	// floorを使いたい
# include <set>	// setを使いたい
using namespace std;

double Calculate(double x, double y, char op)
{
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
	if (op == '+') return U"＋";

	if (op == '-') return U"－";

	if (op == '*') return U"×";

	if (op == '/') return U"÷";
	

	throw runtime_error("Invalid operator");
}
struct GameCalculator
{
	int32 A = 0, B = 0, C = 0, D = 0;
	int answer = 0;
	set<pair<int, string>> correct;
	string submit = "???";

	void update()
	{
		A = Random(1, 9);
		B = Random(1, 9);
		C = Random(1, 9);
		D = Random(1, 9);

		string op = "+-*/";
		Array<pair<int, string>> answers;
		correct.clear();
		submit = "???";

		for (int32 opAB = 0; opAB < 4; opAB++)	// AとBの間の演算記号
		{
			for (int32 opBC = 0; opBC < 4; opBC++)	// BとCの間の演算記号
			{
				for (int32 opCD = 0; opCD < 4; opCD++)	// CとDの間の演算記号
				{
					// はじめに左からかけ算割り算から計算して
					// 次に左から足し算引き算を計算すればよい
					double ans = 0.0;
					double a = 1.0 * A;
					double b = 1.0 * B;
					double c = 1.0 * C;
					double d = 1.0 * D;
					if (opBC < 2 and opCD >= 2)
					{
						// ?+*みたいな順番のとき 1->3->2
						b = Calculate(a, b, op[opAB]);	if (abs(d) < 1e-9 and op[opCD]=='/') throw runtime_error("Zero division Status 1");

						c = Calculate(c, d, op[opCD]);	if (abs(c) < 1e-9 and op[opBC] == '/') throw runtime_error("Zero division Status 2");

						ans = Calculate(b, c, op[opBC]);
					}
					else if (opAB < 2 and opBC >= 2)
					{
						// +*?みたいな順番のとき 2->3->1
						c = Calculate(b, c, op[opBC]);	if (abs(d) < 1e-9 and op[opCD] == '/') throw runtime_error("Zero division Status 3");

						b = Calculate(c, d, op[opCD]);	if (abs(b) < 1e-9 and op[opAB] == '/') throw runtime_error("Zero division Status 4");

						ans = Calculate(a, b, op[opAB]);
					}
					else
					{
						// その他は前から順に計算すればよい 1->2->3
						b = Calculate(a, b, op[opAB]);	if (abs(c) < 1e-9 and op[opBC] == '/') throw runtime_error("Zero division Status 5");

						c = Calculate(b, c, op[opBC]);	if (abs(d) < 1e-9 and op[opCD] == '/') throw runtime_error("Zero division Status 6");

						ans = Calculate(c, d, op[opCD]);
					}

					string str;
					str.push_back(op[opAB]);
					str.push_back(op[opBC]);
					str.push_back(op[opCD]);
					
					if (ans == floor(ans))	// 小数部分が0のとき(整数のとき)
					{
						answers.push_back({ ans,str });

						correct.insert({ans,str});
					}
				}
			}
		}

		answer = answers.choice().first;	//	解が存在する整数の中からランダムで一つ選ぶ
	}

	bool IsCorrect()
	{
		return correct.count({ answer,submit });
	}

	int32 getNotDecided()
	{
		int32 select = -1;

		if (submit[0] == '?') select = 0;
		else if (submit[1] == '?') select = 1;
		else if (submit[2] == '?') select = 2;

		return select;
	}
};

void DrawEquation(GameCalculator& game, const Font& font)
{
	int32 strSize = 80;
	Size rectSize(120, 120);

	Rect rect{ 100,200,rectSize };
	int32 select = game.getNotDecided();

	Array<Rect> rects;

	font(U"{}"_fmt(game.A)).drawAt(strSize, rect.center(), Palette::Black);

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
		}
	}

	font(U"{}"_fmt(game.B)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

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
		}
	}

	font(U"{}"_fmt(game.C)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

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
		}
	}

	font(U"{}"_fmt(game.D)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	font(U"=").drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	font(U"{}"_fmt(game.answer)).drawAt(strSize, rect.moveBy(120, 0).center(), Palette::Black);

	// tutorial35みよう
}
void DrawOperater(const Font& font, GameCalculator& game)
{
	Size rectSize(120, 120);

	Rect rect{ 300,400,rectSize };

	int32 select = game.getNotDecided();

	// +
	rect.drawFrame(2);
	font(U"＋").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked() and select >= 0)
		{
			game.submit[select] = '+';
		}
	}

	// -
	rect.moveBy(180, 0).drawFrame(2);
	font(U"－").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked() and select >= 0)
		{
			game.submit[select] = '-';
		}
	}

	// ×
	rect.moveBy(180, 0).drawFrame(2);
	font(U"×").draw(80, Arg::center = rect.center(), ColorF{ 0.1 });
	if (rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked() and select >= 0)
		{
			game.submit[select] = '*';
		}
	}

	// ÷
	rect.moveBy(180, 0).drawFrame(2);
	font(U"÷").draw(80, Arg::center = rect.center(),ColorF{0.1});
	if (rect.mouseOver())
	{
		// マウスカーソルを手の形にする
		Cursor::RequestStyle(CursorStyle::Hand);

		if (rect.leftClicked() and select >= 0)
		{
			game.submit[select] = '/';
		}
	}
}
void DrawEffectAnswer(bool answer)
{
	// 作成と同時に計測を開始
	Stopwatch stopwatch{ StartImmediately::Yes };
	if (answer)
	{
		while (stopwatch.s() < 1.0)
		{
			Circle{ Vec2{ 640, 360 },200 }.drawFrame(10,Palette::Red);
		}
	}
	else
	{
		while (stopwatch.s() < 1.0)
		{
			Shape2D::Cross(200, 10, Vec2{ 640, 360 }).draw(Palette::Aliceblue);
		}
	}
	stopwatch.pause();
}
bool Button(const Rect& rect, const Font& font, const String& text, bool enabled)
{
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
		.draw(ColorF{ 0.9, 0.8, 0.6 });

	// 枠を描く
	rect.stretched(-3).rounded(3)
		.drawFrame(2, ColorF{ 0.4, 0.3, 0.2 });

	// テキストを描く
	font(text).drawAt(40, rect.center(), ColorF{ 0.4, 0.3, 0.2 });

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
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Window::Resize(1280, 720);

	// 太文字のフォントを作成する | Create a bold font with MSDF method
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };

	GameCalculator game;
	game.update();
	while (System::Update())
	{
		/////////////////////////
		//
		//	更新
		//
		/////////////////////////
		if (Button(Rect{ Arg::center(640,600),240,80 }, font, U"答えを提出", (game.getNotDecided()==-1)))
		{
			DrawEffectAnswer(game.IsCorrect());
			if(game.IsCorrect()) game.update();
		}

		/////////////////////////
		//
		//	描画
		//
		/////////////////////////
		DrawEquation(game, font);
		DrawOperater(font,game);
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//

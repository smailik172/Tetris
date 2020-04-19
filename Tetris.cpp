//подключим SDL и stdio
#include <SDL.h>  //x86
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <math.h>



class Lifes
{

private:
	//Некоторые константы нашего окна
	static const int SCREEN_WIDTH = 1166;
	static const int SCREEN_HEIGHT = 568;

	SDL_Window* window;
	SDL_Renderer* ren;
	void (Lifes::* initializeLifes[3]) () = { &Lifes::InitLifesOne ,&Lifes::InitLifesTwo, &Lifes::InitLifesThree };
	//два массива для точек - текущее состояние и новое 
	int lifes[SCREEN_WIDTH][SCREEN_HEIGHT];
	int lifesNew[SCREEN_WIDTH][SCREEN_HEIGHT];
	//Инициализация SDL, окна и рендера
	bool InitSDL()
	{
		bool res = true;
		//инициализируем SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		//создаем окно (имя окна, координаты (x,y), размеры окна, флаги)
		//int request = SDL_GetDesktopDisplayMode(0, &displayMode);
		//window = SDL_CreateWindow("Hello World!", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
		window = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			res = false;
		}

		//рендер (окно, индекс драйвера (-1 - первый подходящий), флаги (аппаратное ускорение и вертикальная синхронизация)
		ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == nullptr) {
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		return res;
	}
	//дестрой SDL, окна и рендера
	void QuitSDL()
	{
		SDL_DestroyWindow(window);
		window = NULL;

		SDL_DestroyRenderer(ren);
		ren = NULL;

		SDL_Quit();
	}

	//методы начальной отрисовки точек
	void InitLifesOne()
	{
		int xx[] = { rand() % (SCREEN_WIDTH - 1) + 1, rand() % (SCREEN_WIDTH - 1) + 1 };
		int yy[] = { rand() % (SCREEN_HEIGHT - 1) + 1, rand() % (SCREEN_HEIGHT - 1) + 1 };
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if ((i == xx[0]) | (i == xx[1]) | (j == yy[0]) | (j == yy[1]))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	void InitLifesTwo()
	{
		int xx = SCREEN_WIDTH / 8 + rand() % 100;
		int xx2 = SCREEN_WIDTH / 6 + rand() % 100;
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if ((i - xx == j) | (i + j + xx == SCREEN_WIDTH + 1) | (i - xx2 == j) | (i + j + xx2 == SCREEN_WIDTH + 1))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	void InitLifesThree()
	{
		int xx = SCREEN_WIDTH / 2 - 100 + rand() % 200;
		int kk = 100 + rand() % 100;
		int zz = 100000;
		int yy = SCREEN_HEIGHT / 2 - 100 + rand() % 200;
		int yy2 = SCREEN_HEIGHT / 3 - 100 + rand() % 200;
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if (((int)pow(i - xx, 2) * yy / zz == j) | ((int)pow(i - xx, 2) * yy / zz - 1 == j) | ((int)pow(i - xx, 2) * yy / zz + 1 == j) | (j == yy) | (j == yy2))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	//случайным образом выбираем вариант отрисовки
	void InitLifes()
	{
		int k = rand() % (sizeof(initializeLifes) / sizeof(initializeLifes[0]));
		(this->*initializeLifes[k])();
		//(this->*initializeLifes[2])();
		Paint();
		SDL_Delay(500);
	}

	/*определяем новое состояние точек по правилам:
	  точка живая и рядом <2 живых - точка умирает (вырождение)
	  точка живая и рядом 2-3 живых - точка живет дальше
	  точка живая и рядом >3 живых - точка умирает (перенаселение)
	  точка мертва и рядом ровно 3 живых - точка оживает
	  идея взята отсюда - https://www.youtube.com/watch?v=C2vgICfQawE&feature=emb_logo
	*/
	void NewLifes()
	{
		int k = 0;
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
			{
				k = 0;
				k += lifes[i - 1][j];
				k += lifes[i + 1][j];
				k += lifes[i - 1][j - 1];
				k += lifes[i + 1][j - 1];
				k += lifes[i - 1][j + 1];
				k += lifes[i + 1][j + 1];
				k += lifes[i][j - 1];
				k += lifes[i][j + 1];
				if ((lifes[i][j] == 1) & ((k < 2) | (k > 3)))
					lifesNew[i][j] = 0;
				else if ((lifes[i][j] == 0) & (k == 3))
					lifesNew[i][j] = 1;
			}
	}

	//копируем полученные значения точек во второй массив
	void CopyLifes()
	{
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
				lifes[i][j] = lifesNew[i][j];
	}

	//отрисовка точек на экране
	void Paint()
	{

		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
				if (lifes[i][j] == 1)
					SDL_RenderDrawPoint(ren, i, j);

		SDL_RenderPresent(ren);
	}


public:
	Lifes()
	{

	}

	~Lifes()
	{
		QuitSDL();
	}

	bool Initialize()
	{
		if (!InitSDL())
		{
			QuitSDL();
			return false;
		}
		else
		{
			InitLifes();//инициализация массива данных
			Paint();
		}
		return true;
	}

	//вызываем функции обновления состояния точек и пререрисовки экрана, пока не закроем окно
	void GamesLifes()
	{
		SDL_Event event;
		bool quit = false;
		bool newInitLifes = false;
		while (!quit)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					quit = true;
				if (event.type == SDL_MOUSEBUTTONDOWN)
					newInitLifes = true;
			}
			if ((newInitLifes) & (!quit))
			{
				InitLifes();//инициализация массива данных
				newInitLifes = false;
			}
			NewLifes();
			CopyLifes();
			Paint();
		}
	}

};

struct coord
{
	int x;
	int y;
};

class Figure
{
public:
	coord center;
	coord more[3];
	int color;
	Figure() {}
	Figure(int x, int y)
	{
		center.x = x;
		center.y = y;
		color = 0;
		for (coord k : more)
		{
			k.x = x;
			k.y = y;
		}
	}
	virtual void Turn()
	{}
	void Lower()
	{
		center.y++;
		for (int i = 0; i < 3; i++)
			more[i].y++;
	}

	void Right()
	{
		center.x++;
		for (int i = 0; i < 3; i++)
			more[i].x++;
	}
	void Left()
	{
		center.x--;
		for (int i = 0; i < 3; i++)
			more[i].x--;
	}

	void Copy(Figure & copyTo)
	{
		this->color = copyTo.color;
		this->center.x = copyTo.center.x;
		this->center.y = copyTo.center.y;
		for (int i = 0; i < 3; i++)
		{
			this->more[i].x = copyTo.more[i].x;
			this->more[i].y = copyTo.more[i].y;
		}
	}
};

class Lines :public Figure
{
public:
	Lines(int x, int y) :Figure(x, y)
	{
		for (int i = 0; i < 3; i++)
			more[i].x = x;
		more[0].y = y + 1;
		more[1].y = y - 1;
		more[2].y = y - 2;
		color = 1;
	}
	void Turn() override
	{
		if (more[0].x == center.x)
		{
			for (int i = 0; i < 3; i++)
				more[i].y = center.y;
			more[0].x = center.x - 1;
			more[1].x = center.x + 1;
			more[2].x = center.x + 2;
		}
		else
		{
			for (int i = 0; i < 3; i++)
				more[i].x = center.x;
			more[0].y = center.y + 1;
			more[1].y = center.y - 1;
			more[2].y = center.y - 2;
		}
	}
};

class Cubes :public Figure
{
public:
	Cubes(int x, int y) :Figure(x, y)
	{
		more[0].x = x;
		more[0].y = y - 1;
		more[1].x = x + 1;
		more[1].y = y;
		more[2].x = x + 1;
		more[2].y = y - 1;
		color = 2;
	}
};

class LeftZ :public Figure
{
public:
	LeftZ(int x, int y) :Figure(x, y)
	{
		more[0].x = x + 1;
		more[0].y = y;
		more[1].x = x;
		more[1].y = y - 1;
		more[2].x = x - 1;
		more[2].y = y - 1;
		color = 3;
	}
	void Turn() override
	{
		if (more[1].x == center.x)
		{
			more[1].x = center.x + 1;
			more[2].x = center.x;
			more[2].y = center.y + 1;
		}
		else
		{
			more[1].x = center.x;
			more[2].x = center.x - 1;
			more[2].y = center.y - 1;
		}
	}
};

class RightZ :public Figure
{
public:
	RightZ(int x, int y) :Figure(x, y)
	{
		more[0].x = x - 1;
		more[0].y = y;
		more[1].x = x;
		more[1].y = y - 1;
		more[2].x = x + 1;
		more[2].y = y - 1;
		color = 4;
	}
	void Turn() override
	{
		if (more[1].x == center.x)
		{
			more[1].x = center.x - 1;
			more[2].x = center.x;
			more[2].y = center.y + 1;
		}
		else
		{
			more[1].x = center.x;
			more[2].x = center.x + 1;
			more[2].y = center.y - 1;
		}
	}
};

class FigureT :public Figure
{
public:
	FigureT(int x, int y) :Figure(x, y)
	{
		more[0].x = x - 1;
		more[0].y = y;
		more[1].x = x;
		more[1].y = y - 1;
		more[2].x = x + 1;
		more[2].y = y;
		color = 5;
	}
	void Turn() override
	{
		if ((more[0].x == center.x - 1) & (more[0].y == center.y))
		{
			more[0].x = center.x;
			more[0].y = center.y - 1;
			more[1].x = center.x + 1;
			more[1].y = center.y;
			more[2].x = center.x;
			more[2].y = center.y + 1;
		}
		else if ((more[0].x == center.x) & (more[0].y == center.y - 1))
		{
			more[0].x = center.x + 1;
			more[0].y = center.y;
			more[1].x = center.x;
			more[1].y = center.y + 1;
			more[2].x = center.x - 1;
			more[2].y = center.y;
		}
		else if ((more[0].x == center.x + 1) & (more[0].y == center.y))
		{
			more[0].x = center.x;
			more[0].y = center.y + 1;
			more[1].x = center.x - 1;
			more[1].y = center.y;
			more[2].x = center.x;
			more[2].y = center.y - 1;
		}
		else if ((more[0].x == center.x) & (more[0].y == center.y + 1))
		{
			more[0].x = center.x - 1;
			more[0].y = center.y;
			more[1].x = center.x;
			more[1].y = center.y - 1;
			more[2].x = center.x + 1;
			more[2].y = center.y;
		}
	}
};

class Tetris
{
private:

	//Некоторые константы нашего окна
	static const int SCREEN_WIDTH = 1166;
	static const int SCREEN_HEIGHT = 568;

	SDL_Window* window;
	SDL_Renderer* ren;

	static const int startStep = 100;
	//Параметры игры
	//размеры поля тетриса
	static const int fieldWidth = 10;//ширина поля
	static const int fieldHeigh = 24;//высота поля
	static const int fieldHidden = 4; //количество скрытых верхних строк

	//координаты появления новой фигуры
	static const int newFigX = 4;
	static const int newFigY = 2;
	//скорость
	int speed;
	//пределы скорости
	static const int speedMin = 1000;
	static const int speedMax = 100;
	//размеры кубика
	static const int cubeSize = 20;
	//расстояние между кубиками
	static const int between = 2;
	//размеры тени
	static const int shadow = cubeSize / 8;

	//позиция поля
	static const int fieldLeft = 100;
	static const int fieldTop = 100;
	//массив данных тетриса
	int field[fieldWidth][fieldHeigh];

	//ссылки на конструкторы фигру

	Figure* newFigure, * checkFigure;
	bool newFig;
	bool checkRightFigure()
	{
		if ((checkFigure->center.x < 0) | (checkFigure->center.y < 0) | (checkFigure->center.x >= fieldWidth) | (checkFigure->center.y >= fieldHeigh))
			return false;
		for (int i = 0; i < 3; i++)
			if ((checkFigure->more[i].x < 0) | (checkFigure->more[i].y < 0) | (checkFigure->more[i].x >= fieldWidth) | (checkFigure->more[i].y >= fieldHeigh))
				return false;
		if (field[checkFigure->center.x][checkFigure->center.y] != 0)
			return false;
		for (int i = 0; i < 3; i++)
			if (field[checkFigure->more[i].x][checkFigure->more[i].y] != 0)
				return false;
		return true;
	}

	void EndGame()
	{
		for (int i = 0; i < fieldWidth; i++)
			for (int j = fieldHidden; j < fieldHeigh; j++)
			{
				field[i][j] = 6;
			}
		Paint();
	}
	//очистка строк
	void Clear()
	{
		int sum;
		int swap;
		int cnt[fieldHeigh];
		cnt[0] = 0;
		for (int j = fieldHeigh - 1; j > 0; j--)
		{
			sum = 0;
			for (int i = 0; i < fieldWidth; i++)
				if (field[i][j] > 0)
					sum++;
			if (sum == fieldWidth)
			{
				for (int i = 0; i < fieldWidth; i++)
					field[i][j] = 0;
				sum = 0;
			}
			cnt[j] = sum;
		}

		for (int j = fieldHeigh - 1; j > 0; j--)
		{
			if (cnt[j] == 0)
			{
				int k = j - 1;
				int index = j;
				while (k > 0)
				{
					if (cnt[k] > 0)
					{
						index = k;
						break;
					}
					k--;
				}
				if (index != j)
				{
					for (int i = 0; i < fieldWidth; i++)
					{
						swap = field[i][index];
						field[i][index] = field[i][j];
						field[i][j] = swap;
					}
					swap = cnt[j];
					cnt[j] = cnt[index];
					cnt[index] = swap;
				}
				else
					break;
			}
		}
	}
	//Инициализация SDL, окна и рендера
	bool InitSDL()
	{
		bool res = true;
		//инициализируем SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		//создаем окно (имя окна, координаты (x,y), размеры окна, флаги)
		//int request = SDL_GetDesktopDisplayMode(0, &displayMode);
		//window = SDL_CreateWindow("Hello World!", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
		window = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			res = false;
		}

		//рендер (окно, индекс драйвера (-1 - первый подходящий), флаги (аппаратное ускорение и вертикальная синхронизация)
		ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == nullptr) {
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		return res;
	}
	//дестрой SDL, окна и рендера
	void QuitSDL()
	{
		SDL_DestroyWindow(window);
		window = NULL;

		SDL_DestroyRenderer(ren);
		ren = NULL;

		SDL_Quit();
	}

	void PaintRect(int x, int y, int color)
	{
		SDL_Rect rect;
		setColorDrawShadow(color);
		rect = { fieldLeft + between + 1 + x * (cubeSize + between),fieldTop + between + 1 + (y - fieldHidden) * (cubeSize + between),
			  cubeSize, cubeSize };
		SDL_RenderFillRect(ren, &rect);
		setColorDraw(color);
		rect = { fieldLeft + between + 1 + x * (cubeSize + between),fieldTop + between + 1 + (y - fieldHidden) * (cubeSize + between),
			  cubeSize - shadow, cubeSize - shadow };
		SDL_RenderFillRect(ren, &rect);
	}

	void Paint()
	{

		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);

		//Рисуем границы поля
		SDL_Rect field1 = { fieldLeft, fieldTop,fieldWidth * (cubeSize + between) + between + 2, (fieldHeigh - fieldHidden) * (cubeSize + between) + between + 2 };
		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderFillRect(ren, &field1);
		SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderDrawRect(ren, &field1);

		//Рисуем сетку
		SDL_Rect rect;
		SDL_SetRenderDrawColor(ren, 0xAF, 0xAF, 0xAF, 0xCF);
		for (int i = 0; i <= fieldWidth; i++)
		{
			rect = { fieldLeft + 1 + i * (cubeSize + between),fieldTop + 1,
					  between, (fieldHeigh - fieldHidden) * (cubeSize + between) + between };

			SDL_RenderFillRect(ren, &rect);
		}
		for (int j = 0; j <= fieldHeigh - fieldHidden; j++)
		{
			rect = { fieldLeft + 1,fieldTop + 1 + j * (cubeSize + between),
					  fieldWidth * (cubeSize + between) + between, between };

			SDL_RenderFillRect(ren, &rect);

		}

		for (int i = 0; i < fieldWidth; i++)
			for (int j = fieldHidden; j < fieldHeigh; j++)
			{
				if (field[i][j] != 0)
				{
					PaintRect(i, j, field[i][j]);
				}
			}
		if (newFigure != nullptr)
		{
			if (newFigure->center.y >= fieldHidden)
				PaintRect(newFigure->center.x, newFigure->center.y, newFigure->color);
			for (int i = 0; i < 3; i++)
				if (newFigure->more[i].y >= fieldHidden)
					PaintRect(newFigure->more[i].x, newFigure->more[i].y, newFigure->color);
		}
		//SDL_RenderDrawPoint(ren, i, j);

		SDL_RenderPresent(ren);
	}

	void setColorDraw(int c)
	{
		switch (c)
		{
		case 0:
			SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
			break;
		case 1:
			SDL_SetRenderDrawColor(ren, 0xAF, 0x50, 0x50, 0xFF);
			break;
		case 2:
			SDL_SetRenderDrawColor(ren, 0x50, 0xAF, 0x50, 0xFF);
			break;
		case 3:
			SDL_SetRenderDrawColor(ren, 0x50, 0x50, 0xAF, 0xFF);
			break;
		case 4:
			SDL_SetRenderDrawColor(ren, 0x50, 0xAF, 0xAF, 0xFF);
			break;
		case 5:
			SDL_SetRenderDrawColor(ren, 0xAF, 0xAF, 0x50, 0xFF);
			break;
		case 6:
			SDL_SetRenderDrawColor(ren, 0xAF, 0x50, 0xAF, 0xFF);
			break;
		default:
			SDL_SetRenderDrawColor(ren, 0xAF, 0xAF, 0xAF, 0xCF);
			break;
		}
		return;
	}

	void setColorDrawShadow(int c)
	{
		switch (c)
		{
		case 0:
			SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
			break;
		case 1:
			SDL_SetRenderDrawColor(ren, 0x8F, 0x40, 0x40, 0xFF);
			break;
		case 2:
			SDL_SetRenderDrawColor(ren, 0x40, 0x8F, 0x40, 0xFF);
			break;
		case 3:
			SDL_SetRenderDrawColor(ren, 0x40, 0x40, 0x8F, 0xFF);
			break;
		case 4:
			SDL_SetRenderDrawColor(ren, 0x40, 0x8F, 0x8F, 0xFF);
			break;
		case 5:
			SDL_SetRenderDrawColor(ren, 0x8F, 0x8F, 0x40, 0xFF);
			break;
		case 6:
			SDL_SetRenderDrawColor(ren, 0x8F, 0x40, 0x8F, 0xFF);
			break;
		default:
			SDL_SetRenderDrawColor(ren, 0xAF, 0xAF, 0xAF, 0xCF);
			break;
		}
		return;
	}

	void InitField()
	{
		for (int i = 0; i < fieldWidth; i++)
			for (int j = 0; j < fieldHeigh; j++)
				field[i][j] = 0;
	}

public:
	Tetris()
	{
		for (int i = 0; i < fieldWidth; i++)
			for (int j = 0; j < fieldHeigh; j++)
				field[i][j] = 0;
		//newFigure = Lines(newFigX, newFigY);
	}

	~Tetris()
	{
		QuitSDL();
	}

	bool Initialize()
	{
		if (!InitSDL())
		{
			QuitSDL();
			return false;
		}
		else
		{
			speed = speedMin;
			InitField();//инициализация массива данных
			Paint();
		}
		return true;
	}
	void Waiting()
	{
		SDL_Event event;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					quit = true;
				if (event.type == SDL_MOUSEBUTTONDOWN)
					quit = true;
			}
		}
	}
	//вызываем функции обновления состояния точек и пререрисовки экрана, пока не закроем окно
	void GamesTetris()
	{
		SDL_Event event;
		bool quit = false;
		int delay = 0;
		int random;
		int step = startStep;
		int minY = fieldHeigh - 1;
		while (!quit)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					quit = true;
				if ((event.type == SDL_KEYDOWN) & (newFigure != nullptr))
				{
					if (event.key.keysym.sym == SDLK_SPACE)
					{
						checkFigure->Copy(*newFigure);
						checkFigure->Turn();
						if (checkRightFigure())
						{
							newFigure->Copy(*checkFigure);
						}
						else
						{
							checkFigure->Left();
							if (checkRightFigure())
							{
								newFigure->Copy(*checkFigure);
							}
							else
							{
								checkFigure->Right();
								checkFigure->Right();
								if (checkRightFigure())
								{
									newFigure->Copy(*checkFigure);
								}
							}
						}
					}
					if (event.key.keysym.sym == SDLK_LEFT)
					{
						checkFigure->Copy(*newFigure);
						checkFigure->Left();
						if (checkRightFigure())
						{
							newFigure->Copy(*checkFigure);
						}
					}
					if (event.key.keysym.sym == SDLK_RIGHT)
					{
						checkFigure->Copy(*newFigure);
						checkFigure->Right();
						if (checkRightFigure())
						{
							newFigure->Copy(*checkFigure);
						}
					}
					if (event.key.keysym.sym == SDLK_DOWN)
					{
						delay += speed;
					}
					if (event.key.keysym.sym == SDLK_LCTRL)
					{
						step = speed;
					}
				}
			}
			if (newFigure == nullptr)
			{
				random = rand() % 5;
				switch (random)
				{
				case 0:
					newFigure = new Cubes(newFigX, newFigY);
					checkFigure = new Cubes(0, 0);
					break;
				case 1:
					newFigure = new Lines(newFigX, newFigY);
					checkFigure = new Lines(0, 0);
					break;
				case 2:
					newFigure = new LeftZ(newFigX, newFigY);
					checkFigure = new LeftZ(0, 0);
					break;
				case 3:
					newFigure = new RightZ(newFigX, newFigY);
					checkFigure = new RightZ(0, 0);
					break;
				case 4:
					newFigure = new FigureT(newFigX, newFigY);
					checkFigure = new FigureT(0, 0);
					break;
				}
			}
			SDL_Delay(10);
			delay += step;
			if (delay > speed)
			{
				delay = 0;
				checkFigure->Copy(*newFigure);
				checkFigure->Lower();
				if (checkRightFigure())
				{
					newFigure->Copy(*checkFigure);
				}
				else
				{
					minY = fieldHeigh;
					field[newFigure->center.x][newFigure->center.y] = newFigure->color;
					if (newFigure->center.y < minY)
						minY = newFigure->center.y;
					for (int i = 0; i < 3; i++)
					{
						field[newFigure->more[i].x][newFigure->more[i].y] = newFigure->color;
						if (newFigure->more[i].y < minY)
							minY = newFigure->more[i].y;
					}
					if (minY < fieldHidden)
					{
						EndGame();
						delete newFigure;
						delete checkFigure;
						break;
					}
					Clear();
					step = startStep;
					delete newFigure;
					delete checkFigure;
					newFigure = nullptr;
					checkFigure = nullptr;
				}
			}
			Paint();
		}
	}
};


int main(int argc, char* args[])
{

	srand(time(0));
	Figure* g = new Lines(4, 5);
	Figure* c = new Lines(0, 0);
	c->Copy(*g);

	c->Turn();
	Tetris* tetr = new Tetris();
	if (tetr->Initialize())
		tetr->GamesTetris(); 
	tetr->Waiting();
	delete tetr;

	return 0;
}
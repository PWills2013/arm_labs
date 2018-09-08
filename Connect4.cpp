// Connect4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <list>
#include <cmath>
#include <ctime>

/********************
SPACE CLASS
********************/
class Space
{
public:
	double Nrcx = 0; //threat level of player "x" at (r,c)
	double Nrco = 0; //threat level of player "o" at (r,c)
	double Src = 0; //Scale factor for height of threat

	char status = '~'; //~ if empty; x if under player x; o if under player o.
};

//returns opponent symbol
char opponent(char symbol)
{
	if (symbol == 'x')
		return 'o';
	else
		return 'x';
}

/********************
BOARD CLASS
********************/
class Board
{
public:
	Space board[6][7];
	int colSize[7];
	Board()
	{
		for (int i = 0; i < 7; i++)
		{
			colSize[i] = 0;
		}
	}

	//move function
	void move(int col, char symbol)
	{
		//valid move test
		if (col >= 0 && col < 7)
		{
			if (colSize[col] < 6)
			{
				board[colSize[col]][col].status = symbol;
				updateThreats(colSize[col], col, symbol);
				colSize[col]++;
			}
		}
	}

	//prints board
	void print()
	{
		for (int r = 5; r >= 0; r--)
		{
			for (int c = 0; c < 7; c++)
			{
				std::cout << board[r][c].status;
				std::cout << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		//			printThreat('x');
		//			printThreat('o');
	}

	//prints board
	void printThreat(char symbol)
	{
		for (int r = 5; r >= 0; r--)
		{
			for (int c = 0; c < 7; c++)
			{
				if (symbol == 'x')
				{
					std::cout << board[r][c].Nrcx;
					std::cout << ' ';
				}
				else
				{
					std::cout << board[r][c].Nrco;
					std::cout << ' ';
				}
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	//returns difference between total threat for "x"- total threat for "o"
	double TxTo(char myPlayer)
	{
		double Tx = 0;  //Total threat of x
		double To = 0;  //Total threat of o
		for (int r = 0; r < 6; r++)
		{
			for (int c = 0; c < 7; c++)
			{
				Tx += board[r][c].Nrcx * board[r][c].Src;
				To += board[r][c].Nrco * board[r][c].Src;
			}
		}
		if (myPlayer == 'x')
			return Tx - To;  //value if player x
		else
			return To - Tx; //value if player o
	}

	//evalueates a wining position
	double evalWin(int row, int col, int dr, int dc, char symbol)
	{
		double count = 0;
		for (int i = 0; i < 4; i++)
		{
			int r = row + i*dr;
			int c = col + i*dc;

			if (!isValid(r, c))
				break;
			char cell = board[r][c].status;
			if (cell == symbol)
				count++;
			else if (cell == '~')
			{
			}
			else
			{
				count = 0;
				break;
			}
		}
		if (count == 0)
			return 0;
		else if (count == 4)
			return 1e9;
		else
			return pow(4, count);
	}


	bool isWinnable(int row, int col, int dr, int dc)
	{
		return isValid(row + 3 * dr, col + 3 * dc);
	}


	double evalLine(int row, int col, int dr, int dc, char symbol)
	{
		int r = row;
		int c = col;
		double result = 0;
		while (isWinnable(r, c, dr, dc))
		{
			result += evalWin(r, c, dr, dc, symbol);
			r += dr;
			c += dc;
		}
		return result;
	}


	double eval(char symbol)
	{
		double result = 0;
		//upward diagonal check
		result += evalLine(2, 0, 1, 1, symbol) - evalLine(2, 0, 1, 1, opponent(symbol));
		result += evalLine(1, 0, 1, 1, symbol) - evalLine(1, 0, 1, 1, opponent(symbol));
		result += evalLine(0, 0, 1, 1, symbol) - evalLine(0, 0, 1, 1, opponent(symbol));
		result += evalLine(0, 1, 1, 1, symbol) - evalLine(0, 1, 1, 1, opponent(symbol));
		result += evalLine(0, 2, 1, 1, symbol) - evalLine(0, 2, 1, 1, opponent(symbol));
		result += evalLine(0, 3, 1, 1, symbol) - evalLine(0, 3, 1, 1, opponent(symbol));
		//downward diagonal check
		result += evalLine(2, 6, 1, -1, symbol) - evalLine(2, 6, 1, -1, opponent(symbol));
		result += evalLine(1, 6, 1, -1, symbol) - evalLine(1, 6, 1, -1, opponent(symbol));
		result += evalLine(0, 6, 1, -1, symbol) - evalLine(0, 6, 1, -1, opponent(symbol));
		result += evalLine(0, 5, 1, -1, symbol) - evalLine(0, 5, 1, -1, opponent(symbol));
		result += evalLine(0, 4, 1, -1, symbol) - evalLine(0, 4, 1, -1, opponent(symbol));
		result += evalLine(0, 3, 1, -1, symbol) - evalLine(0, 3, 1, -1, opponent(symbol));
		//column check
		for (int c = 0; c < 7; c++)
			result += evalLine(0, c, 1, 0, symbol) - evalLine(0, c, 1, 0, opponent(symbol));

		//row check
		for (int r = 0; r < 6; r++)
			result += evalLine(r, 0, 0, 1, symbol) - evalLine(r, 0, 0, 1, opponent(symbol));
		return result;
	}

	bool win(char symbol)
	{
		bool result = eval(symbol) >= 1e5;
		return result;
	}
	bool potentialWin(char symbol)
	{
		bool result = eval(symbol) >= 1e4;
		return result;
	}

	//determines if (row,col) is in range
	static bool isValid(int row, int col)
	{
		return (row >= 0) && (row < 6) && (col >= 0) && (col < 7);
	}

	//determines if is valid
	bool isMoveValid(int col)
	{
		return (col >= 0) && (col < 7) && (colSize[col]) < 6;
	}

	bool updateThreat(int row, int col, char symbol)
	{
		// if empty or ours, then increment threat
		if (isValid(row, col) &&
			((board[row][col].status == '~') || (board[row][col].status == symbol)))
		{
			if (symbol == 'x')
			{
				board[row][col].Nrcx++;
				//					print();
				return true;
			}
			else
			{
				board[row][col].Nrco++;
				//					print();
				return true;
			}
		}
		//			print();
		return false;
	}

	//reduces threat of 'player' by 1
	void reduceThreat(int row, int col, char symbol)
	{
		if (isValid(row, col))
		{
			if (symbol == 'x')
			{
				board[row][col].Nrcx--;
			}
			else
			{
				board[row][col].Nrco--;
			}
		}
		//			print();
	}

	//blocks enemy threats
	void blockSemiLine(int row, int col, int dr, int dc, char symbol)
	{
		for (int i = 0; i < 4; i++)
		{
			reduceThreat(row + i * dr, col + i * dc, symbol);
		}
	}

	//update Theat Line
	void updateThreatLine(int row, int col, int dr, int dc, char symbol)
	{
		int count = 0;
		int i;
		if (updateThreat(row, col, symbol))
		{
			count++;
		}

		for (i = 1; i < 4; i++)
		{
			if (!updateThreat(row + i * dr, col + i * dc, symbol))
				break;
			else
				count++;
		}

		for (i = -1; i > -4; i--)
		{
			if (!updateThreat(row + i * dr, col + i * dc, symbol))
				break;
			else
				count++;
		}
		if (count < 4)
		{
			while (count--)
			{
				i++;
				reduceThreat(row + i * dr, col + i * dc, symbol);
			}
		}
		//if (col == 3)
		//{
		//	blockSemiLine(row, col, dr, dc, symbol);
		//	blockSemiLine(row, col, -dr, -dc, symbol);
		//}
		//if (col < 3)
		//{
		//	blockSemiLine(row, col, -dr, -dc, symbol);
		//}
		//else
		//{
		//	blockSemiLine(row, col, dr, dc, symbol);
		//}
	}

	//update Threats
	void updateThreats(int row, int col, char symbol)
	{
		updateThreatLine(row, col, 1, 0, symbol);  // vertical
		updateThreatLine(row, col, 0, 1, symbol);  // horizontal
		updateThreatLine(row, col, 1, 1, symbol);  // upward diagonal
		updateThreatLine(row, col, 1, -1, symbol); // downward diagonal
	}
};

/*******************
NODE CLASS
********************/
class Node
{
public:
	int parentAction = 3;
	int childAction = 0;
	Board boardState;
	double value = 0;
	int depth = 0;
	double alpha = -999999999999;
	double beta = 999999999999;
	Node *parent = nullptr;

	virtual Node* nextChild(char symbol) = 0;

	/*******************
	ALPHABETA SEARCH
	********************/
	static double alphaBeta(Node* node, double alpha, double beta, char symbol, int* col, char AISymbol)
	{
		//node->boardState.print();
		//std::cout << node->boardState.eval(AISymbol) << std::endl;
		double bestValue;
		if ((node->depth > 7) || node->boardState.potentialWin(symbol) || node->boardState.potentialWin(opponent(symbol)))  //depth limit
		{
			bestValue = node->boardState.eval(AISymbol);
			*col = node->parentAction;
		}
		else if (!node->depth % 2)
		{
			bestValue = alpha;

			//for all children of node - fmax.
			for (Node* c = node->nextChild(symbol); c != nullptr; c = node->nextChild(symbol))
			{
				double childValue = alphaBeta(c, bestValue, beta, symbol, col, AISymbol);
				bestValue = fmax(bestValue, childValue);
				if ((bestValue == childValue) && (c->depth == 1))
				{
					*col = c->parentAction;
				}
				delete c;
				if (beta <= bestValue)
				{
					break;
				}
			}
		}
		else
		{
			bestValue = beta;

			//for all children of node - fmin.
			for (Node* c = node->nextChild(opponent(symbol)); c != nullptr; c = node->nextChild(opponent(symbol)))
			{
				double childValue = alphaBeta(c, alpha, bestValue, opponent(symbol), col, AISymbol);
				bestValue = fmin(bestValue, childValue);
				delete c;
				if (bestValue <= alpha)
				{
					break;
				}
			}
		}
		return bestValue;
	}

	Node(Board board, double alpha, double beta, int depth, Node* parent, int parentAction)
	{
		this->boardState = board;
		this->alpha = alpha;
		this->beta = beta;
		this->depth = depth;
		this->parent = parent;
		this->parentAction = parentAction;
	}
};

class MaxNode : public Node
{
public:
	MaxNode(Board board, double alpha, double beta, int depth, Node* parent, int parentAction)
		:Node(board, alpha, beta, depth, parent, parentAction)
	{

	}

	Node* nextChild(char symbol);
};

class MinNode : public Node
{
public:
	MinNode(Board board, double alpha, double beta, int depth, Node* parent, int parentAction)
		:Node(board, alpha, beta, depth, parent, parentAction)
	{

	}
	Node* nextChild(char symbol);
};

Node* MaxNode::nextChild(char symbol)
{
	if (boardState.isMoveValid(childAction))
	{
		Board childState = this->boardState;
		childState.move(childAction, symbol);
		return new MinNode(childState, alpha, beta, depth + 1, this, childAction++);
	}
	else
	{
		return nullptr;
	}
}

Node* MinNode::nextChild(char symbol)
{
	if (boardState.isMoveValid(childAction))
	{
		Board childState = this->boardState;
		childState.move(childAction, symbol);
		return new MinNode(childState, alpha, beta, depth + 1, this, childAction++);
	}
	else
	{
		return nullptr;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	/*******************
	Player Base Class
	********************/
	class Player
	{
	public:
		char symbol;
		virtual int chooseColumn(Board board) = 0;
		Player(char symbol)
		{
			this->symbol = symbol;
		}
	};

	/*******************
	AI/Computer Player Class
	********************/
	class Computer : public Player
	{
	public:
		Computer(char symbol)
			: Player(symbol)
		{};


		int chooseColumn(Board board)
		{
			int choice = 1;
			time_t startTime = clock();
			Node* root = new MaxNode(board, -1e9, 1e9, 0, nullptr, 0);
			Node::alphaBeta(root, -1e9, 1e9, this->symbol, &choice, this->symbol);
			std::cout << "I moved column: " << choice << " in " << (clock()-startTime)/1000 << " seconds." << std::endl;
			return choice;
			delete root;
		}
	};

	/*******************
	Human Player Class
	********************/
	class Human : public Player
	{
	public:
		Human(char symbol)
			: Player(symbol)
		{};

		int chooseColumn(Board board)
		{
			int choice = 1;
			std::cout << "\nEnter the column number you would like to move(0-6): ";
			std::cin >> choice;
			while (!board.isMoveValid(choice))
			{
				std::cout << "\ninvalid choice!  Enter a number from 0 to 6. ";
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cin >> choice;
			}

			std::cout << "Human moved column: " << choice << std::endl;
			return choice;
		}
	};

	/*******************
	MAIN
	********************/
	Board officialBoard;
	//	officialBoard.move(2, 'x');
	//	officialBoard.move(3, 'o');
	//	officialBoard.move(3, 'x');
	Board temp;
	temp.move(0, 'x');
	temp.move(3, 'x');
	temp.move(2, 'x');
	temp.print();
	std::cout << temp.eval('x') << std::endl;

	std::cout << "Shall I go first? (select y if \"yes\") " << std::endl;
	std::string answer;
	std::cin >> answer;
	Player *player1;
	Player *player2;
	if (answer[0] == 'y')
	{
		player1 = new Computer('x');
		player2 = new Human('o');
	}
	else
	{
		player1 = new Human('x');
		player2 = new Computer('o');
	}
	int moves = 21;
	officialBoard.print();
	while (moves > 0)
	{
		officialBoard.move(player1->chooseColumn(officialBoard), player1->symbol);
		officialBoard.print();
		if (officialBoard.win(player1->symbol))
		{
			std::cout << player1->symbol << " wins!!!" << std::endl;
			break;
		}
		officialBoard.move(player2->chooseColumn(officialBoard), player2->symbol);
		officialBoard.print();
		if (officialBoard.win(player2->symbol))
		{
			std::cout << player2->symbol << " wins!!!" << std::endl;
			break;
		}
		moves--;
	}
	if (moves <= 0)
		std::cout << "Draw!!" << std::endl;

	system("pause");
	return 0;
}

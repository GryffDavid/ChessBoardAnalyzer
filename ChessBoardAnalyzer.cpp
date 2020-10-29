#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//Only did this as an experiment. Turns out it works.
#define in :

struct Vector2
{
	int x, y;
};

struct Piece
{
	Vector2 position;
	char pieceType;
};

char boardArray[8][8];

//Returns true if there is a piece blocking the attack
bool CheckPerpendicular(Vector2 kingPos, Vector2 piecePos)
{
	//Check all positions along the vector. If one of those positions isn't '.', return true
	Vector2 dir = { kingPos.x - piecePos.x, kingPos.y - piecePos.y };

	//Shared X axis: Check along the Y for a block
	if (dir.x == 0)
	{
		int dirY = dir.y / abs(dir.y);

		for (int y = piecePos.y + dirY; y != kingPos.y; y += dirY)
		{
			if (boardArray[y][piecePos.x] != '.')
			{
				if (toupper(boardArray[y][piecePos.x]) == 'K')
				{
					return false;
				}

				return true;
			}
		}

		return false;
	}

	//Shared Y axis: Check along the X axis for a block
	if (dir.y == 0)
	{
		int dirX = dir.x / abs(dir.x);

		for (int x = piecePos.x + dirX; x != kingPos.x; x += dirX)
		{
			if (boardArray[piecePos.y][x] != '.')
			{
				if (toupper(boardArray[piecePos.y][x]) == 'K')
				{
					return false;
				}

				return true;
			}
		}

		return false;
	}

	return false;
}

//Returns true if there is a piece blocking a diagonal capture
bool CheckDiagonal(Vector2 kingPos, Vector2 piecePos)
{
	Vector2 dir = { kingPos.x - piecePos.x, kingPos.y - piecePos.y };
	Vector2 normalDir = { dir.x / abs(dir.x), dir.y / abs(dir.y) };

	if (abs(normalDir.x) == abs(normalDir.y))
	{
		int y = piecePos.y + normalDir.y;

		for (int x = piecePos.x + normalDir.x; x != kingPos.x; x += normalDir.x)
		{
			if (boardArray[y][x] != '.')
			{
				if (toupper(boardArray[y][x]) == 'K')
				{
					return false;
				}

				return true;
			}

			y += normalDir.y;
		}

		return false;
	}

	return true;
}

bool CheckMoves(vector<Vector2>& movelist, vector<Piece>& pieceList)
{
	for (Piece& piece in pieceList)
	{
		for (int moveIndex = 0; moveIndex < movelist.size(); moveIndex++)
		{
			bool DeadMove = false;
			Vector2 move = movelist[moveIndex];

			//King took the piece
			if ((piece.position.x == move.x) && (piece.position.y == move.y)) continue;

			switch (toupper(piece.pieceType))
			{
			case 'R': //ROOK
			{
				if ((move.x == piece.position.x) || (move.y == piece.position.y))
				{
					if (CheckPerpendicular(move, piece.position) == false)
						DeadMove = true;
				}
			}
			break;

			case 'B': //BISHOP
			{
				Vector2 myDir = { abs(piece.position.x - move.x), abs(piece.position.y - move.y) };

				if (myDir.x == myDir.y)
				{
					if (CheckDiagonal(move, piece.position) == false)
					{
						DeadMove = true;
					}
				}
			}
			break;

			case 'P': //PAWN
			{
				//Had to check case here because black pawns move down and white pawns move up
				if (isupper(piece.pieceType))
				{
					if ((move.x == piece.position.x + 1 && move.y == piece.position.y - 1) ||
						(move.x == piece.position.x - 1 && move.y == piece.position.y - 1))
						DeadMove = true;
				}
				else
				{
					if ((move.x == piece.position.x + 1 && move.y == piece.position.y - 1) ||
						(move.x == piece.position.x + 1 && move.y == piece.position.y + 1))
						DeadMove = true;
				}
			}
			break;

			case 'K': //KING
			{
				Vector2 myDir = { abs(piece.position.x - move.x), abs(piece.position.y - move.y) };

				if ((myDir.x == 1 && myDir.y == 1) || (myDir.x == 0 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 0))
				{
					DeadMove = true;
				}
			}
			break;

			case 'Q': //QUEEN
			{
				Vector2 myDir = { abs(piece.position.x - move.x), abs(piece.position.y - move.y) };

				if (myDir.x == myDir.y)
				{
					if (CheckDiagonal(move, piece.position) == false)
					{
						DeadMove = true;
					}
				}

				if ((move.x == piece.position.x) || (move.y == piece.position.y))
				{
					if (CheckPerpendicular(move, piece.position) == false)
						DeadMove = true;
				}
			}
			break;

			case 'N': //KNIGHT
			{
				Vector2 myDir = { abs(piece.position.x - move.x), abs(piece.position.y - move.y) };

				if ((myDir.x == 2 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 2))
				{
					DeadMove = true;
				}
			}
			break;
			}

			if (DeadMove == true)
				movelist[moveIndex] = { -1, -1 };
		}
	}

	return false;
}

int main()
{
	vector<Piece> whitePieces;
	vector<Piece> blackPieces;

	Vector2 whiteKingPos = { 0, 0 };
	Vector2 blackKingPos = { 0, 0 };

	int WhiteMoveCount = 0;
	int BlackMoveCount = 0;

	vector<Vector2> whiteMovesList;
	vector<Vector2> blackMovesList;

	//Load board into array and get piece and king locations
	for (int i = 0; i < 8; i++)
	{
		string boardRow;
		getline(cin, boardRow);

		for (int p = 0; p < 8; p++)
		{
			boardArray[i][p] = boardRow[p];

			if (boardRow[p] != '.')
			{
				if (isupper(boardRow[p]))
					whitePieces.push_back(Piece{ Vector2{p, i}, boardRow[p] });

				if (!isupper(boardRow[p]))
					blackPieces.push_back(Piece{ Vector2{p, i}, boardRow[p] });

				if (boardRow[p] == 'K')
					whiteKingPos = { p, i };

				if (boardRow[p] == 'k')
					blackKingPos = { p, i };
			}
		}
	}

	//Gets open moves for the positions around the kings. Checks for open spaces or an opposing piece to take.
	//Kings can only move 1 space, but in any direction. Loop through all the spaces around the king for potential moves
	//Ignore any moves that aren't on the board or are obstructed by a piece on the same team
	for (int kingXMove = -1; kingXMove < 2; kingXMove++)
	{
		for (int kingYMove = -1; kingYMove < 2; kingYMove++)
		{
			Vector2 whiteMove = { whiteKingPos.x + kingXMove, whiteKingPos.y + kingYMove };
			Vector2 blackMove = { blackKingPos.x + kingXMove, blackKingPos.y + kingYMove };

			if (whiteMove.x >= 0 && whiteMove.x <= 7 &&
				whiteMove.y >= 0 && whiteMove.y <= 7 &&
				(boardArray[whiteMove.y][whiteMove.x] == '.' || islower(boardArray[whiteMove.y][whiteMove.x])))
			{
				whiteMovesList.push_back(whiteMove);
			}

			if (blackMove.x >= 0 && blackMove.x <= 7 &&
				blackMove.y >= 0 && blackMove.y <= 7 &&
				(boardArray[blackMove.y][blackMove.x] == '.' || isupper(boardArray[blackMove.y][blackMove.x])))
			{
				blackMovesList.push_back(blackMove);
			}
		}
	}

	blackMovesList.push_back({ blackKingPos.x, blackKingPos.y });
	whiteMovesList.push_back({ whiteKingPos.x, whiteKingPos.y });

	CheckMoves(blackMovesList, whitePieces);
	CheckMoves(whiteMovesList, blackPieces);

	//If a move isn't marked as -1, -1, it's a valid move
	for (Vector2 move in whiteMovesList)
	{
		if (move.x != -1 && move.y != -1)
			WhiteMoveCount++;
	}

	for (Vector2 move in blackMovesList)
	{
		if (move.x != -1 && move.y != -1)
			BlackMoveCount++;
	}

	if (WhiteMoveCount == 0)
		cout << "B" << endl;
	else if (BlackMoveCount == 0)
		cout << "W" << endl;
	else
		cout << "N" << endl;
}
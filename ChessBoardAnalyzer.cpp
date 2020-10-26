#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//Only did this as an experiment
#define in : 

//Previous version worked for all test cases, but did not account for attack moves being blocked diagonally.
//Still needs to be cleaned up a bit and commented, but the code works.

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

bool CheckPerpendicular(Vector2 kingPos, Vector2 piecePos)
{
    //Returns true if there is a piece blocking the attack
    //Check all positions along the axis. If one of those positions isn't '.', return true
    Vector2 dir = {kingPos.x - piecePos.x, kingPos.y - piecePos.y};

    if (dir.x == 0)
    {
        int dirY = dir.y/abs(dir.y);

        for (int y = piecePos.y + dirY; y != kingPos.y; y += dirY)
        {            
            if (boardArray[y][piecePos.x] != '.')
            {
                if (toupper(boardArray[y][piecePos.x]) == 'K') 
                { return false; }
                
                return true;
            }
        }

        return false;
    }

    if (dir.y == 0)    
    {
        int dirX = dir.x/abs(dir.x);

        for (int x = piecePos.x + dirX; x != kingPos.x; x += dirX)
        {            
            if (boardArray[piecePos.y][x] != '.')
            {
                if (toupper(boardArray[piecePos.y][x]) == 'K') 
                { return false; }

                return true;
            }
        }

        return false;
    }

    return false;
}

bool CheckDiagonal(Vector2 kingPos, Vector2 piecePos)
{
    //Returns true if there is a piece blocking a diagonal capture
    Vector2 dir = {kingPos.x - piecePos.x, kingPos.y - piecePos.y};
    Vector2 normalDir = { dir.x/abs(dir.x), dir.y/abs(dir.y) };

    if (abs(normalDir.x) == abs(normalDir.y))
    {
        int y = piecePos.y + normalDir.y;

        for (int x = piecePos.x + normalDir.x; x != kingPos.x; x += normalDir.x)
        {
            if (boardArray[y][x] != '.')
            {
                if (toupper(boardArray[y][x]) == 'K')
                { return false; }  

                return true;
            }

            y += normalDir.y;
        }

        return false;
    }

    return true;
}

int main()
{
    vector<Piece> whitePieces;
    vector<Piece> blackPieces;

    Vector2 whiteKingPos = {0, 0};
    Vector2 blackKingPos = {0, 0};    

    bool isBlackMate = false; 
    bool isWhiteMate = false;

    int WhiteMoveCount = 0;
    int BlackMoveCount = 0;
    
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
                { whitePieces.push_back(Piece{Vector2{p, i}, boardRow[p]}); }

                if (!isupper(boardRow[p]))
                { blackPieces.push_back(Piece{Vector2{p, i}, boardRow[p]}); }

                if (boardRow[p] == 'K')
                    whiteKingPos = {p, i};
                
                if (boardRow[p] == 'k')
                    blackKingPos = {p, i};
            }
        }
    }
    
    vector<Vector2> whiteMovesList;
    vector<Vector2> blackMovesList;

    //Gets open moves for the kings. Checks for open spaces or an opposing piece to take.
    for (int kingXMove = -1; kingXMove < 2; kingXMove++)
    {
        for (int kingYMove = -1; kingYMove < 2; kingYMove++)
        {
            Vector2 whiteMove = {whiteKingPos.x + kingXMove, whiteKingPos.y + kingYMove};
            Vector2 blackMove = {blackKingPos.x + kingXMove, blackKingPos.y + kingYMove};
            
            if (whiteMove.x >= 0 && 
                whiteMove.x <= 7 &&
                whiteMove.y >= 0 && 
                whiteMove.y <= 7 &&
                (boardArray[whiteMove.y][whiteMove.x] == '.' || islower(boardArray[whiteMove.y][whiteMove.x])))
            {
                whiteMovesList.push_back(whiteMove);
            }

            if (blackMove.x >= 0 && 
                blackMove.x <= 7 &&
                blackMove.y >= 0 && 
                blackMove.y <= 7 &&
                (boardArray[blackMove.y][blackMove.x] == '.' || isupper(boardArray[blackMove.y][blackMove.x])))
            {
                blackMovesList.push_back(blackMove);
            }
        }
    }

    blackMovesList.push_back({blackKingPos.x, blackKingPos.y});
    whiteMovesList.push_back({whiteKingPos.x, whiteKingPos.y});

    //Check whether moves are covered by opposing pieces
    for (Piece& piece in whitePieces)
    {
        for (int moveIndex = 0; moveIndex < blackMovesList.size(); moveIndex++)
        {
            bool DeadMove = false;
            Vector2 move = blackMovesList[moveIndex];
            //King took the piece
            if ((piece.position.x == move.x) && (piece.position.y == move.y)) continue;

            switch(toupper(piece.pieceType))
            {
                case 'R': //WHITE ROOK
                {
                    if ((move.x == piece.position.x) || (move.y == piece.position.y))                    
                    {
                        if (CheckPerpendicular(move, piece.position) == false)
                            DeadMove = true;
                    }
                }
                break;

                case 'B': //WHITE BISHOP
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};
                    
                    if (myDir.x == myDir.y)
                    {
                        if (CheckDiagonal(move, piece.position) == false)
                        { DeadMove = true; }
                    }
                }
                break;

                case 'P': //WHITE PAWN
                {
                    if ((move.x == piece.position.x + 1 && move.y == piece.position.y - 1) ||
                        (move.x == piece.position.x - 1 && move.y == piece.position.y - 1))
                        { DeadMove = true; }
                }
                break;

                case 'K': //WHITE KING
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if ((myDir.x == 1 && myDir.y == 1) || (myDir.x == 0 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 0))
                    { DeadMove = true; }
                }
                break;   

                case 'Q': //WHITE QUEEN
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if (myDir.x == myDir.y)
                    {
                        if (CheckDiagonal(move, piece.position) == false)
                        { DeadMove = true; }
                    }

                    if ((move.x == piece.position.x) || (move.y == piece.position.y))                    
                    {
                        if (CheckPerpendicular(move, piece.position) == false)
                            DeadMove = true;
                    }
                }
                break;   

                case 'N': //WHITE KNIGHT
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if ((myDir.x == 2 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 2))
                    { DeadMove = true; }
                }
                break;          
            }

            if (DeadMove == true)
                blackMovesList[moveIndex] = {-1, -1};
        }
    }
    for (Piece& piece in blackPieces)
    {
        for (int moveIndex = 0; moveIndex < whiteMovesList.size(); moveIndex++)
        {
            bool DeadMove = false;
            Vector2 move = whiteMovesList[moveIndex];

            //King took the piece
            if ((piece.position.x == move.x) && (piece.position.y == move.y)) continue;

            switch(toupper(piece.pieceType))
            {
                case 'R': //BLACK ROOK
                {
                    if ((move.x == piece.position.x) || (move.y == piece.position.y))                    
                    {
                        if (CheckPerpendicular(move, piece.position) == false)
                            DeadMove = true;
                    }
                }
                break;

                case 'B': //BLACK BISHOP
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};
                    
                    if (myDir.x == myDir.y)
                    {
                        if (CheckDiagonal(move, piece.position) == false)
                            DeadMove = true; 
                    }
                }
                break;

                case 'P': //BLACK PAWN
                {
                    if ((move.x == piece.position.x + 1 && move.y == piece.position.y + 1) ||
                        (move.x == piece.position.x - 1 && move.y == piece.position.y + 1))
                        { DeadMove = true; }
                }
                break;

                case 'K': //BLACK KING
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if ((myDir.x == 1 && myDir.y == 1) || (myDir.x == 0 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 0))
                    { DeadMove = true; }
                }
                break;   

                case 'Q': //BLACK QUEEN
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if (myDir.x == myDir.y)
                    {
                        if (CheckDiagonal(move, piece.position) == false)
                            DeadMove = true;
                    }

                    if ((move.x == piece.position.x) || (move.y == piece.position.y))                    
                    {
                        if (CheckPerpendicular(move, piece.position) == false)
                            DeadMove = true;
                    }
                }
                break;   

                case 'N': //BLACK KNIGHT
                {
                    Vector2 myDir = {abs(piece.position.x - move.x), abs(piece.position.y - move.y)};

                    if ((myDir.x == 2 && myDir.y == 1) || (myDir.x == 1 && myDir.y == 2))
                    { DeadMove = true; }
                }
                break;          
            }

            if (DeadMove == true)
                whiteMovesList[moveIndex] = {-1, -1};
        }        
    }

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

    if (BlackMoveCount == 0)    
        isBlackMate = true;

    if (WhiteMoveCount == 0)    
        isWhiteMate = true;    

    if (WhiteMoveCount > 0 && BlackMoveCount > 0)
    {
        isBlackMate = false;
        isWhiteMate = false;
    }
    
    if (isWhiteMate == true)    
        cout << "B" << endl;    
    else if (isBlackMate == true)    
        cout << "W" << endl;    
    else    
        cout << "N" << endl;    
}
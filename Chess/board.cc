#include "board.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "move.h"
#include "piece.h"
#include "king.h"
#include "queen.h"
#include "pawn.h"
#include "knight.h"
#include "rook.h"
#include "bishop.h"
using namespace std;

class TextDisplay;
class XWindow;


void Board::insert (Pos pos, char letter) {
    int r = pos.row;
    int c = pos.col;
    if (letter == 'R') { pieces[r][c] = new Rook(White,pos); }
    else if (letter == 'N') { pieces[r][c] = new Knight(White,pos); }
    else if (letter == 'B') { pieces[r][c] = new Bishop(White,pos); }
    else if (letter == 'Q') { pieces[r][c] = new Queen(White,pos); }
    else if (letter == 'K') { pieces[r][c] = new King(White,pos); }
    else if (letter == 'P') { pieces[r][c] = new Pawn(White,pos); }
    else if (letter == 'r') { pieces[r][c] = new Rook(Black,pos); }
    else if (letter == 'n') { pieces[r][c] = new Knight(Black,pos); }
    else if (letter == 'b') { pieces[r][c] = new Bishop(Black,pos); }
    else if (letter == 'q') { pieces[r][c] = new Queen(Black,pos); }
    else if (letter == 'k') { pieces[r][c] = new King(Black,pos); }
    else { pieces[r][c] = new Pawn(Black,pos); } //letter is 'p'
}
void Board::remove (Pos pos) {
    pieces[pos.row][pos.col] = NULL;
}
vector<vector<Piece*>> Board::getPieces() const { return pieces; }

Board::Board() { //sets up new 8x8 board
    for(int i=0; i<8; ++i) {
        vector<Piece*> P;
        for (int j=0; j<8; ++j) {
            P.emplace_back(NULL);
        }
        pieces.emplace_back(P);
    }
}


Board::~Board() {}


ostream& operator<<(ostream &os, const Board &b) {
    //store column and row indices
    std::vector<std::vector<char>> theDisplay;
    for(int i=0; i<10; ++i) {
        vector<char> V;
        for (int j=0; j<10; ++j) {
            V.emplace_back(' ');
        }
        theDisplay.emplace_back(V);
    }
    //add column characters
    theDisplay[0][0] = '8';
    theDisplay[1][0] = '7';
    theDisplay[2][0] = '6';
    theDisplay[3][0] = '5';
    theDisplay[4][0] = '4';
    theDisplay[5][0] = '3';
    theDisplay[6][0] = '2';
    theDisplay[7][0] = '1';
    //add row characters
    theDisplay[9][2] = 'a';
    theDisplay[9][3] = 'b';
    theDisplay[9][4] = 'c';
    theDisplay[9][5] = 'd';
    theDisplay[9][6] = 'e';
    theDisplay[9][7] = 'f';
    theDisplay[9][8] = 'g';
    theDisplay[9][9] = 'h';
    //add tile colours
    for(int i=0; i<8; ++i) {
        for(int j=2; j<10; ++j) {
            if ((i+j)%2 != 0) { //if black tiles
                theDisplay[i][j] = '_';
            }
        }
    }
    for(int i=0; i<10; ++i) {
        for(int j=0; j<10; ++j) {
            if((i>=2 && i<10) && (j>=0 && j<8)) {
                if (b.getPieces()[i][j] != NULL) { //if piece position is empty
                    os << b.getPieces()[i][j]; //overload piece operator
                } else {
                    os << theDisplay[i][j];
                }
            } else { //out of 8x8 bound, we will print alphabet and row indeces
                os << theDisplay[i][j];
            }
            os << theDisplay[i][j];
        }
        os << endl;
    }
    os << endl;
    return os;
}

void Board::makeTheMove(Piece* moved, Piece* target, char prm){
	const Pos mPos = moved->getPos(); //moving from mPos
	const Pos tPos = target->getPos(); //moving to tPos
	bool RegularMove = true;

	//Promotion Condition
	if(moved->isPawn() == true) {
		Color c;
		if(tPos.row == 7) {
			c = Black;
		} else if(tPos.row == 0) {
			c = Black;
		}

		if(prm == 'R') {
			Piece* promotionPiece = new Rook(c, {tPos.row, tPos.col});
		} else if (prm == 'B') {
			Piece* promotionPiece = new Bishop(c, {tPos.row, tPos.col});
		} else {
			Piece* promotionPiece = new Queen(c, {tPos.row, tPos.col});
		}
		target = promotionPiece;
		moved = nullptr;
		Move *m = new Move(mPos, tPos, nullptr, "Promotion");
		moves->emplace_back(m);
	}
		
		

	//possible enPassant capture:
	if (((mPos.row == 3 && tPos.row == 2) || (mPos.row == 4 && tPos.row == 5)) &&
		(abs(mPos.col - tPos.col) == 1) && 
		(target == nullptr) &&
		(moved->isPawn() == true)) {
		RegularMove = false;
		Move *m = new Move(mPos, tPos, pieces[mPos.row][tPos.col], "enPassantCapture");
		moves->emplace_back(m);
   		target = moved; //capturing pawn moves to target
   		target->UpdatePos(tPos); //Update Position of the piece
   		moved = nullptr; //old position of the capturing pawn
   		pieces[mPos.row][tPos.col] = nullptr; //position of the captured pawn
   	}


   	if ((moves.empty() == false) && (moves.back()->specialMove == "enPassantCapture")) {
		//Setting enPassant field of the pieces to false after enPassantCapture
		//We don't set enPassant field of captured piece to false
   		pieces[moves.back()->newPos.row][moves.back()->newPos.col]->setPassant(false);
   		if ((moves.back()->newPos.col + 1 == moves.back()->oldPos.col) &&
   			(moves.back()->oldPos.col - 2 >= 0)) {
   			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col - 2]->setPassant(false);
   		}
   		if ((moves.back()->newPos.col - 1 == moves.back()->oldPos.col) &&
   		(moves.back()->oldPos.col + 2 <= 7)) {
   		pieces[moves.back()->oldPos.row][moves.back()->oldPos.col + 2]->setPassant(false);
  		}
	}

	if ((moves.empty() == false) && (moves.back()->specialMove == "enPassantSetup")) {
    	//if the next move is not enPassant we set all enPassant fields to false before
    	//the other conditions
	pieces[moves.back()->newPos.row][moves.back()->newPos.col]->setPassant(false);
	pieces[moves.back()->newPos.row][moves.back()->newPos.col + 1]->setPassant(false);
	pieces[moves.back()->newPos.row][moves.back()->newPos.col - 1]->setPassant(false);
	}


	if ((moved->isPawn() == true) && //possible enPassant setup (2 forward move)
		((mPos.row == 1 && tPos.row == 3) || (mPos.row == 6 && tPos.row == 4))) {
		RegularMove = false;
	    Move *m = new Move(mPos, tPos, nullptr, "enPassantSetup");
	    moves->emplace_back(m);
	    moved->setPassant(true);
		target = moved; //2 forward move
		target->UpdatePos(tPos);
		moved = nullptr; 
		pieces[tPos.row][tPos.col - 1]->setPassant(true);
		pieces[tPos.row][tPos.col + 1]->setPassant(true);
	}


	if ((moved->getMoved() == false) &&   //short-castling condition for both colors
		((mPos.row == 0) || (mPos.row == 7)) && (mPos.col = 4) &&
		(tPos.row == 6)) {
		RegularMove = false;
	    Move *m = new Move(mPos, tPos, nullptr, "Castling");
	    moves->emplace_back(m);
	    target = moved;
	    target->UpdatePos(tPos);
	    moved = nullptr;
	    pieces[mPos.row][5] = pieces[mPos.row][7]; //moves the castle to the new loc
	    pieces[mPos.row][5]->UpdatePos({mPos.row, 5})
	    pieces[mPos.row][7] = nullptr;
	    pieces[mPos.row][5]->setMoved(true);
	}

	if ((moved->getMoved() == false) &&   //long-castling condition for both colors
		((mPos.row == 0) || (mPos.row == 7)) && (mPos.col = 4) &&
		(tPos.row == 2)) {
		RegularMove = false;
	    Move *m = new Move(mPos, tPos, nullptr, "Castling");
	    moves->emplace_back(m);
	    target = moved;
	    target->UpdatePos(tPos);
	     moved = nullptr;
	    pieces[mPos.row][3] = pieces[mPos.row][0]; //moves the castle to the new loc
	    pieces[mPos.row][3]->UpdatePos({mPos.row, 3})
	    pieces[mPos.row][0] = nullptr; 
	    pieces[mPos.row][3]->setMoved(true);
	}

	if(RegularMove == true) {
		if(pieces[mPos.row][mPos.col]->getMoved() == false) {
			Move* m = new Move(mPos, tPos, target, "FirstMove");
			moves.emplace_back(m); //info about move is pushed to moved vec in board
			target = moved; //target cell points to moved piece
			target->UpdatePos(tPos);
			moved = nullptr; //freeing old cell
		} else {
			Move* m = new Move(mPos, tPos, target, "Regular");
			moves.emplace_back(m); //info about move is pushed to moved vec in board
			target = moved; //target cell points to moved piece
			target->UpdatePos(tPos);
			moved = nullptr; //freeing old cell
		}
	}
}


void Board::undo() {
	if(moves.empty() == false) {
		if(moves.back()->specialMove == "enPassantCapture") {
			pieces[moves.back()->oldPos.row][moves.back()->newPos.col] = moves.back()->captured; //Put the captured piece back
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[newPos.row][newPos.col]; //Put the attacker pawn back
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
			pieces[moves.back()->newPos.row][moves.back()->newPos.col] = nullptr;
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setPassant(true);
			if ((moves.back()->newPos.col + 1 == moves.back()->oldPos.col) &&
   				(moves.back()->oldPos.col - 2 >= 0)) {
   				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col - 2]->setPassant(true);
   			}
   			if ((moves.back()->newPos.col - 1 == moves.back()->oldPos.col) &&
   				(moves.back()->oldPos.col + 2 <= 7)) {
   				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col + 2]->setPassant(true);
  			}
  			moves.pop_back();
		}
		if(moves.back()->specialMove == "enPassantSetup") {
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[moves.back()->newPos.row][moves.back()->newPos.col]; //Put the 2 forward pawn back
			pieces[moves.back()->newPos.row][moves.back()->newPos.col] = nullptr;
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setPassant(false);
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setMoved(false);
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
			pieces[moves.back()->newPos.row][moves.back()->newPos.col + 1]->setPassant(false);
			pieces[moves.back()->newPos.row][moves.back()->newPos.col - 1]->setPassant(false);
  			moves.pop_back();
		}
		if(moves.back()->specialMove == "Castling") {
			if(moves.back()->newPos.col == 2) { //long castling
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[newPos.row][newPos.col]; //Put king back
				pieces[moves.back()->newPos.row][moves.back()->newPos.col] = nullptr;
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setMoved(false);
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
				pieces[moves.back()->newPos.row][0] = pieces[newPos.row][3];
				pieces[moves.back()->newPos.row][3] = nullptr;
				pieces[moves.back()->newPos.row][0]->setMoved(false);
				pieces[moves.back()->oldPos.row][0]->UpdatePos({moves.back()->oldPos.row, 0});
				moves.pop_back();
			} else if (oves.back()->newPos.col == 6) { //short castling
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[moves.back()->newPos.row][moves.back()->newPos.col]; //Put king back
				pieces[moves.back()->newPos.row][moves.back()->newPos.col] = nullptr;
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setMoved(false);
				pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
				pieces[moves.back()->newPos.row][7] = pieces[newPos.row][5];
				pieces[moves.back()->newPos.row][5] = nullptr;
				pieces[moves.back()->newPos.row][7]->setMoved(false);
				pieces[moves.back()->oldPos.row][7]->UpdatePos({moves.back()->oldPos.row, 7});
				moves.pop_back();
			}
		}
		if(moves.back()->specialMove == "Promotion") {
			Color c;
			if(moves.back()->newPos.row == 7) {
				c = Black;
			} else {
				c = White;
			}
			pieces[moves.back()->newPos.row][moves.back()->newPos.col] = nullptr;
			Piece* promotionPiece = new Pawn(c, moves.back()->oldPos);
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = promotionPiece;
			moves.pop_back();
		}

		if(moves.back()->specialMove == "Regular") {
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[newPos.row][newPos.col];
			pieces[moves.back()->newPos.row][moves.back()->newPos.col] = moves.back()->captured;
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
			moves.pop_back();
		}

		if(moves.back()->specialMove == "FirstMove") {
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col] = pieces[newPos.row][newPos.col];
			pieces[moves.back()->newPos.row][moves.back()->newPos.col] = moves.back()->captured;
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->UpdatePos(moves.back()->oldPos);
			pieces[moves.back()->oldPos.row][moves.back()->oldPos.col]->setMoved(false);
			moves.pop_back();
		}
	}
}

bool Board::outOfRange(const Pos p) {
    if (p.row > 7 || p.col > 7) return true;
    else if (p.row < 0 || p.col < 0) return true;
    
    return false;
}

void Board::undo() {}

bool Board::isAttacked(Pos cellPos) {
    Piece* piece = pieces[cellPos.row][cellPos.col];
    for(auto &row:pieces) {
        for(auto &p:row) {
            //skipping owned pieces and empty cells:
            if(p == nullptr) continue;
            if(piece->getColor() == p->getColor()) continue;
            //checking if p can move to curPos
            if (p->IsLegal(cellPos, pieces)){
                return true;
            }
        }
    }
    return false; //has checked all enemy pieces and none of them attacks player's king
}



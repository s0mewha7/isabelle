#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "masks.h"
#include "move.h"
#include "movegen.h"
#include "types.h"

void addPawnMoves(std::vector<uint16_t>& moves, uint64_t bb, const int shift) {
	while (bb) {
		int sqr = popBit(bb);
		bool isPromotion = (1ull << sqr & (rank1Mask | rank8Mask));
		if (isPromotion) {
			moves.push_back(createMove(sqr - shift, sqr, PROMOTION_QUEEN));
			moves.push_back(createMove(sqr - shift, sqr, PROMOTION_ROOK));
			moves.push_back(createMove(sqr - shift, sqr, PROMOTION_BISHOP));
			moves.push_back(createMove(sqr - shift, sqr, PROMOTION_KNIGHT));
		}
		else {
			moves.push_back(createMove(sqr - shift, sqr, NORMAL_MOVE));
		}
	}
}

void addPieceMoves(std::vector<uint16_t>& moves, uint64_t bb, const int from) {
	while (bb) {
		moves.push_back(createMove(from, popBit(bb), NORMAL_MOVE));
	}
}

void genPawnMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	const int forward = (b.turn == WHITE) ? N : S;
	uint64_t pawns = b.pieces[PAWN] & b.colors[b.turn];

	if (!noisyOnly) {
		uint64_t pawnPushes = ((pawns << 8) >> (b.turn << 4)) & b.colors[NO_COLOR];
		uint64_t pawnDoublePushes = ((pawnPushes << 8) >> (b.turn << 4)) & ((b.turn == WHITE) ? rank4Mask : rank5Mask) & b.colors[NO_COLOR];
		addPawnMoves(moves, pawnPushes, forward);
		addPawnMoves(moves, pawnDoublePushes, forward * 2);
	}

	const int forwardLeft = (b.turn == WHITE) ? NW : SE;
	const int forwardRight = (b.turn == WHITE) ? NE : SW;
	const uint64_t targets = b.colors[!b.turn] & ~b.pieces[KING];
	uint64_t pawnLeftCaptures = targets & ((b.turn == WHITE) ? (pawns << 7) & ~fileHMask : (pawns >> 7) & ~fileAMask);
	uint64_t pawnRightCaptures = targets & ((b.turn == WHITE) ? (pawns << 9) & ~fileAMask : (pawns >> 9) & ~fileHMask);
	addPawnMoves(moves, pawnLeftCaptures, forwardLeft);
	addPawnMoves(moves, pawnRightCaptures, forwardRight);

	uint64_t ep = (b.epSquare == -1) ? 0ull : pawnAttacks[b.epSquare][!b.turn] & pawns;
	while (ep) {
		moves.push_back(createMove(popBit(ep), b.epSquare, EP_MOVE));
	}
}

void genKnightMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	uint64_t knights = b.pieces[KNIGHT] & b.colors[b.turn];
	while (knights) {
		int sqr = popBit(knights);
		uint64_t bb = knightAttacks[sqr] & ~(b.colors[b.turn] | b.pieces[KING]);
		if (noisyOnly) bb &= b.colors[!b.turn];
		addPieceMoves(moves, bb, sqr);
	}
}

void genBishopMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	uint64_t bishops = b.pieces[BISHOP] & b.colors[b.turn];
	const uint64_t occ = ~b.colors[NO_COLOR];
	while (bishops) {
		int sqr = popBit(bishops);
		uint64_t bb = getBishopMagic(occ, sqr);
		bb &= ~b.colors[b.turn] & ~b.pieces[KING];
		if (noisyOnly) bb &= b.colors[!b.turn];
		addPieceMoves(moves, bb, sqr);
	}
}

void genRookMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	uint64_t rooks = b.pieces[ROOK] & b.colors[b.turn];
	const uint64_t occ = ~b.colors[NO_COLOR];
	while (rooks) {
		int sqr = popBit(rooks);
		uint64_t bb = getRookMagic(occ, sqr);
		bb &= ~b.colors[b.turn] & ~b.pieces[KING];
		if (noisyOnly) bb &= b.colors[!b.turn];
		addPieceMoves(moves, bb, sqr);
	}
}

void genQueenMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	uint64_t queens = b.pieces[QUEEN] & b.colors[b.turn];
	const uint64_t occ = ~b.colors[NO_COLOR];
	while (queens) {
		int sqr = popBit(queens);
		uint64_t bb = (getBishopMagic(occ, sqr) | getRookMagic(occ, sqr)) & (~b.colors[b.turn] & ~b.pieces[KING]);
		if (noisyOnly) bb &= b.colors[!b.turn];
		addPieceMoves(moves, bb, sqr);
	}
}

void genKingMoves(const Board& b, std::vector<uint16_t>& moves, bool noisyOnly) {
	uint64_t king = b.pieces[KING] & b.colors[b.turn];
	while (king) {
		int sqr = popBit(king);
		uint64_t bb = kingAttacks[sqr] & ~(b.colors[b.turn] | b.pieces[KING]);
		if (noisyOnly) bb &= b.colors[!b.turn];
		addPieceMoves(moves, bb, sqr);
	}
	if (noisyOnly || inCheck(b, b.turn)) return;
	if (b.turn == WHITE && b.squares[E1] == W_KING) {
		if ((b.castlingRights & WK_CASTLING) &&
			(b.squares[F1] == EMPTY) &&
			(b.squares[G1] == EMPTY) &&
			(b.squares[H1] == W_ROOK) &&
			(!squareIsAttacked(b, WHITE, F1)) &&
			(!squareIsAttacked(b, WHITE, G1))) 
		{
			moves.push_back(createMove(E1, G1, CASTLE_MOVE));
		}
		if ((b.castlingRights & WQ_CASTLING) &&
			(b.squares[D1] == EMPTY) &&
			(b.squares[C1] == EMPTY) &&
			(b.squares[B1] == EMPTY) &&
			(b.squares[A1] == W_ROOK) &&
			(!squareIsAttacked(b, WHITE, D1)) &&
			(!squareIsAttacked(b, WHITE, C1)))
		{
			moves.push_back(createMove(E1, C1, CASTLE_MOVE));
		}
	}
	else if (b.turn == BLACK && b.squares[E8] == B_KING) {
		if ((b.castlingRights & BK_CASTLING) &&
			(b.squares[F8] == EMPTY) &&
			(b.squares[G8] == EMPTY) &&
			(b.squares[H8] == B_ROOK) &&
			(!squareIsAttacked(b, BLACK, F8)) &&
			(!squareIsAttacked(b, BLACK, G8)))
		{
			moves.push_back(createMove(E8, G8, CASTLE_MOVE));
		}
		if ((b.castlingRights & BQ_CASTLING) &&
			(b.squares[D8] == EMPTY) &&
			(b.squares[C8] == EMPTY) &&
			(b.squares[B8] == EMPTY) &&
			(b.squares[A8] == B_ROOK) &&
			(!squareIsAttacked(b, BLACK, D8)) &&
			(!squareIsAttacked(b, BLACK, C8)))
		{
			moves.push_back(createMove(E8, C8, CASTLE_MOVE));
		}
	}
}

std::vector<uint16_t> genAllMoves(const Board& b) {
	std::vector<uint16_t> moves;
	moves.reserve(45);
	genPawnMoves(b, moves, false);
	genKnightMoves(b, moves, false);
	genBishopMoves(b, moves, false);
	genRookMoves(b, moves, false);
	genQueenMoves(b, moves, false);
	genKingMoves(b, moves, false);
	return moves;
}

std::vector<uint16_t> genNoisyMoves(const Board& b) {
	std::vector<uint16_t> noisyMoves;
	noisyMoves.reserve(10);
	genPawnMoves(b, noisyMoves, true);
	genKnightMoves(b, noisyMoves, true);
	genBishopMoves(b, noisyMoves, true);
	genRookMoves(b, noisyMoves, true);
	genQueenMoves(b, noisyMoves, true);
	genKingMoves(b, noisyMoves, true);
	return noisyMoves;
}
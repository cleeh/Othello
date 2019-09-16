// Fill out your copyright notice in the Description page of Project Settings.

#include "OthelloTree.h"

OthelloTree::OthelloTree()
{
	root = NULL;
	pn = NULL;
}

OthelloTree::OthelloTree(TArray<EStoneColor> board, EStoneColor turn)
{
	root = new OthelloTreeNode(board, -1, 0, turn);
	pn = root;
}

OthelloTree::~OthelloTree()
{
	delete root;
}

void OthelloTree::SetRoot(TArray<EStoneColor> board, EStoneColor turn)
{
	if (root == NULL)
	{
		root = new OthelloTreeNode(board, -1, 0, turn);
		pn = root;
	}
}

void OthelloTree::SetPN(OthelloTreeNode * n)
{
	pn = n;
}

OthelloTreeNode * OthelloTree::GetRoot()
{
	return root;
}

OthelloTreeNode * OthelloTree::GetPN()
{
	return pn;
}

void OthelloTree::NodeChangeData(TArray<EStoneColor> board, int position, int score, EStoneColor turn)
{
	pn->SetData(board, position, score, turn);
}

OthelloTreeNode* OthelloTree::ParentInsertChild(TArray<EStoneColor> board, int position, int score, EStoneColor turn, OthelloTreeNode* parent)
{
	OthelloTreeNode* temp;
	OthelloTreeNode* result = NULL;

	OthelloTreeNode* OriginalPN = pn;
	if (parent) {
		pn = parent;
	}

	if (pn == NULL) {
		result = new OthelloTreeNode(board, position, score, turn);
		pn->SetChild(result);
		result->SetParent(pn);
	}
	else
	{
		temp = pn->GetChild();
		if (temp == NULL) {
			result = new OthelloTreeNode(board, position, score, turn);
			pn->SetChild(result);
			result->SetParent(pn);
			return;
		}
		else {
			while (true) {
				if (temp->GetSibling() == NULL)	{
					result = new OthelloTreeNode(board, position, score, turn);
					temp->SetSibling(result);
					result->SetParent(pn);
					break;
				}
				temp = temp->GetSibling();
			}
		}
	}

	pn = OriginalPN;
	return result;
}

#pragma region Minimax Algorithm
bool OthelloTree::CheckPossibility(TArray<EStoneColor> board, int start_x, int start_y)
{
	if (start_x < 0 || start_y < 0) return false;

	EStoneColor target_color = board[start_y * AOthelloBlockGrid::Size + start_x];
	EStoneColor other_color;
	if (target_color == EStoneColor::Black) other_color = EStoneColor::White;
	else if (target_color == EStoneColor::White) other_color = EStoneColor::Black;

	if (board[start_y * AOthelloBlockGrid::Size + start_x] != EStoneColor::None)
	{
		return false;
	}
	else
	{
		// West
		bool Flag = false;
		for (int x = start_x - 1; x >= 0; x--)
		{
			EStoneColor Target = board[start_y * AOthelloBlockGrid::Size + x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// East
		Flag = false;
		for (int x = start_x + 1; x < AOthelloBlockGrid::Size; x++)
		{
			EStoneColor Target = board[start_y * AOthelloBlockGrid::Size + x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// South
		Flag = false;
		for (int y = start_y - 1; y >= 0; y--)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + start_x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// North
		Flag = false;
		for (int y = start_y + 1; y < AOthelloBlockGrid::Size; y++)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + start_x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// Northern-West
		Flag = false;
		for (int x = start_x - 1, y = start_y + 1; x >= 0 && y < AOthelloBlockGrid::Size; x--, y++)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + x];


			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// Northern-East
		Flag = false;
		for (int x = start_x + 1, y = start_y + 1; x < AOthelloBlockGrid::Size && y < AOthelloBlockGrid::Size; x++, y++)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// Southern-West
		Flag = false;
		for (int x = start_x - 1, y = start_y - 1; x >= 0 && y >= 0; x--, y--)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}

		// Southern-East
		Flag = false;
		for (int x = start_x + 1, y = start_y - 1; x < AOthelloBlockGrid::Size && y >= 0; x++, y--)
		{
			EStoneColor Target = board[y * AOthelloBlockGrid::Size + x];

			// same color
			if (Target == target_color)
			{
				if (Flag) return true;
				else break;
			}
			// other color
			else if (Target == other_color)
			{
				Flag = true;
			}
			// no color
			else if (Target == EStoneColor::None)
			{
				break;
			}
		}
	}

	return false;
}

TArray<EStoneColor> OthelloTree::GetTreeNodeScore(TArray<EStoneColor> board, uint8 stone_x, uint8 stone_y, OUT int& changed_number)
{
	const int32 Size = AOthelloBlockGrid::Size;
	EStoneColor target_color = board[stone_y * Size + stone_x];
	EStoneColor other_color;
	if (target_color == EStoneColor::Black) other_color = EStoneColor::White;
	else if (target_color == EStoneColor::White) other_color = EStoneColor::Black;

	TArray<bool> StoneToChange;
	StoneToChange.Empty();
	StoneToChange.SetNum(Size*Size);

	// West
	for (int x = stone_x - 1; x >= 0; x--)
	{
		EStoneColor Target = board[stone_y * Size + stone_x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (x > 0) StoneToChange[stone_y*Size + x] = true;
			else
			{
				for (int i = 0; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = x; i < stone_x; i++) StoneToChange[stone_y*Size + i] = false;
			break;
		}
	}

	// East
	for (int x = stone_x + 1; x < Size; x++)
	{
		EStoneColor Target = board[stone_y * Size + x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (x < Size - 1) StoneToChange[stone_y*Size + x] = true;
			else
			{
				for (int i = Size - 1; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = x; i > stone_x; i--) StoneToChange[stone_y*Size + i] = false;
			break;
		}
	}

	// North
	for (int y = stone_y + 1; y < Size; y++)
	{
		EStoneColor Target = board[y * Size + stone_x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y < Size - 1) StoneToChange[y*Size + stone_x] = true;
			else
			{
				for (int j = Size - 1; j > stone_y; j--) StoneToChange[j*Size + stone_x] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int j = y; j > stone_y; j--) StoneToChange[j*Size + stone_x] = false;
			break;
		}
	}

	// South
	for (int y = stone_y - 1; y >= 0; y--)
	{
		EStoneColor Target = board[y * Size + stone_x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y > 0) StoneToChange[y*Size + stone_x] = true;
			else
			{
				for (int j = 0; j < stone_y; j++) StoneToChange[j*Size + stone_x] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int j = y; j < stone_y; j++) StoneToChange[j*Size + stone_x] = false;
			break;
		}
	}

	// Northern-West
	for (int x = stone_x - 1, y = stone_y + 1; x >= 0 && y < Size; x--, y++)
	{
		EStoneColor Target = board[y*Size + x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y < Size - 1 && x > 0) StoneToChange[y*Size + x] = true;
			else
			{
				for (int i = stone_x - 1, j = stone_y + 1; i > 0 && j < Size - 1; i--, j++)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = stone_x - 1, j = stone_y + 1; i > 0 && j < Size - 1; i--, j++)
				StoneToChange[j*Size + i] = false;
			break;
		}
	}

	// Northern-East
	for (int x = stone_x + 1, y = stone_y + 1; x < Size && y < Size; x++, y++)
	{
		EStoneColor Target = board[y * Size + x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y < Size - 1 && x < Size - 1) StoneToChange[y*Size + x] = true;
			else
			{
				for (int i = stone_x + 1, j = stone_y + 1; i < Size - 1 && j < Size - 1; i++, j++)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = stone_x + 1, j = stone_y + 1; i < Size - 1 && j < Size - 1; i++, j++)
				StoneToChange[j*Size + i] = false;
			break;
		}
	}

	// Southern-West
	for (int x = stone_x - 1, y = stone_y - 1; x >= 0 && y >= 0; x--, y--)
	{
		EStoneColor Target = board[y*Size + x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y > 0 && x > 0) StoneToChange[y*Size + x] = true;
			else
			{
				for (int i = stone_x - 1, j = stone_y - 1; i > 0 && j > 0; i--, j--)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = stone_x - 1, j = stone_y - 1; i > 0 && j > 0; i--, j--)
				StoneToChange[j*Size + i] = false;
			break;
		}
	}

	// Southern-East
	for (int x = stone_x + 1, y = stone_y - 1; x < Size && y >= 0; x++, y--)
	{
		EStoneColor Target = board[y*Size + x];

		// same stone color
		if (Target == target_color)
		{
			break;
		}
		// other stone color
		else if (Target == other_color)
		{
			if (y > 0 && x < Size - 1) StoneToChange[y*Size + x] = true;
			else
			{
				for (int i = stone_x + 1, j = stone_y - 1; i < Size - 1 && j > 0; i++, j--)
					StoneToChange[j*Size + i] = false;
				break;
			}
		}
		// no stone
		else if (Target == EStoneColor::None)
		{
			for (int i = stone_x + 1, j = stone_y - 1; i < Size - 1 && j > 0; i++, j--)
				StoneToChange[j*Size + i] = false;
			break;
		}
	}

	int ChangedStoneNumber = 0;
	for (int y = 0; y < Size; y++)
		for (int x = 0; x < Size; x++)
		{
			const int index = y*Size + x;

			if (StoneToChange[index])
			{
				if (board[index] == EStoneColor::Black)
					board[index] = EStoneColor::White;
				else if (board[index] == EStoneColor::White)
					board[index] = EStoneColor::Black;
				ChangedStoneNumber++;
			}
		}
	changed_number = ChangedStoneNumber;

	return board;
}

void OthelloTree::CalculateChilds()
{
	EStoneColor other;
	if (pn->GetTurn() == EStoneColor::Black) other = EStoneColor::White;
	else if (pn->GetTurn() == EStoneColor::White) other = EStoneColor::Black;

	for (int i = 0; i < AOthelloBlockGrid::Size * AOthelloBlockGrid::Size; i++) {
		int x = i % AOthelloBlockGrid::Size;
		int y = i / AOthelloBlockGrid::Size;

		if (CheckPossibility(pn->GetBoard(), x, y)) {
			int score;
			TArray<EStoneColor> board = GetTreeNodeScore(pn->GetBoard(), x, y, score);

			ParentInsertChild(board, i, score, other);
		}
	}
}

void OthelloTree::CalculateTree(unsigned int level)
{
	unsigned int degree = 0;

	CalculateChilds();
	do {
		// 최하단 노드 생성
		for (; degree < level - 1; degree++) {
			pn = pn->GetChild();
			CalculateChilds();
		}

		// 형제 노드 계산
		OthelloTreeNode* NodePointer = pn;
		while (NodePointer = NodePointer->GetSibling()) {
			pn = NodePointer;
			CalculateChilds();
		}

		// 부모 노드로 올라가기
		do {
			degree++;
			pn = pn->GetParent();
		} while (!pn->GetSibling() && pn != root);
	} while (pn != root);
}

int GetMinimaxData(TArray<int>& procedure)
{

}


#pragma endregion
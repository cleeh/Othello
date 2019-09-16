// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OthelloTreeNode.h"
#include "OthelloBlockGrid.h"

/**
 * 
 */
class OTHELLO_API OthelloTree
{
public:
	OthelloTree();
	OthelloTree(TArray<EStoneColor> board, EStoneColor turn);
	~OthelloTree();

private:
	OthelloTreeNode* root;
	OthelloTreeNode* pn; // 현재 작업중인 노드

public:
	void SetRoot(TArray<EStoneColor> board, EStoneColor turn);
	void SetPN(OthelloTreeNode* n);
	
	OthelloTreeNode* GetRoot();
	OthelloTreeNode* GetPN();

	// 갱신
	void NodeChangeData(TArray<EStoneColor> board, int position, int score, EStoneColor turn);

	// 삽입
	OthelloTreeNode* ParentInsertChild(TArray<EStoneColor> board, int position, int score, EStoneColor turn, OthelloTreeNode* parent = NULL);

	// 삭제
	void ParentDeleteChild(uint8 level);

public:
	bool OthelloTree::CheckPossibility(TArray<EStoneColor> board, int start_x, int start_y);
	TArray<EStoneColor> GetTreeNodeScore(TArray<EStoneColor> board, uint8 stone_x, uint8 stone_y, OUT int& changed_number);
	void CalculateChilds();
	void CalculateTree(unsigned int level);
};

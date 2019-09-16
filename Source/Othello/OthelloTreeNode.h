// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OthelloBlock.h"

/**
 * 
 */
class OTHELLO_API OthelloTreeNode
{
public:
	OthelloTreeNode();
	OthelloTreeNode(TArray<EStoneColor> board, int position, int score, EStoneColor turn);
	~OthelloTreeNode();

private:
	TArray<EStoneColor> board;
	int position;
	int score;
	EStoneColor turn;
	OthelloTreeNode* parent;
	OthelloTreeNode* sibling;
	OthelloTreeNode* child;
	
public:
	void DeleteNode(OthelloTreeNode* n);
	
	void SetData(TArray<EStoneColor> board, int position, int score, EStoneColor turn);
	void SetParent(OthelloTreeNode* parent);
	void SetSibling(OthelloTreeNode* sibling);
	void SetChild(OthelloTreeNode* child);

	TArray<EStoneColor> GetBoard();
	int GetPosition();
	int GetScore();
	EStoneColor GetTurn();
	OthelloTreeNode* GetParent();
	OthelloTreeNode* GetSibling();
	OthelloTreeNode* GetChild();
	

};

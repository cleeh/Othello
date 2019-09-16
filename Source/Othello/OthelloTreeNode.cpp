// Fill out your copyright notice in the Description page of Project Settings.

#include "OthelloTreeNode.h"

OthelloTreeNode::OthelloTreeNode()
{
}

OthelloTreeNode::OthelloTreeNode(TArray<EStoneColor> board, int position, int score, EStoneColor turn)
{
	this->board = board;
	this->position = position;
	this->score = score;
	this->turn = turn;
	this->parent = NULL;
	this->sibling = NULL;
	this->child = NULL;
}

OthelloTreeNode::~OthelloTreeNode()
{
	DeleteNode(child);
	DeleteNode(sibling);
}

//========== 삭제 함수 ==========//
void OthelloTreeNode::DeleteNode(OthelloTreeNode * n)
{
	if (n == NULL) return;
	delete n;
}

//========== Set 함수 ==========//
void OthelloTreeNode::SetData(TArray<EStoneColor> board, int position, int score, EStoneColor turn)
{
	this->board = board;
	this->position = position;
	this->score = score;
	this->turn = turn;
}

void OthelloTreeNode::SetParent(OthelloTreeNode * parent)
{
	this->parent = parent;
}

void OthelloTreeNode::SetSibling(OthelloTreeNode * sibling)
{
	this->sibling = sibling;
}

void OthelloTreeNode::SetChild(OthelloTreeNode * child)
{
	this->child = child;
}

//========== Get 함수 ==========//
TArray<EStoneColor> OthelloTreeNode::GetBoard()
{
	return board;
}

int OthelloTreeNode::GetPosition()
{
	return position;
}

int OthelloTreeNode::GetScore()
{
	return score;
}

EStoneColor OthelloTreeNode::GetTurn()
{
	return turn;
}

OthelloTreeNode * OthelloTreeNode::GetParent()
{
	return parent;
}

OthelloTreeNode * OthelloTreeNode::GetChild()
{
	return child;
}

OthelloTreeNode * OthelloTreeNode::GetSibling()
{
	return sibling;
}
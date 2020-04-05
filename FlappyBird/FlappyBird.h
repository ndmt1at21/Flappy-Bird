#pragma once
#include "ndmtEngine.h"
#include <list>
#include <vector>
#include "Graphics/Sprite.h"
#include "Graphics/Rect.h"
#include "System/Score.h"

class FlappyBird : public mt::ndmtEngine
{
private:
	// Bird
	std::vector<mt::Sprite*> m_bird;
	std::vector<mt::Sprite*> m_birdWing;
	float m_fJump = -150.0f;
	float m_fGravity = 500.0f;
	float m_fVelocity = 0.0f;
	float m_fPosition = 0.0f;
	int m_nPosBirdX = 0;
	int m_totalFrame = 0;

	// Pipe
	std::vector<mt::Sprite*> m_pipe;
	float m_fLevelSection = 0.0f;
	float m_fSectionWidth = 0.0f;
	std::list<int> m_listSection;
	const int m_spaceTwoPipe = 38;

	// Ground
	mt::Sprite* m_ground;
	std::list<int> m_listGround;
	float m_fLevelGround = 0.0f;
	float m_fGroundWidth = 0.0f;
	int m_nPosYGround = 0;

	// Parameter of game
	mt::Score m_score;
	bool m_bBegin = true;
	bool m_bResetGame = false;
	bool m_bHasCollided = false;

protected:
	bool OnUserCreate();
	bool OnUserUpdate(float fElapseTime);
	bool OnUserDestroy();

protected:
	void startScreen(float fElapseTime);
	void overScreen(float fElapseTime);
	void DrawResult();

	mt::RectInt DrawBird(float fElapseTime);
	void DrawBirdOver(float fElapseTime, int posYStop);

	std::vector<mt::RectInt> DrawPipe(float fElapseTime);
	void DrawGround(float fElapseTime);
	
	void DrawScore(int score, const mt::Vector2i& position);
	void calScore(const mt::RectInt& bird, const mt::RectInt& pipe);
	
	bool checkCollided(const mt::RectInt& birdBound, std::vector<mt::RectInt> boundFirstPipe);
};

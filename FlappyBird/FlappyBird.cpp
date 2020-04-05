#include "FlappyBird.h"
#include <thread>

bool FlappyBird::OnUserCreate()
{
	// Load Bird
	m_bird.resize(4);
	m_birdWing.resize(3);
	for (int i = 0; i < (int)m_bird.size(); i++)
		m_bird[i] = new mt::Sprite(L"Asset/bird" + std::to_wstring(i) + L".spr");

	for (int i = 0; i < m_birdWing.size(); i++)
		m_birdWing[i] = new mt::Sprite(L"Asset/bird" + std::to_wstring(i) + L"Wing.spr");

	// Load Pipe
	m_pipe.resize(2);
	for (int i = 0; i < 2; i++)
		m_pipe[i] = new mt::Sprite(L"Asset/pipe" + std::to_wstring(i) + L".spr");
	m_listSection = { 0, 0 };
	m_fSectionWidth = (float)m_nScreenWidth / (m_listSection.size() - 1);

	//Load Ground
	m_ground = new mt::Sprite(L"Asset/ground.spr");
	m_listGround = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	m_fGroundWidth = (float)m_nScreenWidth / (m_listGround.size() - 1);

	// Init
	m_score.setScore(-1);
	m_nPosBirdX = m_nScreenWidth / 5;
	m_fPosition = m_nScreenHeight / 4;
	m_nPosYGround = m_nScreenHeight / 2 + m_nScreenHeight / 3;
	srand(time(NULL));
	
	return true;
}

bool FlappyBird::OnUserUpdate(float fElapseTime)
{
	// Handle event
	mt::Event event;

	if (m_bResetGame)
	{
		m_bBegin = true;
		m_score.setScore(-1);
		m_fVelocity = 0.0f;
		m_fPosition = m_nScreenHeight / 4;
		m_totalFrame = 0;

		m_bResetGame = false;
		m_bHasCollided = false;

		m_listSection = { 0, 0, 0 };
		m_listGround = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	}

	// Erase Screen
	mt::Rectangle rect(mt::Vector2i(m_nScreenWidth, m_nScreenHeight));
	rect.setPosition(mt::Vector2i(0, 0));
	rect.setFillAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_CYAN | mt::BG_CYAN));
	draw(&rect);

	if (m_bHasCollided)
	{
		overScreen(fElapseTime);
	}
	else
	{

		// Display
		if (m_bBegin)
		{
			startScreen(fElapseTime);
		}
		else
		{
			std::vector<mt::RectInt> boundNextPipe = DrawPipe(fElapseTime);
			mt::RectInt birdBound = DrawBird(fElapseTime);

			// Check Collided
			if (checkCollided(birdBound, boundNextPipe))
			{
				m_bHasCollided = true;
				m_score.Save(L"score.dat");
			}

			calScore(birdBound, boundNextPipe[1]);
			DrawScore(m_score.getScore(), mt::Vector2i(m_nScreenWidth / 2, m_nScreenHeight / 3));
		}
		
		// Draw Ground
		DrawGround(fElapseTime);
	}
	
	return true;
}

bool FlappyBird::OnUserDestroy()
{
	return true;
}

void FlappyBird::startScreen(float fElapseTime)
{
	if (mt::Event::isKeyPressed(VK_SPACE))
		m_bBegin = false;

	m_bird[0]->setPosition(mt::Vector2i(m_nPosBirdX, m_fPosition));
	draw(m_bird[0]);
}

void FlappyBird::overScreen(float fElapseTime)
{
	DrawPipe(0);
	if (m_fPosition >= m_nPosYGround - 10)
	{
		DrawResult();
		
		if (mt::Event::isKeyPressed(VK_SPACE))
			m_bResetGame = true;
	}
	DrawBirdOver(fElapseTime, m_nPosYGround - 10);
	DrawGround(0);
}

void FlappyBird::DrawResult()
{
	mt::Rectangle rect(mt::Vector2i(29, 46));
	rect.setOutlineAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_BLACK));
	rect.setPosition(mt::Vector2i(m_nScreenWidth / 2 - 13, m_nScreenHeight / 2 - 41));
	draw(&rect);

	rect.setSize(mt::Vector2i(28, 45));
	rect.setFillAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_DARK_YELLOW | mt::BG_DARK_YELLOW));
	rect.setPosition(mt::Vector2i(m_nScreenWidth / 2 - 12, m_nScreenHeight / 2 - 40));
	draw(&rect);

	mt::Text text(L"SCORE", L"small", 1);
	text.setPosition(mt::Vector2i(m_nScreenWidth / 2, m_nScreenHeight / 2 - 37));
	draw(&text);

	text.setText(L"BEST");
	text.setAlign(1);
	text.setPosition(mt::Vector2i(m_nScreenWidth / 2, m_nScreenHeight / 2 - 15));
	draw(&text);

	DrawScore(m_score.getScore(), mt::Vector2i(m_nScreenWidth / 2, m_nScreenHeight / 2 - 30));
	DrawScore(mt::Score::getMaxScore(L"score.dat").getScore(), mt::Vector2i(m_nScreenWidth / 2, m_nScreenHeight / 2 - 8));
}

mt::RectInt FlappyBird::DrawBird(float fElapseTime)
{
	m_fVelocity += m_fGravity * fElapseTime;
	if (mt::Event::isKeyPressed(VK_SPACE))
		m_fVelocity = m_fJump;

	m_fPosition += m_fVelocity * fElapseTime;

	int alpha = 0;
	if (m_fVelocity < 150)			alpha = 45;
	else if (m_fVelocity < 200)		alpha = 0;
	else if (m_fVelocity < 230)		alpha = -45;
	else if (m_fVelocity >= 230)	alpha = -90;

	if (m_totalFrame > 100)	m_totalFrame = 0;
	m_totalFrame++;

	mt::Sprite* currentBird = nullptr;
	switch (alpha)
	{
	case 45:
	{
		if (m_totalFrame > 50)	currentBird = m_birdWing[1];
		else					currentBird = m_bird[1];

		break;
	}

	case 0:
	{
		if (m_totalFrame > 50)	currentBird = m_birdWing[0];
		else					currentBird = m_bird[0];

		break;
	}

	case -45:
	{
		if (m_totalFrame > 50)	currentBird = m_birdWing[2];
		else					currentBird = m_bird[2];

		break;
	}

	case -90:
	{
		currentBird = m_bird[3];
		break;
	}
	}

	currentBird->setPosition(mt::Vector2i(m_nPosBirdX, (int)m_fPosition));
	draw(currentBird);

	return currentBird->getGlobalBound();
}

void FlappyBird::DrawBirdOver(float fElapseTime, int posYStop)
{
	if (m_fPosition < posYStop)
	{
		m_fVelocity += m_fGravity * fElapseTime;
		m_fPosition += m_fVelocity * fElapseTime;

		m_bird[3]->setPosition(mt::Vector2i(m_nPosBirdX, m_fPosition));
	}

	draw(m_bird[3]);
}

std::vector<mt::RectInt> FlappyBird::DrawPipe(float fElapseTime)
{
	m_fLevelSection += 60.0f * fElapseTime;

	if (m_fLevelSection > m_fSectionWidth)
	{
		m_fLevelSection = 0;
		m_listSection.pop_front();

		int h = rand() % (m_nScreenHeight / 3) + (m_nScreenHeight / 8);
		m_listSection.push_back(h);
	}

	int nSection = 0;
	std::vector<mt::RectInt> boundNextPipe(2);
	for (auto h : m_listSection)
	{
		if (h == 0)
		{
			m_pipe[1]->setPosition(mt::Vector2i(0, -m_pipe[1]->getHeight()));
			m_pipe[0]->setPosition(mt::Vector2i(0, -m_nScreenHeight));
		}
		else
		{
			m_pipe[1]->setPosition(mt::Vector2f(nSection * m_fSectionWidth - m_fLevelSection, h - m_pipe[1]->getHeight()));
			m_pipe[0]->setPosition(mt::Vector2f(nSection * m_fSectionWidth - m_fLevelSection, h + m_spaceTwoPipe));
		}

		if (nSection == 1)
		{
			boundNextPipe[0] = m_pipe[0]->getGlobalBound();
			boundNextPipe[1] = m_pipe[1]->getGlobalBound();
		}

		draw(m_pipe[1]);
		draw(m_pipe[0]);
		nSection++;
	}

	return boundNextPipe;
}

void FlappyBird::DrawGround(float fElapseTime)
{
	int nPosY = m_nPosYGround;
	int nPosX = 0;

	m_fLevelGround += 60.0f * fElapseTime;
	if (m_fLevelGround > m_fGroundWidth)
	{
		m_fLevelGround = 0;
		m_listGround.pop_front();
		m_listGround.push_back(1);
	}

	mt::Line line;

	line.setLine(mt::Vector2i(nPosX, nPosY), mt::Vector2i(m_nScreenWidth, nPosY));
	line.setOutlineAttribute(mt::Vector2i(mt::PIXEL_HALF, mt::FG_DARK_RED));
	draw(&line);

	line.setLine(mt::Vector2i(0, nPosY + 1), mt::Vector2i(m_nScreenWidth, nPosY + 1));
	line.setOutlineAttribute(mt::Vector2i(mt::PIXEL_THREEQUARTERS, mt::FG_YELLOW | mt::BG_GREEN));
	draw(&line);

	mt::Rectangle rect(mt::Vector2i(m_nScreenWidth, 3));
	rect.setPosition(mt::Vector2i(nPosX, nPosY + 2));
	rect.setFillAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_GREEN | mt::BG_GREEN));
	draw(&rect);

	line.setLine(mt::Vector2i(nPosX, nPosY + 5), mt::Vector2i(m_nScreenWidth, nPosY + 5));
	line.setOutlineAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_DARK_YELLOW));
	draw(&line);

	rect.setPosition(mt::Vector2i(nPosX, nPosY + 6));
	rect.setSize(mt::Vector2i(m_nScreenWidth, m_nScreenHeight - (nPosY + 5)));
	rect.setFillAttribute(mt::Vector2i(mt::PIXEL_SOLID, mt::FG_YELLOW | mt::BG_YELLOW));
	draw(&rect);

	int nGround = 0;
	for (auto g : m_listGround)
	{
		if (g != 0)
		{
			m_ground->setPosition(mt::Vector2i((int)(nGround * m_fGroundWidth - m_fLevelGround), nPosY + 2));
			draw(m_ground);
		}
		nGround++;
	}
}

void FlappyBird::calScore(const mt::RectInt& bird, const mt::RectInt& pipe)
{
	int section = 1;

	if (pipe.left == section * m_fSectionWidth)
		m_score++;
}

void FlappyBird::DrawScore(int  score, const mt::Vector2i& position)
{
	mt::Text text;
	
	if (score >= 0)
		text.setText(std::to_wstring(score));
	else
		text.setText(std::to_wstring(0));

	text.setFontType(L"big");
	text.setPosition(position);
	text.setAlign(1);
	draw(&text);
}

bool FlappyBird::checkCollided(const mt::RectInt& birdBound, std::vector<mt::RectInt> boundFirstPipe)
{
	// Collided with ground
	if (birdBound.top + birdBound.height > m_nPosYGround)
		return true;

	//Coliid width top
	if (birdBound.top == 0)
		return true;

	//Collided width pipes
	for (int i = 0; i < boundFirstPipe.size(); i++)
	{
		if (birdBound.interset(boundFirstPipe[i]))
			return true;
	}

	return false;
}
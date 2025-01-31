#pragma once

#include "Vec2.h"

#include <SFML/Graphics.hpp>

class CTransform
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 velocity = { 0.0, 0.0 };
	int angle = 0;
	float rotation = 0.0f;
	int speed = 0;

	CTransform(const Vec2& p, const Vec2& v, int a, float r, int s)
		: pos(p), velocity(v), angle(a), rotation(r), speed(s) {}
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);

	}
};

class CCollision
{
public:
	float radius = 0;

	CCollision(float r)
		: radius(r) {}
};

class CScore
{
public:
	int score = 0;

	CScore(int s)
		: score(s) {}
};

class CLifespan
{
public:
	int remaining = -1;
	int total = -1;

	CLifespan(int total=-1)
		: remaining(total), total(total) {}
};

class CInput
{
public:
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	CInput() {}
};
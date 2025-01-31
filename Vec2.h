#pragma once

class Vec2 {
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float, float);

	bool operator == (const Vec2&) const;
	bool operator != (const Vec2&) const;

	Vec2 operator + (const Vec2&) const;
	Vec2 operator - (const Vec2&) const;
	Vec2 operator / (const float) const;
	Vec2 operator * (const float) const;
	Vec2 operator * (const Vec2&) const;

	void operator += (const Vec2&);
	void operator -= (const Vec2&);
	void operator *= (const float);
	void operator /= (const float);

	float distsq(const Vec2&) const;
};
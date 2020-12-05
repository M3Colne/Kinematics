#pragma once

#include "Graphics.h"
#include <cmath>
#include <cassert>

struct Segment
{
	//Variables
	Vec2 pos = Vec2(0.0f, 0.0f);
	float len = 0.0f;
	float theta = 0.0f;
	Color color = Colors::White;
	//Variables

	//Functions
	Segment() = default;
	Segment(Vec2 p, float l, float a, Color c)
		:
		pos(p),
		len(l),
		theta(a),
		color(c)
	{
	}
	Vec2 calculateEnd() const
	{
		return pos + Vec2(cos(theta), sin(theta)) * len;
	}
	void Follow(Vec2 target)
	{
		//Update the head
		const Vec2 dir = target - calculateEnd();
		if (dir.x != 0.0f && dir.y != 0.0f)
		{
			pos = target;
			theta = dir.GetAngle() - 3.1415926f;
		}
	}
	//Functions
};

struct Tentacle
{
	//Variables
	int nSegments = 0;
	Segment* segments = nullptr;
	//Variables

	//Functions
	Tentacle(int n, Vec2 s, float l, float a, Color c)
		:
		nSegments(n)
	{
		//Initializing
		segments = new Segment[n];
		for (int i = 0; i < n; i++)
		{
			segments[i] = Segment(s, l, a, c);
		}
	}
	Tentacle(const Tentacle& other) = delete;
	Tentacle& operator=(const Tentacle& other) = delete;
	~Tentacle()
	{
		delete[] segments;
	}

	void Draw(Graphics& gfx)
	{
		auto segIt = segments;
		for (int i = 0; i < nSegments; i++)
		{
			gfx.DrawLine(segIt->pos, segIt->calculateEnd(), segIt->color);
			segIt++;
		}
	}
	void Follow(Vec2 target)
	{
		segments[0].Follow(target);
		for (int i = 1; i < nSegments; i++)
		{
			segments[i].Follow(segments[i - 1].calculateEnd());
		}
	}
	//Functions
};
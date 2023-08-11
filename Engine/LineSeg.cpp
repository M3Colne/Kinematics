#include "LineSeg.h"
#include <assert.h>

void LineSeg::Segment::CapAngle()
{
	while (angle >= 6.2831853f)
	{
		angle -= 6.2831853f;
	}
	while (angle < 0.0f)
	{
		angle += 6.2831853f;
	}
}

LineSeg::Segment::Segment(Vec2 posA, Vec2 posB)
	:
	A(posA),
	lenght((posB - posA).GetLength()),
	angle((posB - posA).GetAngle())
{

}

LineSeg::Segment::Segment(Vec2 pos, float lenght, float angle)
	:
	A(pos),
	lenght(lenght),
	angle(angle)
{}

void LineSeg::Segment::FollowFromHead(const Vec2 & target)
{
	if ((target - GetA()).GetLengthSq() != 0.0f) //this if statement is to avoid a bug that glitches a segment if it's not moving
	{
		ChangeAngleAtA((GetB() - target).GetAngle());
	}
	MoveA_At(target);
}

void LineSeg::Segment::FollowFromTail(const Vec2& target)
{
	if ((GetB() - target).GetLengthSq() != 0.0f) //this if statement is to avoid a bug that glitches a segment if it's not moving
	{
		ChangeAngleAtB((GetA() - target).GetAngle());
	}
	MoveB_At(target);
}

Vec2 LineSeg::Segment::GetB() const
{
	return A + Vec2(cos(angle), sin(angle)) * lenght;
}

Vec2 LineSeg::Segment::GetA() const
{
	return A;
}

float LineSeg::Segment::GetAngle() const
{
	return angle;
}

float LineSeg::Segment::GetLenght() const
{
	return lenght;
}

void LineSeg::Segment::MoveBy(const Vec2& delta)
{
	A += delta;
}

void LineSeg::Segment::MoveA_At(const Vec2& pos)
{
	A = pos;
}

void LineSeg::Segment::MoveB_At(const Vec2& pos)
{
	MoveA_At(pos - GetB() + GetA());
}

void LineSeg::Segment::RotateByA(float dA)
{
	angle += dA;
	CapAngle();
}

void LineSeg::Segment::ChangeAngleAtA(float a)
{
	RotateByA(a - angle);
}

void LineSeg::Segment::RotateByB(float dB)
{
	const Vec2 B = GetB();
	const float bAngle = (GetA() - B).GetAngle();
	A = Vec2(cos(bAngle + dB), sin(bAngle + dB)) * lenght + B;
	angle = (B - GetA()).GetAngle();

	CapAngle();
}

void LineSeg::Segment::ChangeAngleAtB(float a)
{
	RotateByB(a - GetAngle() - 3.1415926f);
}

LineSeg::LineSeg(int n, Vec2 s, float l, float a, Color c)
	:
	color(c)
{
	//Initializing
	sex.reserve(n);
	const float segLen = l / n;
	const Vec2 dir = Vec2(cos(a), sin(a)) * segLen;
	for (int i = 0; i < n; i++)
	{
		sex.push_back(Segment(s + dir * float(i), segLen, a));
	}
}

void LineSeg::Draw(Graphics& gfx, bool drawJoints) const
{
	for (const auto& s : sex)
	{
		//The connections
		gfx.DrawLine(s.GetA(), s.GetB(), color);
	}

	if (drawJoints)
	{
		for (const auto& s : sex)
		{
			for (int j = -2; j < 2; j++)
			{
				for (int i = -2; i < 2; i++)
				{
					gfx.PutPixel(int(s.GetA().x) + i, int(s.GetA().y) + j, Colors::Red);
				}
			}
		}
		const Vec2 t(GetTail().GetB());
		for (int j = -2; j < 2; j++)
		{
			for (int i = -2; i < 2; i++)
			{
				gfx.PutPixel(int(t.x) + i, int(t.y) + j, Colors::Red);
			}
		}
	}
}

void LineSeg::FollowFromHead(const Vec2& target) //Follow just means to translate directly to the target, nothing physically complex
{
	assert(!sex.empty());

	//Check if we are already here
	if ((target - GetHead().GetA()).GetLengthSq() > 0.0f)
	{
		//Let the first head follow the target
		auto& seg = sex.begin();
		seg->FollowFromHead(target);

		//And the rest follow the previous one's tail
		for (seg += 1; seg != sex.end(); ++seg)
		{
			seg->FollowFromHead((seg - 1)->GetB());
		}
	}
}

void LineSeg::AnchoredFollowFromHead(const Vec2& target)
{
	//FABRIK algorithm
	const Vec2 base(GetTail().GetB());

	for (unsigned char iter = 0; iter < fabrikMaxIterations; iter++)
	{
		if (iter % 2 == 0)
		{
			FollowFromHead(target);
		}
		else
		{
			FollowFromTail(base);
		}
	}

	//This is as a final measure to finalize the FABRIK algorithm with a connection to the base
	if (fabrikMaxIterations % 2 == 1)
	{
		FollowFromTail(base);
	}
}

void LineSeg::FollowFromTail(const Vec2& target) //Follow just means to translate directly to the target, nothing physically complex
{
	assert(!sex.empty());

	//Check if we are already there
	if ((target - GetTail().GetB()).GetLengthSq() > 0.0f)
	{
		//Let the last tail follow the target
		auto& seg = sex.rbegin();
		seg->FollowFromTail(target);

		//And the rest follow the previous one's head
		for (seg += 1; seg != sex.rend(); ++seg)
		{
			seg->FollowFromTail((seg - 1)->GetA());
		}
	}
}

void LineSeg::AnchoredFollowFromTail(const Vec2& target)
{
	//FABRIK algorithm
	const Vec2 base(GetHead().GetA());
	for (unsigned char iter = 0; iter < fabrikMaxIterations; iter++)
	{
		if (iter % 2 == 0)
		{
			FollowFromTail(target);
		}
		else
		{
			FollowFromHead(base);
		}
	}

	//This is as a final measure to finalize the FABRIK algorithm with a connection to the base
	if (fabrikMaxIterations % 2 == 1)
	{
		FollowFromHead(base);
	}
}

void LineSeg::MoveBy(const Vec2& delta)
{
	for (auto& s : sex)
	{
		s.MoveBy(delta);
	}
}

void LineSeg::MoveAt(const Vec2& newP)
{
	const Vec2 delta = newP - GetHead().GetA();
	MoveBy(delta);
}

void LineSeg::RotateSegment(unsigned int id, float dT)
{
	assert(id >= 0 && id < sex.size());

	const Vec2 oldB = sex[id].GetB();
	sex[id].RotateByA(dT); //The only actual piece of code is just this line but the rest is to optimize the fixing of the line segment
	const Vec2 delta = sex[id].GetB() - oldB;

	//Fast fix
	for (unsigned int i = id + 1; i < nSegments(); i++)
	{
		sex[i].MoveBy(delta);
	}
}

void LineSeg::RotateThroughSegment(unsigned int id, float dT)
{
	assert(id >= 0 && id < sex.size());

	for (int i = id; i < nSegments(); i++)
	{
		sex[i].RotateByA(dT);
	}


	for (auto& i = sex.begin() + id + 1; i != sex.end(); ++i)
	{
		i->MoveA_At((i - 1)->GetB());
	}
}

void LineSeg::Rotate(float dT)
{
	RotateThroughSegment(0, dT);
}

unsigned int LineSeg::nSegments() const
{
	return unsigned int(sex.size());
}

const LineSeg::Segment& LineSeg::GetHead() const
{
	return *sex.begin();
}

const LineSeg::Segment& LineSeg::GetTail() const
{
	return *sex.rbegin();
}

float LineSeg::GetLenght() const
{
	float sum = 0.0f;
	for (const auto& s : sex)
	{
		sum += s.GetLenght();
	}

	return sum;
}

void LineSeg::AddSegment()
{
	AddSegment(*sex.rbegin());
}

void LineSeg::AddSegment(const Segment& seg)
{
	sex.push_back(seg);
}

void LineSeg::RemoveSegment()
{
	if (!sex.empty())
	{
		sex.pop_back();
	}
}
#include "Graphics.h"
#include <vector>

struct LineSeg
{
private:
    //Variables
    class Segment
    {
    private:
        //Data
        Vec2 A = { 0.0f, 0.0f };
        float lenght = 0.0f;
        float angle = 0.0f;
    public:
        //Data
    private:
        //Functions
        void CapAngle();
    public:
        //Functions
        Segment() = default;
        Segment(Vec2 posA, Vec2 posB);
        Segment(Vec2 pos, float lenght, float angle);

        void FollowFromHead(const Vec2& target);
        void FollowFromTail(const Vec2& target);
        Vec2 GetB() const;
        Vec2 GetA() const;
        float GetAngle() const;
        float GetLenght() const;
        void MoveBy(const Vec2& delta);
        void MoveA_At(const Vec2& pos);
        void MoveB_At(const Vec2& pos);
        void RotateByA(float dA);
        void ChangeAngleAtA(float a);
        void RotateByB(float dB);
        void ChangeAngleAtB(float a);
    };

    std::vector<Segment> sex;
    Color color;
    static constexpr char fabrikMaxIterations = 6;
private:
    //Functions
public:
    //Variables
public:
    //Functions
    LineSeg() = default;
    LineSeg(int n, Vec2 s, float l, float a, Color c);

    void Draw(Graphics& gfx, bool drawJoints) const;

    void FollowFromHead(const Vec2& target);
    void AnchoredFollowFromHead(const Vec2& target);
    void FollowFromTail(const Vec2& target);
    void AnchoredFollowFromTail(const Vec2& target);
    void MoveBy(const Vec2& delta);
    void MoveAt(const Vec2& newP);
    void RotateSegment(unsigned int id, float dT);
    void RotateThroughSegment(unsigned int id, float dT);
    void Rotate(float dT);

    unsigned int nSegments() const;
    const Segment& GetHead() const;
    const Segment& GetTail() const;
    float GetLenght() const;
    void AddSegment();
    void AddSegment(const Segment& seg);
    void RemoveSegment();
};
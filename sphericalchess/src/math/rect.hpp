
#pragma once


namespace Math
{

template<typename C>
struct Rectangle2
{
    using Component = C;
    using Vector    = Vector2<Component>;

    Component x;              //!< Leftmost coordinate of rectangle.
    Component y;              //!< Topmost coordinate of rectangle.
    Component width;
    Component height;

    static Rectangle2 FromBounds(Vector a, Vector b)
    {
        Vector lower;
        Vector upper;

        std::tie(lower, upper) = MinMax(a, b);

        return Rectangle2{ lower, upper - lower };
    }

    Rectangle2() = default;
    
    Rectangle2(Component x, Component y, Component width, Component height) : x(x), y(y), width(width), height(height)
    {
    }

    Rectangle2(Vector position, Vector size) : x(position.x), y(position.y), width(size.x), height(size.y)
    {
    }

    template<typename T>
    Rectangle2(const Rectangle2<T>& a) : x(a.x), y(a.y), width(a.width), height(a.height)
    {
    }

    Component Left()  const { return x; }
    Component Right() const { return x + width; }
    Component Top()   const { return y; }
    Component Bot()   const { return y + height; }

    
    Vector Position() const { return { x, y }; }
    Vector Size()     const { return { width, height }; }

    
    Vector LeftTop()    const { return { Left(), Top() }; }
    Vector LeftCenter() const { return { Left(), CenterY() }; }
    Vector LeftBot()    const { return { Left(), Bot() }; }

    Vector CenterTop() const { return { CenterX(), Top() }; }
    Vector Center()    const { return { CenterX(), CenterY()}; }
    Vector CenterBot() const { return { CenterX(), Bot() }; }

    Vector RightTop()    const { return { Right(), Top() }; }
    Vector RightCenter() const { return { Right(), CenterY() }; }
    Vector RightBot()    const { return { Right(), Bot() }; }

private:

    Component CenterX() const { return x + width  / 2; }
    Component CenterY() const { return y + height / 2; }

};

using Rect2  = Rectangle2<float>;
using Rect2i = Rectangle2<int>;


}


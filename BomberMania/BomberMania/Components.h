#ifndef BREAKOUT_COMPONENTS_H
#define BREAKOUT_COMPONENTS_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Utilities.h"

#include "EntityManager.h"


struct Component
{
    bool		has{ false };
    Component() = default;
};

struct CShape : public Component
{

    std::shared_ptr<sf::Shape> shape{ nullptr };
    CShape() = default;

    CShape(std::shared_ptr<sf::Shape> shape, const sf::Color& fill, const sf::Color& outline = sf::Color::Black, float thickness = 1.f)
        : shape(shape)
    {
        shape->setFillColor(fill);
        shape->setOutlineColor(outline);
        shape->setOutlineThickness(thickness);

        centerOrigin(shape.get());
    }
};

struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;


    CSprite(const sf::Texture& t)
        : sprite(t) {
        centerOrigin(sprite);
    }


    CSprite(const sf::Texture& t, sf::IntRect r)
        : sprite(t, r) {
        centerOrigin(sprite);
    }
};

struct CAnimation : public Component {
    Animation   animation;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};

enum class TileType
{
    Destructable,
    None,
};

struct CTile : public Component
{
    CTile() = default;
    CTile(const TileType& t) : type(t) {}

    TileType type;
};

struct CBomb : public Component
{
    CBomb() : lifespan(sf::seconds(2.0f)), explosionSize(1), isOnBomb(true), player(nullptr) {}
    CBomb(sPtrEntt playerParameter) : lifespan(sf::seconds(2.0f)), explosionSize(1), isOnBomb(true), player(playerParameter) {}

    sf::Time lifespan;
    int explosionSize;
    bool isOnBomb;
    sPtrEntt player; // Para verifica??o de quem est? soltando a bomba.
};

struct COwner : public Component
{
    COwner() = default;
    COwner(sPtrEntt player) : owner(player) {}

    sPtrEntt owner;
};

struct CPlayer : public Component
{
    CPlayer() : fire(1), bomb(1), speed(1.0f), droped(0) {}

    int fire;   // tamanho da explos?o
    int bomb;   // quantas bombas ele pode soltar no total
    float speed;  // velocidade do player

    std::string sprite;
    int droped; // quantas bombas ele j? soltou na fase
};

struct CTransform : public Component
{

    sf::Transformable  tfm;
    sf::Vector2f	pos{ 0.f, 0.f };
    sf::Vector2f	prevPos{ 0.f, 0.f };
    sf::Vector2f	vel{ 0.f, 0.f };
    sf::Vector2f	scale{ 1.f, 1.f };

    float           angVel{ 0 };
    float	        angle{ 0.f };

    CTransform() = default;
    CTransform(const sf::Vector2f& p) : pos(p) {}
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v)
        : pos(p), prevPos(p), vel(v) {}

};


struct CCollision : public Component
{
    float radius{ 0.f };

    CCollision() = default;
    CCollision(float r)
        : radius(r) {}
};


struct CBoundingBox : public Component
{
    sf::Vector2f offset = { 0.0f, 0.0f };
    sf::Vector2f size = { 0.0f, 0.0f };
    sf::Vector2f halfSize = { 0.0f, 0.0f };

    CBoundingBox() = default;

    CBoundingBox(const sf::Vector2f& s)
        : size(s), halfSize(0.5f * s)
    {
    }

    CBoundingBox(const sf::Vector2f& s, const sf::Vector2f& offset)
        : offset(offset), size(s), halfSize(0.5f * s)
    {
    }
};

struct CState : public Component {
    std::string state{ "none" };

    CState() = default;
    CState(const std::string& s) : state(s) {}

};

struct CInput : public Component
{
    bool up{ false };
    bool left{ false };
    bool right{ false };
    bool down{ false };


    CInput() = default;
};


struct CScore : public Component
{
    CScore() = default;

    int score{ 0 };
};

#endif //BREAKOUT_COMPONENTS_H
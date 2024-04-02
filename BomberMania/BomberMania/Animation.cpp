//
// Created by David Burchill on 2022-11-24.
//

#include "Animation.h"
#include "Utilities.h"
#include "Assets.h"


Animation::Animation(const std::string& name,
    const sf::Texture& t,
    std::vector<sf::IntRect> frames,
    sf::Time tpf,
    bool repeats)
    : m_name(name)
    , m_frames(frames)
    , m_timePerFrame(tpf)
    , m_isRepeating(repeats)
    , m_countDown(tpf)
    , m_sprite(t, m_frames[0])
    , m_playing(false)
{
    centerOrigin(m_sprite);

    std::cout << name << " tpf: " << m_timePerFrame.asMilliseconds() << "ms\n";
}


void Animation::update(sf::Time dt)
{
    if (!m_playing)
        return;

    m_countDown -= dt;
    if (m_countDown < sf::Time::Zero) {
        m_countDown = m_timePerFrame;
        m_currentFrame += 1;

        if (m_currentFrame == m_frames.size() && !m_isRepeating)
            return;  // on the last frame of non-repeating animaton, leave it
        else
            m_currentFrame = (m_currentFrame % m_frames.size());

        changeFrame();
    }
}


bool Animation::hasEnded() const {
    return (m_currentFrame >= m_frames.size());
}


const std::string& Animation::getName() const {
    return m_name;
}


sf::Sprite& Animation::getSprite() {
    return m_sprite;
}

sf::Vector2f Animation::getBB() const {
    return static_cast<sf::Vector2f>(m_frames[m_currentFrame].getSize());
}

void Animation::play()
{
    if (!m_playing)
    {
        m_countDown = sf::Time::Zero;
        m_playing = true;
    }
}

void Animation::stop()
{
    m_playing = false;
    m_currentFrame = 0;

    changeFrame();
}

void Animation::changeFrame()
{
    m_sprite.setTextureRect(m_frames[m_currentFrame]);
    centerOrigin(m_sprite);
}

void Animation::changeTexture(const std::string& name)
{
    m_sprite.setTexture(Assets::getInstance().getTexture(name));
}
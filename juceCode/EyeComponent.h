#pragma once
#include <JuceHeader.h>

//==============================================================================
// Clickable eye that toggles between two states:
//   off → vertical slit pupil (cephalopod resting)
//   on  → round pupil + outer ring (active / parameter enabled)
//
// Set onStateChanged to wire up to a parameter.
//==============================================================================
class EyeComponent : public juce::Component
{
public:
    std::function<void (bool isOn)> onStateChanged;

    EyeComponent()
    {
        setSize (kW, kH);
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    void setLabel (const juce::String& text)
    {
        label = text;
        repaint();
    }

    bool getIsOn() const { return isOn; }

    //==========================================================================
    void paint (juce::Graphics& g) override
    {
        const float ecx = kW * 0.5f;
        const float ecy = kEyeY;
        const float R   = kRadius;

        // Hover ring
        if (isHovered)
        {
            g.setColour (lite.withAlpha (0.7f));
            g.drawEllipse (ecx - R * 1.25f, ecy - R * 1.25f,
                           R * 2.5f, R * 2.5f, 2.0f);
        }

        // Eye white
        g.setColour (juce::Colours::white);
        g.fillEllipse (ecx - R, ecy - R, R * 2.0f, R * 2.0f);

        if (!isOn)
        {
            // Vertical slit pupil
            g.setColour (pupil);
            juce::Path slit;
            slit.addRoundedRectangle (ecx - R * 0.18f, ecy - R * 0.41f,
                                      R * 0.36f,       R * 0.82f, 4.0f);
            g.fillPath (slit);

            // Specular highlight
            g.setColour (juce::Colours::white.withAlpha (0.63f));
            g.fillEllipse (ecx + R * 0.08f, ecy - R * 0.35f,
                           R * 0.27f, R * 0.27f);
        }
        else
        {
            // Round pupil
            g.setColour (pupil);
            g.fillEllipse (ecx - R * 0.59f, ecy - R * 0.59f,
                           R * 1.18f, R * 1.18f);

            // Specular highlight
            g.setColour (juce::Colours::white.withAlpha (0.67f));
            g.fillEllipse (ecx + R * 0.04f, ecy - R * 0.32f,
                           R * 0.28f, R * 0.28f);

            // Active glow ring
            g.setColour (lite);
            g.drawEllipse (ecx - R * 1.175f, ecy - R * 1.175f,
                           R * 2.35f, R * 2.35f, 2.0f);
        }

        // State label below eye
        g.setFont (juce::Font (juce::FontOptions ("Courier New", 9.0f,
                                                  juce::Font::plain)));
        g.setColour ((isOn ? dark : body).withAlpha (isOn ? 1.0f : 0.6f));
        g.drawText (label + (isOn ? " ON" : " OFF"),
                    0, (int)(ecy + R + 6.0f), kW, 14,
                    juce::Justification::centred);
    }

    //==========================================================================
    void mouseEnter (const juce::MouseEvent&) override { isHovered = true;  repaint(); }
    void mouseExit  (const juce::MouseEvent&) override { isHovered = false; repaint(); }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.position.getDistanceFrom ({ kW * 0.5f, kEyeY }) < kRadius + 4.0f)
        {
            isOn = !isOn;
            repaint();
            if (onStateChanged) onStateChanged (isOn);
        }
    }

    bool hitTest (int x, int y) override
    {
        return juce::Point<float> ((float) x, (float) y)
                   .getDistanceFrom ({ kW * 0.5f, kEyeY }) < kRadius + 4.0f;
    }

private:
    static constexpr float kRadius = 20.0f;
    static constexpr float kEyeY   = 24.0f;
    static constexpr int   kW      = 60;
    static constexpr int   kH      = 72;

    juce::String label  = "EYE";
    bool isOn           = false;
    bool isHovered      = false;

    const juce::Colour body  { 82,  73, 178 };
    const juce::Colour dark  { 44,  36, 118 };
    const juce::Colour lite  { 165, 160, 225 };
    const juce::Colour pupil { 22,  16,  65  };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EyeComponent)
};

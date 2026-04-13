#pragma once
#include <JuceHeader.h>

//==============================================================================
// Purely visual — no mouse interaction.
// Call setBodyCenter() after setBounds() to position the squid correctly.
//==============================================================================
class BodyComponent : public juce::Component
{
public:
    BodyComponent()
    {
        setInterceptsMouseClicks (false, false);
    }

    void setBodyCenter (float x, float y)
    {
        cx = x;
        cy = y;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        const float bw = 108.0f, bh = 152.0f;

        // ── Fins ──────────────────────────────────────────────────────────────
        g.setColour (dark);
        {
            juce::Path fin;
            fin.startNewSubPath (cx - bw * 0.50f, cy - bh * 0.06f);
            fin.lineTo          (cx - bw * 0.90f, cy + bh * 0.09f);
            fin.lineTo          (cx - bw * 0.50f, cy + bh * 0.23f);
            fin.closeSubPath();
            g.fillPath (fin);
        }
        {
            juce::Path fin;
            fin.startNewSubPath (cx + bw * 0.50f, cy - bh * 0.06f);
            fin.lineTo          (cx + bw * 0.90f, cy + bh * 0.09f);
            fin.lineTo          (cx + bw * 0.50f, cy + bh * 0.23f);
            fin.closeSubPath();
            g.fillPath (fin);
        }

        // ── Main body ─────────────────────────────────────────────────────────
        g.setColour (body);
        {
            juce::Path shape;
            shape.startNewSubPath (cx,             cy - bh * 0.57f);
            shape.cubicTo (cx + bw * 0.54f, cy - bh * 0.48f,
                           cx + bw * 0.57f, cy + bh * 0.09f,
                           cx + bw * 0.48f, cy + bh * 0.44f);
            shape.cubicTo (cx + bw * 0.28f, cy + bh * 0.57f,
                           cx - bw * 0.28f, cy + bh * 0.57f,
                           cx - bw * 0.48f, cy + bh * 0.44f);
            shape.cubicTo (cx - bw * 0.57f, cy + bh * 0.09f,
                           cx - bw * 0.54f, cy - bh * 0.48f,
                           cx,             cy - bh * 0.57f);
            shape.closeSubPath();
            g.fillPath (shape);
        }

        // ── Center line ───────────────────────────────────────────────────────
        g.setColour (lite);
        g.drawLine (cx, cy - bh * 0.44f,
                    cx, cy + bh * 0.30f, 1.5f);
    }

private:
    float cx = 0.0f, cy = 0.0f;

    const juce::Colour body { 82,  73, 178 };
    const juce::Colour dark { 44,  36, 118 };
    const juce::Colour lite { 165, 160, 225 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BodyComponent)
};

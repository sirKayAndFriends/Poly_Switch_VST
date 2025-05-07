#pragma once
#include "JuceHeader.h"
#include "skVibes.h"

void SKVibes::createDivisions(int width, int height, int numX, int numY, int margin)
{
    float newWidth = width - (margin * 2.0);
    float newHeight = height - (margin * 2.0);

    numGridX = numX;
    numGridY = numY;

    unitWidth = newWidth / numX;
    unitHeight = newHeight / numY;

    float startX = margin;
    float startY = margin;

    for (int j = 0; j < numY; j++)
    {
        for (int i = 0; i < numX; i++)
        {
            gridSquare.push_back(juce::Rectangle<float>(startX, startY, unitWidth, unitHeight));
            startX += unitWidth;
        }
        startX = margin;
        startY += unitHeight;
    }
}

void SKVibes::createSubDivisions(int numSubDivisions)
{
    float subWidth = gridSquare[0].getWidth() / numSubDivisions;
    float subHeight = gridSquare[0].getHeight() / numSubDivisions;
    float subXStart = gridSquare[0].getX();
    float subYStart = gridSquare[0].getY();

    numSubGrid = numSubDivisions;
    
    for (int k = 0; k < gridSquare.size(); k++)
    {
        for (int i = 0; i < numSubDivisions; i++)
        {
            for (int j = 0; j < numSubDivisions; j++)
            {
                gridSubSquares.push_back(juce::Rectangle<float>(subXStart, subYStart, subWidth, subHeight));
                subXStart += subWidth;
            }
            subYStart += subHeight;
            subXStart = gridSquare[k].getX();
        }
        subXStart = gridSquare[k].getX();
        subYStart = gridSquare[k].getY();
    }
}

std::vector<juce::Rectangle<float>>& SKVibes::getGrid()
{
    return gridSquare;
}

std::vector<juce::Rectangle<float>>& SKVibes::getSubGrid()
{
    return gridSubSquares;
}

void SKVibes::getGridCentres()
{
    for (int i = 0; i < gridSquare.size(); i++)
    {
        gridCentres.push_back(juce::Point<float>(gridSquare[i].getCentreX(), gridSquare[i].getCentreY()));
    }
}

juce::Point<float>& SKVibes::getGridRectPoint(int row, int col)
{
    int index = (col * numGridX) + row;
    return juce::Point<float>(gridSquare[index].getX(), gridSquare[index].getY());
}

juce::Rectangle<float>& SKVibes::getGridRect(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridSquare[index];
}

juce::Point<float> SKVibes::getGridRectBottomLeft(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridSquare[index].getBottomLeft();
}

juce::Point<float> SKVibes::getGridRectBottomRight(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridSquare[index].getBottomRight();
}

juce::Point<float> SKVibes::getGridRectTopLeft(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridSquare[index].getTopLeft();
}

juce::Point<float> SKVibes::getGridRectTopRight(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridSquare[index].getTopRight();
}

std::vector<float> SKVibes::getUnitSize()
{
    return std::vector<float> {gridSquare[0].getWidth(), gridSquare[0].getHeight()};
}

juce::Point<float> SKVibes::drawFromCentre(int row, int col, float width, float height)
{
    int index = (col * numGridX) + row;
    return juce::Point<float>(gridCentres[index].x - width / 2.0, gridCentres[index].y - height / 2.0);
}

juce::Point<float> SKVibes::drawFromEdge(int row, int col, float width, float height)
{
    int index = (col * numGridX) + row;
    return juce::Point<float>(gridSquare[index].getX() - width / 2.0, gridSquare[index].getY() - height / 2.0);
}

juce::Point<float>& SKVibes::convertToBottomRight(int row, int col, float width, float height)
{
    int index = (col * numGridX) + row;
    return juce::Point<float>(gridSquare[index].getX() - width, gridSquare[index].getY() - height);
}

juce::Point<float>& SKVibes::getGridCentrePoint(int row, int col)
{
    int index = (col * numGridX) + row;
    return gridCentres[index];
}

void SKVibes::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
   
    auto radius = (float)juce::jmin(width / 2, height / 2);
    auto knobRect = juce::Rectangle<float>(x, y, width, height);
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

    juce::Point<float> centre(knobRect.getCentreX(), knobRect.getCentreY());

    g.setColour(fill);
    g.fillEllipse(knobRect);

    juce::Path p;
    auto pointerThickness = 4.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, radius);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centre));

    auto centreDot = juce::Rectangle<float>(pointerThickness, pointerThickness).withCentre(centre);

    g.setColour(juce::Colours::black);
    g.fillPath(p);
    g.fillEllipse(centreDot);

}

struct panelParams SKVibes::setupPanels(float x, float y, double width, double height, int outerCol, int innerCol)
{
    panelParams params;

    params.outerColour = juce::Colour(outerCol);
    params.innerColour = juce::Colour(innerCol);

    params.position = juce::Point<float>(x, y);
    params.width = width;
    params.height = height;

    return params;
}


void SKVibes::drawPanel(juce::Graphics& g, double x, double y, double width, double height, int bgCol)
{
    juce::Rectangle<float> bgPanel(x, y, width, height);
    g.setColour(juce::Colour(bgCol));
    g.fillRect(bgPanel);
}

void SKVibes::drawDoublePanel(juce::Graphics& g, panelParams* panel)
{
    float cornerRound = 6.0f;
    juce::Rectangle<float> outerPanel(panel->position.x, panel->position.y, panel->width, panel->height);
    juce::Rectangle<float> innerPanel(panel->position.x + 10.0, panel->position.y + 10.0, panel->width - (10.0 * 2.0f), panel->height - (10.0 * 2.0f));
    g.setColour(panel->outerColour);
    g.fillRoundedRectangle(outerPanel, cornerRound);
    g.setColour(panel->innerColour);
    g.fillRoundedRectangle(innerPanel, cornerRound);
}

void SKVibes::drawLabels(juce::Slider* knob, juce::String& text)
{
    juce::Label label;
    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setFont(SKVibes::getPluginFont());
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::ColourIds::textColourId, juce::Colour(juce::Colours::black));
    juce::Rectangle<int> knobBounds = knob->getBounds();
    /*juce::Point<int> centre(knobBounds.getCentreX(), knobBounds.getCentreY() + knobBounds.getHeight() / 4);
    auto border = juce::Rectangle<int>(width, height).withCentre(centre);*/
    label.setBounds(knobBounds.getX(), knobBounds.getY(), knobBounds.getWidth(), knobBounds.getHeight());
}

struct visualParams SKVibes::setVisualParams(int x, int y, int width, int height)
{
    visualParams params;

    params.lineStartX = x;
    params.lineY = y;

    params.visualWidth = width;
    params.lineEndX = params.lineStartX + params.visualWidth; //450
    //CONTROL DOT (central point) defines other points
    params.visualCurveWidth = params.visualWidth / 4.0f; //100
    params.controlMin = params.lineStartX + (params.visualCurveWidth / 2); //50 + 50
    params.controlMax = params.lineEndX - (params.visualCurveWidth / 2); //450 - 50  
    params.controlWidth = params.controlMax - params.controlMin;
    //bounds = 100 -> 400 = 300
    params.maxY = params.lineY - height;
    params.humpHeight = params.lineY - params.maxY;
    
    return params;
}

void SKVibes::drawMidVisual(juce::Graphics& g, visualParams* params, float& peakX, float& peakY)
{
    float lineThickness = 2.5f;

    auto centre = juce::Point<float>(params->lineStartX + (params->visualWidth / 2), params->lineY);
    auto box = juce::Rectangle<float>(params->visualWidth + (lineThickness * 2), params->humpHeight + (lineThickness * 2)).withCentre(centre);

    params->lineStart.setXY(params->lineStartX, params->lineY);//100
    params->lineEnd.setXY(params->lineEndX, params->lineY);//400
    params->curveStart.setXY(peakX - (params->visualCurveWidth / 2), params->lineY);//200
    params->curveEnd.setXY(peakX + (params->visualCurveWidth / 2), params->lineY);//300
    params->controlDot.setXY(peakX, peakY);

    params->curve.clear();
    params->curve.startNewSubPath(params->lineStart);
    params->curve.lineTo(params->curveStart);
    params->curve.quadraticTo(params->controlDot, params->curveEnd);
    params->curve.lineTo(params->lineEnd);

    g.setColour(juce::Colour(juce::Colours::black));
    g.strokePath(params->curve, juce::PathStrokeType(lineThickness));
    g.drawRoundedRectangle(box, 6.0f, lineThickness);
}

void SKVibes::diodeInit(juce::Button* button, float x, float y, int size, int colour, bool direction, bool state, int index)
{
    juce::Point<float> tmpMidPoint(x + size / 2.0, y + size / 2.0);
    diodes[index]->button = button;
    diodes[index]->position = juce::Point<float>(x, y);
    diodes[index]->bounds = juce::Rectangle<float>(x, y, size, size);
    diodes[index]->midPoint = tmpMidPoint;
    
    diodes[index]->triangle = 
    {
        {juce::Point<float>(tmpMidPoint.x, y + size * 0.25)},
        {juce::Point<float>(tmpMidPoint.x - (size * 0.25), y + size - (size / 3.0))},
        {juce::Point<float>(tmpMidPoint.x + (size * 0.25), y + size - (size / 3.0))}
    };

    diodes[index]->wire =
    {
        {juce::Point<float>(tmpMidPoint.x, y)},
        {juce::Point<float>(tmpMidPoint.x, y + size)}
    };

    diodes[index]->cathode =
    {
        {juce::Point<float>(tmpMidPoint.x - (size * 0.25), y + size * 0.25)},
        {juce::Point<float>(tmpMidPoint.x + (size * 0.25), y + size * 0.25)}
    };

    diodes[index]->softColour = colour;
    diodes[index]->direction = direction;
    diodes[index]->unit = size;
    diodes[index]->thickness = 4.0;
    diodes[index]->state = state;

    if (state == true)
    {
        diodes[index]->hardColour = skColours::skTomato;
    }

    else
    {
        diodes[index]->hardColour = skColours::skCocoa;
    }
}

void SKVibes::drawDiode(juce::Graphics& g, int index, bool state)
{
    juce::Path diode;

    if (state == true)
    {
        g.setColour(juce::Colour(diodes[index]->hardColour));
    }

    else
    {
        g.setColour(juce::Colour(diodes[index]->softColour));
    } 

    diode.addTriangle(diodes[index]->triangle[0], diodes[index]->triangle[1], diodes[index]->triangle[2]);
    diode.startNewSubPath(diodes[index]->wire[0]);
    diode.lineTo(diodes[index]->wire[1]);
    diode.startNewSubPath(diodes[index]->cathode[0]);
    diode.lineTo(diodes[index]->cathode[1]);

    if (diodes[index]->direction == true)
    {
        diode.applyTransform(juce::AffineTransform().rotated(juce::MathConstants<float>::pi, diodes[index]->midPoint.x, diodes[index]->midPoint.y));
    }

    g.strokePath(diode, juce::PathStrokeType(diodes[index]->thickness));
    g.fillPath(diode);

    g.setColour(juce::Colour(skColours::skCocoa));
    g.drawEllipse(diodes[index]->bounds, diodes[index]->thickness);
}

void SKVibes::diodeSort(juce::Button* button, bool state)
{
    for (int i = 0; i < diodes.size(); i++)
    {
        if (button == diodes[i]->button)
        {
            if (state == true)
            {
                diodes[i]->hardColour = skTomato;
                break;
            } 
            
            else if(state == false)
            {
                diodes[i]->hardColour = skCocoa;
                break;
            }          
        }
    }
}
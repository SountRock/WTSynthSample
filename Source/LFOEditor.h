/*
  ==============================================================================

    LFOEditor.h
    Created: 6 May 2021 5:37:07pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "LFO.h"
#include "ModulationSystem.h"
#include "RgbColor.h"
#define REPAINT_FPS 24
using Constrainer = juce::ComponentBoundsConstrainer;
using fPoint = juce::Point<float>;

class LFOPoint
{
public:
    LFOPoint(float x, float y, bool isBezier, bool isSplit=false) :
    xPos(x),
    yPos(y),
    bezier(isBezier),
    split(isSplit)
    {
        
    }
    void setFrontHandle(float newX, float newY)
    {
        frontHandleX = newX;
        frontHandleY = newY;
        if(!split)
        {
            auto dX = newX - xPos;
            auto dY = newY - yPos;
            rearHandleX = xPos - dX;
            rearHandleY = yPos - dY;
        }
    }
    void setRearHandle(float newX, float newY)
    {
        rearHandleX = newX;
        rearHandleY = newY;
        if(!split)
        {
            auto dX = xPos - newX;
            auto dY = yPos - newY;
            frontHandleX = xPos + dX;
            frontHandleY = yPos + dY;
        }
    }
    void setPosition(float newX, float newY)
    {
        auto dX = newX - xPos;
        auto dY = newY - yPos;
        yPos = newY;
        xPos = newX;
        if(bezier)
        {
            setFrontHandle(xPos + dX, yPos + dX);
            setRearHandle(xPos - dX, yPos - dY);
        }
    }
    fPoint getCenter() {return fPoint(xPos, yPos);}
    fPoint getFront() {return fPoint(frontHandleX, frontHandleY);}
    fPoint getRear() {return fPoint(rearHandleX, rearHandleY);}
    bool isBezier() {return bezier;}
    void setBezier(bool should) {bezier = should; }
    void setSplit(bool should) {split = should; }
private:
    float xPos, yPos;
    bool bezier;
    bool split;
    float frontHandleX;
    float frontHandleY;
    float rearHandleX;
    float rearHandleY;
};

class LFOShape
{
public:
    LFOShape();
    void applyToArray();
    void addPoint(LFOPoint* newPoint)
    {
        points.push_back(newPoint);
    }
    void removePoint(LFOPoint* toRemove)
    {
        int idx = 0;
        for(auto point : points)
        {
            if(point == toRemove)
            {
                points.erase(points.begin() + idx);
                return;
            }
            ++idx;
        }
    }
    std::vector<LFOPoint*> points;
    lfoArray outputArray;
};

class PointHandle : public juce::Component
{
public:
    PointHandle(juce::Component* parent=nullptr, float x=0.5f, float y = 0.5f) :
    dX(0),
    parentComp(parent),
    relativeCenter(fPoint(x, y))
    {
        resized();
        setRepaintsOnMouseActivity(true);
    }
    void resized() override
    {
        setSize(dX, dX);
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }
    void paint(juce::Graphics& g) override
    {
        g.setColour(UXPalette::lightGray);
        g.fillEllipse(getLocalBounds().toFloat());
    }
    void mouseDown(const juce::MouseEvent& e) override
    {
        dragger.startDraggingComponent(this, e);
    }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        dragger.dragComponent(this, e, &constrainer);
    }
    void moved() override
    {
        if(parentComp == nullptr)
            relativeCenter = getBounds().getCentre().toFloat() / juce::Point<int>(getParentWidth(), getParentHeight()).toFloat();
        else
        {
            relativeCenter = getBounds().getCentre().toFloat() / juce::Point<int>(parentComp->getWidth(), parentComp->getHeight()).toFloat();
        }
    }
    fPoint getCenter()
    {
        return relativeCenter;
    }
    void setCenter(float x, float y) {relativeCenter = fPoint(x, y); }
    void startDrag(const juce::MouseEvent& e) {dragger.startDraggingComponent(this, e); }
    void drag(const juce::MouseEvent& e) {dragger.dragComponent(this, e, &constrainer); }
    int dX;
    Constrainer constrainer;
    juce::ComponentDragger dragger;
    juce::Component* parentComp; //! so that the center point can be relative to a higher level component rather than the immediate parent
protected:
    fPoint relativeCenter;
};

class CenterHandle : public PointHandle
{
public:
    CenterHandle(PointHandle* front, PointHandle* rear, juce::Component* container=nullptr, float x=0.5f, float y=0.5f) :
    PointHandle(container, x, y),
    frontComp(front),
    rearComp(rear)
    {
        
    }
    void mouseDown(const juce::MouseEvent& e) override
    {
        dragger.startDraggingComponent(this, e);
        if(frontComp != nullptr)
            frontComp->startDrag(e);
        if(rearComp != nullptr)
            rearComp->startDrag(e);
    }
    void mouseDrag(const juce::MouseEvent& e) override
    {
        dragger.dragComponent(this, e, &constrainer);
        if(frontComp != nullptr)
            frontComp->drag(e);
        if(rearComp != nullptr)
            rearComp->drag(e);
    }
    PointHandle* const frontComp;
    PointHandle* const rearComp;
};

class CurveHandle : public PointHandle
{
public:
    CurveHandle(juce::Component* container=nullptr) : PointHandle(nullptr)
    {
        
    }
};

enum class PointType
{
    Linear,
    Bezier,
    Split
};



class LFOEditor : public juce::Component, juce::Timer
{
public:
    LFOEditor(lfoArray* arr);
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void connectPoints(LFOPoint* first, LFOPoint* second); //!  sets the array's values for this sector of the shape, ensures the corrects paths are drawn
    void addPoint();
    void calculatePaths();
    void removePoint(int idx);
    juce::Rectangle<float> fBounds;
    LFOPoint startPoint;
    LFOPoint endPoint;
    juce::OwnedArray<CenterHandle> pointHandles;
    juce::OwnedArray<juce::Path> paths;
    lfoArray* const linkedArray;
    lfoArray dataArray;
};

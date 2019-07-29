#pragma once
#include "scanView.h"

class PlotBackground
{
private:
	CScanView m_scanview;
public:
	PlotBackground();
	~PlotBackground();
	void ChoiceBackground(CDC &memDC, int calibration_X, int calibration_Y, int point);
	void DrawPlotBackground(CDC &memDC, int calibration_X, int calibration_Y);
	void OnDeskStandardPlot(CDC &memDC, int deskcenterX, int deskcenterY);
	float OndistXtoendPtX(float distX);
	float OndistYtoendPtY(float distY);
};


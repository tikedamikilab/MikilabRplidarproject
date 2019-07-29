#include "stdafx.h"
#include "PlotBackground.h"

PlotBackground::PlotBackground()
{
}
PlotBackground::~PlotBackground()
{
}
void PlotBackground::ChoiceBackground(CDC &memDC, int calibration_X, int calibration_Y, int point) {
	if (point == 0) {
		DrawPlotBackground(memDC, calibration_X, calibration_Y);
	}
	else if (point == 1) {
		DrawPlotBackground(memDC, calibration_X, calibration_Y);
	}
	else {
		DrawPlotBackground(memDC, calibration_X, calibration_Y);
	}
}
void PlotBackground::DrawPlotBackground(CDC &memDC, int calibration_X, int calibration_Y) {
	CPen backpen;
	backpen.CreatePen(PS_SOLID, 2, RGB(128, 128, 128));
	//backpen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	memDC.SelectPen(backpen);
	//外枠
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2992 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2992 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4180 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4180 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(10000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(10000 + calibration_X), OndistYtoendPtY(-983 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-983 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4900 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4900 + calibration_X), OndistYtoendPtY(-4490 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4500 + calibration_X), OndistYtoendPtY(-4490 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4500 + calibration_X), OndistYtoendPtY(2740 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-6000 + calibration_X), OndistYtoendPtY(2740 - calibration_Y));
	//集中
	memDC.MoveTo(OndistXtoendPtX(3420 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3420 + calibration_X), OndistYtoendPtY(-3330 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2400 + calibration_X), OndistYtoendPtY(-3330 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2400 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
	//和室と多目的のパーティション
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(4000 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7450 + calibration_X), OndistYtoendPtY(755 - calibration_Y));
	//机
	OnDeskStandardPlot(memDC, 0 + calibration_X, 0 - calibration_Y);
	OnDeskStandardPlot(memDC, -2465 + calibration_X, 1749 - calibration_Y);
	OnDeskStandardPlot(memDC, -2318 + calibration_X, -1933 - calibration_Y);
	OnDeskStandardPlot(memDC, 2428 + calibration_X, -2018 - calibration_Y);
	OnDeskStandardPlot(memDC, 2155 + calibration_X, 1950 - calibration_Y);
	//擬似窓前のキャビネット
	memDC.MoveTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-2290 + calibration_X), OndistYtoendPtY(3700 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(3700 - calibration_Y));
	memDC.MoveTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3240 + calibration_X), OndistYtoendPtY(3470 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3890 + calibration_X), OndistYtoendPtY(3470 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(3890 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	//配席PC
	memDC.MoveTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(4090 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-5220 + calibration_X), OndistYtoendPtY(3750 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4090 + calibration_X), OndistYtoendPtY(3750 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(-4090 + calibration_X), OndistYtoendPtY(4420 - calibration_Y));
	//多目的
	memDC.MoveTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(1483 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7215 + calibration_X), OndistYtoendPtY(1483 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(7215 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(4815 + calibration_X), OndistYtoendPtY(2883 - calibration_Y));
	//和室
	memDC.MoveTo(OndistXtoendPtX(9250 + calibration_X), OndistYtoendPtY(-1200 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6506 + calibration_X), OndistYtoendPtY(-1200 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(6506 + calibration_X), OndistYtoendPtY(-2298 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5769 + calibration_X), OndistYtoendPtY(-2298 - calibration_Y));
	memDC.LineTo(OndistXtoendPtX(5769 + calibration_X), OndistYtoendPtY(-5050 - calibration_Y));
}
inline void PlotBackground::OnDeskStandardPlot(CDC &memDC, int deskcenterX, int deskcenterY) {
	//机
	memDC.MoveTo(OndistXtoendPtX(1000 + deskcenterX), OndistYtoendPtY(130 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(-130 + deskcenterX), OndistYtoendPtY(1000 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(-1000 + deskcenterX), OndistYtoendPtY(-130 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(130 + deskcenterX), OndistYtoendPtY(-1000 + deskcenterY));
	memDC.LineTo(OndistXtoendPtX(1000 + deskcenterX), OndistYtoendPtY(130 + deskcenterY));
}
inline float PlotBackground::OndistXtoendPtX(float distX) {
	return distX * m_scanview.getDistscal() + m_scanview.getCenterPt().x;
}
inline float PlotBackground::OndistYtoendPtY(float distY) {
	return m_scanview.getCenterPt().y - distY * m_scanview.getDistscal();
}
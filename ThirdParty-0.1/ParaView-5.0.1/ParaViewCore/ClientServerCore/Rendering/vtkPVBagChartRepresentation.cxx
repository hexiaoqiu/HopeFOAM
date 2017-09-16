/*=========================================================================

  Program:   ParaView
  Module:    vtkPVBagChartRepresentation.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVBagChartRepresentation.h"

#include "vtkAppendPolyData.h"
#include "vtkBrush.h"
#include "vtkCellArray.h"
#include "vtkChartXY.h"
#include "vtkColor.h"
#include "vtkColorTransferFunction.h"
#include "vtkCommand.h"
#include "vtkContextView.h"
#include "vtkContourFilter.h"
#include "vtkDataObjectToTable.h"
#include "vtkDoubleArray.h"
#include "vtkExtractBlock.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkInformationVector.h"
#include "vtkLookupTable.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPen.h"
#include "vtkPlotBag.h"
#include "vtkPlotHistogram2D.h"
#include "vtkPlotLine.h"
#include "vtkPointData.h"
#include "vtkProcessModule.h"
#include "vtkPVContextView.h"
#include "vtkTable.h"

#include <algorithm>
#include <string>

vtkStandardNewMacro(vtkPVBagChartRepresentation);
vtkCxxSetObjectMacro(vtkPVBagChartRepresentation,LookupTable,vtkScalarsToColors)

//----------------------------------------------------------------------------
vtkPVBagChartRepresentation::vtkPVBagChartRepresentation() :
  LineThickness(1),
  LineStyle(0),
  LookupTable(NULL),
  BagVisibility(0),
  Opacity(1.),
  PointSize(5),
  GridLineThickness(1),
  GridLineStyle(0),
  XAxisSeriesName(NULL),
  YAxisSeriesName(NULL),
  DensitySeriesName(NULL),
  UseIndexForXAxis(true)
{
  this->BagColor[0] = 1.0;
  this->BagColor[1] = this->BagColor[2] = 0.0;
  this->SelectionColor[0] = this->SelectionColor[2] = 1.0;
  this->SelectionColor[1] = 0.0;
  this->LineColor[0] = this->LineColor[1] = this->LineColor[2] = 0.0;
  this->PointColor[0] = this->PointColor[1] = this->PointColor[2] = 0.0;
  this->P99Color[0] = this->P99Color[2] = 0.5;
  this->P99Color[1] = 0.0;
  this->P50Color[0] = this->P50Color[2] = 0.2;
  this->P50Color[1] = 0.0;

  vtkNew<vtkColorTransferFunction> lut;
  lut->SetColorSpaceToDiverging();
  lut->AddRGBPoint(0, 59. / 255., 76. / 255., 192. / 255.);
  lut->AddRGBPoint(1, 221. / 255., 221. / 255., 221. / 255.);
  lut->AddRGBPoint(2, 180. / 255., 4. / 255., 38. / 255.);
  SetLookupTable(lut.Get());
}

//----------------------------------------------------------------------------
vtkPVBagChartRepresentation::~vtkPVBagChartRepresentation()
{
  SetLookupTable(NULL);
}

//----------------------------------------------------------------------------
void vtkPVBagChartRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
bool vtkPVBagChartRepresentation::AddToView(vtkView* view)
{
  if (!this->Superclass::AddToView(view))
    {
    return false;
    }

  if (this->GetChart())
    {
    this->GetChart()->SetVisible(this->GetVisibility());
    }

  return true;
}

//----------------------------------------------------------------------------
bool vtkPVBagChartRepresentation::RemoveFromView(vtkView* view)
{
  if (this->GetChart())
    {
    this->GetChart()->GetPlot(0)->SetInputData(0);
    this->GetChart()->SetVisible(false);
    }

  return this->Superclass::RemoveFromView(view);
}

//----------------------------------------------------------------------------
void vtkPVBagChartRepresentation::SetVisibility(bool visible)
{
  this->Superclass::SetVisibility(visible);

  vtkChartXY* chart = this->GetChart();
  if (chart && !visible)
    {
    // Refer to vtkChartRepresentation::PrepareForRendering() documentation to
    // know why this is cannot be done in PrepareForRendering();
    chart->SetVisible(false);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
vtkChartXY* vtkPVBagChartRepresentation::GetChart()
{
  if (this->ContextView)
    {
    return vtkChartXY::SafeDownCast(
      this->ContextView->GetContextItem());
    }

  return 0;
}

//----------------------------------------------------------------------------
void vtkPVBagChartRepresentation::PrepareForRendering()
{
  this->Superclass::PrepareForRendering();

  vtkChartXY* chart = this->GetChart();
  vtkPlotBag* bagPlot = 0;
  vtkPlotHistogram2D* gridPlot = 0;
  vtkPlotLine* p50LinePlot = 0;
  vtkPlotLine* p99LinePlot = 0;
  for (int i = 0; i < chart->GetNumberOfPlots() &&
                 (!bagPlot || !gridPlot || !p50LinePlot || !p99LinePlot); i++)
    {
    // Search for the first bag plot of the chart. For now
    // the chart only manage one bag plot
    if (vtkPlotBag::SafeDownCast(chart->GetPlot(i)))
      {
      bagPlot = vtkPlotBag::SafeDownCast(chart->GetPlot(i));
      }
    if (vtkPlotHistogram2D::SafeDownCast(chart->GetPlot(i)))
      {
      gridPlot = vtkPlotHistogram2D::SafeDownCast(chart->GetPlot(i));
      }
    if (vtkPlotLine::SafeDownCast(chart->GetPlot(i)))
      {
      if (p50LinePlot == NULL)
        {
        p50LinePlot = vtkPlotLine::SafeDownCast(chart->GetPlot(i));
        }
      else
        {
        p99LinePlot = vtkPlotLine::SafeDownCast(chart->GetPlot(i));
        }
      }
    }
  if (!gridPlot)
    {
    gridPlot = vtkPlotHistogram2D::New();
    chart->AddPlot(gridPlot);
    gridPlot->Delete();
    }
  if (!p50LinePlot)
    {
    p50LinePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
    p50LinePlot->SetLabel("P50");
    p50LinePlot->PolyLineOff();
    p50LinePlot->SelectableOff();
    }
  if (!p99LinePlot)
    {
    p99LinePlot = vtkPlotLine::SafeDownCast(chart->AddPlot(vtkChart::LINE));
    p99LinePlot->SetLabel("P99");
    p99LinePlot->PolyLineOff();
    p99LinePlot->SelectableOff();
    }
  if (!bagPlot)
    {
    // Create and add a bag plot to the chart
    bagPlot = vtkPlotBag::SafeDownCast(chart->AddPlot(vtkChart::BAG));
    }
  
  vtkTable* plotInput = this->GetLocalOutput();
  bool hasBagPlot = plotInput && plotInput->GetNumberOfRows() > 0;
  chart->SetVisible(
    (hasBagPlot  || this->LocalGrid != NULL) && this->GetVisibility());
  bagPlot->SetVisible(hasBagPlot);
  bagPlot->SetBagVisible(this->BagVisibility != 0);

  // Set line (bag boundaries) pen properties
  bagPlot->GetLinePen()->SetWidth(this->LineThickness);
  bagPlot->GetLinePen()->SetLineType(this->LineStyle);
  bagPlot->GetLinePen()->SetColorF(this->LineColor);
  bagPlot->GetLinePen()->SetOpacityF(this->Opacity);

  // Set bag (polygons) brush properties
  bagPlot->GetBrush()->SetColorF(this->BagColor);
  bagPlot->GetBrush()->SetOpacityF(this->Opacity);

  // Set point pen properties
  bagPlot->GetPen()->SetWidth(this->PointSize);
  bagPlot->GetPen()->SetLineType(this->LineStyle);
  bagPlot->GetPen()->SetColorF(this->PointColor);
  bagPlot->GetPen()->SetOpacityF(1.0);

  // Set selection point pen properties
  bagPlot->GetSelectionPen()->SetColorF(this->SelectionColor);

  if (hasBagPlot)
    {
    // We only consider the first vtkTable.
    if (this->YAxisSeriesName && this->DensitySeriesName)
      {
      bagPlot->SetUseIndexForXSeries(this->UseIndexForXAxis);
      if (this->UseIndexForXAxis)
        {
        bagPlot->SetInputData(plotInput,
          this->YAxisSeriesName, this->DensitySeriesName);
        }
      else
        {
        bagPlot->SetInputData(plotInput, this->XAxisSeriesName,
          this->YAxisSeriesName, this->DensitySeriesName);
        }
      }
    }

  gridPlot->SetInputData(this->LocalGrid);
  gridPlot->SetVisible(this->LocalGrid != NULL);
  p50LinePlot->SetVisible(this->LocalGrid != NULL);
  p99LinePlot->SetVisible(this->LocalGrid != NULL);

  if (this->LocalGrid)
    {
    double range[2];
    this->LocalGrid->GetScalarRange(range);

    gridPlot->SetTransferFunction(GetLookupTable());
    if (gridPlot->GetTransferFunction())
    {
      vtkLookupTable* lut = vtkLookupTable::SafeDownCast(
        gridPlot->GetTransferFunction());
      vtkColorTransferFunction* tf =
        vtkColorTransferFunction::SafeDownCast(gridPlot->GetTransferFunction());
      if (lut)
        {
        lut->SetTableRange(range);
        lut->Build();
        }
      else if (tf)
        {
        double oldRange[2];
        tf->GetRange(oldRange);
        int numberOfNodes = tf->GetSize();
        double* newNodes = new double[6*numberOfNodes];
        for (int i = 0; i < numberOfNodes; ++i)
          {
          double* newNode = &newNodes[6*i];
          tf->GetNodeValue(i, newNode);
          newNode[0] = (range[1] - range[0]) * (newNode[0] - oldRange[0])
            / (oldRange[1] - oldRange[0]) + range[0];
          }
        tf->RemoveAllPoints();
        for (int i = 0; i < numberOfNodes; ++i)
          {
          double* newNode = &newNodes[6*i];
          tf->AddRGBPoint(newNode[0], newNode[1], newNode[2], newNode[3],
                          newNode[4], newNode[5]);
          }
        delete [] newNodes;
        }
    }

    vtkDataArray* densities = this->LocalGrid->GetPointData()->GetScalars();
    double totalSumOfDensities = 0.;
    std::vector<double> sortedDensities;
    sortedDensities.reserve(densities->GetNumberOfTuples());
    for (vtkIdType pixel = 0; pixel < densities->GetNumberOfTuples(); ++pixel)
      {
      double density = densities->GetTuple1(pixel);
      sortedDensities.push_back(density);
      totalSumOfDensities += density;
      }
    std::sort(sortedDensities.begin(), sortedDensities.end());
    double sumOfDensities = 0.;
    double sumForMedian= totalSumOfDensities * 0.5;
    double sumForP99 = totalSumOfDensities * 0.01;
    double median = 0.;
    double p99 = 0.;
    for (std::vector<double>::const_iterator it = sortedDensities.begin();
         it != sortedDensities.end(); ++it)
      {
      sumOfDensities += *it;
      if (sumOfDensities >= sumForMedian && median == 0.)
        {
        median = *it;
        break;
        }
      if (sumOfDensities >= sumForP99 && p99 == 0.)
        {
        p99 = *it;
        }
      }

    vtkNew<vtkContourFilter> medianContour;
    medianContour->SetInputData(this->LocalGrid);
    medianContour->SetNumberOfContours(1);
    medianContour->SetValue(0, median);
    medianContour->Update();

    vtkNew<vtkTable> medianTable;
    SetPolyLineToTable(vtkPolyData::SafeDownCast(
      medianContour->GetOutput()), medianTable.Get());
    p50LinePlot->SetInputData(medianTable.Get(), "X", "Y");

    p50LinePlot->GetPen()->SetWidth(this->GridLineThickness);
    p50LinePlot->GetPen()->SetLineType(this->GridLineStyle);
    p50LinePlot->SetColor(this->P50Color[0], this->P50Color[1], this->P50Color[2]);
    p50LinePlot->SetOpacity(this->Opacity);

    vtkNew<vtkContourFilter> p99Contour;
    p99Contour->SetInputData(this->LocalGrid);
    p99Contour->SetNumberOfContours(1);
    p99Contour->SetValue(0, p99);
    p99Contour->Update();

    vtkNew<vtkTable> p99Table;
    SetPolyLineToTable(vtkPolyData::SafeDownCast(p99Contour->GetOutput()),
                       p99Table.Get());
    p99LinePlot->SetInputData(p99Table.Get(), "X", "Y");

    p99LinePlot->GetPen()->SetWidth(this->GridLineThickness);
    p99LinePlot->GetPen()->SetLineType(this->GridLineStyle);
    p99LinePlot->SetColor(this->P99Color[0], this->P99Color[1], this->P99Color[2]);
    p99LinePlot->SetOpacity(this->Opacity);
    }
}

//----------------------------------------------------------------------------
void vtkPVBagChartRepresentation::SetPolyLineToTable(vtkPolyData* polyline,
  vtkTable* table)
{
  vtkNew<vtkDoubleArray> x;
  x->SetName("X");
  vtkNew<vtkDoubleArray> y;
  y->SetName("Y");

  vtkCellArray* lines = polyline->GetLines();
  lines->InitTraversal();
  for (vtkIdType nbpts, *pts; lines->GetNextCell(nbpts, pts);)
    {
    for (vtkIdType i = 0; i < nbpts; ++i)
      {
      double* point = polyline->GetPoint(pts[i]);
      x->InsertNextValue(point[0]);
      y->InsertNextValue(point[1]);
      }
    }
  table->AddColumn(x.Get());
  table->AddColumn(y.Get());
}

//----------------------------------------------------------------------------
int vtkPVBagChartRepresentation::RequestData(vtkInformation* request,
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (vtkProcessModule::GetProcessType() != vtkProcessModule::PROCESS_RENDER_SERVER &&
      inputVector[0]->GetNumberOfInformationObjects() == 1)
    {
    this->LocalGrid = NULL;
    vtkSmartPointer<vtkDataObject> data;

    // don't process data is the cachekeeper has already cached the result.
    data = vtkDataObject::GetData(inputVector[0], 0);
    data = this->TransformInputData(inputVector, data);
    if (data && data->IsA("vtkMultiBlockDataSet"))
      {
      vtkNew<vtkExtractBlock> extractBlock;
      extractBlock->SetInputData(data);
      extractBlock->PruneOutputOn();
      // Prune input dataset to only process blocks on interest.
      // If input is not a multiblock dataset, we make it one so the rest of the
      // pipeline is simple.
      for (std::set<unsigned int>::const_iterator iter = this->CompositeIndices.begin();
        iter != this->CompositeIndices.end(); ++iter)
        {
        const int index= *iter;
        extractBlock->RemoveAllIndices();
        extractBlock->AddIndex(index);
        extractBlock->Update();
        vtkDataObject* block = extractBlock->GetOutput()->GetBlock(0);
        if ( block && block->IsA("vtkImageData"))
          {
          this->LocalGrid = vtkImageData::SafeDownCast(block);
          }
        }
      }
    }
  return this->Superclass::RequestData(request, inputVector, outputVector);
}

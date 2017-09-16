/*=========================================================================

  Program:   ParaView
  Module:    vtk3DWidgetRepresentation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtk3DWidgetRepresentation
// .SECTION Description
// vtk3DWidgetRepresentation is a vtkDataRepresentation subclass for 3D widgets
// and their representations. It makes it possible to add 3D widgets to
// vtkPVRenderView.

#ifndef vtk3DWidgetRepresentation_h
#define vtk3DWidgetRepresentation_h

#include "vtkPVClientServerCoreRenderingModule.h" //needed for exports
#include "vtkDataRepresentation.h"
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkAbstractWidget;
class vtkPVRenderView;
class vtkWidgetRepresentation;
class vtkTransform;

class VTKPVCLIENTSERVERCORERENDERING_EXPORT vtk3DWidgetRepresentation : public vtkDataRepresentation
{
public:
  static vtk3DWidgetRepresentation* New();
  vtkTypeMacro(vtk3DWidgetRepresentation, vtkDataRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the widget.
  void SetWidget(vtkAbstractWidget*);
  vtkGetObjectMacro(Widget, vtkAbstractWidget);

  // Description:
  // Get/Set the representation.
  void SetRepresentation(vtkWidgetRepresentation*);
  vtkGetObjectMacro(Representation, vtkWidgetRepresentation);

  // Description:
  // Set to true to add the vtkWidgetRepresentation to the non-composited
  // renderer. false by default.
  vtkSetMacro(UseNonCompositedRenderer, bool);
  vtkGetMacro(UseNonCompositedRenderer, bool);
  vtkBooleanMacro(UseNonCompositedRenderer, bool);

  // Description:
  // Get/Set whether the widget is enabled.
  void SetEnabled(bool);
  vtkGetMacro(Enabled, bool);
  vtkBooleanMacro(Enabled, bool);

  // Description:
  // Set a custom transform on the widget. Currently only
  // the implicit plane widget handles this ability
  void SetCustomWidgetTransform(vtkTransform *transform);

//BTX
protected:
  vtk3DWidgetRepresentation();
  ~vtk3DWidgetRepresentation();

  // Description:
  // Adds the representation to the view.  This is called from
  // vtkView::AddRepresentation().  Subclasses should override this method.
  // Returns true if the addition succeeds.
  virtual bool AddToView(vtkView* view);

  // Description:
  // Removes the representation to the view.  This is called from
  // vtkView::RemoveRepresentation().  Subclasses should override this method.
  // Returns true if the removal succeeds.
  virtual bool RemoveFromView(vtkView* view);

  // Description:
  // Updates 'Enabled' on this->Widget.
  void UpdateEnabled();

  // Description:
  // Callback whenever the representation is modified. We call UpdateEnabled()
  // to ensure that the widget is not left enabled when the representation is
  // hidden.
  void OnRepresentationModified();

  // Description:
  // Callback whenever the view is modified. If the view's interactor has
  // changed, we will pass that to the vtkAbstractWidget instance and then call
  // UpdateEnabled().
  void OnViewModified();

  // Description:
  // Forwards this->CustomTransform to the representation.
  void UpdateTransform();

  bool Enabled;
  bool UseNonCompositedRenderer;
  vtkAbstractWidget* Widget;
  vtkWidgetRepresentation* Representation;
  vtkWeakPointer<vtkPVRenderView> View;

  vtkTransform* CustomTransform;
private:
  vtk3DWidgetRepresentation(const vtk3DWidgetRepresentation&); // Not implemented
  void operator=(const vtk3DWidgetRepresentation&); // Not implemented
  unsigned long RepresentationObserverTag;
  unsigned long ViewObserverTag;
//ETX
};

#endif

/*=========================================================================

  Program:   ParaView
  Module:    vtkPVDataRepresentation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVDataRepresentation
// .SECTION Description
// vtkPVDataRepresentation adds some ParaView specific API to data
// representations.
// .SECTION See Also
// vtkPVDataRepresentationPipeline

#ifndef vtkPVDataRepresentation_h
#define vtkPVDataRepresentation_h

#include "vtkPVClientServerCoreRenderingModule.h" //needed for exports
#include "vtkDataRepresentation.h"
#include "vtkWeakPointer.h" // needed for vtkWeakPointer
class vtkInformationRequestKey;

class VTKPVCLIENTSERVERCORERENDERING_EXPORT vtkPVDataRepresentation : public vtkDataRepresentation
{
public:
  vtkTypeMacro(vtkPVDataRepresentation, vtkDataRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // vtkAlgorithm::ProcessRequest() equivalent for rendering passes. This is
  // typically called by the vtkView to request meta-data from the
  // representations or ask them to perform certain tasks e.g.
  // PrepareForRendering.
  // Overridden to skip processing when visibility if off.
  virtual int ProcessViewRequest(vtkInformationRequestKey* request_type,
    vtkInformation* inInfo, vtkInformation* outInfo);

  // Description:
  // This is one of the most important functions. In VTK pipelines, it's very
  // easy for the pipeline to decide when it needs to re-execute.
  // vtkAlgorithm::Update() can go up the entire pipeline to see if any filters
  // MTime changed (among other things) and if so, it can re-execute the pipeline.
  // However in case of representations, the real input connection may only be
  // present on the data-server nodes. In that case the
  // vtkPVDataRepresentation::RequestData() will only get called on the
  // data-server nodes. That means that representations won't be able to any
  // data-delivery in RequestData(). We'd need some other mechanisms to
  // synchronize data-delivery among processes. To avoid that conundrum, the
  // vtkSMRepresentationProxy calls MarkModified() on all processes whenever any
  // filter in the pipeline is modified. In this method, the
  // vtkPVDataRepresentation subclasses should ensure that they mark all
  // delivery related filters dirty in their RequestData to ensure they execute
  // then next time they are updated. The vtkPVDataRepresentation also uses a
  // special executive which avoids updating the representation unless
  // MarkModified() was called since the last Update(), thus acting as a
  // update-suppressor.
  virtual void MarkModified();

  // Description:
  // Initialize the representation with an identifier range so each internal
  // representation can own a unique ID.
  // If a representation requires more IDs than the set of ids provided,
  // the representation MUST complains by an error or abort explaining how many
  // ids where expected so the number of reserved ids could be easily adjust.
  // Unless noted otherwise, this method must be called before calling any
  // other methods on this class.
  // @CallOnAllProcessess
  // Internally you can pick an id that follow that condition
  // minIdAvailable <= id <= maxIdAvailable
  // Return the minIdAvailable after initialization so that new range could be used
  virtual unsigned int Initialize(unsigned int minIdAvailable, unsigned int maxIdAvailable);

  // Description:
  // Return 0 if the Initialize() method was not called otherwise a unique ID
  // that will be shared across the processes for that same object.
  unsigned int GetUniqueIdentifier() { return this->UniqueIdentifier; }

  // Description:
  // Get/Set the visibility for this representation. When the visibility of
  // representation of false, all view passes are ignored.
  virtual void SetVisibility(bool val)
    { this->Visibility = val; }
  vtkGetMacro(Visibility, bool);

  // Description:
  // Returns the data object that is rendered from the given input port.
  virtual vtkDataObject* GetRenderedDataObject(int vtkNotUsed(port))
    { return this->GetInputDataObject(0, 0); }

  // Description:
  // Set the update time.
  virtual void SetUpdateTime(double time);
  vtkGetMacro(UpdateTime, double);

  // Description:
  // Set whether the UpdateTime is valid.
  vtkGetMacro(UpdateTimeValid, bool);

  // Description:
  // @deprecated No longer needed. Simply remove these methods from your
  // subclass implementation.
  VTK_LEGACY(virtual void SetUseCache(bool));
  VTK_LEGACY(virtual void SetCacheKey(double val));

  // Description:
  // Typically a representation decides whether to use cache based on the view's
  // values for UseCache and CacheKey.
  // However in some cases we may want to
  // force a representation to cache irrespective of the view (e.g. comparative
  // views). In which case these ivars can up set. If ForcedCacheKey is true, it
  // overrides UseCache and CacheKey. Instead, ForcedCacheKey is used.
  virtual void SetForcedCacheKey(double val)
    { this->ForcedCacheKey = val; }
  virtual void SetForceUseCache(bool val)
    { this->ForceUseCache = val; }

  // Description:
  // Returns whether caching is used and what key to use when caching is
  // enabled.
  virtual double GetCacheKey();
  virtual bool GetUseCache();

  // Description:
  // Called by vtkPVDataRepresentationPipeline to see if using cache is valid
  // and will be used for the update. If so, it bypasses all pipeline passes.
  // Subclasses should override IsCached(double) to indicate if a particular
  // entry is cached.
  bool GetUsingCacheForUpdate();

  vtkGetMacro(NeedUpdate,  bool);

  // Description:
  // Making these methods public. When constructing composite representations,
  // we need to call these methods directly on internal representations.
  virtual bool AddToView(vtkView* view);
  virtual bool RemoveFromView(vtkView* view);

  // Description:
  // Retrieves an output port for the input data object at the specified port
  // and connection index. This may be connected to the representation's
  // internal pipeline.
  // Overridden to use vtkPVTrivialProducer instead of vtkTrivialProducer
  virtual vtkAlgorithmOutput* GetInternalOutputPort()
    { return this->GetInternalOutputPort(0); }
  virtual vtkAlgorithmOutput* GetInternalOutputPort(int port)
    { return this->GetInternalOutputPort(port, 0); }
  virtual vtkAlgorithmOutput* GetInternalOutputPort(int port, int conn);

  // Description:
  // Provides access to the view.
  vtkView* GetView() const;

//BTX
protected:
  vtkPVDataRepresentation();
  ~vtkPVDataRepresentation();

  // Description:
  // Subclasses should override this method when they support caching to
  // indicate if the particular key is cached. Default returns false.
  virtual bool IsCached(double cache_key)
    { (void)cache_key; return false; }

  // Description:
  // Create a default executive.
  virtual vtkExecutive* CreateDefaultExecutive();

  // Description:
  // Overridden to invoke vtkCommand::UpdateDataEvent.
  virtual int RequestData(vtkInformation*,
    vtkInformationVector**, vtkInformationVector*);

  virtual int RequestUpdateExtent(vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector);

  virtual int RequestUpdateTime (vtkInformation*,
                                  vtkInformationVector**,
                                 vtkInformationVector*);

  double UpdateTime;
  bool UpdateTimeValid;
  unsigned int UniqueIdentifier;
private:
  vtkPVDataRepresentation(const vtkPVDataRepresentation&); // Not implemented
  void operator=(const vtkPVDataRepresentation&); // Not implemented

  bool Visibility;
  bool ForceUseCache;
  double ForcedCacheKey;
  bool NeedUpdate;

  class Internals;
  Internals* Implementation;
  vtkWeakPointer<vtkView> View;
//ETX
};

#endif

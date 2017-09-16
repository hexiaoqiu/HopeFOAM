/*=========================================================================

  Program:   ParaView
  Module:    vtkPVDataSetAttributesInformation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPVDataSetAttributesInformation - List of array info
// .SECTION Description
// Information associated with vtkDataSetAttributes object (i.e point data).
// This object does not have any user interface.  It is created and destroyed
// on the fly as needed.  It may be possible to add features of this object
// to vtkDataSetAttributes.  That would eliminate all of the "Information"
// in ParaView.

#ifndef vtkPVDataSetAttributesInformation_h
#define vtkPVDataSetAttributesInformation_h

#include "vtkPVClientServerCoreCoreModule.h" //needed for exports
#include "vtkPVInformation.h"
#include "vtkDataSetAttributes.h" // needed for NUM_ATTRIBUTES

class vtkCollection;
class vtkDataSetAttributes;
class vtkFieldData;
class vtkPVArrayInformation;
class vtkGenericAttributeCollection;

class VTKPVCLIENTSERVERCORECORE_EXPORT vtkPVDataSetAttributesInformation : public vtkPVInformation
{
public:
  static vtkPVDataSetAttributesInformation* New();
  vtkTypeMacro(vtkPVDataSetAttributesInformation, vtkPVInformation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Returns the field association to which the instance corresponds to.
  // Returned value can be vtkDataObject::POINT, vtkDataObject::CELL,
  // vtkDataObject::FIELD, etc i.e. vtkDataObject::FieldAssociations or
  // vtkDataObject::AttributeTypes.
  vtkGetMacro(FieldAssociation, int);
  vtkSetMacro(FieldAssociation, int);

  // Description:
  // Transfer information about a single vtk data object into
  // this object.
  void CopyFromDataSetAttributes(vtkDataSetAttributes* data);
  void DeepCopy(vtkPVDataSetAttributesInformation* info);

  void CopyFromFieldData(vtkFieldData* data);

  void CopyFromGenericAttributesOnPoints(vtkGenericAttributeCollection *data);
  void CopyFromGenericAttributesOnCells(vtkGenericAttributeCollection *data);
  
  
  // Description:
  // Intersect information of argument with information currently
  // in this object.  Arrays must be in both
  // (same name and number of components)to be in final.
  void AddInformation(vtkDataSetAttributes* da);
  void AddInformation(vtkPVDataSetAttributesInformation* info);
  virtual void AddInformation(vtkPVInformation* info);

  // Description:
  // Remove all infommation. next add will be like a copy.
  void Initialize();

  // Description:
  // Access to information.
  int                    GetNumberOfArrays() const;
  // Because not all the arrays have to be the same length:
  int                    GetMaximumNumberOfTuples() const;
  vtkPVArrayInformation* GetArrayInformation(int idx) const;
  vtkPVArrayInformation* GetArrayInformation(const char *name) const;

  // Description:
  // For getting default scalars ... (vtkDataSetAttributes::SCALARS).
  vtkPVArrayInformation* GetAttributeInformation(int attributeType);

  // Description:
  // Mimicks data set attribute call.  Returns -1 if array (of index) is
  // not a standard attribute.  Returns attribute type otherwise.
  int IsArrayAnAttribute(int arrayIndex);

  // Description:
  // Manage a serialized version of the information.
  virtual void CopyToStream(vtkClientServerStream*);
  virtual void CopyFromStream(const vtkClientServerStream*);

  // Description:
  // Specify whether or not to sort the arrays. The default is
  // to sort.
  vtkSetMacro(SortArrays, bool);
  vtkGetMacro(SortArrays, bool);
  vtkBooleanMacro(SortArrays, bool);

protected:
  vtkPVDataSetAttributesInformation();
  ~vtkPVDataSetAttributesInformation();

  // Data information collected from remote processes.
  vtkCollection* ArrayInformation;
  // Standard cell attributes.
  short          AttributeIndices[vtkDataSetAttributes::NUM_ATTRIBUTES];
  int FieldAssociation;

private:
  bool SortArrays;

  vtkPVDataSetAttributesInformation(const vtkPVDataSetAttributesInformation&); // Not implemented
  void operator=(const vtkPVDataSetAttributesInformation&); // Not implemented
};

#endif

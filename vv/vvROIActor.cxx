/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

#include "vvROIActor.h"
#include "vvImageContour.h"
#include "vvSlicerManager.h"
#include "vvBinaryImageOverlayActor.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//------------------------------------------------------------------------------
vvROIActor::vvROIActor() {
  mImageContour.clear();
  mOverlayActors.clear();
  mIsVisible = true;
  mOpacity = 0.7;
  mIsSelected = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvROIActor::~vvROIActor() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetROI(clitk::DicomRT_ROI * s) {
  mROI = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetSlicerManager(vvSlicerManager * s) {
  mSlicerManager = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetVisible(bool b) {
  mIsVisible = b;
  if (!b) { // remove actor
    for(unsigned int i= 0; i<mOverlayActors.size(); i++) 
      mOverlayActors[i]->HideActors();
  }
  else {
    for(unsigned int i= 0; i<mOverlayActors.size(); i++) 
      mOverlayActors[i]->ShowActors();
  }
  Update();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvROIActor::IsVisible() {
  return mIsVisible;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Initialize() {
  if (mROI->GetImage()) {
    mImageContour.clear();
    mOverlayActors.clear();
    for(int i=0;i<mSlicerManager->NumberOfSlicers(); i++) {
      mImageContour.push_back(new vvImageContour);
      mImageContour[i]->setSlicer(mSlicerManager->GetSlicer(i));
      mImageContour[i]->setImage(mROI->GetImage());
      //mImageContour[i]->setColor(1.0, 0.0, 0.0);
      mImageContour[i]->setColor(1.0-mROI->GetDisplayColor()[0], 
      				 1.0-mROI->GetDisplayColor()[1], 
      				 1.0-mROI->GetDisplayColor()[2]);
      mImageContour[i]->setPreserveMemoryModeEnabled(true);
      mImageContour[i]->setSlicer(mSlicerManager->GetSlicer(i));
      
      mOverlayActors.push_back(new vvBinaryImageOverlayActor);
      mOverlayActors[i]->SetImage(mROI->GetImage(), mROI->GetBackgroundValueLabelImage());
      mOverlayActors[i]->SetColor(mROI->GetDisplayColor()[0], 
				  mROI->GetDisplayColor()[1], 
				  mROI->GetDisplayColor()[2]);
      mOverlayActors[i]->SetOpacity(mOpacity);
      mOverlayActors[i]->SetSlicer(mSlicerManager->GetSlicer(i));
      mOverlayActors[i]->Initialize();
    }
    
    connect(mSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    //connect(mSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    connect(mSlicerManager, SIGNAL(AVerticalSliderHasChanged(int, int)), SLOT(UpdateSlice(int, int)));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Update() {
  for(int i=0; i<mSlicerManager->NumberOfSlicers(); i++) {
    UpdateSlice(i, mSlicerManager->GetSlicer(i)->GetSlice());
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateSlice(int slicer, int slices) {
  if (!mROI->GetImage())  return;
  
  if (!mIsVisible) return;

  if (!mSlicerManager) {
    std::cerr << "Error. No mSlicerManager ?" << std::endl;
    exit(0);
  }

  // CONTOUR HERE 
  DD("vvROIActor::UpdateSlice");
  DD(mROI->GetName());
  DD(mIsSelected);
  DD(mROI->GetBackgroundValueLabelImage());
  if (mIsSelected) {
    mImageContour[slicer]->update(1.0);//mROI->GetBackgroundValueLabelImage()); 
    //    mImageContour[slicer]->showActors();
  }

  // Refresh overlays
  mOverlayActors[slicer]->UpdateSlice(slicer, slices);

  // Do not used the following line : TOO SLOW.
  // mSlicerManager->GetSlicer(slicer)->GetRenderWindow()->Render(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvROIActor::UpdateOpacity(double d) {
//   if (d == mOpacity) return;
//   mOpacity = d; 
//   for(unsigned int i=0; i<mOverlayActors.size(); i++) {
//     mOverlayActors[i]->SetOpacity(d);
//     mOverlayActors[i]->UpdateColor();
//   }
//   mSlicerManager->Render(); 
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetOpacity(double d) {
  mOpacity = d; 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateColor() {
  for(unsigned int i=0; i<mOverlayActors.size(); i++) {
    mOverlayActors[i]->SetOpacity(mOpacity);
    mOverlayActors[i]->SetColor(mROI->GetDisplayColor()[0], 
				mROI->GetDisplayColor()[1], 
				mROI->GetDisplayColor()[2]);
    mOverlayActors[i]->UpdateColor();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
double vvROIActor::GetOpacity() {
  return mOpacity;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetSelected(bool b) {
  mIsSelected = b;
  if (b) {
    for(int i=0; i<mSlicerManager->NumberOfSlicers(); i++) {
      mImageContour[i]->SetLineWidth(3.0);
      mImageContour[i]->showActors();
    }
  }
  else {
    for(int i=0; i<mSlicerManager->NumberOfSlicers(); i++) {
      mImageContour[i]->hideActors();
    }
  } 
}
//------------------------------------------------------------------------------

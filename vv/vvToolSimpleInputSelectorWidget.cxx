#ifndef VVTOOLINPUTSELECTORWIDGET_CXX
#define VVTOOLINPUTSELECTORWIDGET_CXX
#include "vvToolSimpleInputSelectorWidget.h"
#include "vvSlicerManager.h"
#include <QAbstractButton>

//------------------------------------------------------------------------------
vvToolSimpleInputSelectorWidget::vvToolSimpleInputSelectorWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f) {
  setupUi(this);  
  setEnabled(true);
  mSlicerManagerList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::Initialize() {
  // Connect signals & slots  
  connect(mInputSelectionButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(mInputSelectionButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(mInputSequenceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeInput(int)));  
  if (mSlicerManagerList.size() == 1) {
    if (!mAllowSkip) accept();
  }
  if (mSlicerManagerList.size() == 0) {
    reject();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::SetText(QString & s) {
  mGroupBox->setTitle(s);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::EnableAllowSkip(bool b) {
  mAllowSkip = b;
  if (mAllowSkip) {
    mInputSelectionButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Discard);
    connect(mInputSelectionButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(skip(QAbstractButton*)));
  }
  else 
    mInputSelectionButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::SetInputList(const std::vector<vvSlicerManager*> & l, int index) {
  if (l.size() == 0) {
    // TODO !!!
    DD("no input > error message");
    reject();
  }
  mInputSequenceBox->clear();
  for(unsigned int i=0; i<l.size(); i++)
    mSlicerManagerList.push_back(l[i]);
  mCurrentIndex = index;
  for (unsigned int i = 0; i < mSlicerManagerList.size(); i++) {
    mInputSequenceBox->addItem(mSlicerManagerList[i]->GetFileName().c_str());
  }
  mInputSequenceBox->setCurrentIndex(mCurrentIndex);
  changeInput(mCurrentIndex);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::setEnabled(bool b) {
  QWidget::setEnabled(b);
  mInputSelectionButtonBox->setEnabled(b);
  mInputSequenceBox->setEnabled(b);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::accept() {
  mInputSelectionButtonBox->setEnabled(false);
  mInputSequenceBox->setEnabled(false);
  emit accepted();
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::reject() {
  emit rejected();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::skip(QAbstractButton* b) {
  if (b->text() == "Discard") emit sigskip();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolSimpleInputSelectorWidget::changeInput(int index) {
  if (index<0) return;
  mCurrentIndex = index;
  vvImage * mCurrentImage = mSlicerManagerList[index]->GetImage();
  unsigned int d = mCurrentImage->GetNumberOfDimensions();
  QString size;
  QString spacing;
  for(unsigned int i=0; i<d-1; i++) {
    size.append(QString("%1").arg(mCurrentImage->GetSize()[i]));
    size.append("x");
    spacing.append(QString("%1").arg(mCurrentImage->GetSpacing()[i]));
    spacing.append("x");
  }
  size.append(QString("%1").arg(mCurrentImage->GetSize()[d-1]));
  spacing.append(QString("%1").arg(mCurrentImage->GetSpacing()[d-1]));
  mLabelInputInfo->setText(QString("Image: %1D %2   %3    %4")
                           .arg(d)
                           .arg(mCurrentImage->GetScalarTypeAsString().c_str())
                           .arg(size)
                           .arg(spacing));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvSlicerManager * vvToolSimpleInputSelectorWidget::GetSelectedInput() {
  return mSlicerManagerList[GetSelectedInputIndex()];
}
//------------------------------------------------------------------------------

#endif

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
#include <utility>
#include <cassert>
#include <ctime>
#include <string>
#include <ostream>
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopWidget>

#include "clitkIO.h"
#include "vvMainWindow.h"
#include "vvConstants.h"

#include <vtkFileOutputWindow.h>
#include <vtkSmartPointer.h>
#include <itkFileOutputWindow.h>
#include <itkSmartPointer.h>

#include <sys/types.h>
#include <sys/stat.h>
#include  <errno.h> 

void load_image_first_error()
{
  std::cerr << "You need to load an image before adding an overlay!" << std::endl;
  exit(1);
}

std::string create_timed_string()
{
  time_t t;
  time(&t);
  
  struct tm* pt = localtime(&t);
  
  const int size = 64;
  char st[size];
  strftime(st, size, "%Y%m%d-%H%M%S", pt);
  
  return st;
}

//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
  CLITK_INIT;

  QApplication app( argc, argv );
  Q_INIT_RESOURCE(vvIcons);
  //QPixmap pixmap(":/splashscreen.PNG");
  QSplashScreen *splash = new QSplashScreen(QPixmap(QString::fromUtf8(":/new/prefix1/splashscreen.PNG")));
  /*splash->showMessage("VV 1.0 developped by Léon Bérard c`ancer center http://oncora1.lyon.fnclcc.fr and CREATIS-LRMN http://www.creatis.insa-lyon.fr",(Qt::AlignRight | Qt::AlignBottom));*/
  //  splash->show();
  QTimer::singleShot(2000, splash, SLOT(close()));
  while (!splash->isHidden())
    app.processEvents();

  vvMainWindow window;

  //Try to give the window a sensible default size
  int width=QApplication::desktop()->width()*0.8;
  int height=QApplication::desktop()->height()*0.9;
  if (width> 1.5*height)
    width=1.5*height;
  window.resize(width,height);

  window.show();

  std::vector<std::string> sequence_filenames;
  enum {P_NORMAL,P_SEQUENCE,P_WINDOW,P_LEVEL};
  int parse_mode=P_NORMAL;
  int n_image_loaded=0;
  std::string win(""), lev("");

  if (argc >1) {
    for (int i = 1; i < argc; i++) {
      std::string current = argv[i];
      if (!current.compare(0,2,"--")) { //We are parsing an option
        if (parse_mode == P_SEQUENCE) {//First finish the current sequence
          window.LoadImages(sequence_filenames,MERGEDWITHTIME);
          sequence_filenames.clear();
          parse_mode=P_NORMAL;
        }
        if (current=="--vf") {
          if (!n_image_loaded) load_image_first_error();
          window.AddField(argv[i+1],n_image_loaded-1);
          i++; //skip vf name
        } else if (current=="--overlay") {
          if (!n_image_loaded) load_image_first_error();
          window.AddOverlayImage(n_image_loaded-1,argv[i+1]);
          i++; //skip overlay name
        } else if (current=="--roi") {
          if (!n_image_loaded) load_image_first_error();
          window.AddROI(n_image_loaded-1,argv[i+1]);
          i++; //skip roi name
        } else if (current=="--fusion") {
          if (!n_image_loaded) load_image_first_error();
          window.AddFusionImage(n_image_loaded-1,argv[i+1]);
          i++; //skip fusion name
        } else if (current == "--sequence") {
          n_image_loaded++; //count only one for the sequence
          parse_mode=P_SEQUENCE;
        } else if (current == "--window") {
          parse_mode=P_WINDOW;
        } else if (current == "--level") {
          parse_mode=P_LEVEL;
        } else if (current == "--log") {
	  std::string log_dir = "/tmp/vv-log";
	  int err = mkdir(log_dir.c_str(), S_IRWXU | S_IRUSR | S_IWUSR | S_IRWXG | S_IRGRP | S_IWGRP | S_IRWXO | S_IROTH | S_IWOTH); 
	  if (err && errno != EEXIST)
	    std::cout << "Error creating log directory with errno " << errno << std::endl;

	  std::string log_file = log_dir + "/" + create_timed_string() + ".log";
	  vtkSmartPointer<vtkFileOutputWindow> vtk_log = vtkFileOutputWindow::New();
	  vtk_log->SetFileName(log_file.c_str());
	  vtk_log->FlushOn();
	  vtkOutputWindow::SetInstance(vtk_log);
	  
	  itk::SmartPointer<itk::FileOutputWindow> itk_log = itk::FileOutputWindow::New();
	  itk_log->SetFileName(log_file.c_str());
	  itk_log->FlushOn();
	  itk::OutputWindow::SetInstance(itk_log);
	}
      } else if (parse_mode == P_SEQUENCE) {
        sequence_filenames.push_back(current);
      } else if (parse_mode == P_WINDOW) {
        win=current;
        parse_mode=P_NORMAL;
      } else if (parse_mode == P_LEVEL) {
        lev=current;
        parse_mode=P_NORMAL;
      } else {
        std::vector<std::string> image;
        image.push_back(current);
        window.LoadImages(image,IMAGE);
        n_image_loaded++;
      }
    }
    if (parse_mode == P_SEQUENCE) { //Finish any current sequence
      window.LoadImages(sequence_filenames,MERGEDWITHTIME);
      sequence_filenames.clear();
      parse_mode=P_NORMAL;
    }
  }

  if(win!="" && lev!="") {
    window.WindowLevelChanged(atof(win.c_str()), atof(lev.c_str()), 6, 0);
    window.ApplyWindowLevelToAllImages();
  }

  return app.exec();
}

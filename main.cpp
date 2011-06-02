#include "wingscreator.h"
#include <QtGui/QImage>
#include <QtCore/qfile.h>


int main(int argc, char** argv)
{
  QImage img;
  const QString fname("../test.wings");
  WingsCreator wings_creator;
  wings_creator.create(fname,256,256,img);
  QFile out_file("test.png");
  out_file.open(QIODevice::WriteOnly);
  img.save(&out_file,"PNG");
  out_file.close();
  return 0;
}

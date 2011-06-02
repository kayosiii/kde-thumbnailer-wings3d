#include "wingscreator.h"

#include <kdebug.h>
#include <kdebug.h>
#include <kfilterdev.h>
#include <QtCore/QFile>
#include <zlib.h>
#include <QtCore/qbuffer.h>
#include "../git/fuji/Fuji/Private/Middleware/zlib/zutil.h"

extern "C" {
  KDE_EXPORT ThumbCreator *new_creator() {
    return new WingsCreator;
  }
}

WingsCreator::WingsCreator() {}

enum {
  TOKEN_BYTE = 'a',TOKEN_INT32,TOKEN_DOUBLE_STR,TOKEN_ATOM,
  TOKEN_TUPLE = 'h',
  TOKEN_ARRAY_END = 'j',TOKEN_STRING,TOKEN_ARRAY_START,TOKEN_MAP
};

void WingsCreator::skip_token(QDataStream& in){
  qint8 token; in >> token;
  switch(token){
    case TOKEN_BYTE:
      in.skipRawData(1);
      break;
    case TOKEN_INT32:
      in.skipRawData(4);
      break;
    case TOKEN_DOUBLE_STR:
      in.skipRawData(31);
      break;
    case TOKEN_ATOM:
      qint16 atom_size; in >> atom_size;
      in.skipRawData(atom_size);
      break;
    case TOKEN_TUPLE:
      skip_tuple(in);
      break;
    case TOKEN_ARRAY_END:
      break;
    case TOKEN_STRING:
      qint16 str_size; in >> str_size;
      in.skipRawData(str_size);
      break;
    case TOKEN_ARRAY_START:
      skip_array(in);
      break;
    case TOKEN_MAP:
      qint32 map_size; in >> map_size;
      in.skipRawData(map_size);
      break;
  }
}

void WingsCreator::skip_tuple(QDataStream& in) {
  qint8 length; in >> length;
  for (int i=0; i<length; i++) {
    skip_token(in);
  }
}

void WingsCreator::skip_array(QDataStream& in) {
  qint32 length; in >> length;
  for (int i=0; i<length; i++) {
    skip_token(in);
  }
  in.skipRawData(1);
}

void WingsCreator::skip_tuple_header(QDataStream& in){
  in.skipRawData(2);
}

QByteArray WingsCreator::read_atom(QDataStream& in) {
  in.skipRawData(1);
  qint16 size; in >> size;
  char atom[size];
  in.readRawData(atom,size);
  return QByteArray(atom);
}

qint8 WingsCreator::read_byte(QDataStream& in){
  in.skipRawData(1);
  qint8 out; in >> out;
  return out;
}

qint32 WingsCreator::read_int(QDataStream& in) {
  in.skipRawData(1);
  qint32 out; in >> out;
  return out;
}



QByteArray WingsCreator::read_archive(const QString& path) {
  QFile in_file(path);
  if (in_file.open(QIODevice::ReadOnly)==false) {
    in_file.close();
    return NULL;
  }
  QDataStream in;
  in.setDevice(&in_file);
  char file_header[15];
  in.readRawData(file_header,15);
  qint32 compressed_size; in >> compressed_size;
  qint16 m; in >> m;
  qint32 uncompressed_size; in >> uncompressed_size;
  compressed_size -= 6;
  QByteArray zdata;
  zdata.resize(compressed_size);
  in.readRawData(zdata.data(),compressed_size);
  in_file.close();

  QByteArray data;
  data.resize(uncompressed_size);
  uLongf d = (uLongf) uncompressed_size;
  ::uncompress((
    uchar *)data.data(),&d,
    (uchar *)zdata.data(),
    (uLongf)compressed_size
  );

  QFile out_file("out.dump");
  out_file.open(QIODevice::WriteOnly);
  out_file.write(data);
  out_file.close();
  return data;
}


bool WingsCreator::create(const QString& path, int width, int height, QImage& img) {
  QByteArray data = read_archive(path);
  if(data == NULL) {return false;}
  QBuffer buf;
  buf.setData(data);
  QDataStream in;
  buf.setBuffer(&data);
  buf.open(QIODevice::ReadOnly);
  in.setDevice(&buf);

  skip_tuple_header(in);
  skip_token(in); //wings
  qint8 version; in >> version;
  skip_tuple_header(in);
  skip_token(in); //shapes
  skip_token(in); //materials
  skip_token(in); //

  in.skipRawData(1);
  qint32 n_props; in >> n_props;
  for (int i=0; i<n_props;i++) {
    skip_tuple_header(in);
    QByteArray section = read_atom(in);
    if (section.startsWith("thumbnail")) {
      skip_tuple_header(in);
      skip_token(in); //e3d_image
      QByteArray format = read_atom(in);
      in.skipRawData(1);
      qint8 depth; in >> depth;
      in.skipRawData(1);
      qint8 something; in >> something;
//       QByteArray origin = read_atom(in);
      skip_token(in);
      in.skipRawData(1);
      qint32 width; in >> width;
      in.skipRawData(1);
      qint32 height; in >> height;
      in.skipRawData(1); // map header
      qint32 img_datasize; in >> img_datasize;
      char img_data[img_datasize];
      in.readRawData(img_data,img_datasize);
      QImage out_img((unsigned char*) img_data,256,256,QImage::Format_RGB888);
      img = out_img.mirrored();
      buf.close();
      return true;
    } else {
      skip_token(in);
    }
  }
  in.skipRawData(1);
  buf.close();
  return false;
}

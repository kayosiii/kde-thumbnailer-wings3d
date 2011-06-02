#ifndef _WINGSCREATOR_H_
#define _WINGSCREATOR_H_

#include <kio/thumbcreator.h>

#include <QtCore/qdatastream.h>
#include <QtGui/QImage>

class WingsCreator : public ThumbCreator {
public:
    WingsCreator();
    virtual bool create(const QString &path, int width, int height, QImage &img);
    void skip_token(QDataStream &in);
    void skip_tuple(QDataStream &in);
    void skip_array(QDataStream &in);
    void skip_tuple_header(QDataStream &in);
    QByteArray read_atom(QDataStream &in);
    qint8 read_byte(QDataStream &in);
    qint32 read_int(QDataStream &in);
    QByteArray read_archive(const QString &path);
};

#endif
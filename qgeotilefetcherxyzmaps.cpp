#include "qgeotilefetcherxyzmaps.h"
#include "qgeomapreplyxyz.h"
#include "qgeotiledmapxyzmaps.h"
#include "qgeotiledmappingmanagerenginexyzmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QSize>
#include <QDir>
#include <QUrl>
#include <QUrlQuery>
#include <QTime>
#include <QDirIterator>
#include <QFileInfo>

#include <math.h>
#include <iostream>
#include <map>

QT_BEGIN_NAMESPACE

namespace
{
int  _getServerNum(int x, int y, int max)
{
    return (x + 2 * y) % max;
}

QString  _tileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
{
    QString  quadKey;

    for (int i = levelOfDetail; i > 0; i--)
    {
        char  digit = '0';
        int   mask  = 1 << (i - 1);

        if ((tileX & mask) != 0)
        {
            digit++;
        }

        if ((tileY & mask) != 0)
        {
            digit++;
            digit++;
        }

        quadKey.append(digit);
    }

    return quadKey;
}
}

QGeoTileFetcherXYZmaps::QGeoTileFetcherXYZmaps(const QVariantMap                    &parameters,
                                               QGeoTiledMappingManagerEngineXYZmaps *engine,
                                               const QSize                          &tileSize):
    QGeoTileFetcher(engine),
    m_engineXYZmaps(engine),
    m_tileSize(tileSize)
{
    if (parameters.contains(QStringLiteral("source")))
    {
        mPath = parameters.value(QStringLiteral("source")).toString();

        auto  separator = QString(QDir::separator());

        if (!mPath.endsWith(separator))
        {
            mPath.append(separator);
        }
    }

    // find extionsion
    {
        QDirIterator  it(mPath, QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext())
        {
            auto  i = it.fileInfo();

            if (i.isFile())
            {
                mFileSuffix = i.completeSuffix();

                break;
            }

            it.next();
        }

        mPath = parameters.value(QStringLiteral("source")).toString();
    }
}

QGeoTileFetcherXYZmaps::~QGeoTileFetcherXYZmaps()
{
}

QGeoTiledMapReply * QGeoTileFetcherXYZmaps::getTileImage(const QGeoTileSpec &spec)
{
    QString            surl     = _getURL(spec.mapId(), spec.x(), spec.y(), spec.zoom());
    QGeoTiledMapReply *mapReply = new QGeoMapReplyXYZmaps(surl, spec);

    return mapReply;
}

QString  QGeoTileFetcherXYZmaps::_getURL(int type, int x, int y, int zoom)
{
    auto     separator = QString(QDir::separator());
    QString  res       = mPath + QString("%1" + separator + "%2" + separator + "%3." + mFileSuffix).arg(zoom).arg(x).arg(y);

    return res;
}

QT_END_NAMESPACE

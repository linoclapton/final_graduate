/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QRENDERHOVERPOINTS_H
#define QRENDERHOVERPOINTS_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>
#include <QColorDialog>


QT_FORWARD_DECLARE_CLASS(QBypassWidget)

class QRenderHoverPoints : public QWidget
{
    Q_OBJECT
public:
    enum PointShape {
        CircleShape,
        RectangleShape
    };

    enum LockType {
        LockToLeft   = 0x01,
        LockToRight  = 0x02,
        LockToTop    = 0x04,
        LockToBottom = 0x08
    };

    enum SortType {
        NoSort,
        XSort,
        YSort
    };

    enum ConnectionType {
        NoConnection,
        LineConnection,
        CurveConnection
    };

    QRenderHoverPoints(QWidget *widget, PointShape shape);

    bool eventFilter(QObject *object, QEvent *event);

    void paintPoints();

    inline QRectF boundingRect() const;
    void setBoundingRect(const QRectF &boundingRect) { m_bounds = boundingRect; }

    QPolygonF points() const { return m_points; }
    void setPoints(const QPolygonF &points);

    QSizeF pointSize() const { return m_pointSize; }
    void setPointSize(const QSizeF &size) { m_pointSize = size; }

    SortType sortType() const { return m_sortType; }
    void setSortType(SortType sortType) { m_sortType = sortType; }

    ConnectionType connectionType() const { return m_connectionType; }
    void setConnectionType(ConnectionType connectionType) { m_connectionType = connectionType; }

    void setConnectionPen(const QPen &pen) { m_connectionPen = pen; }
    void setShapePen(const QPen &pen) { m_pointPen = pen; }
    void setShapeBrush(const QBrush &brush) { m_pointBrush = brush; }

    void setPointLock(int pos, LockType lock) { m_locks[pos] = lock; }

    void setEditable(bool editable) { m_editable = editable; }
    bool editable() const { return m_editable; }

    QVector<QColor>& getRepresentColor() {return m_colors;}
    void setColor(int index,QColor color) {m_colors[index] = color;}

public slots:
    void setEnabled(bool enabled);
    void setDisabled(bool disabled) { setEnabled(!disabled); }

signals:
    void pointsChanged(QPolygonF);
    void oneDTFChanged();

public:
    void firePointChange();

private:
    inline QRectF pointBoundingRect(int i) const;
    void movePoint(int i, const QPointF &newPos, bool emitChange = true);

    QWidget *m_widget;

    QPolygonF m_points;
    QVector<QColor> m_colors;
    QRectF m_bounds;
    PointShape m_shape;
    SortType m_sortType;
    ConnectionType m_connectionType;

    QVector<uint> m_locks;

    QSizeF m_pointSize;
    int m_currentIndex;
    bool m_editable;
    bool m_enabled;

    QHash<int, int> m_fingerPointMapping;

    QPen m_pointPen;
    QBrush m_pointBrush;
    QPen m_connectionPen;
};


inline QRectF QRenderHoverPoints::pointBoundingRect(int i) const
{
    QPointF p = m_points.at(i);
    qreal w = m_pointSize.width();
    qreal h = m_pointSize.height();
    qreal x = p.x() - w / 2;
    qreal y = p.y() - h / 2;
    return QRectF(x, y, w, h);
}

inline QRectF QRenderHoverPoints::boundingRect() const
{
    if (m_bounds.isEmpty())
        return m_widget->rect();
    else
        return m_bounds;
}

#endif // QRENDERHOVERPOINTS_H

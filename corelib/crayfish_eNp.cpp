/*
Crayfish - A collection of tools for TUFLOW and other hydraulic modelling packages
Copyright (C) 2014 Lutra Consulting

info at lutraconsulting dot co dot uk
Lutra Consulting
23 Chestnut Close
Burgess Hill
West Sussex
RH15 8HN

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "crayfish_eNp.h"

#include "math.h"
#include <QPointF>

#include <QVector3D>
#include <QVector2D>
#include <limits>

static bool ENP_isInside(const QVector<QPointF>& pX, QPointF pP) {
     //https://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
      int i, j, c = 0;

      for (i = 0, j = pX.size()-1; i < pX.size(); j = i++) {
        if ( ((pX[i].y()>pP.y()) != (pX[j].y()>pP.y())) &&
         (pP.x() < (pX[j].x()-pX[i].x()) * (pP.y()-pX[i].y()) / (pX[j].y()-pX[i].y()) + pX[i].x()) )
           c = !c;
      }
      return c;
}

static double ENP_contangent(QPointF a, QPointF b, QPointF c) {
    //http://geometry.caltech.edu/pubs/MHBD02.pdf
    QVector3D ba (b-a);
    QVector3D bc (b-c);

    double dp = QVector3D::dotProduct(bc, ba);
    QVector3D cp = QVector3D::crossProduct(bc, ba);

    return dp/cp.length();
}

bool ENP_physicalToLogical(const QVector<QPointF>& pX, QPointF pP, QVector<double>& lam)
{
    //http://geometry.caltech.edu/pubs/MHBD02.pdf
    if (pX.size() < 3 || pX.size() != lam.size())
        return false;

    if (!ENP_isInside(pX, pP)) {
        return false;
    }

    double weightSum = 0;
    size_t prev = pX.size() -1;
    size_t next = 1;
    for (int i=0; i<pX.size(); i++)
    {
        double cotPrev = ENP_contangent(pP, pX[i], pX[prev]);
        double cotNext = ENP_contangent(pP, pX[i], pX[next]);
        double len2 = QVector2D(pX[i] - pP).lengthSquared();
        lam[i] = (cotPrev + cotNext) / len2;

        ++prev;
        if (prev == pX.size()) prev = 0;
        ++next;
        if (next == pX.size()) next = 0;
    }

    for (QVector<double>::iterator lamit=lam.begin(); lamit!=lam.end(); ++lamit)
    {
        *lamit = (*lamit)/weightSum;
    }

    return true;
}

void ENP_centroid(const QVector<QPointF>& pX, double& cx, double& cy)
{
    cx = 0;
    cy = 0;

    if (pX.isEmpty())
        return;

    for (QVector<QPointF>::const_iterator it=pX.begin(); it!=pX.end(); ++it)
    {
        cx += it->x();
        cy += it->y();
    }

    cx /= pX.size();
    cy /= pX.size();
}

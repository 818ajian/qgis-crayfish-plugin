/*
Crayfish - A collection of tools for TUFLOW and other hydraulic modelling packages
Copyright (C) 2012 Peter Wells for Lutra Consulting

peter dot wells at lutraconsulting dot co dot uk
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

#ifndef CRAYFISHVIEWER_H
#define CRAYFISHVIEWER_H

#include "crayfish_viewer_global.h"

#include <QImage>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QColor>

#include <math.h>

struct Element;
struct Node;
struct BBox;
struct E4Qtmp;

struct DataSet;
struct Output;

class CRAYFISHVIEWERSHARED_EXPORT CrayfishViewer {
public:

    CrayfishViewer(QString);
    ~CrayfishViewer();
    QImage* draw();

    bool loadedOk(){ return mLoadedSuccessfully; }
    bool warningsEncountered(){ return mWarningsEncountered; }
    int getLastWarning(){ return mLastWarning; }
    int getLastError() { return mLastError; }
    bool loadDataSet(QString);
    bool loadAsciiDataSet(QString fileName);
    bool isDataSetLoaded(QString fileName);

    double valueAtCoord(const Output *output, double xCoord, double yCoord);

    // mesh information

    uint nodeCount() const { return mNodeCount; }
    uint elementCount() const { return mElemCount; }
    uint elementCount_E4Q() const { return mE4Qcount; }
    uint elementCount_E3T() const { return mE3Tcount; }
    QRectF meshExtent() const { return QRectF(QPointF(mXMin,mYMin), QPointF(mXMax,mYMax)); }

    // rendering options

    void setCanvasSize(const QSize& size);
    QSize canvasSize() const;

    void setExtent(double llX, double llY, double pixelSize);
    QRectF extent() const;
    double pixelSize() const { return mPixelSize; }

    void setMeshRenderingEnabled(bool enabled);
    bool isMeshRenderingEnabled() const;

    void setMeshColor(const QColor& color);
    QColor meshColor() const;

    void setCurrentDataSetIndex(int index);
    int currentDataSetIndex() const;
    int dataSetCount() const { return mDataSets.size(); }
    const DataSet* dataSet(int dataSetIndex) const;
    const DataSet* currentDataSet() const;

    void setNoProjection();
    bool setProjection(const QString& srcProj4, const QString& destProj4);
    bool hasProjection() const;
    QString sourceCrsProj4() const { return mSrcProj4; }
    QString destCrsProj4() const { return mDestProj4; }

private:
    bool mLoadedSuccessfully;
    bool mWarningsEncountered;
    ViewerError::Enum mLastError;
    ViewerWarning::Enum mLastWarning;
    QImage* mImage;

    // global rendering options
    int mCanvasWidth;   //!< width of the current view (pixels)
    int mCanvasHeight;  //!< height of the current view (pixels)
    double mLlX;        //!< X of current view's lower-left point (mesh coords)
    double mLlY;        //!< Y of current view's lower-left point (mesh coords)
    double mUrX;        //!< X of current view's upper-right point (mesh coords)
    double mUrY;        //!< Y of current view's upper-right point (mesh coords)
    double mPixelSize;  //!< units (in mesh) per pixel (on screen)
    bool mRenderMesh;   //!< whether to render the mesh as a wireframe
    QColor mMeshColor;  //!< color used for rendering of the wireframe
    int mCurDataSetIdx; //!< index of the current dataset

    // envelope of the mesh
    double mXMin;
    double mXMax;
    double mYMin;
    double mYMax;

    // mesh topology - nodes and elements
    uint mElemCount;
    Element* mElems;
    uint mNodeCount;
    Node* mNodes;
    E4Qtmp* mE4Qtmp;
    uint mE4Qcount;
    uint mE3Tcount;

    std::vector<DataSet*> mDataSets;  //!< datasets associated with the mesh

    bool mProjection; //!< whether doing reprojection from mesh coords to map coords
    QString mSrcProj4;  //!< CRS's proj.4 string of the source (layer)
    QString mDestProj4; //!< CRS's proj.4 string of the destination (project)
    Node* mProjNodes; //!< reprojected nodes
    BBox* mProjBBoxes; //!< reprojected bounding boxes of elements

    void computeMeshExtent();
    bool nodeInsideView(uint nodeIndex);
    bool elemOutsideView(uint);
    QPoint realToPixel(double, double);
    QPoint realToPixel(int nodeIndex);
    QPointF realToPixelF(double, double);
    QPointF realToPixelF(int nodeIndex);
    void paintRow(uint, int, int, int, const DataSet* ds, const Output* output);
    bool interpolatValue(uint, double, double, double*, const Output* output);
    QPointF pixelToReal(int, int);
    void updateBBox(BBox& bbox, const Element& elem, Node* nodes);


    void renderContourData(const DataSet* ds, const Output* output);
    void renderVectorData(const DataSet* ds, const Output* output);
    void renderMesh();
};

float mag(float input){
    if(input < 0.0){
        return -1.0;
    }
    return 1.0;
}

#endif // CRAYFISHVIEWER_H

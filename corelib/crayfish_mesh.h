/*
Crayfish - A collection of tools for TUFLOW and other hydraulic modelling packages
Copyright (C) 2015 Lutra Consulting

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

#ifndef CRAYFISH_MESH_H
#define CRAYFISH_MESH_H

#include <QPointF>
#include <QString>
#include <QVector>


struct BBox;
struct E4Qtmp;
class DataSet;
class ElementOutput;
class Output;
class NodeOutput;


struct Node
{
    double x;
    double y;

    bool operator==(const Node& other) const { return x == other.x && y == other.y; }
    QPointF toPointF() const { return QPointF(x,y); }
    void setId(int id) {mId = id;}
    int id() const {return mId;}
private:
    int mId;    //!< just a reference to the ID in the input file (internally we use indices)
};

struct BBox
{
  double minX;
  double maxX;
  double minY;
  double maxY;

  bool isPointInside(double x, double y) const { return x >= minX && x <= maxX && y >= minY && y <= maxY; }
};

class Element
{
public:
    enum Type
    {
      Undefined,
      ENP,
      E4Q,
      E3T,
      E2L
    };

    int nodeCount() const { return mP.size(); }
    bool isDummy() const { return mEType == Undefined; }
    Type eType() const {return mEType;}

    void setEType(Type eType) {
        Q_ASSERT(eType != ENP);
        switch (eType) {
            case E4Q : setEType(eType, 4); break;
            case E3T : setEType(eType, 3); break;
            case E2L : setEType(eType, 2); break;
            default: setEType(eType, 0);
        }
    }
    void setEType(Type eType, int node_count) {
        mEType = eType;
        mP.resize(node_count);
    }
    uint p(int idx) const {Q_ASSERT(idx < nodeCount()); return mP[idx];}
    void setP(int idx, uint val) {Q_ASSERT(idx < nodeCount()); mP[idx] = val;}
    void setP(uint* vals) {for (int i=0; i<nodeCount(); i++) {mP[i] = vals[i];}}
    void setId(int id) {mId = id;}
    int id() const {return mId;}
private:
    int mId;        //!< just a reference to the ID in the input file (internally we use indices)
    Type mEType;
    QVector<uint> mP; //!< indices of nodes
};


/** core Mesh data structure: nodes + elements */
class BasicMesh
{
public:
  typedef QVector<Node> Nodes;
  typedef QVector<Element> Elements;
  typedef QVector<DataSet*> DataSets;

  BasicMesh(const Nodes& nodes, const Elements& elements);
  ~BasicMesh();

  const Nodes& nodes() const { return mNodes; }
  const Elements& elements() const { return mElems; }

  int elementCountForType(Element::Type type);

protected:

  // mesh topology
  Nodes mNodes;
  Elements mElems;

};

class ValueAccessor;

/** Adds data + functionality for reprojection, identification, support for rendering */
class Mesh : public BasicMesh
{
public:
  Mesh(const Nodes& nodes, const Elements& elements);
  ~Mesh();

  void addDataSet(DataSet* ds);

  const DataSets& dataSets() const { return mDataSets; }

  BBox extent() const { return mExtent; }

  double valueAt(const Output* output, double xCoord, double yCoord) const;
  bool valueAt(uint elementIndex, double x, double y, double* value, const Output* output) const;

  bool vectorValueAt(uint elementIndex, double x, double y, double* valueX, double* valueY, const Output* output) const;

  void setSourceCrs(const QString& srcProj4); // proj4
  void setSourceCrsFromWKT(const QString& wkt); // wkt

  void setDestinationCrs(const QString& destProj4);
  bool hasProjection() const;
  QString sourceCrs() const { return mSrcProj4; }
  QString destinationCrs() const { return mDestProj4; }

  BBox projectedExtent() const { return mProjection ? mProjExtent : mExtent; }
  const Node* projectedNodes() const { return mProjection ? mProjNodes : mNodes.constData(); }
  const Node& projectedNode(int nodeIndex) const { return mProjection ? mProjNodes[nodeIndex] : mNodes[nodeIndex]; }
  const BBox& projectedBBox(int elemIndex) const { return mProjection ? mProjBBoxes[elemIndex] : mBBoxes[elemIndex]; }

  //! calculate centroid of given element (takes reprojection into account)
  void elementCentroid(int elemIndex, double& cx, double& cy) const;

protected:

  bool reprojectMesh();
  void setNoProjection();

  BBox computeMeshExtent(bool projected);
  void computeTempRendererData();

  //! low-level interpolation routine
  bool interpolate(uint elementIndex, double x, double y, double* value, const NodeOutput* output, const ValueAccessor* accessor) const;

  //! low-level interpolation routine for element-centered results
  bool interpolateElementCentered(uint elementIndex, double x, double y, double* value, const ElementOutput* output, const ValueAccessor* accessor) const;

  BBox mExtent; //!< unprojected mesh extent

  // associated data
  DataSets mDataSets;  //!< pointers to datasets are owned by this class

  // cached data for rendering

  E4Qtmp* mE4Qtmp;   //!< contains rendering information for quads
  int* mE4QtmpIndex; //!< for conversion from element index to mE4Qtmp indexes
  BBox* mBBoxes; //! bounding boxes of elements (non-projected)

  // reprojection support

  QString mSrcProj4;  //!< CRS's proj.4 string of the source (layer)
  QString mDestProj4; //!< CRS's proj.4 string of the destination (project)

  bool mProjection; //!< whether doing reprojection from mesh coords to map coords
  Node* mProjNodes; //!< reprojected nodes
  BBox* mProjBBoxes; //!< reprojected bounding boxes of elements
  BBox mProjExtent;
};


#endif // CRAYFISH_MESH_H

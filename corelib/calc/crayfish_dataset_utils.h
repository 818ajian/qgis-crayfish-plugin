#ifndef CRAYFISH_DATASET_UTILS_H
#define CRAYFISH_DATASET_UTILS_H

#include <QStringList>
#include <QMap>
#include <QVector>
#include <algorithm>
#include <functional>
#include <math.h>
#include <numeric>

#include "crayfish_mesh.h"
#include "crayfish_dataset.h"
#include "crayfish_output.h"

class CrayfishDataSetUtils
{
private:
  float ffilter(float val1, float filter) const;
  float fadd(float val1, float val2) const;
  float fsubtract(float val1, float val2) const;
  float fmultiply(float val1, float val2) const;
  float fdivide(float val1, float val2) const;
  float fpower(float val1, float val2) const;
  float fequal(float val1, float val2) const;
  float fnotEqual(float val1, float val2) const;
  float fgreaterThan(float val1, float val2) const;
  float flesserThan(float val1, float val2) const;
  float flesserEqual(float val1, float val2) const;
  float flogicalAnd(float val1, float val2) const;
  float flogicalOr(float val1, float val2) const;
  float flogicalNot(float val1) const;
  float fchangeSign(float val1) const;
  float fmin(float val1, float val2) const;
  float fmax(float val1, float val2) const;
  float fabs(float val1) const;
  float fsum_aggr(QVector<float> vals) const;
  float fmin_aggr(QVector<float> vals) const;
  float fmax_aggr(QVector<float> vals) const;
  float favg_aggr(QVector<float> vals) const;

  Output* canditateOutput(DataSet &dataset, int time_index) const;
  const Output* constCanditateOutput(const DataSet& dataset, int time_index) const;
  int outputTimesCount(const DataSet& dataset1, const DataSet& dataset2) const;

  void func1(DataSet& dataset1, std::function<float(float)> func) const;
  void func2(DataSet& dataset1, const DataSet& dataset2, std::function<float(float,float)> func) const;
  void funcAggr(DataSet& dataset1, std::function<float(QVector<float>)> func) const;

  const Mesh* mMesh;
  bool mIsValid; // all used datasets (in datasetMap) do have outputs for same times.
                 // all used dataset names are present in mesh
  Output::Type mOutputType; // mesh can work only with one output types, so you cannot mix
                            // e.g. one dataset with element outputs and one with node outputs
  QVector<float> mTimes;
  QMap < QString, const DataSet* > mDatasetMap;

public:
    CrayfishDataSetUtils(const Mesh* mesh, const QStringList& usedDatasetNames);
    bool isValid();
    const Mesh* mesh() const { return mMesh; }
    const DataSet* dataset( const QString& datasetName ) const {const DataSet* ds = mDatasetMap[datasetName]; return ds;}

    void ones( DataSet& dataset1) const;
    void nodata( DataSet& dataset1) const;
    void copy( DataSet& dataset1, const QString& datasetName ) const;
    void copy( DataSet& dataset1, const DataSet& dataset2 ) const;
    void tranferOutputs( DataSet& dataset1, DataSet& dataset2 ) const;


    void number( DataSet& dataset1, float val) const;

    void logicalNot(DataSet& dataset1) const {return func1(dataset1, std::bind(&CrayfishDataSetUtils::flogicalNot, this, std::placeholders::_1));}
    void changeSign(DataSet& dataset1) const {return func1(dataset1, std::bind(&CrayfishDataSetUtils::fchangeSign, this, std::placeholders::_1));}
    void abs(DataSet& dataset1) const {return func1(dataset1, std::bind(&CrayfishDataSetUtils::fabs, this, std::placeholders::_1));}

    void add(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fadd, this, std::placeholders::_1, std::placeholders::_2));}
    void subtract(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fsubtract, this, std::placeholders::_1, std::placeholders::_2));}
    void multiply(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fmultiply, this, std::placeholders::_1, std::placeholders::_2));}
    void divide(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fdivide, this, std::placeholders::_1, std::placeholders::_2));}
    void power(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fpower, this, std::placeholders::_1, std::placeholders::_2));}
    void equal(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fequal, this, std::placeholders::_1, std::placeholders::_2));}
    void notEqual(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fnotEqual, this, std::placeholders::_1, std::placeholders::_2));}
    void greaterThan(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fgreaterThan, this, std::placeholders::_1, std::placeholders::_2));}
    void lesserThan(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::flesserThan, this, std::placeholders::_1, std::placeholders::_2));}
    void lesserEqual(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::flesserEqual, this, std::placeholders::_1, std::placeholders::_2));}
    void logicalAnd(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::flogicalAnd, this, std::placeholders::_1, std::placeholders::_2));}
    void logicalOr(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::flogicalOr, this, std::placeholders::_1, std::placeholders::_2));}
    void min(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fmin, this, std::placeholders::_1, std::placeholders::_2));}
    void max(DataSet& dataset1, const DataSet& dataset2) const {return func2(dataset1, dataset2, std::bind(&CrayfishDataSetUtils::fmax, this, std::placeholders::_1, std::placeholders::_2));}
    void filter( DataSet& dataset1, const DataSet& filter ) const {return func2(dataset1, filter, std::bind(&CrayfishDataSetUtils::ffilter, this, std::placeholders::_1, std::placeholders::_2));}

    void sum_aggr(DataSet& dataset1) const {return funcAggr(dataset1, std::bind(&CrayfishDataSetUtils::fsum_aggr, this, std::placeholders::_1));}
    void min_aggr(DataSet& dataset1) const {return funcAggr(dataset1, std::bind(&CrayfishDataSetUtils::fmin_aggr, this, std::placeholders::_1));}
    void max_aggr(DataSet& dataset1) const {return funcAggr(dataset1, std::bind(&CrayfishDataSetUtils::fmax_aggr, this, std::placeholders::_1));}
    void avg_aggr(DataSet& dataset1) const {return funcAggr(dataset1, std::bind(&CrayfishDataSetUtils::favg_aggr, this, std::placeholders::_1));}
};


#endif // CRAYFISH_DATASET_UTILS_H

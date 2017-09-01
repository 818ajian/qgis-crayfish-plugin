#include "calc/crayfish_dataset_utils.h"


CrayfishDataSetUtils::CrayfishDataSetUtils(const Mesh &mesh, const QStringList& usedDatasetNames)
    : mMesh(mesh)
    , mIsValid(false)
{
    // First populate datasetMap and see if we have all datasets present
    foreach (const QString& datasetName, usedDatasetNames )
    {
        DataSet* ds = mMesh.dataSet(datasetName);
        if (ds == 0)
            return;

        mDatasetMap.insert(datasetName, ds);
    }

    // Now populate used times and check that all datasets do have some times
    // OR just one time (== one output)
    bool times_populated = false;
    foreach (const DataSet* ds, mDatasetMap.values()) {
        if (ds->outputCount() == 0) {
            // dataset must have at least 1 output
            return;
        }

        if (ds->isTimeVarying()) {

            if (times_populated)
            {
                if (ds->outputCount() != mTimes.size()) {
                    // different number of outputs in the datasets
                    return;
                }
            }

            for(int output_index=0; output_index < ds->outputCount(); ++output_index) {
                const Output* o = ds->constOutput(output_index);
                if (times_populated) {
                    if ( fabs(mTimes[output_index] - o->time) > 1e-5 )
                    {
                        // error, the times in different datasets differs
                        return;
                    }
                } else {
                    mTimes.append(o->time);
                }
            }

            times_populated = true;
        }
    }

    // case of all datasets not time varying of usedDatasetNames is empty
    if (mTimes.isEmpty()) {
        mTimes.push_back(0.0f);
    }

    mIsValid = true;
}

bool CrayfishDataSetUtils::isValid()
{
    return mIsValid;
}

DataSet CrayfishDataSetUtils::number(float val) const
{
    DataSet ds("number");
    NodeOutput* o = new NodeOutput();
    o->init(mMesh.nodes().size(),
            mMesh.elements().size(),
            DataSet::Scalar);


    memset(o->getActive().data(), val == -9999, mMesh.elements().size()); // All cells active
    memset(o->getValues().data(), val, mMesh.elements().size()); // All values val

    ds.addOutput(o);

    return ds;
}

DataSet CrayfishDataSetUtils::ones() const {
    DataSet ds = number(1);
    ds.setName("ones");
    return ds;
}

DataSet CrayfishDataSetUtils::nodata() const {
    DataSet ds = number(-9999);
    ds.setName("nodata");
    return ds;
}

DataSet CrayfishDataSetUtils::copy( const QString& datasetName ) const {
    DataSet ds("copy");
    const DataSet* ds0 = dataset(datasetName);

    for(int output_index=0; output_index < ds0->outputCount(); ++output_index) {
        const Output* o0 = ds0->constOutput(output_index);

        if (o0->type() == Output::TypeNode) {
            const NodeOutput* node_o0 = ds0->constNodeOutput(output_index);

            NodeOutput* output = new NodeOutput();
            output->init(mMesh.nodes().size(),
                    mMesh.elements().size(),
                    DataSet::Scalar);

            for (int n=0; n<mMesh.nodes().size(); ++n)
            {
                output->getValues()[n] = node_o0->loadedValues()[n];
            }

            memset(output->getActive().data(), 1, mMesh.elements().size()); // All cells active
            ds.addOutput(output);

        } else {
            const ElementOutput* elem_o0 = ds0->constElemOutput(output_index);

            ElementOutput* output = new ElementOutput();
            output->init(mMesh.elements().size(), false);
            for (int n=0; n<mMesh.elements().size(); ++n)
            {
                output->getValues()[n] = elem_o0->loadedValues()[n];
            }

            ds.addOutput(output);
        }
    }

    return ds;
}


const Output* CrayfishDataSetUtils::canditateOutput(const DataSet& dataset, int time_index) const {
    if (dataset.isTimeVarying()) {
        Q_ASSERT(dataset.outputCount() > time_index);

        return dataset.constOutput(time_index);
    } else {
        Q_ASSERT(dataset.outputCount() == 1);
        return dataset.constOutput(0);
    }
}

int CrayfishDataSetUtils::outputTimesCount(const DataSet& dataset1, const DataSet& dataset2) const {
    if (dataset1.isTimeVarying() && dataset2.isTimeVarying()) {
        return mTimes.size();
    } else {
        return 1;
    }
}

DataSet CrayfishDataSetUtils::func1(const DataSet& dataset1, std::function<float(float)> func) const {
    DataSet ds("");

    for (int time_index=0; time_index<dataset1.outputCount(); ++time_index) {
        const Output* o1 = canditateOutput(dataset1, time_index);
        if (o1->type() == Output::TypeNode ) {
            NodeOutput* output = new NodeOutput();
            output->init(mMesh.nodes().size(),
                    mMesh.elements().size(),
                    DataSet::Scalar);

            for (int n=0; n<mMesh.nodes().size(); ++n)
            {
                float val1 = mMesh.valueAt(n, o1);
                float res_val = -9999;
                if (val1 != -9999) {
                    res_val = func(val1);
                }
                output->getValues()[n] = res_val;
            }

            memset(output->getActive().data(), 1, mMesh.elements().size()); // All cells active
            ds.addOutput(output);

        } else {
            ElementOutput* output = new ElementOutput();
            output->init(mMesh.elements().size(), false);

            for (int n=0; n<mMesh.elements().size(); ++n)
            {
                float val1 = mMesh.valueAt(n, o1);
                float res_val = -9999;
                if (val1 != -9999) {
                    res_val = func(val1);
                }
                output->getValues()[n] = res_val;
            }

            ds.addOutput(output);
        }
    }

    return ds;
}


DataSet CrayfishDataSetUtils::func2(const DataSet& dataset1, const DataSet& dataset2, std::function<float(float,float)> func) const {
    DataSet ds("");

    for (int time_index=0; time_index<outputTimesCount(dataset1, dataset2); ++time_index) {
        const Output* o1 = canditateOutput(dataset1, time_index);
        const Output* o2 = canditateOutput(dataset2, time_index);

        if ((o1->type() == o2->type()) && (o1->type() == Output::TypeNode )) {

            NodeOutput* output = new NodeOutput();
            output->init(mMesh.nodes().size(),
                    mMesh.elements().size(),
                    DataSet::Scalar);

            for (int n=0; n<mMesh.nodes().size(); ++n)
            {
                float val1 = mMesh.valueAt(n, o1);
                float val2 = mMesh.valueAt(n, o2);
                float res_val = -9999;
                if ((val1 != -9999) && (val2 != -9999) ) {
                    res_val = func(val1, val2);
                }

                output->getValues()[n] = res_val;
            }

            memset(output->getActive().data(), 1, mMesh.elements().size()); // All cells active
            ds.addOutput(output);

        } else if ((o1->type() == o2->type()) && (o1->type() == Output::TypeElement )) {

            ElementOutput* output = new ElementOutput();
            output->init(mMesh.elements().size(), false);

            for (int n=0; n<mMesh.elements().size(); ++n)
            {
                float val1 = mMesh.valueAt(n, o1);
                float val2 = mMesh.valueAt(n, o2);
                float res_val = -9999;
                if ((val1 != -9999) && (val2 != -9999) ) {
                    res_val = func(val1, val2);
                }

                output->getValues()[n] = res_val;
            }
            ds.addOutput(output);
        } else { // mixture, do element-output since we have interpolation functions for that

            ElementOutput* output = new ElementOutput();
            output->init(mMesh.elements().size(), false);

            for (int n=0; n<mMesh.elements().size(); ++n)
            {
                double val1, val2;
                double cx, cy;
                mMesh.elementCentroid(n, cx, cy);
                bool res1 = mMesh.valueAt(n, cx, cy, &val1, o1);
                bool res2 = mMesh.valueAt(n, cx, cy, &val2, o2);
                float res_val = -9999.0;

                if (res1 && res2) {
                    if ((val1 != -9999.0) && (val2 != -9999.0) ) {
                        res_val = func(float(val1), float(val2));
                    }
                }
                output->getValues()[n] = res_val;
            }
            ds.addOutput(output);

        }
    }
    return ds;
}

DataSet CrayfishDataSetUtils::funcAggr(const DataSet& dataset1, std::function<float(QVector<float>)> func) const {
    DataSet ds("");

    const Output* o0 = canditateOutput(dataset1, 0);

    if (o0->type() == Output::TypeNode ) {
        QVector < float > vals;
        NodeOutput* output = new NodeOutput();
        output->init(mMesh.nodes().size(),
                mMesh.elements().size(),
                DataSet::Scalar);

        for (int n=0; n<mMesh.nodes().size(); ++n)
        {
            for (int time_index=0; time_index<dataset1.outputCount(); ++time_index) {
                const Output* o1 = canditateOutput(dataset1, time_index);
                Q_ASSERT(o1->type() == o0->type());
                float val1 = mMesh.valueAt(n, o1);
                if (val1 != -9999) {
                    vals.push_back(val1);
                }
            }

            float res_val = -9999;
            if (!vals.isEmpty()) {
                res_val = func(vals);
            }

            output->getValues()[n] = res_val;
        }

        memset(output->getActive().data(), 1, mMesh.elements().size()); // All cells active
        ds.addOutput(output);
    } else {
        QVector < float > vals;
        ElementOutput* output = new ElementOutput();
        output->init(mMesh.elements().size(), false);

        for (int n=0; n<mMesh.elements().size(); ++n)
        {
            for (int time_index=0; time_index<dataset1.outputCount(); ++time_index) {
                const Output* o1 = canditateOutput(dataset1, time_index);
                Q_ASSERT(o1->type() == o0->type());
                float val1 = mMesh.valueAt(n, o1);
                if (val1 != -9999) {
                    vals.push_back(val1);
                }
            }

            float res_val = -9999;
            if (!vals.isEmpty()) {
                res_val = func(vals);
            }

            output->getValues()[n] = res_val;
        }
        ds.addOutput(output);
    }

    return ds;
}

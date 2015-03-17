
#include "crayfish.h"
#include "crayfish_mesh.h"
#include "crayfish_dataset.h"
#include "crayfish_output.h"
#include "crayfish_renderer.h"

#define CF_TYPES
typedef Mesh* MeshH;
typedef const Node* NodeH;
typedef const Element* ElementH;
typedef DataSet* DataSetH;
typedef const Output* OutputH;
typedef Renderer::Config* RendererConfigH;
typedef Renderer* RendererH;
typedef QVariant* VariantH;
typedef QImage* ImageH;
typedef ColorMap* ColorMapH;

#include "crayfish_capi.h"

static LoadStatus sLastLoadStatus;


int CF_Version()
{
  // TODO: generate automatically
  return 0x010300; // 1.3
}


MeshH CF_LoadMesh(const char* meshFile)
{
  return (MeshH) Crayfish::loadMesh(QString::fromUtf8(meshFile), &sLastLoadStatus);
}


void CF_CloseMesh(MeshH mesh)
{
  delete mesh;
}


int CF_Mesh_nodeCount(MeshH mesh)
{
  return mesh->nodes().count();
}


int CF_Mesh_elementCount(MeshH mesh)
{
  return mesh->elements().count();
}


NodeH CF_Mesh_nodeAt(MeshH mesh, int index)
{
  if (index < 0 || index >= mesh->nodes().count())
    return 0;

  const Node& n = mesh->nodes()[index];
  return &n;
}


ElementH CF_Mesh_elementAt(MeshH mesh, int index)
{
  if (index < 0 || index >= mesh->elements().count())
    return 0;

  const Element& e = mesh->elements()[index];
  return &e;
}


int CF_Mesh_dataSetCount(MeshH mesh)
{
  return mesh->dataSets().count();
}


DataSetH CF_Mesh_dataSetAt(MeshH mesh, int index)
{
  if (index < 0 || index >= mesh->dataSets().count())
    return 0;

  DataSet* ds = mesh->dataSets()[index];
  return ds;
}


int CF_DS_type(DataSetH ds)
{
  return ds->type();
}


// helper to return string data - without having to deal with memory too much.
// returned pointer is valid only next call. also not thread-safe.
const char* _return_str(const QString& str)
{
  static QByteArray lastStr;
  lastStr = str.toUtf8();
  return lastStr.constData();
}


const char* CF_DS_name(DataSetH ds)
{
  return _return_str(ds->name());
}


int CF_DS_outputCount(DataSetH ds)
{
  return ds->outputCount();
}


OutputH CF_DS_outputAt(DataSetH ds, int index)
{
  if (index < 0 || index >= ds->outputCount())
    return 0;

  return ds->output(index);
}


MeshH CF_DS_mesh(DataSetH ds)
{
  return (MeshH)ds->mesh();
}


float CF_O_time(OutputH o)
{
  return o->time;
}


float CF_O_valueAt(OutputH o, int index)
{
  return o->values[index];
}

char CF_O_statusAt(OutputH o, int index)
{
  return o->active[index];
}

DataSetH CF_O_dataSet(OutputH o)
{
  return (DataSetH)o->dataSet;
}


bool CF_Mesh_loadDataSet(MeshH mesh, const char* path)
{
  Mesh::DataSets lst = Crayfish::loadDataSet(QString::fromUtf8(path), mesh, &sLastLoadStatus);
  if (!lst.count())
    return false;

  foreach (DataSet* ds, lst)
    mesh->addDataSet(ds);
  return true;
}


int CF_LastLoadError()
{
  return sLastLoadStatus.mLastError;
}


int CF_LastLoadWarning()
{
  return sLastLoadStatus.mLastWarning;
}


RendererH CF_R_create(RendererConfigH cfg, ImageH img)
{
  return new Renderer(*cfg, *img);
}


void CF_R_destroy(RendererH rend)
{
  delete rend;
}


void CF_R_draw(RendererH rend)
{
  rend->draw();
}


RendererConfigH CF_RC_create()
{
  return new Renderer::Config();
}


void CF_RC_destroy(RendererConfigH cfg)
{
  delete cfg;
}


void CF_RC_setView(RendererConfigH cfg, int width, int height, double llx, double lly, double pixelSize)
{
  cfg->outputSize = QSize(width, height);
  cfg->llX = llx;
  cfg->llY = lly;
  cfg->pixelSize = pixelSize;
}


void CF_RC_setOutput(RendererConfigH cfg, OutputH output)
{
  cfg->output = output;
}


void CF_Mesh_extent(MeshH mesh, double* xmin, double* ymin, double* xmax, double* ymax)
{
  BBox b = mesh->extent();
  *xmin = b.minX;
  *ymin = b.minY;
  *xmax = b.maxX;
  *ymax = b.maxY;
}


void CF_DS_valueRange(DataSetH ds, float* vMin, float* vMax)
{
  *vMin = ds->minZValue();
  *vMax = ds->maxZValue();
}


void CF_RC_setParam(RendererConfigH cfg, const char* key, VariantH value)
{
  QString k = QString::fromAscii(key);
  if (k == "mesh")
    cfg->mesh.mRenderMesh = value->toBool();
  else if (k == "meshcolor")
    cfg->mesh.mMeshColor = value->value<QColor>();
  else if (k == "contours")
    cfg->ds.mRenderContours = value->toBool();
  else if (k == "colormap")
    cfg->ds.mColorMap = value->value<ColorMap>();
  else if (k == "vectors")
    cfg->ds.mRenderVectors = value->toBool();
  else
    qDebug("[setParam] unknown key: %s", key);
}


VariantH CF_V_create()
{
  return new QVariant();
}


void CF_V_destroy(VariantH v)
{
  delete v;
}


void CF_V_fromInt(VariantH v, int i)
{
  *v = QVariant(i);
}


int CF_V_toInt(VariantH v)
{
  return v->toInt();
}


void CF_V_fromColor(VariantH v, int r, int g, int b, int a)
{
  *v = QVariant::fromValue(QColor(r,g,b,a));
}


ColorMapH CF_CM_create()
{
  return new ColorMap();
}


void CF_CM_destroy(ColorMapH cm)
{
  delete cm;
}


void CF_V_toColorMap(VariantH v, ColorMapH cm)
{
  *cm = v->value<ColorMap>();
}


void CF_V_fromColorMap(VariantH v, ColorMapH cm)
{
  *v = QVariant::fromValue(*cm);
}


int CF_CM_itemCount(ColorMapH cm)
{
  return cm->items.count();
}


double CF_CM_itemValue(ColorMapH cm, int index)
{
  return cm->items[index].value;
}


int CF_CM_itemColor(ColorMapH cm, int index)
{
  return cm->items[index].color;
}


const char* CF_CM_itemLabel(ColorMapH cm, int index)
{
  return _return_str( cm->items[index].label );
}


void CF_CM_setItemCount(ColorMapH cm, int count)
{
  cm->items.resize(count);
}


void CF_CM_setItemValue(ColorMapH cm, int index, double value)
{
  cm->items[index].value = value;
}


void CF_CM_setItemColor(ColorMapH cm, int index, int color)
{
  cm->items[index].color = color;
}


void CF_CM_setItemLabel(ColorMapH cm, int index, const char* label)
{
  cm->items[index].label = QString::fromUtf8(label);
}


ColorMapH CF_CM_createDefault(double vmin, double vmax)
{
  return new ColorMap(ColorMap::defaultColorMap(vmin, vmax));
}

void CF_CM_clip(ColorMapH cm, int* clipLow, int* clipHigh)
{
  *clipLow = cm->clipLow;
  *clipHigh = cm->clipHigh;
}


void CF_CM_setClip(ColorMapH cm, int clipLow, int clipHigh)
{
  cm->clipLow = clipLow;
  cm->clipHigh = clipHigh;
}


int CF_CM_alpha(ColorMapH cm)
{
  return cm->alpha;
}


void CF_CM_setAlpha(ColorMapH cm, int alpha)
{
  cm->alpha = alpha;
}


int CF_CM_method(ColorMapH cm)
{
  return cm->method;
}


void CF_CM_setMethod(ColorMapH cm, int method)
{
  cm->method = (ColorMap::Method) method;
}

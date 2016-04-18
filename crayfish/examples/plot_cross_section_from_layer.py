"""
Python script that produces a cross section plot from features of a layer
"""

import csv
from qgis.core import QgsGeometry, QgsPoint, QgsFeature, QgsVectorLayer

import crayfish.plot

from . import load_example_mesh

# Load mesh and a results file
mesh = load_example_mesh()

# Get data for the plot
output = mesh.dataset(1).output(0)  # first output of second dataset

# Prepare a memory layer
layer = QgsVectorLayer("LineString", "my layer", "memory")
f = QgsFeature()
f.setGeometry(QgsGeometry.fromPolyline([QgsPoint(1000,1005), QgsPoint(1028,1005)]))
layer.dataProvider().addFeatures([f])

# -or- load a shapefile layer (instead of using temporary memory layer)
#
# layer = QgsVectorLayer("/data/shapefile.shp", "my layer", "ogr")
# if not layer.isValid(): raise Exception("Load failed!")

for feature in layer.getFeatures():

    x, y = crayfish.plot.cross_section_plot_data(output, feature.geometry())

    # print the source geometry and plot data
    print "Geometry:", feature.geometry().exportToWkt()
    for xi,yi in zip(x,y):
        print xi, yi

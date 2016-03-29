Crayfish (QGIS plugin)
======================

<img align="right" src="https://raw.githubusercontent.com/lutraconsulting/qgis-crayfish-plugin/master/crayfish/crayfish_128px.png">

The Crayfish Plugin is visualiser for temporal structured/unstructured grids in QGIS.

You can use Crayfish to load the following file formats in QGIS:


- [NetCDF](https://en.wikipedia.org/wiki/NetCDF): Examples can be found [here](http://apps.ecmwf.int/datasets/data/interim-full-daily/levtype=sfc/)
- [GRIB](https://en.wikipedia.org/wiki/GRIB): Examples can be found [here](http://apps.ecmwf.int/datasets/data/interim-full-daily/levtype=sfc/)
- [XMDF](https://en.wikipedia.org/wiki/XMDF): As an example, hydraulic outputs from TUFLOW modelling package
- [HDF5](https://en.wikipedia.org/wiki/Hierarchical_Data_Format): e.g. Hec RAS 2D output files
-  [DAT](http://www.xmswiki.com/wiki/SMS:ASCII_Dataset_Files_*.dat): e.g. outputs of the BASEMENT hydrodynamic modelling package
-  [Selafin files](http://www.gdal.org/drv_selafin.html) e.g. outputs of the TELEMAC 2D hydrodynamic modelling package

<img src="https://travis-ci.org/lutraconsulting/qgis-crayfish-plugin.svg?branch=master">

### Using Crayfish

For instructions of how to install and use Crayfish, please refer to the [Crayfish resources page][crp] on our website.

### Installing Crayfish on Linux

For installing Crayfish on Linux you need:

* development environment and a compiler installed
* Qt4 and development tools
* HDF5 library (for XMDF format)
* NetCDF library (for AnuGA SWW format)
* GDAL library

On Debian/Ubuntu you need to install the following packages:

```bash
sudo apt-get install build-essential libqt4-dev qt4-qmake libgdal-dev libhdf5-dev libnetcdf-dev libproj-dev git
```

If all this packages are installed you can clone the crayfish plugin using the command:

```bash
git clone https://github.com/lutraconsulting/qgis-crayfish-plugin.git
```

After cloning the source you should run the installation script which compiles and installs everything
to user's QGIS python plugin directory (`~/.qgis2/python/plugins/crayfish`):

```bash
cd qgis-crayfish-plugin
./install.py
```

Now restart QGIS and you are able to use crayfish plugin on your Linux Computer.


### Installing Crayfish on Windows

For 32-bit version:

* Install Microsoft Visual Studio 2008
* Install OSGeo4W (32bit) to C:\OSGeo4W
* Run scripts\crayfish32.bat to build the crayfish DLL

For 64-bit version:

* Install Microsoft Visual Studio 2010
* Install OSGeo4W (64bit) to C:\OSGeo4W64
* Run scripts\crayfish64.bat to build the crayfish DLL


[crp]: http://www.lutraconsulting.co.uk/resources/crayfish

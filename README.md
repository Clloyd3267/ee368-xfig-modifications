# EE368 XFIG Enhancements

A few additional features for the interactive drawing tool XFig.

### Installation

See XFig Installation notes for general installation guide: http://mcj.sourceforge.net/installation.html

1. Clone this repo

```
git clone https://github.com/Clloyd3267/ee368-xfig-modifications.git
```

2. Change directory to XFig source directory

```
cd ee368-xfig-modifications/xfig-3.2.6a
```

3. [Reconfigure](https://stackoverflow.com/questions/33278928/how-to-overcome-aclocal-1-15-is-missing-on-your-system-warning) AC Local Configuration 

```
autoreconf -f -i
```

4. Allow execution to configure script

```
chmod +x configure
```

5. Run the configure script

```
./configure
```

6. Build XFig

```
make -j
```

7. **(Optional)** Clean build

```
make clean
```


### Branches

| Branch Name                 | Description                                                                              |
|-----------------------------|------------------------------------------------------------------------------------------|
| **main**                    | The main release branch.                                                                 |
| **integration**             | A staging branch to integrate changes together for release to main.                      |
| **export-2-vdx-dev**        | A branch for development of the Export to Visio feature.                                 |
| **layer-ctrl-dev**          | A branch for the development of the Layer Control (Bring to Back/Send to Front) feature. |
| **undo-redo-dev**           | A branch for the development of the better Undo/Redo functionality feature.              |
| **quick-menu-dev**          | A branch for the development of the Quick Menu feature.                                  |


**CDL=>** Add more here: Contributors/License/Better installation guide/Feature descriptions/etc..

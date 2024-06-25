# Documentation
***ml4proflow*** uses *[Sphinx](https://www.sphinx-doc.org/en/master/)* for generating code documentation.
> Sphinx is a tool that makes it easy to create intelligent and beautiful documentation, written by Georg Brandl and licensed under the BSD license.
> 
> It was originally created for the Python documentation, and it has excellent facilities for the documentation of software projects in a range of languages. Of course, this site is also created from reStructuredText sources using Sphinx!
# Setup
## Prerequisites
If the recommended template is used, the ```setup.py``` is already correct configured. It should contain the key *extras_require*. An example is given below:
```python 
extras_require={
   "tests": ["pytest", 
            "pandas-stubs",
            "pytest-html",
            "pytest-cov",
            "flake8",
            "mypy"],
   "docs": ["sphinx", "sphinx-rtd-theme", "recommonmark"],
   }
```
Install the dependencies via pip inside the root folder of the ```setup.py``` 
```bash
$ pip install .[docs]
```
> For automatically generate the documentation, the following packages need to be installed inside your python environment:
> - [Sphinx](https://pypi.org/project/Sphinx/)
> - [sphinx-rtd-theme](https://pypi.org/project/sphinx-rtd-theme/)
> - [recommonmark](https://pypi.org/project/recommonmark/)

# Generate Doucmentation
## Initialize Sphinx
To initialize *Sphinx* and set up the document layout, run the quickstart-comment of Sphinx. This will initialize the layout inside the folder ```/docs```. Follow the instructions in the terminal for the correct configuration.
```bash
$ sphinx-quickstart docs
```
> Set the meta data according to your module.
> Separate the source and build directories.

After the last question, new content is placed inside the folder ```/docs```. The new folder structure should be similar to this one:
``` 
docs
└── build
└── source
|   └── _static
|   └── _templates
│   │   conf.py
│   │   index.rst
│   make.bat
│   Makefile
│   README.md
```
## Configure document layout
After the initialization, the newly generated files need to be adapted according to the layout and the structure of the framework, including the python script ```conf.py```This file is called the *build configuration file* and contains almost all settings to customize the generated documentation. 

First of all, you will need to set the correct path of your source code, so Sphinx is available to find the modules. 
To do so, adapt the following lines in the section path setup: 
```python
# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('../../src/ml4proflow_mods/'))
```
The next section in the configuration file contains information about the project. These lines are automatically filled according to the answers during the initialization of Sphinx. 
> Follow the instructions in the terminal for the correct configuration.
These lines do not need to be modified, but you should check, if the present information are correct and complete. 

In the following section *General configuration* you will find some information about the configuration of Sphinx.
Here you should include the extension for including documentation from docstrings. Therefore, extend the list of extensions with ```'sphinx.ext.autodoc'```.  
```python 
extensions = ['sphinx.ext.autodoc']
``` 

To generate the documentation in the same theme as the main framework *ml4proflow*, change the ```html_theme``` option in the section *Options for HTML output*.
 ```python 
html_theme = 'sphinx_rtd_theme'
``` 

To support markdown for the ``README.md`` it is necessary to add the following options at the end of ``conf.py``:
```python
# -- Markdown support --------------------------------------------------------

from recommonmark.parser import CommonMarkParser

# The suffix of source filenames.
source_suffix = ['.rst', '.md']

source_parsers = {
	'.md': CommonMarkParser,
}
```


### Configure the conf.py:
```python 
extensions = ['sphinx.ext.autodoc']

html_theme = 'sphinx_rtd_theme'
``` 


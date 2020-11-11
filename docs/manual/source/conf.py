# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

#import sphinx_rtd_theme

from sphinx.transforms import SphinxTransform
from docutils import nodes
from sphinx import addnodes

# -- Project information -----------------------------------------------------

project = 'The ulab book'
copyright = '2019-2020, Zoltán Vörös and contributors'
author = 'Zoltán Vörös'

# The full version, including alpha/beta/rc tags
release = '1.3.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

latex_maketitle = r'''
\begin{titlepage}
\begin{flushright}
\Huge\textbf{The $\mu$lab book}
\vskip 0.5em
\LARGE
\textbf{Release 1.3.0}
\vskip 5em
\huge\textbf{Zoltán Vörös}
\end{flushright}
\begin{flushright}
\LARGE
\vskip 2em
with contributions by
\vskip 2em
\textbf{Roberto Colistete Jr.}
\vskip 0.2em
\textbf{Jeff Epler}
\vskip 0.2em
\textbf{Taku Fukada}
\vskip 0.2em
\textbf{Diego Elio Pettenò}
\vskip 0.2em
\textbf{Scott Shawcroft}
\vskip 5em
\today
\end{flushright}
\end{titlepage}
'''

latex_elements = {
    'maketitle': latex_maketitle
}


master_doc = 'index'

author=u'Zoltán Vörös'
copyright=author
language='en'

latex_documents = [
(master_doc, 'the-ulab-book.tex', 'The $\mu$lab book',
'Zoltán Vörös', 'manual'),
]

# sphinx-autoapi
extensions.append('autoapi.extension')
autoapi_type = 'python'
autoapi_keep_files = True
autoapi_dirs = ["ulab"]
autoapi_add_toctree_entry = False
autoapi_options = ['members', 'undoc-members', 'private-members', 'show-inheritance', 'special-members']
autoapi_template_dir = '../autoapi/templates'
autoapi_python_class_content = "both"
autoapi_python_use_implicit_namespaces = True
autoapi_root = "."


# Read the docs theme
on_rtd = os.environ.get('READTHEDOCS', None) == 'True'
if not on_rtd:
    try:
        import sphinx_rtd_theme
        html_theme = 'sphinx_rtd_theme'
        html_theme_path = [sphinx_rtd_theme.get_html_theme_path(), '.']
    except ImportError:
        html_theme = 'default'
        html_theme_path = ['.']
else:
    html_theme_path = ['.']


class UlabTransform(SphinxTransform):
    default_priority = 870

    def _convert_first_paragraph_into_title(self):
        title = self.document.next_node(nodes.title)
        paragraph = self.document.next_node(nodes.paragraph)
        if not title or not paragraph:
            return
        if isinstance(paragraph[0], nodes.paragraph):
            paragraph = paragraph[0]
        if all(isinstance(child, nodes.Text) for child in paragraph.children):
            for child in paragraph.children:
                title.append(nodes.Text(" \u2013 "))
                title.append(child)
            paragraph.parent.remove(paragraph)

    def _enable_linking_to_nonclass_targets(self):
        for desc in self.document.traverse(addnodes.desc):
            for xref in desc.traverse(addnodes.pending_xref):
                if xref.attributes.get("reftype") == "class":
                    xref.attributes.pop("refspecific", None)

    def apply(self, **kwargs):
        docname = self.env.docname
        if docname.startswith("ulab/"):
            self._convert_first_paragraph_into_title()
            self._enable_linking_to_nonclass_targets()


def setup(app):
    app.add_transform(UlabTransform)

# Minimal makefile for Sphinx documentation
#
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))


# You can set these variables from the command line.
SPHINXOPTS    = -c $(MAKEFILE_DIR)
SOURCEDIR     = "$(MAKEFILE_DIR)"
BUILDDIR      = "$(MAKEFILE_DIR)/../build-results/doc/"

.PHONY: html

html:
	@sphinx-build -b html $(SPHINXOPTS) "$(SOURCEDIR)" "$(BUILDDIR)"


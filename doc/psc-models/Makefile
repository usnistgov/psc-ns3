EPSTOPDF = epstopdf
DIA = dia
CONVERT = convert

SRC = ../../src
# Temporary source directory, for build
SOURCETEMP = source-temp
FIGURES = $(SOURCETEMP)/figures

# list all model library .rst files that need to be copied to $SOURCETEMP
SOURCES = \
	source/conf.py \
	source/_static \
	source/index.rst \
	source/replace.txt \
	source/antenna.rst \
	$(SRC)/antenna/doc/source/antenna-design-parabolic.inc \
	$(SRC)/antenna/doc/source/antenna-testing-parabolic.inc \
	source/buildings.rst \
	$(SRC)/buildings/doc/source/buildings-design-3gpp.inc \
	$(SRC)/buildings/doc/source/buildings-references-3gpp.inc \
	source/lte.rst \
	$(SRC)/lte/doc/source/lte-user-sidelink.inc \
	$(SRC)/lte/doc/source/lte-user-sidelink-traces.inc \
	$(SRC)/lte/doc/source/lte-user-sidelink-phy-error-model.inc \
	$(SRC)/lte/doc/source/lte-design-sidelink.inc \
	$(SRC)/lte/doc/source/lte-testing-sidelink.inc \
	$(SRC)/lte/doc/source/lte-references-sidelink.inc \
	$(SRC)/sip/doc/sip.rst \
	source/psc.rst \
	$(SRC)/psc/doc/source/psc-design.rst \
	$(SRC)/psc/doc/source/udp-group-echo.rst \
	$(SRC)/psc/doc/source/uav-mobility-energy.rst \
	$(SRC)/psc/doc/source/mcptt.rst \
	$(SRC)/psc/doc/source/intel-http.rst \
	$(SRC)/psc/doc/source/video-models.rst \
	$(SRC)/psc/doc/source/psc-application.rst \
	$(SRC)/psc/doc/source/psc-scenario.rst \
	$(SRC)/psc/doc/source/psc-references.rst \

# list all model library figure files that need to be copied to 
# $SOURCETEMP/figures.  For each figure to be included in all
# documentation formats (html, latex...) the following formats are supported: 
#  1) a single .dia file (preferred option, because it can be edited)
#  2) a single .eps file
#  3) both a .pdf and .png file
SOURCEFIGS = \
	$(SRC)/lte/doc/source/figures/lte-sl-pssch-bitmap.dia \
	$(SRC)/lte/doc/source/figures/prose-non-roaming-arch.dia \
	$(SRC)/lte/doc/source/figures/sidelink-scenarios-1d.dia \
	$(SRC)/lte/doc/source/figures/sidelink-scenarios-1c.dia \
	$(SRC)/lte/doc/source/figures/sidelink-scenarios-1b.dia \
	$(SRC)/lte/doc/source/figures/sidelink-scenarios-1a.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-ue-phy.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-enb-rrc.png \
	$(SRC)/lte/doc/source/figures/lte-sl-ue-rrc.png \
	$(SRC)/lte/doc/source/figures/lte-sl-pool-config.png \
	$(SRC)/lte/doc/source/figures/lte-sl-in-coverage-tx.png \
	$(SRC)/lte/doc/source/figures/lte-sl-in-coverage-rx.png \
	$(SRC)/lte/doc/source/figures/lte-sl-out-of-coverage-tx.png \
	$(SRC)/lte/doc/source/figures/lte-sl-out-of-coverage-rx.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-pscch.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-psbch.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-psdch.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-pssch-harq1.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-pssch-harq2.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-pssch-harq3.png \
	$(SRC)/lte/doc/source/figures/lte-sl-bler-pssch-harq4.png \
	$(SRC)/lte/doc/source/figures/lte-sl-simple-scenario.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-simple-OofCovrg-scenario.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-synch-test-topology1.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-synch-test-topology2.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-o2o-comm-state-machine-relay.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-o2o-comm-state-machine-remote.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-ue-arch-ctrl-plane.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-ue-arch-data-plane.dia \
	$(SRC)/lte/doc/source/figures/lte-sl-o2o-comm-message-exchange.dia \
        $(SRC)/lte/doc/source/figures/helpers.pdf \
        $(SRC)/lte/doc/source/figures/helpers.png \
	$(SRC)/lte/doc/source/figures/lte-sl-relay-selection.png \
	$(SRC)/psc/doc/source/figures/intel-http.png \
	$(SRC)/psc/doc/source/figures/sample-video-cdf.png \
	$(SRC)/psc/doc/source/figures/mcptt-operational-modes-scenario.png \
	$(SRC)/psc/doc/source/figures/mcptt-operational-modes-static-access-cdf.png \
	$(SRC)/psc/doc/source/figures/mcptt-operational-modes-static-m2e-cdf.png \
	$(SRC)/psc/doc/source/figures/mcptt-ptt-duration-cdf.png \
	$(SRC)/psc/doc/source/figures/mcptt-session-duration-cdf.png \
	$(SRC)/psc/doc/source/figures/mcptt-pusher-model-example.png \
	$(SRC)/psc/doc/source/figures/psc-application-client.png \
	$(SRC)/psc/doc/source/figures/psc-application-server.png \
	$(SRC)/psc/doc/source/figures/psc-schoolshooting.png \

# specify figures from which .png and .pdf figures need to be
# generated (all dia and eps figures)
IMAGES_EPS = \
	$(FIGURES)/lte-sl-pssch-bitmap.eps \
	$(FIGURES)/prose-non-roaming-arch.eps \
	$(FIGURES)/sidelink-scenarios-1d.eps \
	$(FIGURES)/sidelink-scenarios-1c.eps \
	$(FIGURES)/sidelink-scenarios-1b.eps \
	$(FIGURES)/sidelink-scenarios-1a.eps \
	$(FIGURES)/lte-sl-ue-phy.eps \
	$(FIGURES)/lte-sl-in-coverage-tx.png \
	$(FIGURES)/lte-sl-in-coverage-rx.png \
	$(FIGURES)/lte-sl-out-of-coverage-tx.png \
	$(FIGURES)/lte-sl-out-of-coverage-rx.png \
	$(FIGURES)/lte-sl-bler-pscch.png \
	$(FIGURES)/lte-sl-bler-psbch.png \
	$(FIGURES)/lte-sl-bler-psdch.png \
	$(FIGURES)/lte-sl-bler-pssch-harq1.png \
	$(FIGURES)/lte-sl-bler-pssch-harq2.png \
	$(FIGURES)/lte-sl-bler-pssch-harq3.png \
	$(FIGURES)/lte-sl-bler-pssch-harq4.png \
	$(FIGURES)/lte-sl-simple-scenario.eps \
	$(FIGURES)/lte-sl-simple-OofCovrg-scenario.eps \
	$(FIGURES)/lte-sl-synch-test-topology1.eps \
	$(FIGURES)/lte-sl-synch-test-topology2.eps \
	$(FIGURES)/lte-sl-o2o-comm-state-machine-relay.eps \
	$(FIGURES)/lte-sl-o2o-comm-state-machine-remote.eps \
	$(FIGURES)/lte-sl-ue-arch-ctrl-plane.eps \
	$(FIGURES)/lte-sl-ue-arch-data-plane.eps \
	$(FIGURES)/lte-sl-o2o-comm-message-exchange.eps \

IMAGES_PNG = ${IMAGES_EPS:.eps=.png}
IMAGES_PDF = ${IMAGES_EPS:.eps=.pdf}

IMAGES = $(IMAGES_EPS) $(IMAGES_PNG) $(IMAGES_PDF)

RESCALE = ../../utils/rescale-pdf.sh

%.eps : %.dia
	@echo dia $(notdir $<)
	@$(DIA) -t eps $< -e $@ >/dev/null

%.png : %.dia
	@echo dia $(notdir $<)
	@$(DIA) -t png $< -e $@ >/dev/null

%.png : %.eps
	@echo convert $(notdir $<)
	@$(CONVERT) $< $@ >/dev/null

%.pdf : %.eps
	@echo epstopdf $(notdir $<)
	@$(EPSTOPDF) $< -o=$@ >/dev/null
	@if test x$($@_width) != x; then $(RESCALE) $($@_width) $@ ; fi

# rescale pdf figures as necessary
$(FIGURES)/prose-non-roaming-arch.pdf_width = 10in
$(FIGURES)/sidelink-scenarios-1d.pdf_width = 10cm
$(FIGURES)/sidelink-scenarios-1c.pdf_width = 10cm
$(FIGURES)/sidelink-scenarios-1b.pdf_width = 10cm
$(FIGURES)/sidelink-scenarios-1a.pdf_width = 10cm
$(FIGURES)/lte-sl-simple-scenario.pdf_width = 4in
$(FIGURES)/lte-sl-simple-OofCovrg-scenario.pdf_width = 3in

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
PAPER         =
BUILDDIR      = build

# Internal variables.
PAPEROPT_a4     = -D latex_paper_size=a4
PAPEROPT_letter = -D latex_paper_size=letter
ALLSPHINXOPTS   = -d $(BUILDDIR)/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) $(SOURCETEMP) 

.PHONY: help clean html dirhtml singlehtml pickle json htmlhelp qthelp devhelp epub latex latexpdf text man changes linkcheck doctest

.NOTPARALLEL:

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  html       to make standalone HTML files"
	@echo "  dirhtml    to make HTML files named index.html in directories"
	@echo "  singlehtml to make a single large HTML file"
	@echo "  pickle     to make pickle files"
	@echo "  json       to make JSON files"
	@echo "  htmlhelp   to make HTML files and a HTML help project"
	@echo "  qthelp     to make HTML files and a qthelp project"
	@echo "  devhelp    to make HTML files and a Devhelp project"
	@echo "  epub       to make an epub"
	@echo "  latex      to make LaTeX files, you can set PAPER=a4 or PAPER=letter"
	@echo "  latexpdf   to make LaTeX files and run them through pdflatex"
	@echo "  text       to make text files"
	@echo "  man        to make manual pages"
	@echo "  changes    to make an overview of all changed/added/deprecated items"
	@echo "  linkcheck  to check all external links for integrity"
	@echo "  doctest    to run all doctests embedded in the documentation (if enabled)"

copy-sources:  $(SOURCES)
	@mkdir -p $(SOURCETEMP) 
	@mkdir -p $(FIGURES) 
	@cp -r -p $(SOURCES) $(SOURCETEMP)
	@cp -r -p $(SOURCEFIGS) $(FIGURES)

clean:
	-rm -rf $(BUILDDIR)/*
	-rm -rf $(SOURCETEMP)

frag: pickle
	@if test ! -d $(BUILDDIR)/frag; then mkdir $(BUILDDIR)/frag; fi
	pushd $(BUILDDIR)/frag && ../../pickle-to-xml.py ../pickle/index.fpickle  > navigation.xml && popd
	cp -r $(BUILDDIR)/pickle/_images $(BUILDDIR)/frag

html: copy-sources $(IMAGES) 
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) $(BUILDDIR)/html
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/html."

dirhtml: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b dirhtml $(ALLSPHINXOPTS) $(BUILDDIR)/dirhtml
	@echo
	@echo "Build finished. The HTML pages are in $(BUILDDIR)/dirhtml."

singlehtml: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b singlehtml $(ALLSPHINXOPTS) $(BUILDDIR)/singlehtml
	@echo
	@echo "Build finished. The HTML page is in $(BUILDDIR)/singlehtml."

pickle: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b pickle $(ALLSPHINXOPTS) $(BUILDDIR)/pickle
	@echo
	@echo "Build finished; now you can process the pickle files."

json: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b json $(ALLSPHINXOPTS) $(BUILDDIR)/json
	@echo
	@echo "Build finished; now you can process the JSON files."

htmlhelp: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b htmlhelp $(ALLSPHINXOPTS) $(BUILDDIR)/htmlhelp
	@echo
	@echo "Build finished; now you can run HTML Help Workshop with the" \
	      ".hhp project file in $(BUILDDIR)/htmlhelp."

qthelp: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b qthelp $(ALLSPHINXOPTS) $(BUILDDIR)/qthelp
	@echo
	@echo "Build finished; now you can run "qcollectiongenerator" with the" \
	      ".qhcp project file in $(BUILDDIR)/qthelp, like this:"
	@echo "# qcollectiongenerator $(BUILDDIR)/qthelp/ns-3.qhcp"
	@echo "To view the help file:"
	@echo "# assistant -collectionFile $(BUILDDIR)/qthelp/ns-3.qhc"

devhelp: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b devhelp $(ALLSPHINXOPTS) $(BUILDDIR)/devhelp
	@echo
	@echo "Build finished."
	@echo "To view the help file:"
	@echo "# mkdir -p $$HOME/.local/share/devhelp/ns-3"
	@echo "# ln -s $(BUILDDIR)/devhelp $$HOME/.local/share/devhelp/ns-3"
	@echo "# devhelp"

epub: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b epub $(ALLSPHINXOPTS) $(BUILDDIR)/epub
	@echo
	@echo "Build finished. The epub file is in $(BUILDDIR)/epub."

latex: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo
	@echo "Build finished; the LaTeX files are in $(BUILDDIR)/latex."
	@echo "Run \`make' in that directory to run these through (pdf)latex" \
	      "(use \`make latexpdf' here to do that automatically)."

latexpdf: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) $(BUILDDIR)/latex
	@echo "Running LaTeX files through pdflatex..."
	make -C $(BUILDDIR)/latex all-pdf
	@echo "pdflatex finished; the PDF files are in $(BUILDDIR)/latex."

text: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b text $(ALLSPHINXOPTS) $(BUILDDIR)/text
	@echo
	@echo "Build finished. The text files are in $(BUILDDIR)/text."

man: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b man $(ALLSPHINXOPTS) $(BUILDDIR)/man
	@echo
	@echo "Build finished. The manual pages are in $(BUILDDIR)/man."

changes: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b changes $(ALLSPHINXOPTS) $(BUILDDIR)/changes
	@echo
	@echo "The overview file is in $(BUILDDIR)/changes."

linkcheck: copy-sources $(IMAGEs)
	$(SPHINXBUILD) -b linkcheck $(ALLSPHINXOPTS) $(BUILDDIR)/linkcheck
	@echo
	@echo "Link check complete; look for any errors in the above output " \
	      "or in $(BUILDDIR)/linkcheck/output.txt."

doctest: copy-sources $(IMAGES)
	$(SPHINXBUILD) -b doctest $(ALLSPHINXOPTS) $(BUILDDIR)/doctest
	@echo "Testing of doctests in the sources finished, look at the " \
	      "results in $(BUILDDIR)/doctest/output.txt."



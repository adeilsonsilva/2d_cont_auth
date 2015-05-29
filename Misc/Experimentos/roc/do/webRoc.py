#!/usr/bin/env python
#############################################################################
# Copyright (C) 2013-2015 OpenEye Scientific Software, Inc.
#############################################################################
# Plotting ROC curve
#############################################################################

import sys
import os
from operator import itemgetter
import matplotlib.pyplot as plt


def main(argv=[__name__]):

    if len(sys.argv) != 4:
        print "usage: <safes> <scores>] <image>"
        sys.exit(0)

    afname = sys.argv[1]
    sfname = sys.argv[2]
    ofname = sys.argv[3]

    f, ext = os.path.splitext(ofname)
    if not IsSupportedImageType(ext):
        print("Format \"%s\" is not supported!" % ext)
        sys.exit(0)

    # read id of safes

    safes = LoadScores(afname)
    print("Loaded %d safes from %s" % (len(safes), afname))

    # read molecule id - score pairs

    nonsafes = LoadScores(sfname)
    print("Loaded %d nonsafes from %s" % (len(nonsafes), sfname))

    # sort scores by ascending order
    sortedsafes = sorted(safes)
    sortednonsafes = sorted(nonsafes)

    print("Plotting ROC Curve ...")
    color = "#008000"  # dark green
    DepictROCCurve(sortedsafes, sortednonsafes, color, ofname, randomline=True)


#def LoadActives(fname):
#
#    actives = []
#    for line in open(fname, 'r').readlines():
#        id = line.strip()
#        actives.append(id)
#
#    return actives


def LoadScores(fname):

    sfile = open(fname, 'r')

    scores = []
    for line in sfile.readlines():
        score = line.strip()
        scores.append((float(score)))

    return scores


def GetRates(safes, nonsafes):

    tpr = [0.0]  # true positive rate
    fpr = [0.0]  # false positive rate
    nrsafes = len(safes)
    nrdecoys = len(nonsafes)

    foundactives = 0.0
    founddecoys = 0.0

    for score in safes:
        foundactives += 1.0

        tpr.append(foundactives / float(nrsafes))
        
	for score in nonsafes:
		founddecoys += 1.0

		fpr.append(founddecoys / float(nrdecoys))

    return tpr, fpr


def SetupROCCurvePlot(plt):

    plt.xlabel("FPR", fontsize=14)
    plt.ylabel("TPR", fontsize=14)
    plt.title("ROC Curve", fontsize=14)


def SaveROCCurvePlot(plt, fname, randomline=True):

    if randomline:
        x = [0.0, 1.0]
        plt.plot(x, x, linestyle='dashed', color='red', linewidth=2, label='random')

    plt.xlim(0.0, 1.0)
    plt.ylim(0.0, 1.0)
    plt.legend(fontsize=10, loc='best')
    plt.tight_layout()
    plt.savefig(fname)


def AddROCCurve(plt, safes, nonsafes, color):

    tpr, fpr = GetRates(safes, nonsafes)

    plt.plot(fpr, tpr, color=color, linewidth=2)


def DepictROCCurve(safes, nonsafes, color, fname, randomline=True):

    plt.figure(figsize=(4, 4), dpi=80)

    SetupROCCurvePlot(plt)
    AddROCCurve(plt, safes, nonsafes, color)
    SaveROCCurvePlot(plt, fname, randomline)


def IsSupportedImageType(ext):
    fig = plt.figure()
    return (ext[1:] in fig.canvas.get_supported_filetypes())


if __name__ == "__main__":
    sys.exit(main(sys.argv))

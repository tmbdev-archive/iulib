#!/usr/bin/python

# Copyright 2008 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
# or its licensors, as applicable.
# 
# You may not use this file except under the terms of the accompanying license.
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you
# may not use this file except in compliance with the License. You may
# obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# Project: 
# File: check-am.py
# Purpose: identify files which are not handled in OCRopus automake
# Responsible: kofler
# Reviewer: 
# Primary Repository:
# Web Sites: www.iupr.org, www.dfki.de

import os, sys, glob


if not os.path.exists('Makefile.am'):
    print >> sys.stderr
    print >> sys.stderr, "Makefile.am not found!"
    print >> sys.stderr



def output(files, kind=""):
    """
    Produce some helpful output for maintaining automake
    """
    if len(files) > 0:
        print
        print "These", kind, "files are not handled:"
        for src in files:
            print src
        print "---"
    else:
        print
        print "OK, all", kind, "files are handled."


# get all cc and h files
ccs = []
for d in ["imgio","imglib","imgbits","utils"]:
    ccs += glob.glob(d+"/*.cc")
hs = []
for d in ["colib","imgbits","imglib","imgio","utils"]:
    hs += glob.glob(d+"/*.h")


# read automake file
amfile = open('Makefile.am')
am = amfile.read()
amfile.close()


# identify missing cc files, also mains and tests
missingccs = []
missingmains = []
missingtests = []
for src in ccs:
    if src not in am:
        if "main-" in src:
            missingmains.append(src)
        elif "test-" in src:
            missingtests.append(src)
        else:
            missingccs.append(src)

# identify missing h files
missinghs = []
for h in hs:
    if h not in am:
        missinghs.append(h)


print
print "Please remember: This script only checks if files are handled at all."
print "It does NOT check whether they are handled correctly!"

# output maintainance information for cc, h, main- and test- files
output(missingccs, "cc")
output(missinghs, "h")
output(missingmains, "main")
#output(missingtests, "test")


#print "dirs", dirs
#print "ccs", ccs
#print "hs", hs
#print am

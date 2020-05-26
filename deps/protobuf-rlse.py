#!/usr/bin/env python3
#-*- coding: utf-8 -*-

# Get Protocol Buffer version numbers, and determine latest release.
# Uses Git tags of GitHub REST API v3.
#   https://developer.github.com/v3/git/tags/
#
# All imports are from Python 3 standard library (hopefully!).
# There should be no need to install additional Python packages.
# Specifically, it does not use the very popular 'requests' package.
#
# Module 'typing' is used to support type hints (optional static type check)
#

from urllib import request  # not 'requests'
from typing import List
import json
import re
import sys

# repository info
repoinfo = {
    "owner": "protocolbuffers",
    "name" : "protobuf",
    "xtag" : "v3.8.0",              # expected tag
}
gbase = "https://api.github.com"
api_tags = "/repos/{owner}/{name}/tags".format(**repoinfo)

def dstr2intl(dstr: str) -> List[int] :
    "Convert dotted numeric string to integer list."
    nnlist = [ int(nn) for nn in dstr.split('.')]
    return nnlist

def getReleases(url) -> List[str] :
    """Get list of tagged "official" releases.
    Does not include RC, beta, etc.
    Release names have the form 'vN.N...[suffix]'.
    GitHub API returns a JSON array of object structures.
    Each object structure
    """
    rlselist = []
    handle = request.urlopen(url)
    releases = json.loads(handle.read())
    for tagmap in releases:
        rlsename = tagmap['name']
        if re.match('^v([\d.])+$', rlsename):
            rlselist.append(rlsename)
    return rlselist

def maxRelease(releases: List[str]) -> str:
    rnlist = None   # release name in List[int] form.
    rnstr = None    # release name in str form
    currmaxl = None # current maximum List[int] value
    currmaxn = None
    for name in releases:
        #print(name)
        intl = dstr2intl(name[1:])
        if currmaxl == None:
            currmaxl = intl; currmaxn = name
            continue
        if intl > currmaxl:
            currmaxl = intl; currmaxn = name
    #print (currmaxn, currmaxl)
    return currmaxn     # 'vN.N.N...'

vexpected = repoinfo["xtag"]

url = gbase + api_tags
releases = getReleases(url)
#print (releases)
maxname = maxRelease(releases)
print (maxname)
if maxname != vexpected:
    print("maxname", maxname, 'does not match expected', vexpected)
    sys.exit(1)

sys.exit(0)

# vim: set sw=4 tw=80 :
